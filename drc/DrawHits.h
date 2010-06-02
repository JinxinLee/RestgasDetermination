// -------------------------------------------------------------------------
// -----                   DrawHits header file               -----
// -----               Created 19/10/09  by Dipanwita Dutta            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  DrawHits.h

 **
 ** Class for Analysising DRC Hits


 **/


#ifndef PNDDRCHITANA_H
#define PNDDRCAHITNA_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif


class DrawHits : public FairTask {

public:

  /** Default constructor **/
  DrawHits();

  /** Constructor with verbosity **/
  DrawHits(Int_t verbose);

   /** Destructor **/
  virtual ~DrawHits();

  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  virtual void Finish();

 /** Rotation in Bar Co-ordinate System **/
 void RotBarCoordinate(TVector3& vector, Int_t  barID);
 


 protected:
  
 private:
 void ProcessPhotonHit();
 void ProcessPhotonMC();
 void WriteToFile();
 void CreateHisto();
 void DrawHisto();
 void ProcessBarHit();

  
  Int_t fDetectorID;
  Double_t fMass;
  Double_t fThetaC, fErrThetaC;
  Int_t fRefIndex;

  TClonesArray* fBarPointArray; // DRC MC points in the bars
  TClonesArray* fPDPointArray; // DRC points in the photon detector
  TClonesArray* fHitArray; // DRC Hits in the bars
  TClonesArray* fPDHitArray; // DRC Hits in the photon detector
  TClonesArray* fMCArray; // DRC Hits in the photon detector
  TList* fHistoList;

  PndGeoDrcPar *fPar;           

  PndGeoDrc* fGeo;                 //!< Basic geometry data of barrel DRC.

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;
  
  Int_t nevents;

  TH1D*     fhThetaC;
  TH2D*     fhThetaCMass;
  TH2D*     fhMomAng;
  TH2D*     fhThetaCMomK;
  TH2D*     fhThetaCMomP;
  TH2D*     fhThetaCMomM;
  TH2D*     fhThetaCMomE;
 
  
  TH1D*     fhLambda;
  TH2D*     fhXYPDHit;
  TH1D*     fhLambdaMC;
  TH2D*     fhXYPDMCPt;
  TH1D*     fhPDTime;
  
  TH2D*     fhXYPDHitKp;
  TH2D*     fhXYPDHitKn;
  TH2D*     fhXYPDHitpip;
  TH2D*     fhXYPDHitpin;
  TH2D*     fhXYPDHitmp;
  TH2D*     fhXYPDHitmn;
  
  TH2D*     fhXYPDMCPtKp;
  TH2D*     fhXYPDMCPtKn;
  TH2D*     fhXYPDMCPtpip;
  TH2D*     fhXYPDMCPtpin;
  TH2D*     fhXYPDMCPtmp;
  TH2D*     fhXYPDMCPtmn;
ClassDef(DrawHits,1)

};

#endif
