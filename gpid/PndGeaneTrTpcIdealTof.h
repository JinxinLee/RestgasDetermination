// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
#pragma once
#ifndef PNDGEANETRTPCIDEALTOF_H
#define PNDGEANETRTPCIDEALTOF_H

//ROOT & PandaRoot Headers
#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "FairGeane.h"
#include "PndTofPoint.h"
#include "PndTpcPoint.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheHit.h"
#include "CbmMCTrack.h"
//C++ headers
#include <algorithm>
#include <vector>
class TGeant3;
class TClonesArray;
class TTree;
class TFile;

class PndGeaneTrTpcIdealTof : public FairTask
{
 public:
  /** Default constructor **/  
  PndGeaneTrTpcIdealTof();

  /** Destructor **/
  virtual ~PndGeaneTrTpcIdealTof();

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
  PndTofPoint	 *fPoint2;

//  TTree *t;
//  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fPndTrackArr;  
  TClonesArray* fMCTrackArr;  
//  TClonesArray* fTrackParFinal;  
  void destroy();
  TGeant3 *gMC3;
  FairGeane *fGeane;
  Int_t fEvent;
  FairGeanePro *fPro;
  ClassDef(PndGeaneTrTpcIdealTof,1);
};

#endif
