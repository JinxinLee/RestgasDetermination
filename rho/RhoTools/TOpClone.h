//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpClone.h,v 1.1.1.1 2001-05-04 16:20:23 marcel Exp $
//
// Description:
//	TOpClone - 
//
//      Unary operator for cloning a decay tree
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   March, 1999  - creation
//
// Copyright Information:
//	Copyright (C) 1999	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TOPCLONE_H
#define TOPCLONE_H

// base class
#include "RhoTools/TUnaryOperator.h"

class TOpClone :  public TUnaryOperator
{
    
public:
    
    
    //--------------------
    // Public interface --
    //--------------------
    
    // constructor
    TOpClone();
    
    // virtual destructor
    virtual ~TOpClone();
    
    // virtual function of BtaUnaryOperator
    TCandidate* Create( const TCandidate& input );
    
    ClassDef(TOpClone,1) //Unary operator for cloning a decay tree    
};

#endif
