//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TAsymGaussConsistency		    				//
//                                                                      //
// Asymmetric gauss consistency						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TAsymGaussConsistency.h"
#include "TMath.h"

#include <assert.h>
#include <math.h> 
#include <float.h>

//extern "C" Double_t erfc(Double_t);

ClassImp(TAsymGaussConsistency)

TBuffer &operator>>(TBuffer &buf, TAsymGaussConsistency *&obj)
{
   obj = (TAsymGaussConsistency *) buf.ReadObject(TAsymGaussConsistency::Class());
   return buf;
}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------


TAsymGaussConsistency::TAsymGaussConsistency( Double_t delta, 
					    Double_t sigmaMinus,
					    Double_t sigmaPlus) 
  : fDelta(delta)
  , fSigmaMinus(sigmaMinus) 
  , fSigmaPlus(sigmaPlus) 
{
  fValue = 0.; fLikelihood = 0.;
  if (delta < 0.) {
    fSign = TConsistency::left;
  }
  if (delta > 0.) {
    fSign = TConsistency::right;
  }
  if ( Calc() ) {
    SetStatus(TConsistency::OK);
  } else {
    SetStatus(TConsistency::underFlow);
  }
}

Bool_t TAsymGaussConsistency::Calc() {
  Double_t sigma = fSigmaMinus;
  if ( fDelta > 0 ) {
    sigma = fSigmaPlus;
  }

  if ( sigma == 0 ) return kFALSE;
  Double_t arg = fDelta/sigma;

  fValue = TMath::Erfc(TMath::Abs(arg*0.70710678118654752440));

  Double_t arg2 = -0.5*arg*arg;
  if ( arg2 < DBL_MIN_EXP ) return kFALSE;

  fLikelihood = TMath::Exp(arg2);
  // normalization
  fLikelihood *= 2./TMath::Sqrt(2.*TMath::Pi())/(fSigmaPlus+fSigmaMinus);
  return kTRUE;
}







