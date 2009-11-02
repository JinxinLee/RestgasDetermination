//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcHitsToWaveform. Module to take the hit list for the 
//      calorimeter and make ADC waveforms from them.
// 
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//      Phil Strother                  Original author
// 		 Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndGlobalEvtTask.h"
#include "IfdProxyDict.h"
#include "IfdSimpleProxyDict.h"
#include "AbsEnv.h"
// #include <iostream>

// static IfdSimpleProxyDict* initialEvtProxy= new IfdSimpleProxyDict;
// IfdProxyDict* gblEvtDict= initialEvtProxy;
// evtDict = 0;
// IfdProxyDict* gblEvtDict=0;

PndGlobalEvtTask::PndGlobalEvtTask()
  : fblaInt(1)
{
}

//--------------
// Destructor --
//--------------

PndGlobalEvtTask::~PndGlobalEvtTask()
{
}


// InitStatus PndGlobalEvtTask::Init()
// {
// 	return kSUCCESS;
// }

void PndGlobalEvtTask::Exec(Option_t* opt)
{
  if ( gblEvtDict ) {
     delete gblEvtDict;
     gblEvtDict=0;
  }
  gblEvtDict = new IfdSimpleProxyDict;
}

ClassImp(PndGlobalEvtTask)
