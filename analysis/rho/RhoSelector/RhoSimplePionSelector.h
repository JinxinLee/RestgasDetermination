#ifndef RHOSIMPLEPIONSELECTOR_H
#define RHOSIMPLEPIONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimplePionSelector                                                //
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


class RhoSimplePionSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoSimplePionSelector ( const char* name="RhoSimplePionSelector", const char* type="pi+" );
    //Destructor
    virtual ~RhoSimplePionSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );


  public:
    ClassDef ( RhoSimplePionSelector,1 )  // Particle selector (Pions)
};

#endif
