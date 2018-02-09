//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoBeamConstraint                                                    //
//                                                                      //
// Definition of a beam constraint for particle fitters                 //
//                                                                      //
// Author List:                                                         //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBeamConstraint.h"

ClassImp ( RhoBeamConstraint );

RhoBeamConstraint::RhoBeamConstraint ( const RhoLorentzVectorErr& beam )
  : fBeamConstraint ( beam )
{}

RhoBeamConstraint::~RhoBeamConstraint()
{}

