// Task for the exercise 1 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#ifndef CBMGEANETR_H
#define CBMGEANETR_H 1


#include "CbmTask.h"
#include "TVector3.h"
#include "CbmGeanePro.h"

class TGeant3;
class TClonesArray;
class TTree;
class TFile;
class CbmPlanePoint;

class CbmGeaneTr : public CbmTask
{
 public:
  /** Default constructor **/  
  CbmGeaneTr();

  /** Destructor **/
  ~CbmGeaneTr();

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
  ClassDef(CbmGeaneTr,1);
  Int_t fEvent;
  CbmGeanePro *fPro;
};

#endif
