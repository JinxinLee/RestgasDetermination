//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class TAdd4Vertexer
//                 The simpliest vertexer
//	
//  Child class of TOperator that allows to build up TCandidate trees
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TADD4VERTEXER_H
#define TADD4VERTEXER_H

#include "TObject.h"
#include "RhoBase/TCandListIterator.h"
#include "TLorentzVector.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TSimpleVertex.h"

class TAdd4Vertexer : public TObject
{
    
    // instance members
    TLorentzVector fP4;
    TMatrixD fErr;
    
public:
    
    // constructors
    TAdd4Vertexer();
    TAdd4Vertexer( const TVector3& p );
    TAdd4Vertexer( const TAdd4Vertexer& o );
    
    // destructor
    virtual ~TAdd4Vertexer();
    
    // the vertex
    virtual VAbsVertex* Compute( TCandListIterator& iter );
    
    // momentum of the decaying state
    virtual TLorentzVector P4() const;
    virtual TMatrixD& P4Err();
    
    ClassDef(TAdd4Vertexer,1) // The simpliest vertexer
};

#endif
