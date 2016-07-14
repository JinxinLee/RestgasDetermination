#ifndef RHOMINUSPARTICLESELECTOR_H
#define RHOMINUSPARTICLESELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoMinusParticleSelector                                             //
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


class RhoMinusParticleSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoMinusParticleSelector ( const char* name="RhoMinusParticleSelector", const char* type=0 );
    //Destructor
    virtual ~RhoMinusParticleSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );

    ClassDef ( RhoMinusParticleSelector,1 )  // Negative particle selector
};

#endif
