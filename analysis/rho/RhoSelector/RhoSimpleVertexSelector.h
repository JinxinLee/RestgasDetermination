#ifndef RHOVERTEXSELECTOR_H
#define RHOVERTEXSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleVertexSelector                                              //
//                                                                      //
// Selector class to estimate the geometric intersection of two tracks  //
// Intended to provide the initial vertex position for V0 fits          //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TNamed.h"
#include "TVector3.h"
#include "RhoBase/RhoVertexSelectorBase.h"

class THistogram;

class RhoSimpleVertexSelector : public RhoVertexSelectorBase
{

  public:
    RhoSimpleVertexSelector ( const char* name = "SimpleVertexSelector", Double_t d=1.0, Double_t a=3.14159265358979323846, Double_t r1=0.0, Double_t r2=1.E8 );
    virtual ~RhoSimpleVertexSelector();
    virtual Bool_t Accept ( RhoCandidate& a, RhoCandidate& b );
    //void ActivateQualityControl();

  private:
    //    THistogram *fQC; //! Do not stream

  public:
    ClassDef ( RhoSimpleVertexSelector,1 )  // Simple vertex selector
};

#endif
