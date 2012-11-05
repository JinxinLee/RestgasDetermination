//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//      Anastasia Karavdina, Uni Mainz      (Copied for Lmd use)
//-----------------------------------------------------------

#ifndef PNDLMDKALMANTASK_H
#define PNDLMDKALMANTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "FairGeanePro.h"
#include "TMatrixD.h"
#include "PndLinTrack.h"
#include "GFTrack.h"
#include "GFFieldManager.h"
#include "PndGeoHandling.h"
// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TGeoManager;

class PndLmdKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndLmdKalmanTask();
  virtual ~PndLmdKalmanTask();

  // Operators


  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}


  // Operations ----------------------

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  // void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TClonesArray* fTrackArray;
  // TClonesArray* fGFTrackArray;
  TClonesArray* fTrackFittedArray;
  TClonesArray* fTrackParFinal;
  TClonesArray* fSdsHitsArray;
  TString fTrackBranchName;
  TString fSdsHitBranchName;
  Bool_t fPersistence;

  GFRecoHitFactory* fTheRecoHitFactory;
  PndGeoHandling* fGeoH;

  /* TH1D* fPH; // momentum histo; */
  /* TH1D* fChi2H; // chi2 histo; */
  /* TH1D* fMassV0; // chi2 histo; */
  /* TH1D* fMassETAC; // chi2 histo; */
  /* TH1D* fMasses; // all particle histo; */

  Int_t fTrackcount;

  Int_t fEvent;
  TGeant3 *gMC3;
  FairGeanePro *fPro;
  int fsensType;//0=strips sensors, 1=Pixel sensors
  Double_t fPbeam;//beam momentum
  Int_t fPDGCode;
  Double_t fCharge;
  // Private Methods -----------------
 //from LUMI frame (with z-axis perpendicular to planes) to global PANDA frame
  void combitransFromLumiFrame(TVector3& hitPos);
  void rotateFromLumiFrame(TVector3& hitPos, bool errrot);
  TMatrixD rotateFromLumiFrame(TMatrixD& hitCov);
  void rotateToLumiFrame(TVector3& hitPos);//TODO: add implemetation for pixel
  TMatrixD rotateToLumiFrame(TMatrixD& hitCov);//TODO: add implemetation for pixel
  PndLinTrack* GenfitTrack2PndLinTrack(const GFTrack* tr);
  GFFieldManager *gfMagField;
//public:
  ClassDef(PndLmdKalmanTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
