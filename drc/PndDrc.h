
// -------------------------------------------------------------------------
// -----                       PndDrc header file                      -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDDRC_H
#define PNDDRC_H


//#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "TGraph.h"
#include "TRandom3.h"

//using namespace std;

class TClonesArray;
class PndDrcPDPoint;
class PndDrcBarPoint;
class FairVolume; 
class PndGeoDrc;

class PndDrc : public FairDetector 
{

 public:

  /** Default constructor **/
  PndDrc();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndDrc(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~PndDrc();
  
  /*! \brief  Kill secondaries at its production point.
    \param ss
  */
  void StopSecondaries(Bool_t ss = kFALSE){fStopSecondaries = ss;}
    
  
  /*! \brief  Kill photons at production point according to the detector efficiency distribution.
    \param dep 
    
    NOTE! Transport efficiency can not be used together with Detector efficiency:
  it can not be used like:
  SetDetEffAtProduction(kTRUE)
  SetTransportEffAtProduction(kTRUE)
  
  at least one of them should be kFALSE
  */
  void SetDetEffAtProduction(Bool_t dep = kFALSE){fDetEffAtProduction = dep;}
  
  /*! \brief  Kill photons at production point according to the transport efficiency distribution.
  \param tra
  
  NOTE! Transport efficiency can not be used together with Detector efficiency:
  it can not be used like:
  SetDetEffAtProduction(kTRUE)
  SetTransportEffAtProduction(kTRUE)
  
  at least one of them should be kFALSE
    
  */
  void SetTransportEffAtProduction(Bool_t tra = kFALSE){fTransportEffAtProduction = tra;}
  
  /*! \brief  Choose between ideal and real mirror:
    \param mir 
    kFALSE - take ideal mirror, reflectivity = 1 for photons within energy range 1..10 eV
    kTRUE  - take real  mirror, reflectivity has realistic values for photons within energy range 1.907..6.199 eV
  */
  void SetMirrorReal(Bool_t mir = kTRUE){fTakeRealReflectivity = mir;}


  /*! \brief  Set time after which photons are killed.
    \param ti The time (ns)
  */
  void SetStopTime(Double_t ti=-1.)
  { fPhoMaxTime = ti;      
    if(fPhoMaxTime > 0.){fStopTime = kTRUE;}
    if(fPhoMaxTime <= 0.){fStopTime = kFALSE;}
  } 

  /*!  \brief No reflected photons from the side of expansion volume
    \param db Flag
    kFALSE = only reflected photons
    kTRUE = only direct photons
  */ 
  void SetOnlyDirectPho(Bool_t db=kFALSE) {fTakeDirect = db;}
  
  /*!  \brief No directo photons that go from the bar end to the PD plane of reflected from the cone part of the EV
    \param db Flag
    kFALSE = all photons
    kTRUE = only bottom reflected photons
  */ 
  void SetOnlyReflectedPho(Bool_t dref=kFALSE) {fTakeReflected = dref;}

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for DIRC detector and mirror.
   **/
     
  virtual void Initialize();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndDrcPoints and PndDrcMirrorPoints and adds 
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);


  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();


  virtual void BeginEvent();
  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();


  /** Accessor to the hit collection **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;


  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;    


  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();


  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* clPD1, TClonesArray* clPD2, TClonesArray* clBar1, TClonesArray* clBar2,  Int_t offset);


  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();
  virtual void ConstructOpGeometry();
  //  void ConstructASCIIGeometry();
  //  virtual void ConstructRootGeometry();
    std::vector<std::string> fListOfSensitives;  
    bool CheckIfSensitive(std::string name);
    
    void NumberOfBounces(TVector3, TVector3, Int_t *, Int_t *, Double_t *, Double_t *);
    Double_t FindPhiRot(Double_t, Double_t);
    Double_t FindOutPoint(Double_t, Double_t, Double_t, Double_t*, Bool_t);

  PndDrcPDPoint* AddHit(Int_t trackID, 
			Int_t copyNo, 
			TVector3 pos, 
			TVector3 mom, 
			Double_t time, 
			Double_t length, 
			Int_t pdgCode,
			Int_t eventID);

  PndDrcBarPoint* AddBarHit(Int_t trackID, 
			    Int_t copyNo, 
			    TVector3 pos, 
			    TVector3 mom, 
			    Double_t time, 
			    Double_t length,
			    Int_t pdgCode,
			    Double_t angIn,
			    Double_t thetaC,
			    Int_t nBar, 
			    Int_t eventID,
			    Double_t mass);


  void SetRunCherenkov(Bool_t ch) { fRunCherenkov = ch; };

 private:
 
  //$$$$$$$$$$$$$$$$$$$$
  Int_t nphotons;
  //$$$$$$$$$$$$$$$$$$$$

 
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
 
  Bool_t fRunCherenkov;            //!  Switch ON/OFF Cherenkov propagation
  Int_t          fTrackID;         //!  track index
  Int_t          fCopyNo;          //!  volume id
  TLorentzVector fPos;             //!  position
  TLorentzVector fMom;             //!  momentum
  Double32_t     fTime;            //!  time
  Double32_t     fLength;          //!  length 
  Double_t       fAngIn;
  Int_t          fNBar;
  Int_t          fPosIndex;                 //! 
  Int_t          volDetector;               //!  MC volume ID of drc
  Double_t       fMass;
  TLorentzVector fMom1;
  TLorentzVector fMom2; //! for transport efficiency calculation
  TLorentzVector fPos2; //! for transport efficiency calculation
  Double_t 	 fBarEnd;
  
  // from Initialisation:
  Double_t       fLambda[1000];
  Double_t       fEfficiency[1000];
  Double_t       fEfficiencyR[1000];
  Double_t 	 fLambdaMin;
  Double_t 	 fLambdaMax;
  Double_t 	 fLambdaStep;
  Double_t 	 fAngleStep;
  Int_t		 fLambdaPoints;
  
  // used in ProcessHits function:
  Int_t		 fbarID;	   //!  ID number of DrcBarSensors
  Int_t          fpdID;		   //!  ID number of DrcPdSensor
  Int_t		 flens3ID;	   //!  ID number of third lenses
  Int_t		 flens2ID;
  Int_t		 flens1ID;
  Int_t		 fbboxID;      	   //!  ID number of DrcBarBoxes
  Int_t 	 fevID;		   //!  ID number of Expansion Volume
  
  TGraph*        fDetEff;          //!  Detector Efficiency as a function of photon wavelength
  Bool_t         fDetEffAtProduction;
  Bool_t     	 fTransportEffAtProduction;
  TRandom3	 frand;
  Int_t          fLastTrackID;
  Double_t       fCollectionEff;//Collection Efficiency 
  Double_t       fPackingFraction;//Packing Efficiency 
 
  Bool_t         fStopTime;
  Double_t       fPhoMaxTime;
  Bool_t         fTakeDirect; 
  Bool_t         fTakeReflected;     
  Int_t 	 fFocusing; 
  Bool_t         fTakeRealReflectivity;
  Bool_t	 fStopSecondaries;

  PndGeoDrc*     fGeo;             //! Pointer to basic DRC geometry data

  Int_t          fPdgCode;
  Double_t       fThetaC;

  TClonesArray*  fDrcPDCollection;        //! Hit collection
  TClonesArray*  fDrcBarCollection;        //! Hit collection in the bar
  Int_t          fEventID;
  
  Int_t aaa;

  // reset all parameters   
  void ResetParameters();

  Int_t  fSenId1, fSenId2, fSenIdBar;
  ClassDef(PndDrc,9)

}; 



inline void PndDrc::ResetParameters() {
  fTrackID = -999; 
  fCopyNo = -999;
  fPos.SetXYZT(-999., -999., -999., -999.);
  fMom.SetXYZT(-999., -999., -999., -999.) ;
  fTime = -999;  
  fLength = -999;
  fPdgCode = -999; 
  fAngIn = -999;
  fThetaC = -999;
  fNBar = -999;
  fEventID = -999;
  fMass = -999;
}


#endif




