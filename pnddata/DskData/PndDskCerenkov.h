// -------------------------------------------------------------------------
// -----                 PndDskCerenkov header file                    -----
// -----                Created 21/03/08  by P. Koch                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSKCERENKOV_H
#define PNDDSKCERENKOV_H

#include "FairMCPoint.h"

class PndDskCerenkov : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndDskCerenkov();

  /** Standard constructor with arguments
   *@param trackID         Index of MCTrack
   *@param detectorID      Detector ID where the first hit is produced
   *@param position        Position of Cerenkov when created [cm]
   *@param momentum        Momentum of Cerenkov when created  [eV]
   *@param time            Time since event start when created [ns]
   *@param energy          Energy of the Cerenkov when created [eV]
   *@param wavelength      Vacuum wavelength hc/energy [nm]
   *@param motherTrackID   Track ID of particle that emitted the Cerenkov
   *@param motherPdgCode   PDG Code of particle that emitted the Cerenkov
   *@param motherPdgName   PDG Name of particle that emitted the Cerenkov
   **/
  PndDskCerenkov(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum,
          Double_t time, Double_t energy, Double_t wavelength,
          Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName);

  /** Copy constructor **/
  PndDskCerenkov(const PndDskCerenkov& cerenkov) { *this = cerenkov; };

  /** Destructor **/
  virtual ~PndDskCerenkov();

  /** Virtual method Print
   **
   ** Screen output of Cerenkov
   **/
  virtual void Print(const Option_t* opt) const;

  /** Accessors **/
  Double_t GetEnergy()        const { return fEnergy; };
  Double_t GetWavelength()    const { return fWavelength; };
  Int_t    GetMotherTrackID() const { return fMotherTrackID; };
  Int_t    GetMotherPdgCode() const { return fMotherPdgCode; };
  TString  GetMotherPdgName() const { return fMotherPdgName; };
  Int_t    GetDetNumber()     const { return fDetNumber; };
  UShort_t GetDetType()       const { return fDetType; };
  Double_t GetDetTime()       const { return fDetTime; };
  TVector3 GetDetMomentum()   const { return fDetMomentum; };
  Double_t GetPrimaryHitAngle()        const { return fPrimaryHitAngle; };
  Double_t GetPrimaryAngleToCerenkov() const { return fPrimaryAngleToCerenkov; };
// This Time Of Flight would be without primarys flighttime within the disk
//   Double_t GetTof()            const { return fDetTime - fTime; };
  Double_t GetNofReflections() const { return fNofReflections; };

  /** Modifiers **/
  void AddReflection() { fNofReflections++; };
  void SetFinalValues(Int_t detNumber, UShort_t detType,
               Double_t detTime, TVector3 detMomentum,
               Double_t primaryHitAngle, Double_t primaryAngleToCerenkov);

 
 protected:

  Double_t fEnergy;             ///< Energy of Cerenkov when created [eV]
  Double_t fWavelength;         ///< Vacuum wavelength hc/energy [nm]
  Double_t fPWay;               //! Projected Way [cm]

  Int_t    fMotherTrackID;      ///< Track ID of particle that emitted the Cerenkov
  Int_t    fMotherPdgCode;      ///< PDG Code of particle that emitted the Cerenkov
  TString  fMotherPdgName;      ///< PDG Name of particle that emitted the Cerenkov

  Int_t    fDetNumber;          ///< Detector number that finally registered the photon.
  UShort_t fDetType;            ///< Detector type
  Double_t fDetTime;            ///< Global time when detected [ns]
  TVector3 fDetMomentum;        ///< Momentum when detected [eV]

  Int_t    fNofReflections;     ///< Number of total reflections the Cerenkov did in the radiator

  Double_t fPrimaryHitAngle;        ///< Angle to the z-Axis when first registered
  Double_t fPrimaryAngleToCerenkov; ///< Angle between momentum of eachs first appearance

  ClassDef(PndDskCerenkov,1)
};

#endif // PNDDSKCERENKOV_H
