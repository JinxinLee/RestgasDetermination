#ifndef TDUMMYFITTER_H
#define TDUMMYFITTER_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDummyFitter			  					//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/VAbsFitter.h"

class TDummyFitter : public VAbsFitter
{
public:
    TDummyFitter( const TCandidate& b ) : VAbsFitter( b ) {}
    virtual ~TDummyFitter() {}
    void Fit() {}
    ClassDef(TDummyFitter,1) //A dummy fitter
};

#endif 
