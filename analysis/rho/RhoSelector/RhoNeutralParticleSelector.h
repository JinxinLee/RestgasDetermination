#ifndef RHONEUTRALPARTICLESELECTOR_H
#define RHONEUTRALPARTICLESELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoNeutralParticleSelector                                           //
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


class RhoNeutralParticleSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoNeutralParticleSelector ( const char* name="RhoNeutralParticleSelector", const char* type=0 );
    //Destructor
    virtual ~RhoNeutralParticleSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );

    ClassDef ( RhoNeutralParticleSelector,1 )  // Neutral particle selector
};

#endif
