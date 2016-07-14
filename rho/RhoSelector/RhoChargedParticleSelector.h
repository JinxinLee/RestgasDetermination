#ifndef RHOCHARGEDPARTICLESELECTOR_H
#define RHOCHARGEDPARTICLESELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Selector classes for cherged particle selection                      //
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
class FairRecoCandidate;

//----------------------------------------------------------------

class RhoChargedParticleSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoChargedParticleSelector ( const char* name="RhoChargedParticleSelector", const char* type=0 );
    //Destructor
    virtual ~RhoChargedParticleSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate& b );
    virtual Bool_t Accept ( FairRecoCandidate& b );

    ClassDef ( RhoChargedParticleSelector,1 )  // Charged particle selector
};

#endif
