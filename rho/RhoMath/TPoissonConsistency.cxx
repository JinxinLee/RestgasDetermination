//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPoissonConsistency			    				//
//                                                                      //
// Poissonian consistency						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TPoissonConsistency.h"

#include <assert.h>
#include <math.h> 
#include <stdlib.h>

#include "RhoMath/TNumRecipes.h"
#include "RhoMath/TGaussConsistency.h"

static Double_t EPS_CONVERGE = 0.01;

ClassImp(TPoissonConsistency)

TBuffer &operator>>(TBuffer &buf, TPoissonConsistency *&obj)
{
   obj = (TPoissonConsistency *) buf.ReadObject(TPoissonConsistency::Class());
   return buf;
}

#include <iostream>
using namespace std;

TPoissonConsistency::TPoissonConsistency(Int_t nObs, Double_t mu) 
: fNObs(nObs), fMu(mu)  
{ 
    if ( fMu > 0 && fNObs >= 0 ) { // idiot check
	if (nObs < mu) {
	    fSign = TConsistency::left;
	}
	if (nObs > mu) {
	    fSign = TConsistency::right;
	}
	Calc();
    } else {
	fValue = fLikelihood = 0;
	SetStatus(TConsistency::underFlow);
    }
}

void 
TPoissonConsistency::Calc() 
{
    Double_t logMu = log( fMu ) ;
    
    // get the likelihood - P(fNObs;mu)
    fLikelihood = GetLikelihood( fNObs, fMu, logMu ) ;
    
    if ( fLikelihood == 0 ) {
	// looks like we are too far from peak
	fValue = 0 ;
	return ; 
    }
    
    // the number can be adjusted
    if ( TMath::Abs( fNObs - fMu ) < 50 ) {
	
	CalcDirectSum() ;
	
    } else if ( fMu > 200 ) {
	
	// gaussian is good for very big numbers only
	CalcGauss() ;
	
    } else {
	
	// Gauss approximation is NOT good enough
	CalcTails( logMu );
	
    }
    
    SetStatus(TConsistency::OK);
}


Double_t
TPoissonConsistency::GetLikelihood( Int_t n, Double_t mu, Double_t logMu ) const
{
    return exp(-mu+n*logMu-TNumRecipes::Gammln(n+1));
}

void 
TPoissonConsistency::CalcGauss()
{  
    
    TGaussConsistency cons(fNObs-fMu,sqrt(fMu));
    fValue = cons.SignificanceLevel();
    
}


void
TPoissonConsistency::CalcDirectSum()
{
    
    // simply count all P(i) which > fLikelihood
    // the algorithm is rather simple, but may be slow for big numbers,
    // so I limit it only for small differences between fNObs and fMu
    
    Double_t P_current = fLikelihood ;
    Double_t significance = 1. ;
    int n = fNObs ;
    if ( fNObs <= fMu ) {
	Double_t P_next = P_current * fMu / (n+1) ;
	while ( P_next > fLikelihood ) {
	    significance -= P_next ;
	    P_current = P_next ;
	    n ++ ;
	    P_next = P_current * fMu / (n+1) ;
	}
    } else {                              // no, need go down
	while ( n > 0 ) {                  // at max to zero
	    Double_t P_next = P_current / fMu * n ;     // get P(I-1)
	    if ( P_next > fLikelihood ) {
		significance -= P_next ;
		P_current = P_next ;
		n -- ;
	    } else {
		break ;                        // finish at first which is <= P(N)
	    }
	}
    }
    
    fValue = significance ;
}


void 
TPoissonConsistency::CalcTails( Double_t logMu )
{
    
    // find the bin on the other side of peak - we need the bin which has P(i) > P(fNObs)
    int otherN = int(2*fMu - fNObs) ;
    if ( otherN < 0 ) otherN = 0 ; 
    
    // Now wander around to find exact position
    Double_t otherL = GetLikelihood( otherN, fMu, logMu ) ;
    
    int direction = 0 ;
    if ( otherN > fMu ) {
	if ( otherL < fLikelihood ) {
	    direction = -1 ;
	} else {
	    direction = +1 ;
	} 
    } else { 
	if ( otherL < fLikelihood ) {
	    direction = +1 ;
	} else {
	    direction = -1 ;
	} 
    }
    
    
    // we need to find such N that P(N) > _ likelihood and 
    // ( P(N+1) <= fLikelihood or P(N-1) <= fLikelihood )
    while ( kTRUE ) {
	int nextN = otherN + direction ;
	if ( nextN < 0 ) {
	    // this can happen only if we are at 0 already but need to go 
	    // even futher - we can't - stop here
	    break ;
	}
	Double_t nextL = direction > 0 ? otherL * fMu / (otherN+1) : otherL / fMu * otherN ;
	
	if ( otherL <= fLikelihood && nextL > fLikelihood ) {
	    // next bin is OK
	    otherN = nextN ;
	    break ;
	} else if ( otherL > fLikelihood && nextL <= fLikelihood ) {
	    // this bin is OK
	    break ;
	} else {
	    // nope, continue then
	    otherN = nextN ;
	    otherL = nextL ;
	}
    } 
    
    // now we should be at the good bin
    if ( otherN > fNObs ) {
	Double_t thisP = TNumRecipes::Gammq ( fNObs+1, fMu ) ;
	Double_t otherP = TNumRecipes::Gammp ( otherN+1, fMu ) ;
	fValue = otherP + thisP ;
    } else {
	Double_t thisP = TNumRecipes::Gammp ( fNObs, fMu ) ;
	Double_t otherP = 0 ;
	if ( otherN > 0 ) otherP = TNumRecipes::Gammq ( otherN, fMu ) ;
	fValue = otherP + thisP ;
    } 
    
}
