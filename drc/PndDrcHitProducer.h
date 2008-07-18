// -------------------------------------------------------------------------
// -----                   PndDrcHitProducer header file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------

/**  PndDrcHitProducer.h

 **
 ** Class for producing DRC hits directly from MCPoints

 **/


#ifndef PNDDRCHITPRODUCER_H
#define PNDDRCHITPRODUCER_H

#include "CbmTask.h"
#include "TClonesArray.h"
#include "CbmMCTrack.h"
#include "PndDrcHit.h"
#include "CbmStack.h"
#include "CbmBaseParSet.h"
#include "PndGeoDrcPar.h"
#include "TString.h"
#include <string>

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif


class PndDrcHitProducer : public CbmTask {

public:

  /** Default constructor **/
  PndDrcHitProducer();

  /** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   */

  /** Standard constructor
   *@param pmt_rad      radius of photomultiplier [cm]
   *@param pmt_dis      distance between PMT tubel [cm]
   *@param det_type     detector type (choose: 1=Protvino, 2=CsI, 3=Hamamatsu
   *@param noise        number of noise hits
   *@param verbose      verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   */
  PndDrcHitProducer(Int_t verbose);

  /** Constructor with name and title. Puts default parameter values. */
  //  PndDrcHitProducer(const char *name, const char *title);

  /** Destructor **/
  virtual ~PndDrcHitProducer();

  /** Initialization of the task **/  
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Executed task **/ 
  virtual void Exec(Option_t* option);

  /** Finish task **/ 
  //  virtual void Finish();

  /** method AddHit
   **
   ** Adds a DrcHit to the HitCollection
   **/
  PndDrcHit* AddHit(Int_t trackID, 
		    Int_t copyNo, 
		    TVector3 posHit, 
		    TVector3 momHit, 
		    Double_t time, 
		    Double_t eLoss,
		    Double_t mass,
		    Int_t pdgCode,
		    Int_t nPhotons,
		    Int_t eventID);

  
 protected:
  /*
    void FindHitPositionSinglePMT(Double_t xPoint, Double_t yPoint,
    Double_t& xHit, Double_t& yHit, Int_t & pmtID);
    void FindHitPositionMAPMT(Double_t xPoint, Double_t yPoint,
			Double_t& xHit, Double_t& yHit, Int_t & pmtID);
			void FindHitPositionCsI(Double_t xPoint, Double_t yPoint,
			Double_t& xHit, Double_t& yHit, Int_t & pmtID);
			
  Double_t OnePhotonAmplitude(Double_t x);
  Double_t GetAmplitude();

  Double_t fPhotomulRadius;   // radius of photomultiplier 
  Double_t fPhotomulDist;     // radius of photomultiplier 
  Int_t  fDetType;            // detector type 
  Int_t  fNoise;              // number of noise hits 

  void SetPhotoDetPar(Int_t det_type, Double_t& lambda_min, Double_t& lambda_max, Double_t& lambda_step, Double_t efficiency[]);
*/
private:

  TVector3 fPosHit;
  Double_t fELoss;
  TVector3 fMomHit;
  Double_t fMass;
  Double_t fThetaC;
  Int_t fPdgCode;
  Int_t fTrackID;

  TClonesArray* fPointArray; // DRC MC points
  TClonesArray* fHitArray; // DRC hits
  TObjArray *fVolumeArray;
  TClonesArray* fListStack;     // Tracks
  

  Double_t c;         // speed of light
  Double_t h;         // Planck constant
  Double_t e;         // elementary charge
  Double_t nrefrac;   // refraction index
  Int_t detection;    // flag for detection
  Int_t nevents;

  Int_t fNHits;
  Int_t fNDoubleHits;
  Double_t fDetX;       /** X-coordinate of photodetector */
  Double_t fDetY;       /** Y-coordinate of photodetector */
  Double_t fDetZ;       /** Z-coordinate of photodetector */
  Double_t fDetWidthX;  /** width of photodetector in x */
  Double_t fDetWidthY;  /** width of photodetector in y */


  TObjArray *fSensNodes;
  TObjArray *fPassNodes;
  PndGeoDrcPar *fPar;           //!

  /** Set the parameters to the default values. **/
  void SetDefaultParameters();

  /** Verbosity level **/
  Int_t fVerbose;
  

  ClassDef(PndDrcHitProducer,1)

};

#endif
