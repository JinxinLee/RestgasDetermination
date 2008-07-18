// Task for the exercise 2 of the GEANE tutorial
// Authors A. Fontana & P. Genova, Sept. 2007

#ifndef CBMGEANETrT_H
#define CBMGEANETrT_H 1

#include "CbmTask.h"
#include "TVector3.h"
#include "CbmGeanePro.h"
#include "CbmGeaneUtil.h"
#include "TFile.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;
class PndSttTrack;
class PndSttHit;
class PndSttPoint;

class CbmGeaneTrT : public CbmTask
{
 public:
  /** Default constructor **/  
  CbmGeaneTrT();

  /** Destructor **/
  ~CbmGeaneTrT();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  // convert coordinates from SD to LAB
  Bool_t CoordSDToMARS(TVector3 o, TVector3 y, TVector3 z, TMatrixT<double> coor, TVector3 &coordinate);

 private:
  /** Input array of Points **/
  TClonesArray* fHitArray;
  TClonesArray* fPointArray;
  TClonesArray* fTrackArray;

  TTree *t;
  TFile *f;
  /** Output array of Hits **/
  TClonesArray* fTrackParIni;  
  TClonesArray* fTrackParGeane;  
  TClonesArray* fTrackParFinal;  
  TClonesArray* fTrackParMC;  

  TGeant3 *gMC3;
  ClassDef(CbmGeaneTrT,1);
  Int_t fEvent;
  CbmGeanePro *fPro;
  CbmGeaneUtil *fUtil;
};

#endif
