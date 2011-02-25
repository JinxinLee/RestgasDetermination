
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
  
  
  /*! \brief  Kill photons at production point according to the detector efficiency distribution.
    \param dep 
  */
  void SetDetEffAtProduction(Bool_t dep = kFALSE){fDetEffAtProduction = dep;}


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
  void SetOnlyDirectPho(Bool_t db=kTRUE) {fTakeDirect = db;}
    
  /*!  \brief Set Prizm:
    \param pr (
    kTRUE = there is a prizm
    kFALSE = there is no prizm
    )    
  */
  void SetPrizm(Bool_t pr = kFALSE){fprizm = pr;}
  
  /*!  \brief Set Focusing System in case of no prism !!:
    \param fo (
    0 = no focusing
    1 = lens
    2 = forward mirror
    )    
  */ 
  void SetFocusingSystem(Int_t fo = 0){
    if(fprizm == kTRUE){
      std::cout<<"SetPrism should be kFALSE!!!"<<std::cout;
    }
    if(fprizm == kFALSE){
      fFocusingSystem = fo;
    }
  }  
  
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
  //  virtual void ConstructOpGeometry();
  //  void ConstructASCIIGeometry();
  //  virtual void ConstructRootGeometry();
    std::vector<std::string> fListOfSensitives;  
    bool CheckIfSensitive(std::string name);

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
  
  // used in ProcessHits function:
  Int_t		 fbarID;	   //!  ID number of DrcBarSensors
  Int_t          fpdID;		   //!  ID number of DrcPdSensor
  Int_t		 flensID;	   //!  ID number of outer lenses
  Int_t		 fbboxID;      	   //!  ID number of DrcBarBoxes
  
  TGraph*        fDetEff;          //!  Detector Efficiency as a function of photon wavelength
  Bool_t         fDetEffAtProduction;
  TRandom3	 frand;
  Int_t          fLastTrackID;
  Double_t       fCollectionEff;//Collection Efficiency 
  Double_t       fPackingFraction;//Packing Efficiency 
  
  Double_t       fSlabEnd;   //!< Slab end defined in Construction and used in ProcessHits
  
  Bool_t         fStopTime;
  Double_t       fPhoMaxTime;
  Bool_t         fTakeDirect;  
  TString        fAtBarEnd;
  Bool_t         fprizm;
  Int_t 	 fFocusingSystem; 
  
  // lens shift:
  Double_t       fdz_lens3;
  Double_t       fdz_lens2;
  Double_t       fdz_lens1;
  Double_t	 fdz_mirr1;
  Double_t	 fdz_mirr2;

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
  ClassDef(PndDrc,4)

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




