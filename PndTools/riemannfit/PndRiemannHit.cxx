//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRiemannHit
//      see PndRiemannHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRiemannHit.h"
#include "TMath.h"
// C/C++ Headers ----------------------
#include "FairHit.h"
#include <assert.h>
#include <iostream>

// Collaborating Class Headers --------
#include "PndRiemannTrack.h"

// Class Member definitions -----------


// scale factor! r=1 => z'=0.5 !!!
// to normalize cm-range 
#define RIEMANNSCALE 1.

ClassImp(PndRiemannHit)


PndRiemannHit::PndRiemannHit(double r, double phi, double z) : fHit(NULL), fCovX(3,3), fZ(z), fVerbose(0)
{
  setXYZ(r*TMath::Cos(phi),r*TMath::Sin(phi),z);
  setDXYZ(1,1,1);
}

PndRiemannHit::PndRiemannHit(FairHit* cl) : fHit(cl), fCovX(3,3), fVerbose(0)
{
  setHit(cl);
}


PndRiemannHit::~PndRiemannHit(){}

void PndRiemannHit::setXYZ(double x, double y, double z)
{
	TVector2 a(x, y);
	double r=a.Mod()/RIEMANNSCALE; 
	double phi=a.Phi();
	double r2=r*r;
	  
	fX.SetX(x/RIEMANNSCALE);
	fX.SetY(y/RIEMANNSCALE);
	fX.SetZ(r2);
	fZ = z;
}

void PndRiemannHit::setDXYZ(double dx, double dy, double dz)
{
	  fCovX[0][0] = TMath::Power(dx/RIEMANNSCALE,2);
	  fCovX[1][1] = TMath::Power(dy/RIEMANNSCALE,2);
	  fCovX[2][2] = TMath::Power(2*dx*dx,2) + TMath::Power(2*dy*dy,2)/RIEMANNSCALE;
	  fDeltaZ = dz/RIEMANNSCALE;
}

void PndRiemannHit::setHit(FairHit* cl)
{
	setXYZ(cl->GetX(), cl->GetY(), cl->GetZ());
	setDXYZ(cl->GetDx(), cl->GetDy(), cl->GetDz());  
}


double
PndRiemannHit::z()const {
  return fZ;
}

void 
PndRiemannHit::calcPosOnTrk(PndRiemannTrack* trk){
  assert(trk!=NULL);
  TVectorD o=trk->orig(); 
  o*=RIEMANNSCALE;// convert back to cm;
  double r=trk->r();
  r*=RIEMANNSCALE;
  const FairHit* firstHit=trk->getHit(0)->hit();
  assert(firstHit!=NULL);
  TVector2 k(firstHit->GetX()-o[0],firstHit->GetY()-o[1]);
  TVector2 l(fHit->GetX()-o[0],fHit->GetY()-o[1]);
  fAlpha=l.DeltaPhi(k);
//  if (fAlpha < 0)
//	  fAlpha += TMath::Pi()*2;
  fS=fAlpha*r;
  if (fVerbose > 1) std::cout<<"z/s "<< fHit->GetZ() << " / " << fS <<std::endl;
}

double PndRiemannHit::sigmaXY() const
{
	double result = TMath::Sqrt(fCovX[0][0] + fCovX[1][1]);
	if (result == 0.){
		return 1.;
	}
	else return result;
}
