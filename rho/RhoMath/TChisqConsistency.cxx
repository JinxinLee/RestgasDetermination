//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TChisqConsistency			    				//
//                                                                      //
// Chisquare consistency						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TChisqConsistency.h"

#include "RhoMath/TNumRecipes.h"
#include "TMath.h"

#include <assert.h>
#include <math.h> 
#include <float.h>

ClassImp(TChisqConsistency)

TBuffer &operator>>(TBuffer &buf, TChisqConsistency *&obj)
{
   obj = (TChisqConsistency *) buf.ReadObject(TChisqConsistency::Class());
   return buf;
}

#include <iostream>
using namespace std;

TChisqConsistency::TChisqConsistency(Double_t chisq, Double_t nDof) :
   fChisq(chisq), fNDof(nDof)
{
  Double_t z2 = 0.5*fChisq;
  Double_t n2 = 0.5*fNDof;

  if (n2<=0 || z2<0) { 
      cerr << " Got unphysical values: chisq = " << chisq 
                      << " #dof = " << nDof << endl;
      fValue=0;
      fLikelihood=0;
      SetStatus(TConsistency::unPhysical);
      return;
  }
  SetStatus(TConsistency::OK);

  // given that n2>0 && z2>=0, gammq will NOT abort
  fValue = TNumRecipes::Gammq(n2,z2);

  if (fChisq==0) { 
     fLikelihood=1; 
  } else {
      Double_t loglike=(n2-1)*TMath::Log(z2)-z2-TNumRecipes::Gammln(n2);
     if ( loglike < DBL_MIN_EXP ) {
        fLikelihood = 0; 
        SetStatus(TConsistency::underFlow);
     } else {
	 fLikelihood = 0.5*TMath::Exp(loglike);
     }
  }
}
