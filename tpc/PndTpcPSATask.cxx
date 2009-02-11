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


// Class Member definitions -----------


PndTpcPSATask::PndTpcPSATask()
  : FairTask("TPC Pulse shape analyser"), _persistence(kFALSE)
{
  _sampleBranchName = "PndTpcSample";
}


PndTpcPSATask::~PndTpcPSATask()
{
  if(_frontend!=0)delete _frontend;
  if(_psa!=0)delete _psa;
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
  _sampleArray=(TClonesArray*) ioman->GetObject(_sampleBranchName);
  
  if(_sampleArray==0)
    {
      Error("PndTpcPSATask::Init","Sample-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _digiArray = new TClonesArray("PndTpcDigi"); 
  ioman->Register("PndTpcDigi","PndTpc",_digiArray,_persistence);

  
  //TODO: Get this from Database!
  _frontend= new PndTpcFrontend(100,    // AdcThreshold
			     100000, // adcmax
			     10,     // adcbits
			     40,     // SamplingFreq_Mhz
			     -60000,      // t0
			     30,     // timebits
			     10);     // PSAthreshold

  _psa= new PndTpcSimplePSAStrategy(10); // threshold (in adc channels)

  return kSUCCESS;
}


void
PndTpcPSATask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcPSATask::Exec"<<std::endl;
  // Reset output Array
  if(_digiArray==0) Fatal("PndTpcPSA::Exec)","No DigiArray");
  _digiArray->Delete();
  std::vector <PndTpcSample*> samplev;
  std::vector <PndTpcDigi*> digis;
  
  Int_t ns=_sampleArray->GetEntriesFast();
  if(ns>0){
    for(Int_t is=0;is<ns;++is){
      PndTpcDigi* digi=_psa->ProcessNext((PndTpcSample*)_sampleArray->At(is));
      if(digi!=0)digis.push_back(digi);
    }
  }
  //sort digis in time;
  std::cout<<"sort Digis in time ... ";
  sort(digis.begin(),digis.end(),PndTpcDigiAge());
  std::cout<<"finished"<<std::endl;
  
  //copy data into digi_array (TClonesvector)
  int ndigi=digis.size();
  for(int idigi=0;idigi<ndigi;++idigi) {
    new((*_digiArray)[idigi]) PndTpcDigi(*(digis[idigi]));
    delete digis[idigi]; // clean up temporay store
  }

  std::cout<<_digiArray->GetEntriesFast()<<" Digis created"<<std::endl;
  return;
}

ClassImp(PndTpcPSATask)
