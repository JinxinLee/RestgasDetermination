// -------------------------------------------------------------------------
// -----                   PndDchFindTracks source file                -----
// -----                  Created 27/01/08  by A. Bubak               -----
// -----                 according to the CbmStsFindTracks             -----
// -------------------------------------------------------------------------


// Pnd includes
#include "GlobalTrackMergingTask.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

// ROOT includes
#include "TClonesArray.h"
#include "TRandom3.h"

#include<iostream>
#include"assert.h"
#include"math.h"

#include"PndTrackCand.h"
#include"GFTrackCand.h"
#include"GFTrack.h"
#include"GFDetPlane.h"
#include"GeaneTrackRep.h"
#include "FairGeanePro.h"
#include"PndGenfitAdapters.h"


// -----   Default constructor   -------------------------------------------
GlobalTrackMergingTask::GlobalTrackMergingTask() {
  fGemCandArr = NULL;
  fDchCandArr = NULL;
  fMvdCandArr = NULL;
  fTrackArr = NULL;
  _geanePro = NULL;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
GlobalTrackMergingTask::~GlobalTrackMergingTask() {
  fGemCandArr->Delete();
  fDchCandArr->Delete();
  fMvdCandArr->Delete();
  fTrackArr->Delete();
  if(fGemCandArr != NULL) delete fGemCandArr;
  if(fDchCandArr != NULL) delete fDchCandArr;
  if(fMvdCandArr != NULL) delete fMvdCandArr;
  if(fTrackArr != NULL) delete fTrackArr;
}
// -------------------------------------------------------------------------



// -----   Public method Init (abstract in base class)  --------------------
InitStatus GlobalTrackMergingTask::Init() {

	// Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- GlobalTrackMergingTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

  fGemCandArr  = (TClonesArray*) ioman->GetObject("GemPndTrackCand");
  if ( !fGemCandArr ) {
    std::cout << "-W- GlobalTrackMergingTask::Init: No GemCand array!" << std::endl;
    return kERROR;
  }

  fDchCandArr  = (TClonesArray*) ioman->GetObject("DchPndTrackCand");
  if ( !fDchCandArr ) {
    std::cout << "-W- GlobalTrackMergingTask::Init: No DchCand array!" << std::endl;
    return kERROR;
  }

  fMvdCandArr  = (TClonesArray*) ioman->GetObject("MVDIdealGFTrackCand");
  if ( !fMvdCandArr ) {
    std::cout << "-W- GlobalTrackMergingTask::Init: No MvdCand array!" << std::endl;
    return kERROR;
  }



  // Create and register DchTrack array
  fTrackArr = new TClonesArray("GFTrack",100);
  ioman->Register("TrackPreFit", "", fTrackArr  , kTRUE);
  

  _geanePro=new FairGeanePro();

  std::cout << "GlobalTrackMergingTask::Init() finished" << std::endl;
  return kSUCCESS;  
  
}
// -------------------------------------------------------------------------



// -----  SetParContainers -------------------------------------------------
void GlobalTrackMergingTask::SetParContainers() {
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("PndGeoPassivePar");
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void GlobalTrackMergingTask::Exec(Option_t* opt) {
  std::cout << "%%%%%%%%%%%%%%%%%%%%%%$%$%$%$%$%$%%$%$%$" << std::endl;
  fTrackArr->Delete();
  std::set<int> trackIds;
  std::vector<GFTrackCand*> candsMvd;
  std::vector<GFTrackCand*> candsGem;
  std::vector<GFTrackCand*> candsDch;
  int nMvdCands = fMvdCandArr->GetEntriesFast();
  int nDchCands = fDchCandArr->GetEntriesFast();
  int nGemCands = fGemCandArr->GetEntriesFast();
  for(int i=0;i<nMvdCands;++i){
    GFTrackCand *c = new GFTrackCand(*((GFTrackCand*)fMvdCandArr->At(i)));
    candsMvd.push_back(c);
    if(!checkSort(c)) return;
    trackIds.insert(c->getMcTrackId());
  }
  for(int i=0;i<nDchCands;++i){
    GFTrackCand *c = PndTrackCand2GenfitTrackCand( (PndTrackCand*)fDchCandArr->At(i) );
    candsDch.push_back(c);
    if(!checkSort(c)) return;
    trackIds.insert(c->getMcTrackId());
  }
  for(int i=0;i<nGemCands;++i){
    GFTrackCand *c = PndTrackCand2GenfitTrackCand( (PndTrackCand*)fGemCandArr->At(i) );
    candsGem.push_back(c);
    if(!checkSort(c)) return;
    trackIds.insert(c->getMcTrackId());
  }
  std::set<int>::iterator iter;
  for(iter=trackIds.begin();iter!=trackIds.end();++iter){
    int trackId = *iter;
    GFTrackCand *theCand=NULL;
    for(unsigned int i=0;i<candsMvd.size();++i){
      if(trackId == candsMvd.at(i)->getMcTrackId()) {
	theCand = candsMvd.at(i);
	break;
      }
    }
    for(unsigned int i=0;i<candsGem.size();++i){
      if(trackId == candsGem.at(i)->getMcTrackId()) {
	if(theCand==NULL){
	  theCand = candsGem.at(i);
	}
	else{
	  theCand->append(*candsGem.at(i));
	}
	break;
      }
    }
    for(unsigned int i=0;i<candsDch.size();++i){
      if(trackId == candsDch.at(i)->getMcTrackId()) {
	if(theCand==NULL){
	  theCand = candsDch.at(i);
	}
	else{
	  theCand->append(*candsDch.at(i));
	}
	break;
      }
    }
    assert(theCand!=NULL);
    assert(checkSort(theCand));
    TVector3 pos = theCand->getPosSeed();
    TVector3 dir = theCand->getDirSeed();
    double qoverp = theCand->getQoverPseed();
    static TRandom3 rand;
    static const double eps = 0.05;
    pos.SetXYZ(
	       rand.Gaus(pos.X(),pos.X()*eps),
	       rand.Gaus(pos.Y(),pos.Y()*eps),
	       rand.Gaus(pos.Z(),pos.Z()*eps)
	       );
    dir.SetXYZ(
	       rand.Gaus(dir.X(),dir.X()*eps),
	       rand.Gaus(dir.Y(),dir.Y()*eps),
	       rand.Gaus(dir.Z(),dir.Z()*eps)
	       );
    TVector3 mom = dir;
    if(fabs(qoverp)>1.E-7){
      mom.SetMag(fabs(1./qoverp));
    }
    else{
      mom.SetMag(1.E6);
    }
    double q=0.;
    if(qoverp<0.)q=-1;
    if(qoverp>0.)q=1;
    GFDetPlane startPlane(pos,dir);
    TVector3 poserr(1.,1.,1.);
    TVector3 momerr(.4,.4,.4);
    GFTrack theTrack( new GeaneTrackRep(_geanePro,
				      startPlane,
				      mom,
				      poserr,
				      momerr,
				      q,
				      2212
				      )
		    );
    theTrack.setCandidate(*theCand);

    int nentries = fTrackArr->GetEntriesFast();
    new ((*fTrackArr)[nentries]) GFTrack(theTrack);
  }

  for(unsigned int i=0;i<candsMvd.size();++i){
    delete candsMvd.at(i);
  }
  for(unsigned int i=0;i<candsDch.size();++i){
    delete candsDch.at(i);
  }
  for(unsigned int i=0;i<candsGem.size();++i){
    delete candsGem.at(i);
  }

}
// -------------------------------------------------------------------------


bool GlobalTrackMergingTask::checkSort(GFTrackCand* c){
  std::vector<double> rhos = c->GetRhos();
  if(rhos.size() <= 1) return true;
  for(int i=1;i<rhos.size();++i){
    if(rhos.at(i)<rhos.at(i-1)) return false;
  }
  return true;
}


ClassImp(GlobalTrackMergingTask)
