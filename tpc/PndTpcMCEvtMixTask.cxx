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
#include "CbmMCTrack.h"
#include <iostream>

// Class Member definitions -----------


PndTpcMCEvtMixTask::PndTpcMCEvtMixTask()
  : FairTask("TPC MC Background Event Addmixer"),
    _mcTrackBranchName("MCTrack"),
    _tpcPointBranchName("PndTpcPoint"),
    _bkgFileName(""),
    _persistence(kFALSE),
    _meanEvtSpacing(100),
    _nbkgEvts(0),
    _minp(0.1),
    _vdrift(0.0027314) // standard velocity in NeCO2(90/10) @ 400V/cm
{}

PndTpcMCEvtMixTask::~PndTpcMCEvtMixTask()
{
  if(_bkgTrackBranch!=NULL){
    delete _bkgTrackBranch;
  }
  if(_bkgPointBranch!=NULL){
    delete _bkgPointBranch;
  }
  if(_bkgTree!=NULL){
    delete _bkgTree;
  }
  if(_inFile!=NULL){
    _inFile->Close();
    delete _inFile;
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
  _mcTrackArray=(TClonesArray*) ioman->GetObject(_mcTrackBranchName);

  if(_mcTrackArray==0)
    {
      Error("Init","mctrack-array not found!");
      return kERROR;
    }

  _mcPointArray=(TClonesArray*) ioman->GetObject(_tpcPointBranchName);

  if(_mcPointArray==0)
    {
      Error("Init","tcppoint-array not found!");
      return kERROR;
    }


  _timeArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",_timeArray,_persistence);

  _trackArray = new TClonesArray("PndTpcMCTracklet");
  ioman->Register("PndTpcMCTracklet","PndTpc",_trackArray,_persistence);

  // open input file with background events
  if(_bkgFileName.IsNull())
    { 
      Error("Init","background file not found");
      return kERROR;
    }
  _inFile=TFile::Open(_bkgFileName,"READ");
  if(!_inFile->IsOpen())
    {
      Error("Init","background file could not be opened");
      return kERROR;
  }
  _bkgTree=(TTree*)_inFile->Get("cbmsim");
  if(_bkgTree==NULL)
    {
      Error("Init","cbmsim tree not found in bkgfile");
      return kERROR;
    }
 
  //check if there are enough bkg events in tree
  Int_t n=_bkgTree->GetEntries();
  if(n<_nbkgEvts)
    {
      Error("Init","Not enough events in bkg file. Setting nbkgEvents to %i",n);
      _nbkgEvts=n;
    }
  
  // Create bkgArray
  _bkgTrackArray = new TClonesArray("CbmMCTrack");
  _bkgTree->SetBranchAddress(_mcTrackBranchName,&_bkgTrackArray);
  _bkgPointArray = new TClonesArray(_tpcPointBranchName);
  _bkgTree->SetBranchAddress(_tpcPointBranchName,&_bkgPointArray);

  return kSUCCESS;
}



void
PndTpcMCEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcMCEvtMixTask::Exec" << std::endl;

  // clean up _bkgArray;
  _timeArray->Delete();
  _trackArray->Delete();

  // Look at this event geantHits in the TPC:
  Int_t iout=_mcPointArray->GetEntriesFast();
  std::cout<<iout<<" tpcpoints in Array"<<std::endl;

  buildTracks(_mcPointArray,_mcTrackArray,0,0); // physics event t0=0, id=0


  Double_t tevent=-_nbkgEvts*0.5*_meanEvtSpacing; // time of first event

  // Get background event
  for(Int_t i=0;i<_nbkgEvts;++i){
    // determine time offset of this event
    tevent+=gRandom->Exp(_meanEvtSpacing);
    new ((*_timeArray)[_timeArray->GetEntriesFast()]) PndTpcEvtTime(tevent,i+1);
    //std::cout<<"tevent="<<tevent<<std::endl;
    // Load bkg array
    _bkgTree->GetEntry(i);
    if(_bkgTrackArray==NULL) Fatal("PndTpcMCEvtMixTask::Exec","bkgTrackArray not loadable");
    if(_bkgPointArray==NULL) Fatal("PndTpcMCEvtMixTask::Exec","bkgPointArray not loadable");
    
    buildTracks(_bkgPointArray,_bkgTrackArray,tevent,i+1);
    
  }
    
  std::cout<<"Created "<<_trackArray->GetEntriesFast()<<" mctracklets"<<std::endl;

}



void 
PndTpcMCEvtMixTask::buildTracks(TClonesArray* tpcpoints,TClonesArray* mctracks,
				Double_t evttime, Int_t evtId)
{
  // loop over TpcPoints -> create a track at every first point in the tpc
  // leave away all tracks with p<_minp
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

    if(mom.Perp()<_minp)continue; // check minimal momentum for track
    // fetch mc-truth
    CbmMCTrack* mct=(CbmMCTrack*)mctracks->At(oldtrackid);
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
    double offset=evttime*_vdrift;
    pos.SetZ(pos.Z()+offset);
    new ((*_trackArray)[_trackArray->GetEntriesFast()])
      PndTpcMCTracklet(pos,mom,
		       q,pdg,
		       oldtrackid,evtId);
  } // end loop over tpcpoints

}


ClassImp(PndTpcMCEvtMixTask)
