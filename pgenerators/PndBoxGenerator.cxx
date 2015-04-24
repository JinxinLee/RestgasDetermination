// -------------------------------------------------------------------------
// -----            PndBoxGenerator source file                        -----
// -----          Created 22/05/07  by S.Spataro                       -----
// -------------------------------------------------------------------------


#include "TRandom.h"
#include "TMath.h"
#include "PndBoxGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

// ------------------------------------------------------------------------
PndBoxGenerator::PndBoxGenerator() :
  fPDGType(0),fMult(0),fPDGMass(0),fPtMin(0),fPtMax(0),
  fPhiMin(0),fPhiMax(0),fEtaMin(0),fEtaMax(0),fYMin(0),fYMax(0),
  fPMin(0),fPMax(0),fThetaMin(0),fThetaMax(0),fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fEtaRangeIsSet(0),fYRangeIsSet(0),fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPtRangeIsSet(0),fPRangeIsSet(0),fInversePIsSet(0)
{
  // Default constructor
}

// ------------------------------------------------------------------------
PndBoxGenerator::PndBoxGenerator(Int_t pdgid, Int_t mult) :
  fPDGType(pdgid),fMult(mult),fPDGMass(0),
  fEtaMin(0),fEtaMax(0),fYMin(0),fYMax(0),
  fPMin(0),fPMax(0),fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fEtaRangeIsSet(0),fYRangeIsSet(0),fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPtRangeIsSet(0),fPRangeIsSet(0),fInversePIsSet(0)

{
  // Constructor. Set default kinematics limits
  SetPhiRange  ();
}
// ------------------------------------------------------------------------
Bool_t PndBoxGenerator::Init()
{
  // Initialize generator

  if (fPhiMax-fPhiMin>360)
    Fatal("Init()","PndBoxGenerator: phi range is too wide: %f<phi<%f",
	  fPhiMin,fPhiMax);
  if (fPRangeIsSet && fPtRangeIsSet)
    Fatal("Init()","PndBoxGenerator: Cannot set P and Pt ranges simultaneously");
  if (fPRangeIsSet && fYRangeIsSet)
    Fatal("Init()","PndBoxGenerator: Cannot set P and Y ranges simultaneously");
  if ( (fThetaRangeIsSet && fYRangeIsSet) ||
       (fThetaRangeIsSet && fEtaRangeIsSet) ||
       (fYRangeIsSet     && fEtaRangeIsSet) )
    Fatal("Init()","PndBoxGenerator: Cannot set Y, Theta or Eta ranges simultaneously");
  if (fPointVtxIsSet && fBoxVtxIsSet)
    Fatal("Init()","PndBoxGenerator: Cannot set point and box vertices simultaneously");
  if (fInversePIsSet && ( ( fPRangeIsSet && ( fPMin==0 || fPMax==0 ) ) || ( fPRangeIsSet && ( fPtMin==0 || fPtMax==0 ) ) ) )
    Fatal("Init()","PndBoxGenerator: Cannot use P == 0 as limit for inverse momentum distribution");
  
  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("PndBoxGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  return kTRUE;
}

// ------------------------------------------------------------------------
Bool_t PndBoxGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in cos(theta)
  
  Double32_t pabs, phi, pt, theta=0, eta, y, mt, px, py, pz, pinv=0;

  // Generate particles
  for (Int_t k = 0; k < fMult; k++) {
    phi = gRandom->Uniform(fPhiMin,fPhiMax) * TMath::DegToRad();

    if      (fPRangeIsSet ) {
      if (fInversePIsSet)
	{
	  pinv = gRandom->Uniform(1./fPMax, 1./fPMin);
	  pabs = 1./pinv;
	}
      else
	{
	  pabs = gRandom->Uniform(fPMin,fPMax);
	}
    }
    else if (fPtRangeIsSet) {
      if (fInversePIsSet)
	{
	  pinv = gRandom->Uniform(1./fPtMax, 1./fPtMin);
	  pt = 1./pinv;
	}
      else
	{
	  pt   = gRandom->Uniform(fPtMin,fPtMax);
	}
    }
    
    if      (fThetaRangeIsSet) {
      if (fCosThetaIsSet)
	theta = acos(gRandom->Uniform(cos(fThetaMin* TMath::DegToRad()),cos(fThetaMax* TMath::DegToRad())));
      else
	theta = gRandom->Uniform(fThetaMin,fThetaMax) * TMath::DegToRad();
    }
    else if (fEtaRangeIsSet) {
      eta   = gRandom->Uniform(fEtaMin,fEtaMax);
      theta = 2*TMath::ATan(TMath::Exp(-eta));
    }
    else if (fYRangeIsSet) {
      y     = gRandom->Uniform(fYMin,fYMax);
      mt = TMath::Sqrt(fPDGMass*fPDGMass + pt*pt);
      pz = mt * TMath::SinH(y);
    }
    
    if (fThetaRangeIsSet || fEtaRangeIsSet) {
      if      (fPRangeIsSet ) {
	pz = pabs*TMath::Cos(theta);
	pt = pabs*TMath::Sin(theta);
      }
      else if (fPtRangeIsSet) 
	pz = pt/TMath::Tan(theta);
    }

    px = pt*TMath::Cos(phi);
    py = pt*TMath::Sin(phi);

    if (fBoxVtxIsSet) {
      fX = gRandom->Uniform(fX1,fX2);
      fY = gRandom->Uniform(fY1,fY2);
    }

    if (fDebug)
      printf("BoxGen: kf=%d, p=(%.2f, %.2f, %.2f) GeV, x=(%.1f, %.1f, %.1f) cm\n",
	     fPDGType, px, py, pz, fX, fY, fZ);

    primGen->AddTrack(fPDGType, px, py, pz, fX, fY, fZ);
  }
  return kTRUE;

}
// ------------------------------------------------------------------------


ClassImp(PndBoxGenerator)
