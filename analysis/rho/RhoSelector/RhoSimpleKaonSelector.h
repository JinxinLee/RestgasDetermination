#ifndef RHOSIMPLEKAONSELECTOR_H
#define RHOSIMPLEKAONSELECTOR_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoSimpleKaonSelctor                                                 //
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


class RhoSimpleKaonSelector : public RhoParticleSelectorBase
{

  public:
    //Constructor
    RhoSimpleKaonSelector ( const char* name="RhoSimpleKaonSelector", const char* type="K+" );
    //Destructor
    virtual ~RhoSimpleKaonSelector() {}

    //operations
    virtual Bool_t Accept ( RhoCandidate* b );
    virtual Bool_t Accept ( FairRecoCandidate* b );


  public:
    ClassDef ( RhoSimpleKaonSelector,1 )  // Particle selector (Kaons)
};

#endif
