// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
// Modified by S.Vanniarajan,  May 2008
// for Geane Propagation to Time of flight 
// detector volume in PANDAROOT
#pragma once
#ifndef PNDGEANETRTPCTOF_H
#define PNDGEANETRTPCTOF_H


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "FairGeane.h"
#include "PndTofHit.h"
#include "PndTpcPoint.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "CbmMCTrack.h"

#include <algorithm>
#include <vector>

class TGeant3;
class TClonesArray;
class TTree;
class TFile;

class PndGeaneTrTpcTof : public FairTask
{
 public:
  /** Default constructor **/  
  PndGeaneTrTpcTof();

  /** Destructor **/
  virtual ~PndGeaneTrTpcTof();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 private:
  /** Input array of Points **/
  TClonesArray* fTrackArray1;
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  //  CbmPlanePoint	 *fPoint1;
//  CbmMCTrack	 *fPoint1;
  PndTpcPoint *fPoint1;
  PndTpcLheTrack *fTrack1;
  PndTofHit	 *fPoint2;

//  TTree *t;
//  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fPndTrackArr;  
  TClonesArray* fMCTrackArr;  
//  TClonesArray* fTrackParFinal;  
  void destroy(void);
  TGeant3 *gMC3;
  FairGeane *fGeane;
  Int_t fEvent;
  FairGeanePro *fPro;
  ClassDef(PndGeaneTrTpcTof,1);
};

#endif
