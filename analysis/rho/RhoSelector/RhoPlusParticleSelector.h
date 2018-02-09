#ifndef RHOPLUSPARTICLESELECTOR_H
#define RHOPLUSPARTICLESELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoPlusParticleSelector                                              //
//                                                                      //
// Selector classes for particle selection                              //
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


class RhoPlusParticleSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoPlusParticleSelector ( const char* name="RhoPlusParticleSelector", const char* type=0 );
    //Destructor
    virtual ~RhoPlusParticleSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );

    ClassDef ( RhoPlusParticleSelector,1 )  // Positive particle selector
};

#endif
