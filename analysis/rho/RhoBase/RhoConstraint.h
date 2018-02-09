#ifndef RHOCONSTRAINT_H
#define RHOCONSTRAINT_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoConstraint                                                        //
//                                                                      //
// Base class for fitting constraints                                   //
//                                                                      //
// Author List:                                                         //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TObject.h"

class RhoConstraint : public TObject
{
  public:
    RhoConstraint();
    ~RhoConstraint();

    ClassDef ( RhoConstraint,1 );
};

#endif // RHOCONSTRAINT_H
