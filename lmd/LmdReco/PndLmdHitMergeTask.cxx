// -------------------------------------------------------------------------
// -----                  PndLmdHitMergeTask source file               -----
// -------------------------------------------------------------------------

#include <cmath>
#include <vector>
#include <map>

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndLmdHitMergeTask.h"
#include "PndSdsHit.h"
#include "PndSdsMergedHit.h"

#include "TH2D.h"

// -----   Default constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask() :
FairTask("LMD Hit Merging Task")
{
  fHitBranchName = "LMDHitsPixel";
  hdxdy = new TH2D("hdxdy","; #deltax, #mum; #deltay, #mum",4e2,-100,100,4e2,-100,100);
  hdz = new TH1D("hdz",";#deltaz, #mum",2e3,-1000,1000);
  mtxpath = "../../input/";
  readAlign = true;
}

// -----   Named constructor   -------------------------------------------
PndLmdHitMergeTask::PndLmdHitMergeTask(const char* name) :
FairTask(name)
{
  fHitBranchName = "LMDHitsPixel";
  hdxdy = new TH2D("hdxdy","; #deltax, #mum; #deltay, #mum",4e2,-100,100,4e2,-100,100);
  hdz = new TH1D("hdz",";#deltaz, #mum",2e3,-1000,1000);
  readAlign = true;
}

// -----   Destructor   ----------------------------------------------------
PndLmdHitMergeTask::~PndLmdHitMergeTask()
{

}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdHitMergeTask::Init()
{
  lmddim = PndLmdDim::Instance();
  TString mtx_perfect =   mtxpath+"trafo_matrices_lmd.dat";
  lmddim -> Read_transformation_matrices(mtx_perfect.Data(), false);
  if(readAlign){
    TString mtx_corr =   mtxpath+"matrices_corrected.txt";
    lmddim -> Read_transformation_matrices(mtx_corr.Data(), true);
  }

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndLmdHitMergeTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject(fHitBranchName);
//
  if ( ! fHitArray )
  {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
    << "No LmdHitsPixel array!" << std::endl;
    return kERROR;
  }

  // set output arrays
  // fMergedHitArray = new TClonesArray("PndSdsHit");
  fMergedHitArray = new TClonesArray("PndSdsMergedHit");
  ioman->Register("LMDHitsMerged", "PndLmd", fMergedHitArray, true);
  Info("Init","Initialisation successfull");
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndLmdHitMergeTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<" **Starting PndLmdHitMergeTask::Exec()**"<<std::endl;

  // Reset output array
  //fMergedHitArray = FairRootManager::Instance()->GetTClonesArray("LmdHits");
  //if ( ! fMergedHitArray ) Fatal("Exec", "No OutputArray");
  fMergedHitArray->Delete();

  // Get input array
  fHitArray = (TClonesArray*)FairRootManager::Instance()->GetObject(fHitBranchName);
  if ( ! fHitArray )
  {
    std::cout << "-W- PndLmdHitMergeTask::Init: "
    << "No Hit array!" << std::endl;
    return;
  }

  // when we have no hits, we can end the event here.
  if (fHitArray->GetEntriesFast() == 0) return;

  // -------   SEARCH Close Hits  ------
  std::vector<unsigned int> mergewithIDs, mergedHits;
  //std::map<unsigned int, std::vector<unsigned int>> backmap;
  //  PndSdsHit* tmphit;
  PndSdsMergedHit* tmphit;
  unsigned int newHits=0;
  for(Int_t iHit = 0; iHit < fHitArray->GetEntriesFast(); iHit++){
    if (fVerbose > 2){
      if(iHit==0) cout<<"#### NEW event ####"<<endl;
    }
    bool skip=false;
    for(unsigned int ijk=0; ijk<mergedHits.size(); ijk++) //check if already merged
      if(iHit==mergedHits.at(ijk))
        skip=true;
    if(skip) continue;
    mergewithIDs.clear();
    PndSdsHit* myHit1 = (PndSdsHit*)(fHitArray->At(iHit));

    int sensor_id1 = myHit1->GetSensorID();
    int ihalf1,iplane1,imodule1,iside1,idie1,isensor1;
    lmddim->Get_sensor_by_id(sensor_id1,ihalf1,iplane1,imodule1,iside1,idie1,isensor1);
  
    for(Int_t jHit = iHit+1; jHit < fHitArray->GetEntriesFast(); jHit++){ //check other hits
      PndSdsHit* myHit2 = (PndSdsHit*)(fHitArray->At(jHit));
      int sensor_id2 = myHit2->GetSensorID();
      int ihalf2,iplane2,imodule2,iside2,idie2,isensor2;
      lmddim->Get_sensor_by_id(sensor_id2,ihalf2,iplane2,imodule2,iside2,idie2,isensor2);
     
      //more cleaver check by plane, side and half check
      if(iplane1==iplane2 && ihalf1==ihalf2 && imodule1==imodule2 && iside1!=iside2){  //actually same Hit from diff. sides: merge
	double dz = (myHit1->GetZ())-(myHit2->GetZ());
	hdz->Fill(1e4*dz);
      //    if(fabs(dz)<0.1 ){  //actually same Hit: merge
	//	if(fabs((myHit1->GetX())-(myHit2->GetX()))<0.016 && fabs((myHit1->GetY())-(myHit2->GetY()))<0.016){
	double dx = (myHit1->GetX())-(myHit2->GetX());
	double dy = (myHit1->GetY())-(myHit2->GetY());
	hdxdy->Fill(1e4*dx,1e4*dy);
	if(fabs(dx)<0.0075 && fabs(dy)<0.0075){ //3*sigma_dx, sigma_dx=25mkm
	  if (fVerbose > 2){
	    if (fVerbose > 4){
	      std::cout<<"HIT1(sensor_id1,ihalf1,iplane1,imodule1,iside1,idie1,isensor1):"<<std::endl;
	      std::cout<<sensor_id1<<", "<<ihalf1<<", "<<iplane1<<", "<<imodule1<<", "<<iside1<<", "<<idie1<<", "<<isensor1<<std::endl;
	      myHit1->Print();
	    }
	    if (fVerbose > 4){
	      std::cout<<"HIT2(sensor_id2,ihalf2,iplane2,imodule2,iside2,idie2,isensor2):"<<std::endl;
	      std::cout<<sensor_id2<<", "<<ihalf2<<", "<<iplane2<<", "<<imodule2<<", "<<iside2<<", "<<idie2<<", "<<isensor2<<std::endl;
	      myHit2->Print();
	    }
	    std::cout<<"----- We are going to merge HITS: -----"<<std::endl;
	    myHit1->Print();
	    std::cout<<".... AND ...."<<std::endl;
	    myHit2->Print();
	  std::cout<<"---------------------------------------"<<std::endl;
	  }
	  mergewithIDs.push_back(jHit);
	  mergedHits.push_back(jHit);
	}
      }
    }
      //    if( fabs(myHit1->GetZ()-myHit2->GetZ())<0.1 ){  //actually same Hit: merge
    // if( fabs(myHit1->GetZ()-myHit2->GetZ())<0.05 && 
    // 	fabs(myHit1->GetZ()-myHit2->GetZ())>0.01 &&
    // 	fabs(myHit1->GetX()-myHit2->GetX())<0.016&& 
    // 	fabs(myHit1->GetY()-myHit2->GetY())<0.016){  //actually same Hit: merge (too pixel size fo x-y)
    //   if (fVerbose > 2){
    // 	std::cout<<"----- We are going to merge HITS: -----"<<std::endl;
    // 	myHit1->Print();
    // 	std::cout<<".... AND ...."<<std::endl;
    // 	myHit2->Print();
    // 	std::cout<<"---------------------------------------"<<std::endl;
    //   }
    //     mergewithIDs.push_back(jHit);
    //     mergedHits.push_back(jHit);
    // }
    //  }

    if(mergewithIDs.size()>0){ //merge hits
      if (fVerbose > 4)
	cout<<"hit merged with: "<<mergewithIDs.size()<<" hits"<<endl;
      // tmphit = new((*fMergedHitArray)[newHits]) PndSdsHit(*myHit1);
      tmphit = new((*fMergedHitArray)[newHits]) PndSdsMergedHit(*myHit1,0);

      double x=tmphit->GetX(), y=tmphit->GetY(), z=tmphit->GetZ();
      for(unsigned int iMerge=0; iMerge<mergewithIDs.size(); iMerge++){ //loop over hits to merge in
        PndSdsHit* myHit2 = (PndSdsHit*)(fHitArray->At(mergewithIDs.at(iMerge)));
        x+=myHit2->GetX(); y+=myHit2->GetY(); z+=myHit2->GetZ();
	tmphit->SetSecondMCHit(myHit2->GetRefIndex()); //!!! works only in case merging 2 hits
	tmphit->SetIsMerged(true);
      }
      x/=(mergewithIDs.size()+1); y/=(mergewithIDs.size()+1); z/=(mergewithIDs.size()+1);
      tmphit->SetX(x); tmphit->SetY(y); tmphit->SetZ(z);
      if (fVerbose > 4){
	cout<<"!!! Merged hit !!!"<<endl;
	tmphit->Print();
      }
    }else{ //dont merge, just use original hit
      //       tmphit = new((*fMergedHitArray)[newHits]) PndSdsHit(*myHit1);
      tmphit = new((*fMergedHitArray)[newHits]) PndSdsMergedHit(*myHit1,-1);
      tmphit->SetIsMerged(false);
    }

    newHits++;
  }
  
  if (fVerbose > 1)
    std::cout << "-I- PndLmdHitMergeTask: out of " << fHitArray->GetEntriesFast()
    << " Hits " << fMergedHitArray->GetEntriesFast()<<" Hits merged."
    << std::endl;
  return;
}

ClassImp(PndLmdHitMergeTask);

