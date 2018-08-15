#ifndef RHOFACTORY_H
#define RHOFACTORY_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoFactory                                                           //
//                                                                      //
// Object factory for RhoCandidate and Vertices (RhoVector3Err)         //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 01                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TClonesArray.h"

class RhoCandidate;
class RhoVector3Err;
class TLorentzVector;
class RhoError;
class RhoCandListIterator;
class TParticlePDG;

class RhoFactory : public TObject
{

  public:
    static RhoFactory* Instance();
    virtual ~RhoFactory();
    static void Reset();
    static RhoCandidate* NewCandidate();
    static RhoCandidate* NewCandidate ( const RhoCandidate& c );
    static RhoCandidate* NewCandidate ( const RhoCandidate* c );

//     static RhoCandidate* NewCandidate ( TLorentzVector p4,
//                                         RhoError& p4Err,
//                                         RhoCandListIterator& iterDau,
//                                         RhoVector3Err& theVertex,
//                                         const TParticlePDG* hypo = 0 );

    static RhoCandidate* GetCandidate ( Int_t );
    static Int_t GetCandidateWatermark();
    static Int_t GetCandPointer();
 
  private:
    RhoFactory() {}
    static RhoFactory*  fgInstance;  //! Do not stream
    static TClonesArray*   fgCandBuffer; //! Candidate buffer
    static Int_t     fgCandPointer; // Actual position
    static Int_t     fgCandWatermark; // Max. position

    ClassDef ( RhoFactory,1 ) //Factory for candidates and vertices

};
#endif
