// -------------------------------------------------------------------------
// -----                PndGasGenerator header file                    -----
// -----               Created 02/04/09  by F. Uhlig                   -----
// -------------------------------------------------------------------------

/**  PndGasGenerator.h 
 *@author Florian Uhlig <f.uhlig@gsi.de>
 *
 * The PndGasGenerator generates particles within given range
 * over pt, phi, eta, y or theta a fixed multiplicity per event.
 * The Vertex is calculated in the following way. X- and Y-positions
 * are randomized in a circle of radius R (beamprofile).
 * The Z-position is randomized according a function describing the
 * gas density in the beampipe around the gas target. 
 * Is it possible to have flat distribution on theta or on cos(theta)
 * Derived from FairGenerator 
 * Original file (first import): FairBoxGenerator.h 
 * author Yuri Kharlov <Yuri.Kharlov@cern.ch>
**/

#ifndef PND_GASGENERATOR_H
#define PND_GASGENERATOR_H

#include "FairGenerator.h"
#include <iostream>

class FairPrimaryGenerator;
class TF1;

class PndGasGenerator : public FairGenerator
{
public:

  /** Default constructor. **/
  PndGasGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  PndGasGenerator(Int_t pdgid, Int_t mult=1);


  /** Destructor **/
  virtual ~PndGasGenerator() {};

  /** Modifiers **/
  void SetPDGType      (Int_t pdg)  {fPDGType = pdg;  };

  void SetMultiplicity (Int_t mult) {fMult    = mult; };

  void SetPRange(Double32_t pmin=0   , Double32_t pmax=10)
    {fPMin=pmin; fPMax=pmax; fPRangeIsSet=kTRUE;}

  void SetPhiRange     (Double32_t phimin=0  , Double32_t phimax=360)
    {fPhiMin=phimin; fPhiMax=phimax;};

  void SetThetaRange   (Double32_t thetamin=0, Double32_t thetamax=90)
    {fThetaMin=thetamin; fThetaMax=thetamax; fThetaRangeIsSet=kTRUE;};

  void SetCosTheta   ()
    {fCosThetaIsSet=kTRUE;};

  void SetZRange  (Double32_t zmin=0, Double32_t zmax=10) 
    {fZMin=zmin; fZMax=zmax;}

  void SetRadius (Double32_t radius=0.2) {fRadius=radius;}

  //  void SetDensityProfile(TString in)

  void SetDebug(Bool_t debug=0) {fDebug = debug;}

  /** Initializer **/
  Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t      fPDGType;             // Particle type (PDG encoding)
  Int_t      fMult;                // Multiplicity

  Double32_t fPDGMass;             // Particle mass [GeV]
  Double32_t fPhiMin, fPhiMax;     // Azimuth angle range [degree]
  Double32_t fPMin,   fPMax;       // Momentum range in lab system
  Double32_t fThetaMin, fThetaMax; // Polar angle range in lab system [degree]
  Double32_t fX, fY, fZ;           // Point vertex coordinates [cm]
  Double32_t fRadius;               // Radius of beam profile

  Bool_t     fThetaRangeIsSet;     // True if theta range is set
  Bool_t     fCosThetaIsSet;       // True if uniform distribution in cos(theta) is set (default -> uniform theta)
  Bool_t     fPRangeIsSet;         // True if abs.momentum range is set
  Double32_t fZMin,   fZMax;       // Minimum and maximum z position
  Bool_t     fDebug;               // Debug switch

  //  TString    fProfileFunction;      // function to generate density profile
  TF1*       fDensityProfile;      // density profile
  
  ClassDef(PndGasGenerator,1);

};


#endif
