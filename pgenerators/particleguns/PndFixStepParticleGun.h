// -------------------------------------------------------------------------
// -----                PndFixStepParticleGun header file                    -----
// -----          Created 30/10/08  by Tobias Stockmanns                      -----
// -------------------------------------------------------------------------

/**  PndFixStepParticleGun.h
*/

#ifndef Pnd_FLATPARTICLEGENERATOR_H
#define Pnd_FLATPARTICLEGENERATOR_H

#include "FairGenerator.h"

#include <iostream>

class FairPrimaryGenerator;

class PndRangeValues
{
public :
	PndRangeValues(Double32_t start = 0, Double32_t stop = 0, Double32_t step = 1): fStart(start), fStop(stop), fStep(step), fActualValue(start)
	{}
  virtual ~PndRangeValues(){};
	Double32_t fStart;
	Double32_t fStop;
	Double32_t fStep;
	Double32_t fActualValue;

	void SetStart(Double32_t val){fStart = val; fActualValue = val;};
	void SetStop(Double32_t val){fStop = val;};
	void SetStep(Double32_t val){fStep = val;};
	void SetActualValue(Double32_t val){fActualValue = val;};

	virtual Double32_t GetStep(){return fStep;}

	ClassDef(PndRangeValues,1);
};


class PndFixStepParticleGun : public FairGenerator
{
public:

  /** Default constructor. **/
  PndFixStepParticleGun();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  PndFixStepParticleGun(Int_t pdgid, Int_t mult=1);

  /** Destructor **/
  virtual ~PndFixStepParticleGun() {};

  /** Modifiers **/
  void SetPDGType      (Int_t pdg)  {fPDGType = pdg;  };

  void SetMultiplicity (Int_t mult) {fMult    = mult; };

  void SetPRange(Double32_t pmin=0   , Double32_t pmax=10, Double32_t pstep = 1)
    {fP.SetStart(pmin); fP.SetStop(pmax); fP.SetStep(pstep); fPRangeIsSet=kTRUE;}

  void SetPtRange      (Double32_t ptmin=0   , Double32_t ptmax=10, Double32_t ptstep=1)
    {fPt.SetStart(ptmin); fPt.SetStop(ptmax); fPt.SetStep(ptstep); fPtRangeIsSet=kTRUE;};

  void SetPhiRange     (Double32_t phimin=0  , Double32_t phimax=360, Double32_t phistep = 1)
    {fPhi.SetStart(phimin); fPhi.SetStop(phimax); fPhi.SetStep(phistep);};

  void SetEtaRange     (Double32_t etamin=-5 , Double32_t etamax=7, Double32_t etastep=1)
    {fEta.SetStart(etamin); fEta.SetStop(etamax); fEta.SetStep(etastep); fEtaRangeIsSet=kTRUE;};

  void SetYRange       (Double32_t ymin=-5, Double32_t ymax=7, Double32_t ystep=1)
    {fRapidity.SetStart(ymin); fRapidity.SetStop(ymax); fRapidity.SetStep(ystep); fYRangeIsSet=kTRUE;};

  void SetThetaRange   (Double32_t thetamin=0, Double32_t thetamax=90, Double32_t thetastep=1)
    {fTheta.SetStart(thetamin); fTheta.SetStop(thetamax); fTheta.SetStep(thetastep); fThetaRangeIsSet=kTRUE;};

  void SetCosThetaRange (Double32_t costhetamin=-1, Double32_t costhetamax=1, Double32_t costhetastep=0.1)
    {fCosTheta.SetStart(costhetamin); fCosTheta.SetStop(costhetamax); fCosTheta.SetStep(costhetastep); fCosThetaIsSet=kTRUE; fThetaRangeIsSet=kTRUE;};

  void SetXYZ   (Double32_t x=0, Double32_t y=0, Double32_t z=0) {
    fX=x; fY=y; fZ=z; fPointVtxIsSet=kTRUE;}

  void SetBoxXYZ (Double32_t x1=0, Double32_t y1=0, Double32_t x2=0, Double32_t y2=0, Double32_t z=0) {
    fX1=x1; fY1=y1; fX2=x2; fY2=y2; fZ=z; fBoxVtxIsSet=kTRUE;}

  void SetDebug(Bool_t debug=0) {fDebug = debug;}

  /** Initializer **/
  Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the PndPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t      fPDGType;             // Particle type (PDG encoding)
  Int_t      fMult;                // Multiplicity
  Int_t		 fEvent;			   // EventCounter

  Double32_t fPDGMass;             // Particle mass [GeV]
//  Double32_t fPtMin,  fPtMax,	fPtStep;      // Transverse momentum range [GeV]
  PndRangeValues fPt;
// Double32_t fPhiMin, fPhiMax,	fPhiStep;     // Azimuth angle range [degree]
  PndRangeValues fPhi;
//  Double32_t fEtaMin, fEtaMax,	fEtaStep;     // Pseudorapidity range in lab system
  PndRangeValues fEta;
//  Double32_t fYMin,   fYMax, 	fYStep;       // Rapidity range in lab system
  PndRangeValues fRapidity;
//  Double32_t fPMin,   fPMax,	fPStep;       // Momentum range in lab system
  PndRangeValues fP;
//  Double32_t fThetaMin, fThetaMax, fThetaStep; // Polar angle range in lab system [degree]
  PndRangeValues fTheta;
  PndRangeValues fCosTheta;
  Double32_t fX, fY, fZ;           // Point vertex coordinates [cm]
  Double32_t fX1, fY1, fX2, fY2;   // Box vertex coords (x1,y1)->(x2,y2)

  Bool_t     fEtaRangeIsSet;       // True if eta range is set
  Bool_t     fYRangeIsSet;         // True if rapidity range is set
  Bool_t     fThetaRangeIsSet;     // True if theta range is set
  Bool_t     fCosThetaIsSet;       // True if uniform distribution in
                                   //cos(theta) is set (default -> not set)
  Bool_t     fPtRangeIsSet;        // True if transverse momentum range is set
  Bool_t     fPRangeIsSet;         // True if abs.momentum range is set
  Bool_t     fPointVtxIsSet;       // True if point vertex is set
  Bool_t     fBoxVtxIsSet;         // True if box vertex is set
  Bool_t     fDebug;               // Debug switch
  Bool_t	 fDoit;					// Stop generating particles at the end of the range
  void CalcActValues(PndRangeValues* val1, PndRangeValues* val2, PndRangeValues* val3);

  ClassDef(PndFixStepParticleGun,1);

};


#endif
