//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcAdcTask
//      see PndTpcAdcTask.hh for details
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
#include "PndTpcAdcTask.h"

// C/C++ Headers ----------------------
#include <algorithm>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TRandom.h"
#include "PndTpcSignal.h"
#include "PndTpcSignalAge.h"
#include "PndTpcSample.h"
#include "PndTpcFrontend.h"
#include "PndTpcCRRCPulseshape.h"
#include "PndTpcDigitizationPolicy.h"

#include <iostream>

// Class Member definitions -----------


PndTpcAdcTask::PndTpcAdcTask()
  : FairTask("TPC Adc"), fpersistence(kFALSE)
{
  fsignalBranchName = "PndTpcSignal";
}


PndTpcAdcTask::~PndTpcAdcTask()
{
  if(ffrontend!=0)delete ffrontend;
  if(fpulseshape!=0)delete fpulseshape;
}

InitStatus
PndTpcAdcTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcAdcTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fsignalArray=(TClonesArray*) ioman->GetObject(fsignalBranchName);
  
  if(fsignalArray==0)
    {
      Error("PndTpcAdcTask::Init","Signal-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fsampleArray = new TClonesArray("PndTpcSample"); 
  ioman->Register("PndTpcSample","PndTpc",fsampleArray,fpersistence);

  
  //TODO: Get this from Database!
  ffrontend= new PndTpcFrontend(100,    // AdcThreshold
			     100000, // adcmax
			     10,     // adcbits
			     40,     // SamplingFreq_Mhz
			     -60000,      // t0
			     30,     // timebits
			     00);     // PSAthreshold

  fpulseshape= new PndTpcCRRCPulseshape(50,50,0.01); // tdiff,tint,tsig

  return kSUCCESS;
}


void
PndTpcAdcTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(fsampleArray==0) Fatal("PndTpcAdc::Exec)","No SampleArray");
  fsampleArray->Delete();
  std::vector <PndTpcSignal*> sv;
  std::vector <PndTpcSample*> samplev;
  
  Int_t ns=fsignalArray->GetEntriesFast();
  if(ns>0){
    for(Int_t is=0;is<ns;++is){
      // move signals into vector
      PndTpcSignal* sig=(PndTpcSignal*)fsignalArray->At(is);
      sv.push_back(sig);
      //fsignalArray->RemoveAt(is);
      //delete sig;
    } // end loop over signals
    //fsignalArray->Clear();
    //sort in time
    std::cout<<"begin sorting signals in time"<<std::endl;
    sort(sv.begin(),sv.end(),PndTpcSignalAge());
    std::cout<<"end sorting signals in time"<<std::endl;

    PndTpcDigitizationPolicy().Digitize(&sv,&samplev,ffrontend,fpulseshape);
  }

  //copy data into sample_array (TClonesvector)
  int nsamp=samplev.size();
  for(int isamp=0;isamp<nsamp;++isamp) {
    new((*fsampleArray)[isamp]) PndTpcSample(*(samplev[isamp]));
    delete samplev[isamp]; // clean up temporay store
  }

  std::cout<<fsampleArray->GetEntriesFast()<<" Samples created"<<std::endl;
  return;
}

ClassImp(PndTpcAdcTask)
