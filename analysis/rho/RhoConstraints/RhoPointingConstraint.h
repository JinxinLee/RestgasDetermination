#ifndef RHOPOINTINGCONSTRAINT_H
#define RHOPOINTINGCONSTRAINT_H
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


#include "RhoConstraint.h"
#include "RhoVector3Err.h"

class RhoPointingConstraint : public RhoConstraint
{
  public:
    RhoPointingConstraint ( const RhoVector3Err& where );
    ~RhoPointingConstraint();

    void SetPointingConstraint ( const RhoVector3Err& where ) {
      fPointingConstraint=where;
    };
    RhoVector3Err GetPointingConstraint() const {
      return fPointingConstraint;
    };

  private:
    RhoVector3Err fPointingConstraint; // [cm]

    ClassDef ( RhoPointingConstraint,1 );
};

#endif // RHOPOINTINGCONSTRAINT_H
