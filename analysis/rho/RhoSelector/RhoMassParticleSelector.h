#ifndef RHOMASSPARTICLESELECTOR_H
#define RHOMASSPARTICLESELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMassParticleSelector                                              //
//                                                                      //
// Selector classes for mass selection                                  //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 99                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <float.h>
#include "TNamed.h"
#include "RhoBase/RhoParticleSelectorBase.h"

class RhoCandidate;
class PndRecoCandidate;

//----------------------------------------------------------------


class RhoMassParticleSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoMassParticleSelector ( const char* name="RhoMassParticleSelector", Double_t cv=0,Double_t w=DBL_MAX,const char* type=0 );
    //Destructor
    virtual ~RhoMassParticleSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( PndRecoCandidate* b );

  private:
    Double_t fCentralValue,fWindow;

  public:
    ClassDef ( RhoMassParticleSelector,1 )  // Particle selector (Mass window)
};

#endif
