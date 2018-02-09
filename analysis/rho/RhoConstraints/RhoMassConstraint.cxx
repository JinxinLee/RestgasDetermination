//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMassConstraint                                                    //
//                                                                      //
// Definition of a mass constraint for particle fitters                 //
//                                                                      //
// Author List:                                                         //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMassConstraint.h"

ClassImp ( RhoMassConstraint );

RhoMassConstraint::RhoMassConstraint ( const Double_t& mass )
  : fMassConstraint ( mass )
{}

RhoMassConstraint::~RhoMassConstraint()
{}

