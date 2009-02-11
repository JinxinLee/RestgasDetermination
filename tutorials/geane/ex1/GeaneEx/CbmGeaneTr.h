// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#ifndef CBMGEANETR_H
#define CBMGEANETR_H 1


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"

class TGeant3;
class TClonesArray;
class TTree;
class TFile;
class CbmPlanePoint;

class FairGeaneTr : public FairTask
{
 public:
  /** Default constructor **/  
  FairGeaneTr();

  /** Destructor **/
  ~FairGeaneTr();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
 private:
  /** Input array of Points **/
  TClonesArray* fPointArray1;
  TClonesArray* fPointArray2;
  CbmPlanePoint	 *fPoint1;
  CbmPlanePoint	 *fPoint2;

  TTree *t;
  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fTrackParFinal;  

  TGeant3 *gMC3;
  ClassDef(FairGeaneTr,1);
  Int_t fEvent;
  FairGeanePro *fPro;
};

#endif
