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
#include "PndTpcDigiMapper.h"
#include "PndTpcEvtTime.h"
#include "PndTpcDigiPar.h"
#include "PndTpcPad.h"
#include "PndTpcPadPlane.h"
#include "PndTpcFrontend.h"
#include "PndTpcGem.h"
#include "PndTpcGas.h"
#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>
#include <deque>
// Class Member definitions -----------

using std::deque;
using std::vector;

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
    fdoTimeSim(false),
    fevtCount(0)
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

  // clean up
  unsigned int nbkg=fDigiVectors->size();
  for(unsigned int ib=0;ib<nbkg;++ib){
    // loop over digis
    vector<PndTpcDigi>* list=(*fDigiVectors)[ib];
    list->clear();
    delete list;
  }
  fDigiVectors->clear();
  delete fDigiVectors;
  fEvtTimes.clear();
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
  fbkgBranch=fbkgTree->GetBranch(fbkgBranchName);
  //fbkgBranch->SetAutoDelete(true);
  //fbkgBranch->SetBasketSize(2000);
  fbkgTree->SetBranchAddress("PndTpcEvtTime",&ftimeArray);
  ftimeBranch=fbkgTree->GetBranch("PndTpcEvtTime");
  //ftimeBranch->SetAutoDelete(true);
  //ftimeBranch->SetBasketSize(2000);
  fnAvailableBkgEvents=fbkgTree->GetEntries();

  fbkgTree->SetBranchStatus("*",0);
  fbkgTree->SetBranchStatus(fbkgBranchName+"*",1);
  fbkgTree->SetBranchStatus("PndTpcEvtTime*",1);
  

  int bytes=0;
  // std::cerr <<  "Loading Branches" << std::endl;
  //bytes+=fbkgBranch->LoadBaskets();
  //bytes+=ftimeBranch->LoadBaskets();
  //bytes+=fbkgTree->LoadBaskets();
  
  //std::cerr <<  "Branches Loaded: " <<bytes<<" basktes"<< std::endl;
  
  std::cerr <<  "Loading Digis" << std::endl;
  // Load complete bkg data into memory
  fDigiVectors = new vector<vector<PndTpcDigi>*>(fnAvailableBkgEvents);
  fEvtTimes.resize(fnAvailableBkgEvents);
  for(unsigned int ib=0;ib<fnAvailableBkgEvents;++ib){
    fbkgTree->GetEntry(ib);
    // loop over digis
    unsigned int ndigis=fbkgArray->GetEntries();
    (*fDigiVectors)[ib]=new vector<PndTpcDigi>(ndigis);
    vector<PndTpcDigi>* list=(*fDigiVectors)[ib];
    for(unsigned int id=0;id<ndigis;++id){
      PndTpcDigi* digi=(PndTpcDigi*)fbkgArray->At(id);
      (*list)[id]=(*digi); // copy digi;
    } // end loop over digis
    PndTpcEvtTime* t=(PndTpcEvtTime*)ftimeArray->At(0);
    fEvtTimes[ib]=(*t);
  }// end loop over bkg data

   std::cerr <<  "Digis Loaded"<< std::endl;

   // drop input file
   finFile->Close();
   delete finFile;
   finFile=NULL;

  fpadPlane= fpar->getPadPlane();
  
  
  // store timing info in this event
  ftimeOutArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",ftimeOutArray,fpersistence);
  ftimeOutArray->Expand(fnbkgEvts+2);
  if(fdoSignals){
    fOutArray = new TClonesArray("PndTpcSignal");
    ioman->Register("PndTpcSignalMixed","PndTpc",fOutArray,false);
  }
  else {
    fOutArray = new TClonesArray("PndTpcDigi");
    ioman->Register("PndTpcDigiMixed","PndTpc",fOutArray,false);

  }

  // remember that only about 50% of digis will actually 
  // be written out because of timing cuts!
  fOutArray->Expand(2000*fnbkgEvts); // this is still quite enough!


 // init the DigiMapper
  //fpar->printParams();
  ffrontend= fpar->getFrontend();
  fpadPlane= fpar->getPadPlane();
  fgem=      fpar->getGem();
  fgas=      fpar->getGas();
  fzGem=     fpar->getZGem();
  double sf= fpar->getFrontend()->samplingFrequency();
  double t0= fpar->getFrontend()->t0();
  double gain=fpar->getGain();

  fMaxDriftTime=150./fgas->VDrift(); 

  std::cout << "T0 " << t0 << "sF " << sf << std::endl;

  PndTpcDigiMapper::getInstance(false)->init(fpadPlane,fgem,fgas,fpar->getPadShapes(),fzGem,t0,sf);
  return kSUCCESS;
}



void
PndTpcEvtMixTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcEvtMixTask::Exec Event" << fevtCount++ << std::endl;
  // clean up fTimeArray
  ftimeOutArray->Delete();
  fOutArray->Delete();
  
  // int bytes=0;
  // std::cerr <<  "Loading Branches" << std::endl;
  // bytes+=fbkgTree->LoadBaskets();
  // std::cerr <<  "Branches Loaded: " <<bytes<<" basktes"<< std::endl;
  
  // reset timer
  double teventSim=ft0;
  
  // prepare drawing events without putting them back
  deque<unsigned int> availableEvents(fnAvailableBkgEvents);
  for(unsigned int i=0;i<fnAvailableBkgEvents;++i){
    availableEvents[i]=i;
  }

  // copy physics events into outarray;
  unsigned int nph=fsignalArray->GetEntries();
  

  for(unsigned int iph=0;iph<nph;++iph){
    if(fdoSignals){
      PndTpcSignal* digi=(PndTpcSignal*)fsignalArray->At(iph);
    
      new((*fOutArray)[iph]) PndTpcSignal(*digi);

    }
    else {
      PndTpcDigi* digi=(PndTpcDigi*)fsignalArray->At(iph);
      //std::cerr << "nSamples="<<digi->nSample() << std::endl;
      new((*fOutArray)[iph]) PndTpcDigi(*digi);
    }
  }
 // Look at this event geantHits in the TPC:
  Int_t iout=fOutArray->GetEntriesFast();
  std::cout<<iout<<" physics signals in OutArray"<<std::endl;
  
  // Get background events
  for(Int_t i=0;i<fnbkgEvts;++i){
    // select event from evailable ones:
    //std::cout << availableEvents.size() << " events left" << std::endl;
    unsigned int EventNum=gRandom->Integer(availableEvents.size());
    Int_t selectEvt=availableEvents.at(EventNum);
    // remove event from list of availables
    availableEvents.erase(availableEvents.begin()+EventNum);
    // -------------
  
    double tevent=fEvtTimes[i].t0();
    // if reshuffel
    if(fdoTimeSim){
       teventSim+=gRandom->Exp(fmeanEvtSpacing);
       new ((*ftimeOutArray)[i]) PndTpcEvtTime(tevent,selectEvt+1);    
       tevent=teventSim;
    }
    double teventClock=PndTpcDigiMapper::getInstance()->t_to_ticks(tevent);
    vector<PndTpcDigi>* bkgDigis=(*fDigiVectors)[selectEvt];
   
    // distinguish between signal and digi mixing!
    Int_t nsig=bkgDigis->size();
    for(Int_t ip=0;ip<nsig;++ip){
      if(fdoSignals){
	// PndTpcSignal* sig=(PndTpcSignal*)fbkgArray->At(ip);
	// // check if signal lies in region of interest
	// unsigned int sec=fpadPlane->GetPad(sig->padId())->sectorId();
	// if(fsectors.size()>0 && fsectors.find(sec)==fsectors.end()){
	//   // std::cout << "Skipping sig. Sector" 
	//   // 		  << sec << " not in list." << std::endl;
	//   continue;
	// }
	// // TODO: modify time of point according to event time
	// sig->sett(sig->t()+tevent);
	// sig->setmcEventId(selectEvt+1); // add because evt 0 = physics event!
	// // Add background to point-array of this event
	// new((*fOutArray)[iout++]) PndTpcSignal(*sig);
      } // if doSignals
      else {
	PndTpcDigi mydigi=(*bkgDigis)[ip];//(PndTpcDigi*)fbkgArray->At(ip);
	// check if signal lies in region of interest
	unsigned int sec=fpadPlane->GetPad(mydigi.padId())->sectorId();
	if(fsectors.size()>0 && fsectors.find(sec)==fsectors.end()){
	  // std::cout << "Skipping sig. Sector" 
	  // 		  << sec << " not in list." << std::endl;
	  continue;
	}
	// TODO: modify time of point according to event time
	mydigi.t(mydigi.t()+teventClock);
	// throw away digis that will not ly inside the physics event window
	// we shut this off again, since otherwise endcap 
	// penetration makes no sense
	//double realtime=PndTpcDigiMapper::getInstance()->t_from_tick(mydigi.t());
	//if(realtime < 0 || realtime > fMaxDriftTime) continue;
	mydigi.shiftEventIds(selectEvt+1);
	// Add background to point-array of this event
	new((*fOutArray)[iout++]) PndTpcDigi(mydigi);
      } // end do digi
     
    } // end loop over digis/signals
  } // end loop over bkg events
    //fbkgArray->Print();

  //ftimeBranch->DropBaskets("all");
  std::cout<<fOutArray->GetEntriesFast()<<" total in OutArray"<<std::endl;
  std::cout<<fOutArray->Capacity()<<" total Capacity of OutArra"<<std::endl;
}

void PndTpcEvtMixTask::FinishTask(){
  //finFile->Close();
}

ClassImp(PndTpcEvtMixTask)
