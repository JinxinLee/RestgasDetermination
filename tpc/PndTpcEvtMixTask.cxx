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
    fnbkgEvts(0)
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
  

  fpadPlane= fpar->getPadPlane();


  return kSUCCESS;
}



void
PndTpcEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcEvtMixTask::Exec" << std::endl;

  // Look at this event geantHits in the TPC:
  Int_t iout=fsignalArray->GetEntriesFast();
  std::cout<<iout<<" signals in signalArray"<<std::endl;

  
  // Get background event
  for(Int_t i=0;i<fnbkgEvts;++i){
    fbkgTree->GetEntry(i);
    double tevent=((PndTpcEvtTime*)ftimeArray->At(0))->t0();
    //std::cout<<"tevent="<<tevent<<std::endl;
    // Load bkg array
    if(fbkgArray==NULL) Fatal("PndTpcEvtMixTask::Exec","bkgArray not loadable");
    // copy bkg array into output array
    Int_t nsig=fbkgArray->GetEntriesFast();
    for(Int_t ip=0;ip<nsig;++ip){
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
      sig->setmcEventId(i+1);
      // Add background to point-array of this event
      new((*fsignalArray)[iout++]) PndTpcSignal(*sig);
    }
  }
    //fbkgArray->Print();
  std::cout<<fsignalArray->GetEntriesFast()<<" total signals in signalArray"<<std::endl;

}

ClassImp(PndTpcEvtMixTask)
