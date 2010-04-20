//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPSATask
//      see PndTpcPSATask.hh for details
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
#include "PndTpcPSATask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "PndTpcSample.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiAge.h"
#include "PndTpcFrontend.h"
#include "PndTpcSimplePSAStrategy.h"
#include "PndTpcPSA_TOT1.h"
#include "PndTpcPSA_AD1.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "PndTpcT2KPulseshape.h"


// Class Member definitions -----------
void
PndTpcPSATask::SetParContainers() {

  std::cout<<"PndTpcPSATask::SetParContainers"<<std::endl;
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


PndTpcPSATask::PndTpcPSATask()
  : FairTask("TPC Pulse shape analyser"), fpersistence(kFALSE), fpeak(100)
{
  fsampleBranchName = "PndTpcSample";
}


PndTpcPSATask::~PndTpcPSATask()
{
  if(ffrontend!=0)delete ffrontend;
  if(fpsa!=0)delete fpsa;
}

InitStatus
PndTpcPSATask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcPSATask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fsampleArray=(TClonesArray*) ioman->GetObject(fsampleBranchName);
  
  if(fsampleArray==0)
    {
      Error("PndTpcPSATask::Init","Sample-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fdigiArray = new TClonesArray("PndTpcDigi"); 
  ioman->Register("PndTpcDigi","PndTpc",fdigiArray,fpersistence);

  
  //TODO: Get this from Database
  //  ffrontend= new PndTpcFrontend(10,    // AdcThreshold
  //		     2048, // adcmax
  //		     11,     // adcbits
  //		     20,     // SamplingFreq_Mhz
  //		     0,      // t0
  //			     16,     // timebits
  //		     10);     // PSAthreshold

  ffrontend = (PndTpcFrontend*) fpar->getFrontend();
  fpulseshape= new PndTpcT2KPulseshape(fpeak);

   if( fpar->getPSA() == 0)	
     {
  	fpsa= new PndTpcSimplePSAStrategy(ffrontend->psaThreshold());
	std::cout << "Using Simple PSA strategy!" << std::endl;
     }
   else 
     if( fpar->getPSA() == 1)	
       {
	 fpsa= new PndTpcPSA_TOT1();
	 std::cout << "Using PSA_TOT strategy!" << std::endl;
       }
     else 
       if( fpar->getPSA() == 2)	
	 {
	   fpsa= new PndTpcPSA_AD1();//fpulseshape);
	   fpsa->setPs(fpulseshape);
	   fpsa->TailCancellation(fTail);
	   fpsa->setFreq(ffrontend->samplingFrequency());
	   std::cout << "Using PSA_AD strategy!" << std::endl;
	 }  
     else return kERROR;
  return kSUCCESS;
}


void
PndTpcPSATask::Exec(Option_t* opt)
{


  std::cout<<"PndTpcPSATask::Exec"<<std::endl;
  // Reset output Array
  if(fdigiArray==0) Fatal("PndTpcPSA::Exec)","No DigiArray");
  fdigiArray->Delete();
  std::vector <PndTpcSample*> samplev;
  std::vector <PndTpcDigi*> digis;
  
  Int_t ns=fsampleArray->GetEntriesFast();
  if(ns>0){

    if ( fpar->getPSA() == 1 || fpar->getPSA() == 2 )
      {
	std::vector<PndTpcSample*> vecSa;
	vecSa.clear();
	for(Int_t is=0;is<ns;++is)
	  vecSa.push_back((PndTpcSample*)fsampleArray->At(is));
	if(vecSa.size()!=0)
	  {
	    std::cout << "Processing " << vecSa.size() << " samples with PSA (th";
	    fpsa->Process(vecSa,digis,ffrontend->psaThreshold());
	    std::cout <<ffrontend->psaThreshold() << ") -- " << digis.size()<< " Digis created."<<std::endl;
	  }
      }
    else {
      for(Int_t is=0;is<ns;++is){
	PndTpcDigi* digi=fpsa->ProcessNext((PndTpcSample*)fsampleArray->At(is));
	if(digi!=0)
	  {
	    //	    digi->fct(((PndTpcSample*)fsampleArray->At(is))->ct());
	    digis.push_back(digi); 
	  }
      }
    }
  }
  //sort digis in time;
  std::cout<<"sort Digis in time ... ";
  sort(digis.begin(),digis.end(),PndTpcDigiAge());
  std::cout<<"finished"<<std::endl;
  
  //copy data into digi_array (TClonesvector)
  int ndigi=digis.size();
  for(int idigi=0;idigi<ndigi;++idigi) {
    new((*fdigiArray)[idigi]) PndTpcDigi(*(digis[idigi]));
    delete digis[idigi]; // clean up temporay store
  }

  std::cout<<fdigiArray->GetEntriesFast()<<" Digis created"<<std::endl;
  return;
}

ClassImp(PndTpcPSATask)
