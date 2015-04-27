// -------------------------------------------------------------------------
// -----                   PndDskFLGHit header file                      -----
// -----                Created 25/04/10  by Y. Liang                   -----
// -------------------------------------------------------------------------

#ifndef PNDDSKFLGHIT_H
#define PNDDSKFLGHIT_H

#include "FairMCPoint.h"

class PndDskFLGHit : public FairMCPoint
{

 public:

  /** Default constructor **/
  PndDskFLGHit();

  /** Standard constructor with arguments
   *@param trackID        Index of MCTrack
   *@param detectorID     Detector ID where the first hit is produced
   *@param position       Position of Particle when created [cm]
   *@param momentum       Momentum of Particle when created  [eV]
   *@param time           Time since event start when created [ns]
   *@param pdgCode        Particles PDG code
   *@param pdgName        Particles PDG name according to pdgCode
   *@param energy         Energy of the Particle when created [GeV]
   *@param motherTrackID  Track ID of particle that created the Particle
   *@param motherPdgCode  PDG Code of particle that created the Particle
   *@param motherPdgName  PDG Name of particle that created the Particle
   **/
  PndDskFLGHit(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum,
          Double_t time, Int_t pdgCode, TString pdgName, Double_t energy,
          Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass,
	  Double_t angIn, Double_t thetaC,
	  TVector3 mom_cherenkov_photon, Int_t num_light_guide, Int_t num_pixel);

  PndDskFLGHit(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time,
          Double_t angIn, Double_t thetaC,
          TVector3 mom_cherenkov_photon, Int_t num_light_guide, Int_t num_pixel);

  /** Copy constructor **/
  //PndDskFLGHit(const PndDskFLGHit& particle) { *this = particle; }; // No need

  /** Destructor **/
  virtual ~PndDskFLGHit();

  /** Virtual method Print
   **
   ** Screen output of Particle
   **/
  virtual void Print(const Option_t* opt) const;

  /** Method SetFinalValues
   **
   ** Set the final values for a particle
   *@param exitPosition Position when particle disappears [cm]
   *@param exitMomentum Momentum when particle disappears [GeV]
   *@param exitTime     Time when particle disappears [ns]
   *@param exitEnergy   Energy when particle disappears [GeV]
   **/
   void SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum,
                Double_t exitTime, Double_t exitEnergy);

  /** Accessors **/
  Int_t    GetPdgCode()       const { return fPdgCode; }
  TString  GetPdgName()       const { return fPdgName; }
  Double_t GetEnergy()        const { return fEnergy; }
  Int_t    GetMotherTrackID() const { return fMotherTrackID; }
  Int_t    GetMotherPdgCode() const { return fMotherPdgCode; }
  TString  GetMotherPdgName() const { return fMotherPdgName; }
  TVector3 GetExitPosition()  const { return fExitPosition; }
  TVector3 GetExitMomentum()  const { return fExitMomentum; }
  Double_t GetExitTime()      const { return fExitTime; }
  Double_t GetExitEnergy()    const { return fExitEnergy; }
  Double_t GetMass()       const { return fMass;};

  Double_t GetAngIn()      const { return fAngIn; };
  Double_t GetThetaC()     const { return fThetaC; };
  TVector3 GetMomPhoton()  const { return fMom_cherenkov_photon; };
  Int_t GetNumLightGuide() const { return fNum_light_guide; };
  Int_t GetNumPixel()      const { return fNum_pixel; };

  /** Modifiers **/
  void SetAngIn(Double_t ang)        { fAngIn= ang; }; 
  void SetThetaC(Double_t theta)     { fThetaC = theta; };
  void SetNumLightGuide(Int_t i)     { fNum_light_guide = i; };
  void SetNumPixel(Int_t i)          { fNum_pixel = i; };
  void SetMomPhoton(TVector3 mom)    { fMom_cherenkov_photon = mom;}; 

 protected:

  Int_t    fPdgCode;           ///< PDG code of particle
  TString  fPdgName;           ///< PDG name according to fPdgCode
  Double_t fEnergy;            ///< Energy [GeV]
  Double_t fMass;               // Mass

  Int_t    fMotherTrackID;     ///< Track ID of the first mother
  Int_t    fMotherPdgCode;     ///< PDG code of the particle with fMotherTrackID
  TString  fMotherPdgName;     ///< PDG name according to fMotherPdgCode

  TVector3 fExitPosition;      ///< Position when particle disappears [cm]
  TVector3 fExitMomentum;      ///< Momentum when particle disappears [GeV]
  Double_t fExitTime;          ///< Time when particle disappears [ns]
  Double_t fExitEnergy;        ///< Energy when particle disappears [GeV]

  Double_t fThetaC;             // Cherenkov Angle
  Double_t fAngIn;                  // Rod number
  TVector3 fMom_cherenkov_photon;
  Int_t    fNum_light_guide;
  Int_t    fNum_pixel;

  ClassDef(PndDskFLGHit,1)
};

#endif // PNDDSKFLGHIT_H
