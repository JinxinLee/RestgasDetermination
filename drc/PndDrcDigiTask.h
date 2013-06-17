// -------------------------------------------------------------------------
// -----                   PndDrcHitProducer header file               -----
// -----               Created 30/10/09  by Dipanwita Dutta
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcDigiTask.h

 **
 ** Class for producing DRC hits directly from MCPoints

 **/


#ifndef PNDDRCDIGITASK_H
#define PNDDRCDIGITASK_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcDigi.h"
#include "PndStack.h"
#include "FairBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>
#include "TH1.h"
#include "TH2.h"
#include "PndGeoDrc.h"
#include "PndGeoHandling.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

#include <map>

class PndDrcDigiTask : public FairTask {

public:

  /** Default constructor **/
  PndDrcDigiTask();

  /** Constructor with verbosity **/
  PndDrcDigiTask(Int_t verbose, Int_t det_type);

   /** Destructor **/
  virtual ~PndDrcDigiTask();

  /** Initialization of the task **/  
  //  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);
  
  /** Reset eventwise counters **/
  void Reset();
  
  /** Finish task **/ 
  virtual void Finish();
  /** method AddPDHit
   **
   ** Adds a DrcDigi to the DigiCollection
   **/
  PndDrcDigi* AddDrcDigi(Int_t index, Int_t iDetectorId, Double_t charge, Double_t TimeStamp, Int_t csflag);

  /** method: FindDrcHitPosition-> finds hit position in PMT plane **/
  void FindDrcHitPosition(Double_t xPoint, Double_t yPoint, Double_t zPoint,
		                          Double_t& xHit, Double_t& yHit, Double_t& zHit, Int_t pmtID);  	  

 /**  Set Photon Detector Parameter **/

  void SetFakeDetEff(Double_t& lambda_min, Double_t& lambda_max,
                                         Double_t& lambda_step, Double_t efficiency[]);			 
  void SetPhotonDetEffNew(Double_t& lambda_min, Double_t& lambda_max,
                                         Double_t& lambda_step, Double_t efficiency[]);
					 
 /** Set Photon Transport Efficiency **/
  void SetPhotonTransportEff(Double_t&, Double_t&, Double_t&, Double_t&, Int_t& , Double_t fEfficiency[]);
 /** Auxiliraly functions **/
 void NumberOfBounces(TVector3, TVector3, Int_t *, Int_t *, Double_t *, Double_t *);
 Double_t FindPhiRot(Double_t, Double_t);
 Double_t FindOutPoint(Double_t, Double_t, Double_t, Double_t*, Bool_t);

  void SetIsDetEfficiency(Bool_t isDetEff){fisDetEff=isDetEff;}  
  void SetTransportEfficiency(Bool_t isTran = 0){fisTransportEff = isTran;}
  void SetChargeSharing(Bool_t ct = 0){fChargeSharing = ct;}
  TVector3 GetSensorDimensions(Int_t sensorID);
  
  
  //########################################
  void SetTilt(Double_t tilt = 0.){ftilt = tilt;} // degrees
  //########################################
  
  void DrawDetEfficiency(Bool_t dr=kFALSE){fDrawing = dr;}
     
 protected:
 
  PndGeoHandling* fGeoH; 	//! ///< converter for detector names 
 
 
 private:
  
 /**   Process MC Points **/
  void ProcessPhotonPoint(); 
  void Smear(Double_t& time, Double_t sigt);
  void ActivatePixel(Int_t sensorDetId, Double_t signalTime, Int_t k, Int_t csflag);
  Double_t FuncD1(Double_t x);
  Double_t FuncD3(Double_t x, Double_t y);
  
  // basic parameters of DIRC
  Double_t fpi;
  Double_t fzup;
  Double_t fzdown;
  Double_t fradius;
  Double_t fhthick;
  Double_t fpipehAngle;
  Double_t fbbGap;
  Double_t fbbnum;
  Double_t fbarnum;
  Double_t fphi0;
  Double_t fdphi;
  Double_t flside;
  Double_t fbarwidth;
  Double_t fMcpActiveArea;
  Double_t fPixelSize;
  Int_t    fNpix;  // in one column/row
  Double_t fPixelGap;
  Double_t fPixelStep;
  Double_t fPixelSigma;
  Double_t fDeadTime; //[ns]
  Double_t fThreshold; // in % of the total probability of 1 to detect a hit
  Double_t fTimeGranularity; // granularity of the time signal [ns]
 
  Bool_t fisDetEff;
  Bool_t fisPixel;
  Bool_t fisTransportEff;
  Bool_t fDrawing;
  Bool_t fChargeSharing;
  Double_t ftilt; 
  Int_t fDetectorID;
  TVector3 fPosHit;
  TVector3 fDPosHit;
  TVector3 fPosPDHit; 
  Double_t fThetaC, fErrThetaC;
  Double_t fTime;
  Int_t fRefIndex;
  Int_t fPixelID;//fPDRefIndex;
  
  Int_t             fNDigis;
  TClonesArray*     fDigis;        /** Output array of PndDrcDigi **/
  
  /** Map of active pixels 
   ** to index of PndDrcDigis **/
  std::map<Int_t, Int_t> fPixelMap; //!
    
  TClonesArray* fBarPointArray; // DRC MC points in the bars
  /** Input array of PndDrcPDPoints **/
  TClonesArray* fPDPointArray; // DRC MC points in the photon plane
  /** Output array of PndDrcDigis **/
  TClonesArray* fDrcDigiArray; // DRC digis
//  TClonesArray* fPDHitArray; // DRC Photon Detector hits
  TClonesArray* fMCArray; // DRC Hits in the photon detector
  
  TH2F* detEffLam;
 
  
  PndGeoDrcPar *fPar;           

  PndGeoDrc* fGeo;                 //!< Basic geometry data of barrel DRC.

  /** Verbosity level **/
  Int_t fVerbose;

  /** Parameters of photodetector **/
  Int_t  fDetType;            // detector type 

  Double_t nRefrac; //Refractive index of photon detector  
  Double_t fSigmat; //Time Resolution in ps
  Double_t fCollectionEff; //Collection Efficiency
  Double_t fPackingFraction; //Packing Fraction or Active Area Ratio  
  Double_t fRoughness; // Surface roughness (bars)
  Int_t fDetection;
  Int_t nevents;
  
  /** Set the parameters to the default values. **/
  void SetParameters();


  ClassDef(PndDrcDigiTask,1)

};

#endif
