#ifndef TCHISQCONSISTENCY_H
#define TCHISQCONSISTENCY_H
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

#include "RhoMath/TConsistency.h"

class TChisqConsistency : public TConsistency {
public:
    
    // default constructor; sets internal state to noMeasure
    TChisqConsistency( Double_t chisq, Double_t nDof );
    
    virtual ~TChisqConsistency() {}
    
protected:
    
    Double_t fChisq;
    Double_t fNDof;
public:
    ClassDef(TChisqConsistency,1) // Chisquare consistency
};

#endif
