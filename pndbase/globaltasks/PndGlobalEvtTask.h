//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndGlobalEvtTask. Module to take the hit list for the 
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
//----------------------------------------------------------------------
#pragma once
#ifndef PndGlobalEvtTask_H
#define PndGlobalEvtTask_H

#include "FairTask.h"

//class IfdProxyDict; 

class PndGlobalEvtTask : public FairTask
{

public:

  // Constructors

  PndGlobalEvtTask();

  // Destructor

  virtual ~PndGlobalEvtTask();

  /** Virtual method Init **/
/*   virtual InitStatus Init(); */


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  


private:
  Int_t fblaInt;
public:
  ClassDef(PndGlobalEvtTask,1); 
};

//extern IfdProxyDict* gblEvtDict;
//IfdProxyDict* initialEvtProxy;
//IfdProxyDict* evtDict; 

#endif





