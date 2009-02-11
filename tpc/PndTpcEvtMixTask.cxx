//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcEvtMixTask
//      see PndTpcEvtMixTask.h for details
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
#include "PndTpcEvtMixTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcSignal.h"
#include "PndTpcEvtTime.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>

// Class Member definitions -----------


PndTpcEvtMixTask::PndTpcEvtMixTask()
  : FairTask("TPC Background Event Addmixer"),
    _inBranchName("PndTpcSignal"),
    _bkgBranchName("PndTpcSignal"),
    _bkgFileName(""),
    _signalArray(NULL),
    _bkgArray(NULL),
    _persistence(kFALSE),
    _meanEvtSpacing(100),
    _nbkgEvts(0)
{}

PndTpcEvtMixTask::~PndTpcEvtMixTask()
{
  if(_bkgBranch!=NULL){
    delete _bkgBranch;
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
PndTpcEvtMixTask::Init()
{

  std::cout<< "PndTpcEvtMixTask::Init()" <<std::endl;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _signalArray=(TClonesArray*) ioman->GetObject(_inBranchName);

  if(_signalArray==0)
    {
      Error("Init","signal-array not found!");
      return kERROR;
    }

  _timeArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",_timeArray,_persistence);


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
  _bkgArray = new TClonesArray(_bkgBranchName);
  _bkgTree->SetBranchAddress(_bkgBranchName,&_bkgArray);



  return kSUCCESS;
}



void
PndTpcEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcEvtMixTask::Exec" << std::endl;

  // clean up _bkgArray;
  _timeArray->Delete();

  // Look at this event geantHits in the TPC:
  Int_t iout=_signalArray->GetEntriesFast();
  std::cout<<iout<<" signals in signalArray"<<std::endl;

  Double_t tevent=-_nbkgEvts*0.5*_meanEvtSpacing; // time of first event

  // Get background event
  for(Int_t i=0;i<_nbkgEvts;++i){
    // determine time offset of this event
    tevent+=gRandom->Exp(_meanEvtSpacing);
    new ((*_timeArray)[_timeArray->GetEntriesFast()]) PndTpcEvtTime(tevent,i+1);
    //std::cout<<"tevent="<<tevent<<std::endl;
    // Load bkg array
    _bkgTree->GetEntry(i);
    if(_bkgArray==NULL) Fatal("PndTpcEvtMixTask::Exec","bkgArray not loadable");
    // copy bkg array into output array
    Int_t nsig=_bkgArray->GetEntriesFast();
    for(Int_t ip=0;ip<nsig;++ip){
      PndTpcSignal* sig=(PndTpcSignal*)_bkgArray->At(ip);
      // TODO: modify time of point according to event time
      sig->sett(sig->t()+tevent);
      sig->setmcEventId(i+1);
      // Add background to point-array of this event
      new((*_signalArray)[iout++]) PndTpcSignal(*sig);
    }
  }
    //_bkgArray->Print();
  std::cout<<_signalArray->GetEntriesFast()<<" total signals in signalArray"<<std::endl;

}

ClassImp(PndTpcEvtMixTask)
