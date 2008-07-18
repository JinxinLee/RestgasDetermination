#ifndef TASYMGAUSSCONSISTENCY_H
#define TASYMGAUSSCONSISTENCY_H
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

#include "RhoMath/TConsistency.h"

class TAsymGaussConsistency : public TConsistency {
public:
    TAsymGaussConsistency( Double_t delta, Double_t sigmaMinus, Double_t sigmaPlus);
    
    virtual ~TAsymGaussConsistency() {}
    
protected:
    Bool_t Calc();      //  consistency of measurement
    
    Double_t fDelta;
    Double_t fSigmaMinus;
    Double_t fSigmaPlus;
public:
    ClassDef(TAsymGaussConsistency,1) // Asymmetric Gauss consistency
};

#endif


