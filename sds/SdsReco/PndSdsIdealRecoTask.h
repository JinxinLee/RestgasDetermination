// -------------------------------------------------------------------------
// -----                 PndSdsIdealRecoTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndSdsIdealRecoTask.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The ideal tracking produces tracks of type PndSdsHit by using MC information
 ** directly, but with the correct data interface.
 **/


#ifndef PNDSDSIDEALRECOTASK_H
#define PNDSDSIDEALRECOTASK_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"

// framework includes
#include "FairTask.h"

// PndSds includes
#include "PndSdsMCPoint.h"
#include "PndSdsGeoHandling.h"

class TClonesArray;

class PndSdsIdealRecoTask : public FairTask
{
 public:

  /** Default constructor **/
  PndSdsIdealRecoTask();

  PndSdsIdealRecoTask(Double_t sx, Double_t sy, Double_t sz);


  /** Destructor **/
  virtual ~PndSdsIdealRecoTask();

   /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
   virtual void SetBranchNames()=0;


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);


 protected:


  TString fBranchName;
  PndSdsGeoHandling* fGeoH;

  /** Input array of Points **/
  TClonesArray* fPointArray;
  TClonesArray* fMctruthArray;

  TString fHitBranchName;
  TString fFolderName;
  /** Output array of Hits **/
  TClonesArray* fHitOutputArray;
  /** Properties **/
  Double_t fSigmaX;       // Variance vector of sensor smearing
  Double_t fSigmaY;       // Variance vector of sensor smearing
  Double_t fSigmaZ;       // Variance vector of sensor smearing
  PndSdsMCPoint* fCurrentPndSdsMCPoint;
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

  ClassDef(PndSdsIdealRecoTask,1);

};

#endif
