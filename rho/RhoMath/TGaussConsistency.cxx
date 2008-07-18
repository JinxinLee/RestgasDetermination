//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TAsymGaussConsistency		    				//
//                                                                      //
// Gaussian consistency							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <math.h> 
#include <float.h> 

#include "TMath.h"

#include "RhoMath/TGaussConsistency.h"

ClassImp(TGaussConsistency)

TBuffer &operator>>(TBuffer &buf, TGaussConsistency *&obj)
{
   obj = (TGaussConsistency *) buf.ReadObject(TGaussConsistency::Class());
   return buf;
}

TGaussConsistency::TGaussConsistency( Double_t delta, Double_t sigma) :
fDelta(delta), fSigma(sigma) 
{ 
    fValue = 0.; fLikelihood = 0.;
    if (delta < 0.) {
	fSign = TConsistency::left;
    }
    if (delta > 0.) {
	fSign = TConsistency::right;
    }
    if( Calc() )
	SetStatus(TConsistency::OK);
    else
	SetStatus(TConsistency::underFlow); 
}


Bool_t 
TGaussConsistency::Calc() 
{
    // Gautier 01/28/99 : add protection against unphysical values of arg
    if( fSigma==0. ) return kFALSE;
    Double_t arg = fDelta/fSigma;
    
    fValue = TMath::Erfc(fabs(arg/1.414));
    
    Double_t arg2 = -0.5*arg*arg;
    if ( arg2 < DBL_MIN_EXP ) return kFALSE;
    
    // OK
    fLikelihood = TMath::Exp(arg2)/fSigma/TMath::Sqrt(2.*TMath::Pi());
    return kTRUE;
}






