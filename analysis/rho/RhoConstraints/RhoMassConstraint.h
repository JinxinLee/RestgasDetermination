#ifndef RHOMASSCONSTRAINT_H
#define RHOMASSCONSTRAINT_H
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


#include "RhoConstraint.h"

class RhoMassConstraint : public RhoConstraint
{
  public:
    RhoMassConstraint ( const Double_t& mass=0. );
    ~RhoMassConstraint();

    void SetMassConstraint ( const Double_t& mass=0. ) {
      fMassConstraint=mass;
    };
    Double_t GetMassConstraint() const {
      return fMassConstraint;
    };

  private:
    Double_t fMassConstraint; //[GeV/c^2]

    ClassDef ( RhoMassConstraint,1 );
};

#endif // RHOMASSCONSTRAINT_H
