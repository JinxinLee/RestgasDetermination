//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcEvtTimeGenTask
//      see PndTpcEvtTimeGenTask.h for details
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
#include "PndTpcEvtTimeGenTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRun.h"
#include "TClonesArray.h"
#include "PndTpcEvtTime.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>

// Class Member definitions -----------


PndTpcEvtTimeGenTask::PndTpcEvtTimeGenTask()
  : FairTask("TPC Background Event Timer"),
     fpersistence(kFALSE),
    fmeanEvtSpacing(100),
    ft0(0),feventid(1)
{}

PndTpcEvtTimeGenTask::~PndTpcEvtTimeGenTask()
{
}

InitStatus
PndTpcEvtTimeGenTask::Init()
{

  std::cout<< "PndTpcEvtTimeGenTask::Init()" <<std::endl;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("Init","RootManager not instantiated!");
      return kERROR;
    }


  ftimeArray = new TClonesArray("PndTpcEvtTime");
  ioman->Register("PndTpcEvtTime","PndTpc",ftimeArray,fpersistence);

  ftevent=ft0;

  return kSUCCESS;
}



void
PndTpcEvtTimeGenTask::Exec(Option_t* opt)
{
  std::cout<< "PndTpcEvtTimeGenTask::Exec" << std::endl;

  // clean up fbkgArray;
  ftimeArray->Delete();

  ftevent+=gRandom->Exp(fmeanEvtSpacing);
  new ((*ftimeArray)[0]) PndTpcEvtTime(ftevent,feventid++);

  return;
}

ClassImp(PndTpcEvtTimeGenTask)
