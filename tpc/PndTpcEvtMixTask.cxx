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
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcSignal.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcEvtTime.h"
#include "PndTpcDigiPar.h"
#include "PndTpcPad.h"
#include "PndTpcPadPlane.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>

// Class Member definitions -----------


PndTpcEvtMixTask::PndTpcEvtMixTask()
  : FairTask("TPC Background Event Addmixer"),
    finBranchName("PndTpcSignal"),
    fbkgBranchName("PndTpcSignal"),
    fbkgFileName(""),
    fsignalArray(NULL),
    ftimeArray(NULL),
    fbkgArray(NULL),
    fpersistence(kFALSE),
    fnbkgEvts(0),
    fmeanEvtSpacing(0),
    ft0(0),
    fdoTimeSim(false)
{}

PndTpcEvtMixTask::~PndTpcEvtMixTask()
{
  if(fbkgBranch!=NULL){
    delete fbkgBranch;
  }
  if(fbkgTree!=NULL){
    delete fbkgTree;
  }
  if(finFile!=NULL){
    finFile->Close();
    delete finFile;
  }
}


void
PndTpcEvtMixTask::SetParContainers() {

  std::cout<<"PndTpcEvtMixTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



InitStatus
PndTpcEvtMixTask::Init()
{

  std::cout<< "PndTpcEvtMixTask::Init()" <<std::endl;

  // check consistency
  if(finBranchName.Contains("Signal"))
    {
      if(!fbkgBranchName.Contains("Signal")){
	Error("Init","Trying to merge Background and Physics on different Levels!");
	return kERROR;
      }
      fdoSignals=true;
    }
  else {
     if(!fbkgBranchName.Contains("Digi")){
	Error("Init","Trying to merge Background and Physics on different Levels!");
	return kERROR;
      }
    fdoSignals=false;
  }

  // check consistency
 

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collections
  fsignalArray=(TClonesArray*) ioman->GetObject(finBranchName);

  if(fsignalArray==0)
    {
      Error("Init","signal-array not found!");
      return kERROR;
    }
  
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
  fbkgArray = new TClonesArray(fbkgBranchName);
  ftimeArray = new TClonesArray("PndTpcEvtTime");
  fbkgTree->SetBranchAddress(fbkgBranchName,&fbkgArray);
  fbkgTree->SetBranchAddress("PndTpcEvtTime",&ftimeArray);
  fnAvailableBkgEvents=fbkgTree->GetEntries();


  fpadPlane= fpar->getPadPlane();

  
  // store timing info in this event
  ftimeOutArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",ftimeOutArray,fpersistence);


  return kSUCCESS;
}



void
PndTpcEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcEvtMixTask::Exec" << std::endl;
  // clean up fTimeArray
  ftimeArray->Delete();

  // Look at this event geantHits in the TPC:
  Int_t iout=fsignalArray->GetEntriesFast();
  std::cout<<iout<<" signals in signalArray"<<std::endl;
  
  // reset timer
  double teventSim=ft0;
  
  // Get background events
  for(Int_t i=0;i<fnbkgEvts;++i){
    Int_t selectEvt=gRandom->Uniform(fnAvailableBkgEvents);
    fbkgTree->GetEntry(selectEvt);
    double tevent=((PndTpcEvtTime*)ftimeArray->At(0))->t0();
    // if reshuffel
    if(fdoTimeSim){
       teventSim+=gRandom->Exp(fmeanEvtSpacing);
       new ((*ftimeOutArray)[i]) PndTpcEvtTime(tevent,selectEvt+1);    
       tevent=teventSim;
    }
    double teventClock=PndTpcDigiMapper::getInstance()->t_to_ticks(tevent);
    //std::cout<<"tevent="<<tevent<<std::endl;
    // Load bkg array
    if(fbkgArray==NULL) Fatal("PndTpcEvtMixTask::Exec","bkgArray not loadable");
    // copy bkg array into output array
    // distinguish between signal and digi mixing!
    Int_t nsig=fbkgArray->GetEntriesFast();
    for(Int_t ip=0;ip<nsig;++ip){
      if(fdoSignals){
	PndTpcSignal* sig=(PndTpcSignal*)fbkgArray->At(ip);
	// check if signal lies in region of interest
	unsigned int sec=fpadPlane->GetPad(sig->padId())->sectorId();
	if(fsectors.size()>0 && fsectors.find(sec)==fsectors.end()){
	  // std::cout << "Skipping sig. Sector" 
	  // 		  << sec << " not in list." << std::endl;
	  continue;
	}
	// TODO: modify time of point according to event time
	sig->sett(sig->t()+tevent);
	sig->setmcEventId(selectEvt+1); // add because evt 0 = physics event!
	// Add background to point-array of this event
	new((*fsignalArray)[iout++]) PndTpcSignal(*sig);
      } // if doSignals
      else {
	PndTpcDigi* digi=(PndTpcDigi*)fbkgArray->At(ip);
	// check if signal lies in region of interest
	unsigned int sec=fpadPlane->GetPad(digi->padId())->sectorId();
	if(fsectors.size()>0 && fsectors.find(sec)==fsectors.end()){
	  // std::cout << "Skipping sig. Sector" 
	  // 		  << sec << " not in list." << std::endl;
	  continue;
	}
	// TODO: modify time of point according to event time
	digi->t(digi->t()+teventClock);
	digi->shiftEventIds(selectEvt+1);
	// Add background to point-array of this event
	new((*fsignalArray)[iout++]) PndTpcDigi(*digi);
      } // end digis
    }
  }
    //fbkgArray->Print();
  std::cout<<fsignalArray->GetEntriesFast()<<" total signals in signalArray"<<std::endl;

}

ClassImp(PndTpcEvtMixTask)
