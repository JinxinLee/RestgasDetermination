// -------------------------------------------------------------------------
// -----            PndFlatParticleGenerator source file                        -----
// -----          Created 10/30/08	by Tobias Stockmanns
// -------------------------------------------------------------------------


#include "PndFlatParticleGenerator.h"

#include "FairPrimaryGenerator.h"

#include "TRandom.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TMath.h"

// ------------------------------------------------------------------------
PndFlatParticleGenerator::PndFlatParticleGenerator() :
  fPDGType(0),fMult(0),fEvent(0), fPDGMass(0),
  fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fEtaRangeIsSet(0),fYRangeIsSet(0),fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPtRangeIsSet(0),fPRangeIsSet(0),
  fPointVtxIsSet(0),fBoxVtxIsSet(0),fDebug(0), fDoit(kTRUE)
{
	fPt.SetStart(0); fPt.SetStop(0); fPt.SetStep(0);
	fPhi.SetStart(0);fPhi.SetStop(0); fPhi.SetStep(0);
	fEta.SetStart(0);fEta.SetStop(0); fEta.SetStep(0);
	fRapidity.SetStart(0);fRapidity.SetStop(0);fRapidity.SetStep(0);
	fP.SetStart(0);fP.SetStop(0); fP.SetStep(0);
	fTheta.SetStart(0);fTheta.SetStop(0);fTheta.SetStep(0);
}

// ------------------------------------------------------------------------
PndFlatParticleGenerator::PndFlatParticleGenerator(Int_t pdgid, Int_t mult) :
  fPDGType(pdgid),fMult(mult), fEvent(0),fPDGMass(0),
  fX(0),fY(0),fZ(0),
  fX1(0),fY1(0),fX2(0),fY2(0),
  fEtaRangeIsSet(0),fYRangeIsSet(0),fThetaRangeIsSet(0),
  fCosThetaIsSet(0),fPtRangeIsSet(0),fPRangeIsSet(0),
  fPointVtxIsSet(0),fBoxVtxIsSet(0),fDebug(1), fDoit(kTRUE)
{
  // Constructor. Set default kinematics limits
	fPt.SetStart(0); fPt.SetStop(0); fPt.SetStep(0);
	fPhi.SetStart(0);fPhi.SetStop(0); fPhi.SetStep(0);
	fEta.SetStart(0);fEta.SetStop(0); fEta.SetStep(0);
	fRapidity.SetStart(0);fRapidity.SetStop(0);fRapidity.SetStep(0);
	fP.SetStart(0);fP.SetStop(0); fP.SetStep(0);
	fTheta.SetStart(0);fTheta.SetStop(0);fTheta.SetStep(0);

	SetPhiRange();
}
// ------------------------------------------------------------------------
Bool_t PndFlatParticleGenerator::Init()
{
  // Initialize generator

  if (fPhi.fStop-fPhi.fStart>360)
    Fatal("Init()","PndFlatParticleGenerator: phi range is too wide: %f<phi<%f",
	  fPhi.fStop,fPhi.fStart);
  if (fPRangeIsSet && fPtRangeIsSet)
    Fatal("Init()","PndFlatParticleGenerator: Cannot set P and Pt ranges simultaneously");
  if (fPRangeIsSet && fYRangeIsSet)
    Fatal("Init()","PndFlatParticleGenerator: Cannot set P and Y ranges simultaneously");
  if ( (fThetaRangeIsSet && fYRangeIsSet) ||
       (fThetaRangeIsSet && fEtaRangeIsSet) ||
       (fYRangeIsSet     && fEtaRangeIsSet) )
    Fatal("Init()","PndFlatParticleGenerator: Cannot set Y, Theta or Eta ranges simultaneously");
  if (fPointVtxIsSet && fBoxVtxIsSet)
    Fatal("Init()","PndFlatParticleGenerator: Cannot set point and box vertices simultaneously");

  // Check for particle type
  TDatabasePDG* pdgBase = TDatabasePDG::Instance();
  TParticlePDG *particle = pdgBase->GetParticle(fPDGType);
  if (! particle) Fatal("PndFlatParticleGenerator","PDG code %d not defined.",fPDGType);
  fPDGMass = particle->Mass();
  return kTRUE;
}

// ------------------------------------------------------------------------
Bool_t PndFlatParticleGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
  // Generate one event: produce primary particles emitted from one vertex.
  // Primary particles are distributed uniformly along
  // those kinematics variables which were limitted by setters.
  // if SetCosTheta() function is used, the distribution will be uniform in
  // cos(theta)

  Double32_t pabs=0, phi, pt=0, theta=0, eta, y, mt, px, py, pz=0;

  // Generate particles
  PndRangeValues* outerVal = &fPhi;
  PndRangeValues* midVal = 0;
  PndRangeValues* innerVal = 0;
  if (fThetaRangeIsSet){
	  if (fCosThetaIsSet){
		  midVal = &fCosTheta;
		  //std::cout << "CosTheta selected: " << midVal->fStart << " " << midVal->fStop << " " << midVal->fStep << " " << midVal->fActualValue << " " <<std::endl;
	  } else
		  midVal = &fTheta;
  }
  else if (fEtaRangeIsSet)
	  midVal = &fEta;
  else if (fYRangeIsSet)
	  midVal = &fRapidity;

  if (fPtRangeIsSet)
	  innerVal = &fPt;
  else if (fPRangeIsSet)
	  innerVal = &fP;

  CalcActValues(outerVal, midVal, innerVal);
 // std::cout << "outerVal, midVal, innerVal: " << outerVal->fActualValue << " " << midVal->fActualValue << " " << innerVal->fActualValue << std::endl;
 // std::cout << "phi, cosTheta, p: " << fPhi.fActualValue << " " << fCosTheta.fActualValue << " " << fP.fActualValue << std::endl;

  if      (fPRangeIsSet ) pabs = fP.fActualValue;
  else if (fPtRangeIsSet) pt   = fPt.fActualValue;

  phi = fPhi.fActualValue * TMath::DegToRad();


    if(fThetaRangeIsSet) {
      if (fCosThetaIsSet)
    	  theta = acos(fCosTheta.fActualValue);
      else
    	  theta = fTheta.fActualValue * TMath::DegToRad();
    }

    else if (fEtaRangeIsSet) {
      eta   = fEta.fActualValue;
      theta = 2*TMath::ATan(TMath::Exp(-eta));
    }
    else if (fYRangeIsSet) {
      y     = fRapidity.fActualValue;
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

/*    if (fBoxVtxIsSet) {
      fX = gRandom->Uniform(fX1,fX2);
      fY = gRandom->Uniform(fY1,fY2);
    }
*/
    if (fDebug)
      printf("FlatGen: kf=%d, p=(%.2f, %.2f, %.2f) GeV, x=(%.1f, %.1f, %.1f) cm\n",
	     fPDGType, px, py, pz, fX, fY, fZ);

    primGen->AddTrack(fPDGType, px, py, pz, fX, fY, fZ);
 // }
  return kTRUE;

}
// ------------------------------------------------------------------------
void PndFlatParticleGenerator::CalcActValues(PndRangeValues* val1, PndRangeValues* val2, PndRangeValues* val3)
{
	if (val1 == 0)
		val1 = new PndRangeValues();
	if (val2 == 0)
		val2 = new PndRangeValues();
	if (val3 == 0)
		val3 = new PndRangeValues();

	if (fDoit){
		val3->fActualValue += val3->fStep;
		if (val3->fActualValue > val3->fStop && fDoit){
			val3->fActualValue = val3->fStart;
			val2->fActualValue += val2->fStep;
			if (val2->fActualValue > val2->fStop && fDoit){
				val2->fActualValue = val2->fStart;
				val1->fActualValue += val1->fStep;
				if (val1->fActualValue > val1->fStop && fDoit){
					fDoit = false;
				}
			}
		}
	}
	if (fDoit) fEvent++;
	else
		std::cout << "End of range reached at EventNr: " << fEvent << std::endl;
}

ClassImp(PndFlatParticleGenerator)
