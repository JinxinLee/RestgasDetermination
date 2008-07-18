#ifndef TGAUSSCONSISTENCY_H
#define TGAUSSCONSISTENCY_H
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

#include "RhoMath/TConsistency.h"

class TGaussConsistency : public TConsistency {
    
public:
    
    // default constructor; sets internal state to noMeasure
    TGaussConsistency( Double_t delta, Double_t sigma);
    
    virtual ~TGaussConsistency() {}
    
protected:
    Bool_t Calc();      //  consistency of measurement
    
    Double_t fDelta;
    Double_t fSigma;
public:
    ClassDef(TGaussConsistency,1) // Gauss consistency
};

#endif





