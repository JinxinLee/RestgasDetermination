// -------------------------------------------------------------------------
// -----                     PndDsk header file                        -----
// -----                Created 23/10/07  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSK_H
#define PNDDSK_H

#include "TLorentzVector.h"
#include "TVector3.h"

#include "FairDetector.h"

class TClonesArray;
class PndDskCerenkov;
class PndDskParticle;


class PndDsk : public FairDetector
{

 public:
 
  /** Default constructor **/
  PndDsk();

  /** Standard constructor with arguments
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndDsk(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndDsk();

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for DIRC detector and mirror.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Decides if actions for a Cerenkov or a Particle have to be taken
   ** and calls the ProcessHitsX method accordingly.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);

  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  /** Virtual method Register
   **
   ** Registers the collections in the ROOT manager.
   **/
  virtual void Register();

  /** Virtual method GetCollection
   **
   ** Accessor to the Cerenkov and Particle collections
   **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;    

  /** Virtual method Reset
   **
   ** Clears the collections
   **/
  virtual void Reset();

  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);

  /** Virtual method ConstructGeometry
   **
   ** Decide how to construct the geometry
   **/
  virtual void ConstructGeometry(); 

  /** Virtual method CheckIfSensitive
   **
   ** Decide if volume is sensitive
   *@param name  Name of the volume to check
   **/
  virtual Bool_t CheckIfSensitive(std::string name);

  /** Method AddCerenkov
   **
   ** Adds a Cerenkov to the collection
   **/
  PndDskCerenkov* AddCerenkov(Int_t trackID, Int_t detectorID, TVector3 position,
          TVector3 momentum, Double_t time, Double_t energy, Double_t wavelength,
          Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName);

  /** Method AddParticle
   **
   ** Adds a Particle to the collection
   **/
  PndDskParticle* AddParticle(Int_t trackID, Int_t detectorID,
          TVector3 position, TVector3 momentum, Double_t time,
          Int_t pdgCode, TString pdgName, Double_t energy,
          Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName);

  /** Modifiers **/
  void SetDebugLevel(Int_t debugLevel) { fDebugLevel = debugLevel; };
  void SetDetectors(Int_t detectorTypes, Int_t detectorsPerArray, Bool_t usingMirrors);
  void SetPDE(Double_t pde) { fPDE = pde; };
  void SetStoreCerenkovs(Bool_t storeCerenkovs) { fStoreCerenkovs = storeCerenkovs; };
  void SetStoreParticles(Bool_t storeParticles) { fStoreParticles = storeParticles; };
  void SetDskVersion(TString geoVersion) { fGeoVersion = geoVersion; };


 private:

  /** Private method CheckIfSensitiveTOP
   **
   ** Decide if volume is sensitive in the TOP-design
   *@param name  Name of the volume to check
   **/
  Bool_t CheckIfSensitiveTOP(std::string name);

  /** Private method CheckIfSensitiveLG
   **
   ** Decide if volume is sensitive in the LG-design
   *@param name  Name of the volume to check
   **/
  Bool_t CheckIfSensitiveLG(std::string name) {; };

  /** Private method ProcessHitsCerenkovTOP
   **
   ** Defines the action to be taken in the TOP design when
   ** Cerenkov does a step inside the active volume.
   ** Creates and adds Cerenkovs to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsCerenkovTOP(FairVolume* vol = 0);

  /** Private method ProcessHitsCerenkovLG
   **
   ** Defines the action to be taken in the LG design when
   ** Cerenkov does a step inside the active volume.
   ** Creates and adds Cerenkovs to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsCerenkovLG(FairVolume* vol = 0) {; };

  /** Private method ProcessHitsParticleTOP
   **
   ** Defines the action to be taken in the TOP design when
   ** Particle does a step inside the active volume.
   ** Creates and adds Particles to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsParticleTOP(FairVolume* vol = 0);

  /** Private method ProcessHitsParticleLG
   **
   ** Defines the action to be taken in the LG design when
   ** Particle does a step inside the active volume.
   ** Creates and adds Particles to collection
   *@param vol  Pointer to the active volume
   **/
  Bool_t ProcessHitsParticleLG(FairVolume* vol = 0) {; };


  TString       fGeoVersion;              //! Design to be used (TOP, LG)
  Int_t         fDebugLevel;              //! Debug level

  TClonesArray* fDskCerenkovCollection;   //! Cerenkov collection
  TClonesArray* fDskParticleCollection;   //! Particle collection

  Bool_t        fStoreCerenkovs;          //! Whether to store Cerenkovs (default) or not
  Bool_t        fStoreParticles;          //! Whether to store Particles (default) or not

  Int_t         fDetectorID;              //! Detector ID (volume) where Cerenkov was created
  Double_t      fEnergy;                  //! Energy [eV / GeV]
  TVector3      fMomentum;                //! Momentum when emitted
  Double_t      fPDE;                     //! Photon Detection Efficiency [0-1, 2]
  Int_t         fPdgCode;                 //! PDG code of current particle
  TVector3      fPosition;                //! Position of first appearance [cm]
  Double_t      fTime;                    //! Global time
  Int_t         fTrackID;                 //! The particle-track ID
  Double_t      fWavelength;              //! Vacuum wavelength hc/fEnergy [nm]

  Int_t         fMotherTrackID;           //! Track ID of the particle that emitted this Cerenkov
  Int_t         fMotherPdgCode;           //! PDG code of the particle that emitted this Cerenkov
  TString       fMotherPdgName;           //! translation of PDG code

  UShort_t      fDetType;                 //! Detectortype that registered the Cerenkov
  Int_t         fDetNumber;               //! Number of the Detector that registered the Cerenkov
  Double_t      fDetTime;                 //! Global time when Cerenkov was detected [ns]
  TVector3      fDetMomentum;             //! Momentum when detected [eV]

  Double_t      fPrimaryAngleToCerenkov;  //! Angle at the moment of first appearance
  Double_t      fPrimaryHitAngle;         //! Angle to the z-Axis at first appearance
  TVector3      fPrimaryHitMomentum;      //! Angle between momentum of eachs first appearance

  TString       fPdgName;                 //! PDG name according to fPdgCode
  TVector3      fEndPosition;             //! Position when particles disappears
  Double_t      fEndTime;                 //! Time when particle disappears
  TVector3      fEndMomentum;             //! Momentum when particle disappears
  Double_t      fEndEnergy;               //! Energy when particle disappears

  Int_t         fDetectorsPerArray;       //! total number of detectors per array
  Int_t         fDetectorTypes;           //! number of detector types
  Bool_t        fUsingMirrors;            //! whether to use mirrors or not

  TLorentzVector tmpLVec;                 //! often needed, avoid allocation

  ClassDef(PndDsk,1)

};

#endif // PNDDSK_H
