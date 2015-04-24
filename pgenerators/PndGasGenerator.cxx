// -------------------------------------------------------------------------
// -----            PndGasGenerator source file                        -----
// -----           Created 02/04/09  by F. Uhlig                       -----
// -------------------------------------------------------------------------


#include "PndGasGenerator.h"

#include "FairPrimaryGenerator.h"

#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

// ------------------------------------------------------------------------
PndGasGenerator::PndGasGenerator() :
  fPDGType(0),fMult(0),fPDGMass(0),
  fPhiMin(0),fPhiMax(0),
  fPMin(0),fPMax(0),fThetaMin(0),fThetaMax(0),fX(0),fY(0),fZ(0),
  fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPRangeIsSet(0),
  fDensityProfile(NULL), fZMin(0.), fZMax(10.)
{
  // Default constructor
}

// ------------------------------------------------------------------------
PndGasGenerator::PndGasGenerator(Int_t pdgid, Int_t mult) :
  fPDGType(pdgid),fMult(mult),fPDGMass(0),
  fPMin(0),fPMax(0),fX(0),fY(0),fZ(0),
  fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPRangeIsSet(0),
  fDensityProfile(NULL), fZMin(0.), fZMax(10.)
{
  // Constructor. Set default kinematics limits
  SetPhiRange  ();
}

// ------------------------------------------------------------------------
Bool_t PndGasGenerator::Init()
{
  // Initialize generator

  if (fPhiMax-fPhiMin>360)
    Fatal("Init()","PndGasGenerator: phi range is too wide: %f<phi<%f",
	  fPhiMin,fPhiMax);

  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("PndGasGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();

  // create the density profile histogram used later to get the random 
  // z position
  fDensityProfile = new TF1("Density Profile","exp(-0.5*((x)/4)**2)",fZMin,fZMax);  
  return kTRUE;
}

// ------------------------------------------------------------------------
Bool_t PndGasGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in cos(theta)
  
  Double32_t pabs, phi, pt, theta=0, eta, y, mt, px, py, pz, pinv=0;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {

    phi = gRandom->Uniform(fPhiMin,fPhiMax) * TMath::DegToRad();

    pabs = gRandom->Uniform(fPMin,fPMax);
   
    
    if (fCosThetaIsSet) {
      theta = acos(gRandom->Uniform(cos(fThetaMin* TMath::DegToRad()),cos(fThetaMax* TMath::DegToRad())));
    } else {
      theta = gRandom->Uniform(fThetaMin,fThetaMax) * TMath::DegToRad();
    }

    

    pz = pabs*TMath::Cos(theta);
    pt = pabs*TMath::Sin(theta);

    px = pt*TMath::Cos(phi);
    py = pt*TMath::Sin(phi);

    // Random 2D point in a circle of radius r (simple beamprofile)
    gRandom->Circle(fX, fY, fRadius);

    // calculate fZ according to some (probability) density function of the gas
    fZ=fDensityProfile->GetRandom(); 

    if (fDebug)
      printf("GasGen: kf=%d, p=(%.2f, %.2f, %.2f) GeV, x=(%.1f, %.1f, %.1f) cm\n",
	     fPDGType, px, py, pz, fX, fY, fZ);


  
    primGen->AddTrack(fPDGType, px, py, pz, fX, fY, fZ);
     


  }
  return kTRUE;

}
// ------------------------------------------------------------------------


ClassImp(PndGasGenerator)
