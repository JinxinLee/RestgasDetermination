#ifndef RHOPARTICLESELECTORBASE_H
#define RHOPARTICLESELECTORBASE_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoParticleSelectorBase                                              //
//                                                                      //
// Selector classes for particle identification                         //
// Particle masses are set from the TDatabasePDG class                  //
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99                                   //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TNamed.h"

class RhoCandidate;
class RhoCandList;
class FairRecoCandidate;
class TParticlePDG;

enum criterion {best,veryLoose,loose,tight,veryTight,variable,all};

class RhoParticleSelectorBase : public TNamed
{

  public:

    //Constructor
    RhoParticleSelectorBase ( const char* name="VAbsPidSelector",const char* type = 0 );
    //Destructor
    virtual ~RhoParticleSelectorBase();

    //operations
    virtual Bool_t Accept ( RhoCandidate* ) = 0;
    virtual Bool_t Accept ( FairRecoCandidate* ) = 0;

    virtual void SetCriterion ( const char* crit );
    void SetCriterion ( criterion crit );
    void Select ( RhoCandList& l );
    void Select ( RhoCandList& in,RhoCandList& out );

  protected:
    TParticlePDG* CPConjugate ( TParticlePDG* aPart );

    void SetTypeAndMass ( RhoCandidate* b );

    TParticlePDG*  fTypePlus; //!Particle type to accept
    TParticlePDG*  fTypeMinus;  //!Particle type to accept
    criterion           fCriterion; //!Criterion to apply for selection

  public:
    ClassDef ( RhoParticleSelectorBase,1 ) // Particle Selector base class
};

#endif
