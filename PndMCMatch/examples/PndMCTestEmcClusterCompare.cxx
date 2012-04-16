// -------------------------------------------------------------------------
// -----                PndMCTestEmcClusterCompare source file             -----
// -----                  Created 20.04.2010  by B.Spruck        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "PndMCTestEmcClusterCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData.h"

#include "PndMCTrack.h"
#include "PndEmcCluster.h"
#include "PndMCEntry.h"

#include "PndDetectorList.h"


// -----   Default constructor   -------------------------------------------
PndMCTestEmcClusterCompare::PndMCTestEmcClusterCompare()
	: FairTask("Creates PndMC test")
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMCTestEmcClusterCompare::~PndMCTestEmcClusterCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTestEmcClusterCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- PndMCTestEmcClusterCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (PndMCMatch*)ioman->GetObject("MCMatch");
if(fMCMatch==0){
std::cerr<<"No MCMatch"<<std::endl;
return kERROR;
}

  	fEmcCluster = (TClonesArray*)ioman->GetObject("EmcCluster");
if(fEmcCluster==0){
std::cerr<<"No EmcCluster"<<std::endl;
return kERROR;
}
  	fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");
if(fMCTrack==0){
std::cout<<"No MCTrack"<<std::endl;
return kERROR;
}

	std::cout << "-I- PndMCTestEmcClusterCompare::Init: Initialization successfull" << std::endl;


  return kSUCCESS;
}


// -------------------------------------------------------------------------
void PndMCTestEmcClusterCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMCTestEmcClusterCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	//PndMCResult myResult = fMCMatch->GetMCInfo(kEmcCluster, kMCTrack);
	//std::cout << myResult;
	for (int i = 0; i < fEmcCluster->GetEntries(); i++){
		//PndMCEntry myLinks = myResult.GetMCLink(i);
		PndEmcCluster* myHit = (PndEmcCluster*)fEmcCluster->At(i);
		PndMCEntry myLinks = fMCMatch->GetMCInfoSingle(FairLink(kEmcCluster, i), kMCTrack);
//		PndMCEntry myLinks = fMCMatch->GetMCInfo(FairLink(kEmcCluster, i), kMCTrack);

				std::cout << "------------- one EmcCluster ---------------" << std::endl;
		std::cout << *myHit;
                std::cout << "Energy: "<<myHit->GetEnergyCorrected()<<std::endl;
                std::cout << "Energy: "<<myHit->energy()<<std::endl;
		std::cout << "Links: "<< myLinks.GetNLinks()<<std::endl;

		std::vector <Int_t> testlist;
		for (int j = 0; j < myLinks.GetNLinks(); j++){
			if (myLinks.GetLink(j).GetType() == kMCTrack){
				testlist.push_back( myLinks.GetLink(j).GetIndex());
//				std::cout << "MCTrack "<<j <<" , MCIndex: "<< myLinks.GetLink(j).GetIndex();
//				PndMCTrack* myMCTrack = (PndMCTrack*)fMCTrack->At(myLinks.GetLink(j).GetIndex());
//				std::cout << " E: " << myMCTrack->Get4Momentum().E() << " MC PID: " << myMCTrack->GetPdgCode() << std::endl;
			}
		}

		std::cout << "cleaned list ..." <<std::endl;
		cleansortmclist(testlist,fMCTrack);
               for (int j = 0; j < testlist.size(); j++){
                        std::cout << "MCTrack (in list)"<<j <<" , MCIndex: "<< testlist[j];
                        PndMCTrack* myMCTrack = (PndMCTrack*)fMCTrack->At(testlist[j]);
                        std::cout << " E: " << myMCTrack->Get4Momentum().E() << " MC PID: " << myMCTrack->GetPdgCode() << std::endl;
                }


		std::cout << "--------------++++------------------" << std::endl;
	}
}

void PndMCTestEmcClusterCompare::Finish()
{
}

// Helper function, does not depend on class, identical to the one in PndEmcHitProducer
void PndMCTestEmcClusterCompare::cleansortmclist( std::vector <Int_t> &newlist,TClonesArray* mcTrackArray)
{
	std::vector <Int_t> tmplist;
	// Sort list...
	std::sort( newlist.begin(), newlist.end());
	// and copy every id only once (even so it might be in the list several times)
	std::unique_copy( newlist.begin(), newlist.end(), std::back_inserter( tmplist ) );

	// Now check if mother or (grand)^x-mother are already in the list
	// (which means i am a secondary)... if so, remove myself
	for(Int_t j=tmplist.size()-1; j>=0; j--){
		bool flag;
		PndMCTrack *pt;
		pt=((PndMCTrack*)mcTrackArray->At(tmplist[j]));
		if(pt->GetMotherID()<0) continue;
		flag=false;
		while(!flag){
			Int_t id;
			id=pt->GetMotherID();
			if(id<0) break;
			pt=(PndMCTrack*)mcTrackArray->At(id);

			for(Int_t k=j-1; k>=0; k--){
				if(tmplist[k]==id){
					tmplist.erase(tmplist.begin()+j);
					flag=true;
					break;
				}
			}
		}
	}
	newlist=tmplist;
}

ClassImp(PndMCTestEmcClusterCompare);
