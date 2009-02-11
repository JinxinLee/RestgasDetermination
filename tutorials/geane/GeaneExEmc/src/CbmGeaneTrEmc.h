// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#ifndef CBMGEANETREMC_H
#define CBMGEANETREMC_H 1


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "PndEmcPoint.h"
#include "PndMCTrack.h"

class TGeant3;
class TClonesArray;
class TTree;
class TFile;

class FairGeaneTrEmc : public FairTask
{
 public:
  /** Default constructor **/  
  FairGeaneTrEmc();

  /** Destructor **/
  ~FairGeaneTrEmc();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
 private:
  /** Input array of Points **/
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  //  CbmPlanePoint	 *fPoint1;
  PndMCTrack	 *fPoint1;
  PndEmcPoint	 *fPoint2;

  TTree *t;
  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fTrackParFinal;  

  TGeant3 *gMC3;
  ClassDef(FairGeaneTrEmc,1);
  Int_t fEvent;
  FairGeanePro *fPro;
};

#endif
