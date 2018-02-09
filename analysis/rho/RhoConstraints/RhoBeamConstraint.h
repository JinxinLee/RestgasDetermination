#ifndef RHOBEAMCONSTRAINT_H
#define RHOBEAMCONSTRAINT_H
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


#include "RhoConstraint.h"
#include "RhoLorentzVectorErr.h"

class RhoBeamConstraint : public RhoConstraint
{
  public:
    RhoBeamConstraint ( const RhoLorentzVectorErr& beam );
    ~RhoBeamConstraint();

    void SetBeamConstraint ( const RhoLorentzVectorErr& beam ) {
      fBeamConstraint=beam;
    };
    RhoLorentzVectorErr GetBeamConstraint() const {
      return fBeamConstraint;
    };

  private:
    RhoLorentzVectorErr fBeamConstraint; //fourmomentum in the LAB [GeV/c]

    ClassDef ( RhoBeamConstraint,1 );
};

#endif // RHOBEAMCONSTRAINT_H
