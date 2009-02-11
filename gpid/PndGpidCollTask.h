/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

#pragma once
#ifndef PNDGPIDCOLLTASK_H
#define PNDGPIDCOLLTASK_H

//C++ headers
#include <algorithm>
#include <vector>

//ROOT & PandaRoot Headers
#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "FairGeane.h"
#include "PndTofHit.h"
#include "PndLhePidTrack.h"
#include "PndTpcPoint.h"
#include "PndMvdMCPoint.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "PndMCTrack.h"

class PndGpidCollTask : public FairTask
{
 public:
  /** Default constructor **/  
  PndGpidCollTask();
  
  /** Destructor **/
  virtual ~PndGpidCollTask();
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  void smear(const Double_t& theta, const Double_t& del );
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  //protected:
  
 private:
  //Functions and methods
  void destroy();
  /** Input array of Points **/
  TClonesArray* fTrackArray1;
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  TClonesArray* fPidTrack;  
  //  CbmPlanePoint	 *fPoint1;
  //  PndMCTrack	 *fPoint1;
  PndTpcPoint *fTpcPoint;
  PndMvdMCPoint *fMvdPoint;
  PndTpcLheTrack *fTrack1;
  PndTofHit	 *fPoint2;
  
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fPndTrackArr;  
  TClonesArray* fMCTrackArr;  
  
  ClassDef(PndGpidCollTask,1);
};
#endif
