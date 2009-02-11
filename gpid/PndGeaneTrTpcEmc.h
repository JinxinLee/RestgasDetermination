// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007
#pragma once
#ifndef PND_GEANETRTPCEMC_H
#define PND_GEANETRTPCEMC_H


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "PndEmcPoint.h"
#include "PndEmcCluster.h"
#include "PndTpcPoint.h"
#include "PndMCTrack.h"

class TGeant3;
class TClonesArray;
class TTree;
class TFile;

class PndGeaneTrTpcEmc : public FairTask
{
 public:
  /** Default constructor **/  
  PndGeaneTrTpcEmc();
  
  /** Destructor **/
  ~PndGeaneTrTpcEmc();
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  //protected:
 private:
  /** Input array of Points **/
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  TClonesArray* fClusterArray;
  //  CbmPlanePoint	 *fPoint1;
  //  PndMCTrack	 *fPoint1;
  PndTpcPoint	 *fPoint1;
  PndEmcPoint	 *fPoint2;
  PndEmcCluster	 *fCluster;
  void destroy(void);
  TTree *t;
  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fTrackParFinal;  
  
  TGeant3 *gMC3;
  ClassDef(PndGeaneTrTpcEmc,1);
  Int_t fEvent;
  FairGeanePro *fPro;
};
#endif
