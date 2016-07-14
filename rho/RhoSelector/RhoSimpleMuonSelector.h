#ifndef RHOSIMPLEMUONSELECTOR_H
#define RHOSIMPLEMUONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleMuonSelector                                               //
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


class RhoSimpleMuonSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoSimpleMuonSelector ( const char* name="RhoSimpleMuonSelector", const char* type="mu-" );
    //Destructor
    virtual ~RhoSimpleMuonSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );


  public:
    ClassDef ( RhoSimpleMuonSelector,1 )  // Particle selector (Muons)
};

#endif
