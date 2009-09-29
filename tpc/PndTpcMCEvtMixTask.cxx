//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcMCEvtMixTask
//      see PndTpcMCEvtMixTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcMCEvtMixTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcEvtTime.h"
#include "PndTpcPoint.h"
#include "PndTpcMCTracklet.h"
#include "FairMCPoint.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "PndMCTrack.h"
#include <iostream>

// Class Member definitions -----------


PndTpcMCEvtMixTask::PndTpcMCEvtMixTask()
  : FairTask("TPC MC Background Event Addmixer"),
    fmcTrackBranchName("MCTrack"),
    ftpcPointBranchName("PndTpcPoint"),
    fbkgFileName(""),
    fpersistence(kFALSE),
    fmeanEvtSpacing(100),
    fnbkgEvts(0),
    fminp(0.1),
    fvdrift(0.0027314) // standard velocity in NeCO2(90/10) @ 400V/cm
{}

PndTpcMCEvtMixTask::~PndTpcMCEvtMixTask()
{
  if(fbkgTrackBranch!=NULL){
    delete fbkgTrackBranch;
  }
  if(fbkgPointBranch!=NULL){
    delete fbkgPointBranch;
  }
  if(fbkgTree!=NULL){
    delete fbkgTree;
  }
  if(finFile!=NULL){
    finFile->Close();
    delete finFile;
  }
}

InitStatus
PndTpcMCEvtMixTask::Init()
{

  std::cout<< "PndTpcMCEvtMixTask::Init()" <<std::endl;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fmcTrackArray=(TClonesArray*) ioman->GetObject(fmcTrackBranchName);

  if(fmcTrackArray==0)
    {
      Error("Init","mctrack-array not found!");
      return kERROR;
    }

  fmcPointArray=(TClonesArray*) ioman->GetObject(ftpcPointBranchName);

  if(fmcPointArray==0)
    {
      Error("Init","tcppoint-array not found!");
      return kERROR;
    }


  ftimeArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",ftimeArray,fpersistence);

  ftrackArray = new TClonesArray("PndTpcMCTracklet");
  ioman->Register("PndTpcMCTracklet","PndTpc",ftrackArray,fpersistence);

  // open input file with background events
  if(fbkgFileName.IsNull())
    { 
      Error("Init","background file not found");
      return kERROR;
    }
  finFile=TFile::Open(fbkgFileName,"READ");
  if(!finFile->IsOpen())
    {
      Error("Init","background file could not be opened");
      return kERROR;
  }
  fbkgTree=(TTree*)finFile->Get("cbmsim");
  if(fbkgTree==NULL)
    {
      Error("Init","cbmsim tree not found in bkgfile");
      return kERROR;
    }
 
  //check if there are enough bkg events in tree
  Int_t n=fbkgTree->GetEntries();
  if(n<fnbkgEvts)
    {
      Error("Init","Not enough events in bkg file. Setting nbkgEvents to %i",n);
      fnbkgEvts=n;
    }
  
  // Create bkgArray
  fbkgTrackArray = new TClonesArray("PndMCTrack");
  fbkgTree->SetBranchAddress(fmcTrackBranchName,&fbkgTrackArray);
  fbkgPointArray = new TClonesArray(ftpcPointBranchName);
  fbkgTree->SetBranchAddress(ftpcPointBranchName,&fbkgPointArray);

  return kSUCCESS;
}



void
PndTpcMCEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcMCEvtMixTask::Exec" << std::endl;

  // clean up fbkgArray;
  ftimeArray->Delete();
  ftrackArray->Delete();

  // Look at this event geantHits in the TPC:
  Int_t iout=fmcPointArray->GetEntriesFast();
  std::cout<<iout<<" tpcpoints in Array"<<std::endl;

  buildTracks(fmcPointArray,fmcTrackArray,0,0); // physics event t0=0, id=0


  Double_t tevent=-fnbkgEvts*0.5*fmeanEvtSpacing; // time of first event

  // Get background event
  for(Int_t i=0;i<fnbkgEvts;++i){
    // determine time offset of this event
    tevent+=gRandom->Exp(fmeanEvtSpacing);
    new ((*ftimeArray)[ftimeArray->GetEntriesFast()]) PndTpcEvtTime(tevent,i+1);
    //std::cout<<"tevent="<<tevent<<std::endl;
    // Load bkg array
    fbkgTree->GetEntry(i);
    if(fbkgTrackArray==NULL) Fatal("PndTpcMCEvtMixTask::Exec","bkgTrackArray not loadable");
    if(fbkgPointArray==NULL) Fatal("PndTpcMCEvtMixTask::Exec","bkgPointArray not loadable");
    
    buildTracks(fbkgPointArray,fbkgTrackArray,tevent,i+1);
    
  }
    
  std::cout<<"Created "<<ftrackArray->GetEntriesFast()<<" mctracklets"<<std::endl;

}



void 
PndTpcMCEvtMixTask::buildTracks(TClonesArray* tpcpoints,TClonesArray* mctracks,
				Double_t evttime, Int_t evtId)
{
  // loop over TpcPoints -> create a track at every first point in the tpc
  // leave away all tracks with p<fminp
  int np=tpcpoints->GetEntriesFast();
  int oldtrackid=-99999;
  for(int i=0;i<np;++i){ // loop over tpcpoints
    PndTpcPoint* point=(PndTpcPoint*)tpcpoints->At(i);
    if(point->GetTrackID()==oldtrackid)continue;
    oldtrackid=point->GetTrackID();
    // new track id ==>
    TVector3 mom;
    point->Momentum(mom);
    // smear momentum
    mom.SetX(gRandom->Gaus(mom.X(),mom.X()*0.01));
    mom.SetY(gRandom->Gaus(mom.Y(),mom.Y()*0.01));
    mom.SetZ(gRandom->Gaus(mom.Z(),mom.Z()*0.01));

    if(mom.Perp()<fminp)continue; // check minimal momentum for track
    // fetch mc-truth
    PndMCTrack* mct=(PndMCTrack*)mctracks->At(oldtrackid);
    Int_t pdg=mct->GetPdgCode();
    TParticlePDG* part=TDatabasePDG::Instance()->GetParticle(pdg);
    if(part==NULL){
      std::cout<<"PDG="<<pdg<<" no particle in Database! Skipping!"<<std::endl;
      continue;
    }
    Double_t q=part->Charge()/3.;

    // begin new track
    TVector3 pos;
    point->Position(pos);

    // smear position
    pos.SetX(gRandom->Gaus(pos.X(),0.05));
    pos.SetY(gRandom->Gaus(pos.Y(),0.05));
    pos.SetZ(gRandom->Gaus(pos.Z(),0.05));

    // adjust reconstructed position in tpc according to event time:
    double offset=evttime*fvdrift;
    pos.SetZ(pos.Z()+offset);
    new ((*ftrackArray)[ftrackArray->GetEntriesFast()])
      PndTpcMCTracklet(pos,mom,
		       q,pdg,
		       oldtrackid,evtId);
  } // end loop over tpcpoints

}


ClassImp(PndTpcMCEvtMixTask)
