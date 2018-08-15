//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoPointingConstraint                                                //
//                                                                      //
// Definition of a pointing constraint for particle fitters             //
//                                                                      //
// Author List:                                                         //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoPointingConstraint.h"

ClassImp ( RhoPointingConstraint );

RhoPointingConstraint::RhoPointingConstraint ( const RhoVector3Err& where )
  : fPointingConstraint ( where )
{}

RhoPointingConstraint::~RhoPointingConstraint()
{}

