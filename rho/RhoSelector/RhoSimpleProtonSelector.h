#ifndef RHOSIMPLEPROTONSELECTOR_H
#define RHOSIMPLEPROTONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleProtonSelector                                              //
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


class RhoSimpleProtonSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoSimpleProtonSelector ( const char* name="RhoSimpleProtonSelector", const char* type="p+" );
    //Destructor
    virtual ~RhoSimpleProtonSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );


  public:
    ClassDef ( RhoSimpleProtonSelector,1 )  // Particle selector (Protons)
};
#endif
