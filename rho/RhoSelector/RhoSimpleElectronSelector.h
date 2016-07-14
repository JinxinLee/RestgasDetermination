#ifndef RHOSIMPLEELECTRONSELECTOR_H
#define RHOSIMPLEELECTRONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleElectronSelector                                            //
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


class RhoSimpleElectronSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoSimpleElectronSelector ( const char* name="RhoSimpleElectronSelector", const char* type="e-" );
    //Destructor
    virtual ~RhoSimpleElectronSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );


  public:
    ClassDef ( RhoSimpleElectronSelector,1 )  // Particle selector (Electrons)
};

#endif
