//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndEvtDeconvTask
//      see PndEvtDeconvTask.hh for details
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
#include "PndTpcEvtDeconvTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"

// Class Member definitions -----------

#define DEBUG 0



PndTpcEvtDeconvTask::PndTpcEvtDeconvTask()
  : FairTask("PndTpc Event Deconvolution"), fPersistence(kFALSE), fRCut(0), fZCut(0) 
{
  fOutTrackBranchName = "RiemannTrackTagged";
  fTrackBranchName = "RiemannTrack";
}


PndTpcEvtDeconvTask::~PndTpcEvtDeconvTask()
{
}

InitStatus
PndTpcEvtDeconvTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcEvtDeconvTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndTpcEvtDeconvTask::Init","riemmann-track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fOutTrackArray = new TClonesArray("PndTpcRiemannTrack"); 
  ioman->Register(fOutTrackBranchName,"PndTpc",fOutTrackArray,fPersistence);
  
  // histograms
  hRetained= new TH1I("hRetained","Number of tracklets after target cut",
		      200,0,200);
  hFoundPhysics= new TH1I("hFoundPhysics","Found Physics tracklets",
			  10,0,10);
  hFoundIDs= new TH1I("hFoundID","Number of physics tracks with at leaast one found tracklet", 10,0,10);

  return kSUCCESS;
}


void
PndTpcEvtDeconvTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcEvtDeconvTask::Exec"<<std::endl;
  fOutTrackArray->Delete();
  unsigned int foundPhysics=0;
  unsigned int presentPhysics=0;
  unsigned int retainedPileup=0;
  McIdCollection physicsID;
  unsigned int ntracks = fTrackArray->GetEntries(); 
  std::cout<< ntracks << " track candidates in mixed event" << std::endl;
  //loop over tracks
  for(unsigned int itr=0;  itr<ntracks; itr++) {
    if(DEBUG) std::cout<<"  ... processing Riemann track no. "<<itr<<std::endl;
    PndTpcRiemannTrack* track = (PndTpcRiemannTrack*) (*fTrackArray)[itr];

    // extrapolate track to z-axis and cut 
    // for the moment: presume physics ecent has been generated at t=0;
    
    TVector3 poca=track->pocaToZ();
     if(track->mcid().DominantID().mceventID()==0){
      ++presentPhysics;
      std::cout << "Physics tracklet: " << track->mcid().DominantID() << "  z="<< poca.Z() << "   r=" << poca.Perp() << std::endl;
    }

    // cut:
    if(poca.Perp()>fRCut)continue;
    if(TMath::Abs(poca.Z())>fZCut)continue;

    // survived cuts -> store track
    
    // keep track of mcids
    if(track->mcid().DominantID().mceventID()==0){
      ++foundPhysics;
      physicsID.AddID(track->mcid().DominantID());
    }
    else ++retainedPileup;

    new((*fOutTrackArray)[fOutTrackArray->GetEntries()]) PndTpcRiemannTrack(*track);
  } //end loop over tracks
  
  std::cout << "Retained "<< fOutTrackArray->GetEntries() << " tracklets in physics event (PR found "<<ntracks<<")" << std::endl;
  std::cout << foundPhysics << " true physics tracklets (PR found "<< presentPhysics <<")"<< std::endl;
  std::cout << "... containing "<< physicsID.nIDs() << " different MCtrackIDs" << std::endl; 

  hRetained->Fill( fOutTrackArray->GetEntries());
  hFoundPhysics->Fill(foundPhysics);
  hFoundIDs->Fill(physicsID.nIDs());

  std::cout << retainedPileup << " pileup tracklets" << std::endl<< std::endl;

  

    std::cout<<"PndTpcEvtDeconvTask::Exec ... finished"<<std::endl;

  return;
}

void 
PndTpcEvtDeconvTask::FinishTask(){
TFile* file=FairRootManager::Instance()->GetOutFile();
 file->mkdir("PndTpcEvtDeconvTask");
 file->cd("PndTpcEvtDeconvTask");
 hRetained->Write();delete  hRetained;
 hFoundPhysics->Write();delete hFoundPhysics;
 hFoundIDs->Write();delete hFoundIDs;

}


  
  



ClassImp(PndTpcEvtDeconvTask);
