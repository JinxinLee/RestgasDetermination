//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndGEMCorrelatorTask
//      see PndGEMCorrelatorTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcGEMCorrelatorTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"

#include "GFRecoHitFactory.h"
#include "GFException.h"

#include "GFDetPlane.h"

#include "GeaneTrackRep.h"
#include "RKTrackRep.h"
//#include "FairGeanePro.h"

#include "PndDetectorList.h"
#include "PndGemHit.h"
#include "PndGemRecoHit.h"


#include "TH1D.h"

// Class Member definitions -----------

bool sortByZ(const std::map<unsigned int, PndGemHit*>& h1,
	     const std::map<unsigned int, PndGemHit*>& h2) {
  TVector3 pos1, pos2;
  h1.begin()->second->Position(pos1);
  h2.begin()->second->Position(pos2);
  return (pos1.Z() < pos2.Z());
}


PndTpcGEMCorrelatorTask::PndTpcGEMCorrelatorTask()
  : FairTask("TPC-GEM Correlator"), fPersistence(kFALSE), fMatchDistance(3.),
    fMinGEMHits(2)
{
  fOutTrackBranchName = "TrackPreFitGEM";
  fTrackBranchName = "TrackPostFit";
  fGEMBranchName = "GEMHit";
  fVerbose = 0;
} 


PndTpcGEMCorrelatorTask::~PndTpcGEMCorrelatorTask()
{
  delete fResHistX;
  delete fResHistY;
  delete fResHistZ;
}

InitStatus
PndTpcGEMCorrelatorTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcGEMCorrelatorTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndTpcGEMCorrelatorTask::Init","track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fOutTrackArray = new TClonesArray("GFTrack"); 
  ioman->Register(fOutTrackBranchName,"PndTpc",fOutTrackArray,fPersistence);
  
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();
  
  fGEMArray=(TClonesArray*) ioman->GetObject(fGEMBranchName);
  if(fGEMArray==0){ 
    TString error = fGEMBranchName;
    error.Append(" array not found");
    Error("PndTpcGEMCorrelatorTask::Init", error);
  } 
  else {
    fTheRecoHitFactory->addProducer
      (ioman->GetBranchId(fGEMBranchName),new GFRecoHitProducer<PndGemHit,PndGemRecoHit>(fGEMArray));
  }


  fResHistX = new TH1D("resHistGEMX", "Extrapolation residual distribution",  200, -2, 2);
  fResHistY = new TH1D("resHistGEMY", "Extrapolation residual distribution",  200, -2, 2);
  fResHistZ = new TH1D("resHistGEMZ", "Extrapolation residual distribution",  200, -2, 2);
  
  return kSUCCESS;
}


void
PndTpcGEMCorrelatorTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcGEMCorrelatorTask::Exec"<<std::endl;
  fOutTrackArray->Delete();
  
  unsigned int ntracks = fTrackArray->GetEntriesFast();
  std::vector<std::map<unsigned int, PndGemHit*> > tempCand; //<arrayID, hit*>
  
  //loop over tracks
  for(unsigned int itr=0;  itr<ntracks; itr++) {
    std::cout<<"  ... processing TPC track no. "<<itr<<std::endl;
    GFTrack* track = (GFTrack*) (*fTrackArray)[itr];
    GFAbsTrackRep* rep = track->getCardinalRep();
    
    unsigned int nhits = fGEMArray->GetEntriesFast();
    
    TVector3 destination, error;
    TVector3 poca, dirInPoca, res;
    
    //loop over GEM  hits 
    for(unsigned int ig=0; ig<nhits; ig++) {
      PndGemHit* hit = (PndGemHit*) (*fGEMArray)[ig];
      hit->Position(destination);
      hit->PositionError(error);
      if(fVerbose) {
	std::cout<<"       processing hit at ";
	destination.Print();
	std::cout<<"       position error: ";
	error.Print();
      }
      	
      
      if(fVerbose)
	std::cout<<"starting extrap"<<std::endl;
      try { 
	rep->extrapolateToPoint(destination, poca, dirInPoca);
      }
      catch(GFException& ex) {
	 if(fVerbose) std::cout<<ex.what()<<std::endl;
	continue;
      }
      
      res.SetXYZ(poca.X()-destination.X(),
		 poca.Y()-destination.Y(),
		 poca.Z()-destination.Z());
      //Fill Histo
      fResHistX->Fill(res.X());
      fResHistY->Fill(res.Y());
      fResHistZ->Fill(res.Z());
      
      //check if hit is close enough
      if(fabs(res.X()) > fMatchDistance*error.X()  ||
	 fabs(res.Y()) > fMatchDistance*error.Y()  ||
	 fabs(res.Z()) > fMatchDistance*error.Z()) {
	if(fVerbose) {
	  std::cout<<"   not close enough: RES was ";
	  res.Print();
	  std::cout<< "  Station: "<<hit->GetStationNr()<<std::endl;
	}
	continue; //hit wasn't close enough to the track
      }
      else {	
	if(fVerbose) {
	  std::cout<<"   added hit - RES was ";
	  res.Print();
	  std::cout<< "  Station: "<<hit->GetStationNr()<<std::endl;
	}
	std::map<unsigned int, PndGemHit*> tmp;
	tmp[ig] = hit;
	tempCand.push_back(tmp);
      }
      
    }//end loop over pixels
   
    std::cout<<"======= TempCand from found GEM  hits has size "<<
      tempCand.size()<<" =======\n"<<std::endl;

    GFTrack* outTrack = new GFTrack(*track);
    outTrack->clearBookkeeping();
    GFTrackCand* outCand = new GFTrackCand(outTrack->getCand());
    
    if(tempCand.size() < fMinGEMHits) {
      (*fOutTrackArray)[fOutTrackArray->GetEntriesFast()] = outTrack;
      continue;
    }
    
    std::sort(tempCand.begin(), tempCand.end(), sortByZ);

    //append hits to track
    for(unsigned int ig=0; ig<tempCand.size(); ig++) {
      outCand->addHit(FairRootManager::Instance()->GetBranchId("GEMHit"),
		      tempCand[ig].begin()->first);		       
    } //end append hits
    
    outTrack->setCandidate(*outCand);
    (*fOutTrackArray)[fOutTrackArray->GetEntriesFast()] = outTrack;
        
  } //end loop over GEM hits

  return;
}

void
PndTpcGEMCorrelatorTask::WriteHistograms(const TString& fname) const {
  TFile* rOut = new TFile(fname, "recreate");
  rOut->cd();
  fResHistX->Write();
  fResHistY->Write();
  fResHistZ->Write();
  rOut->Close();
}

