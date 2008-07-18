#ifndef TVERTEXSELECTOR_H
#define TVERTEXSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSimpleVertexSelector						//
//                                                                      //
// Selector class to estimate the geometric intersection of two tracks  //
// Intended to provide the initial vertex position for V0 fits          //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TNamed.h"
#include "TVector3.h"
#include "RhoBase/VAbsVertexSelector.h"

class THistogram;

class TSimpleVertexSelector : public VAbsVertexSelector {
    
public:
    TSimpleVertexSelector(const char *name = "SimpleVertexSelector", Double_t d=1.0, Double_t a=3.14159265358979323846, Double_t r1=0.0, Double_t r2=1.E8);
    virtual ~TSimpleVertexSelector();
    virtual Bool_t Accept(TCandidate& a, TCandidate& b);
    //void ActivateQualityControl();

private:
    //    THistogram *fQC; //! Do not stream

public:
    ClassDef(TSimpleVertexSelector,1)	// Simple vertex selector
};      

#endif
