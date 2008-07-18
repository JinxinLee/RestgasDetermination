#ifndef TPOISSONCONSISTENCY_H
#define TPOISSONCONSISTENCY_H
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

#include "RhoMath/TConsistency.h"

class TPoissonConsistency : public TConsistency {
    
public:
    
    // default constructor; sets internal state to noMeasure
    TPoissonConsistency( Int_t nObs, Double_t mu);
    
    virtual ~TPoissonConsistency() {}
    
private:
    // helper functions
    
    void Calc();      //  cache consistency of measurement
    
    Double_t GetLikelihood( Int_t n, Double_t mu, Double_t logMu ) const; // stash the formula here
    void   CalcGauss() ;
    void   CalcDirectSum() ;
    void   CalcTails( Double_t logMu ) ;
    
protected:
    Int_t    fNObs;
    Double_t fMu;
public:
    ClassDef(TPoissonConsistency,1) // Poisson consistency
};

#endif





