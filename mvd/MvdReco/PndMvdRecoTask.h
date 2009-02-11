// -------------------------------------------------------------------------
// -----                 PndMvdRecoTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndMvdRecoTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndMvdHit by using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDMVDRECOTASK_H
#define PNDMVDRECOTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "FairTask.h"

// PndMvd includes
#include "PndMvdMCPoint.h"

class TClonesArray;
// class PndMvdHitMaker;

class PndMvdRecoTask : public FairTask
{
 public:

  /** Default constructor **/
  PndMvdRecoTask();

//   PndMvdRecoTask(Double_t sx, Double_t sy, Double_t sz);
  PndMvdRecoTask(std::string type);


  /** Destructor **/
  virtual ~PndMvdRecoTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:
//   PndMvdHitMaker*      fHitMaker;

  Int_t fevent;

  TString fBranchName;

  /** Input array of Points **/
  TClonesArray* fDigiPixelArray;
  TClonesArray* fDigiStripArray;
//   TClonesArray* fMctruthArray;

  /** Output array of Hits **/
  TClonesArray* fHitOutputArray;
  /** Properties **/
  std::string fMakerType;
  Double_t fSigmaX;       // Variance vector of sensor smearing
  Double_t fSigmaY;       // Variance vector of sensor smearing
  Double_t fSigmaZ;       // Variance vector of sensor smearing
  PndMvdMCPoint* fCurrentPndMvdMCPoint;
  TGeoHMatrix* fCurrentTransMat;
  TMatrixT<Double_t> fHitCovMatrix;
  /** smearing and geometry access **/
  void InitTransMat();
  void smear(TVector3& pos);
  void smearLocal(TVector3& pos);
  void CalcDetPlane(TVector3& o,TVector3& u,TVector3& v);


  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndMvdRecoTask,1);

};

#endif
