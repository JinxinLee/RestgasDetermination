// -------------------------------------------------------------------------
// -----                 PndMvdIdealRecoTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndMvdIdealRecoTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndMvdHit by using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDMVDIDEALRECOTASK_H
#define PNDMVDIDEALRECOTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "CbmTask.h"

// PndMvd includes
#include "PndMvdMCPoint.h"
#include "PndMvdGeoHandling.h"

class TClonesArray;

class PndMvdIdealRecoTask : public CbmTask
{
 public:

  /** Default constructor **/
  PndMvdIdealRecoTask();

  PndMvdIdealRecoTask(Double_t sx, Double_t sy, Double_t sz);


  /** Destructor **/
  virtual ~PndMvdIdealRecoTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 private:


  TString fBranchName;
  PndMvdGeoHandling* fGeoH;
  /** Input array of Points **/
  TClonesArray* fPointArray;
  TClonesArray* fMctruthArray;

  /** Output array of Hits **/
  TClonesArray* fHitOutputArray;
  /** Properties **/
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

  ClassDef(PndMvdIdealRecoTask,1);

};

#endif
