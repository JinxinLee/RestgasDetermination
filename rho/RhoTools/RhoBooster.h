//--------------------------------------------------------------------------
// File and Version Information:
//  $Id: RhoBooster.h,v 1.3 2001-12-17 18:46:04 Marcel Exp $
//
// Description:
//  RhoBooster -
//
//      Utility class to
//      boost any candidate from and to the Center of Mass system
//      of a given beta candidate.
//      The class keeps internally a copy to the beta candidate
//      which defines the moving system,
//      and an association map for book keeping (NO! not implemented !).
//      Genealogy is preserved during the boost : Boosting a mother
//      candidate will recursively boost all its daughters.
//
// Environment:
//  Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//  Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Sept, 1997  - creation
//
// Copyright Information:
//  Copyright (C) 1997    Lawrence Berkeley Laboratory
//  Copyright (C) 1997         CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)
//------------------------------------------------------------------------

#ifndef RHOBOOSTER_H
#define RHOBOOSTER_H

#include "TROOT.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TRotation.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoLorentzVectorErr.h"
#include "RhoBase/RhoFactory.h"

class RhoBooster
{
  public:
    //--------------------
    // Public interface --
    //--------------------

    enum BoostSign { To, From };
    //
    // constructors  - either from a RhoCandidate or from a 4-Vector
    //

    // takes a 4-vector
    RhoBooster ( const TLorentzVector& cmFrame, Bool_t saveCovMatrix = kFALSE );

    // takes the particle which defined the moving system as an argument
    RhoBooster ( const RhoCandidate& cand, Bool_t saveCovMatrix = kFALSE );

    // takes the boost vector as an argument
    RhoBooster ( const TVector3& boostVector, Bool_t saveCovMatrix = kFALSE );

    // default constructor
    RhoBooster ( const RhoCandidate* cand=0, Bool_t saveCovMatrix = kFALSE );

    //
    // virtual destructor
    //
    virtual ~RhoBooster();

    //
    // assignment
    //
    void SetBoostVector ( const TVector3& cmFrame ){fBoostVector=cmFrame;};
    void SetBoostVector ( const TLorentzVector& lv ){SetBoostVector(lv.BoostVector());};
    void SetBoostVector ( const RhoCandidate* cand ){SetBoostVector(cand->P4());;};
    void SetBoostVector ( const RhoCandidate& cand ){SetBoostVector(&cand);};

    //
    // accessors
    //
//    const RhoCandidate& RestParticle() const {};

    //
    // public member functions
    //

    // Note : the boost preserves genealogy.
    RhoCandidate& BoostTo ( const RhoCandidate& cand ) const;
    RhoCandidate& BoostFrom ( const RhoCandidate& cand ) const;

    void BoostTo ( RhoCandList& initialList,
                   RhoCandList& boostedList,
                   Bool_t cleanList=kTRUE ) const;
    void BoostFrom ( RhoCandList& initialList,
                     RhoCandList& boostedList,
                     Bool_t cleanList=kTRUE ) const;

//     // Boost and sort according to momentum   (returns a owned map)
//     TCandMap*   BoostAndSort ( RhoCandList&,
//                                RhoCandList&,
//                                BoostSign sign=To );

//    TLorentzVector BoostedP4 ( const RhoCandidate& cand, BoostSign sign=To ) const {};
//    RhoLorentzVectorErr BoostedError ( const RhoCandidate& cand, BoostSign sign=To ) const {};
//    const TLorentzRotation& RotateAndBoost ( BoostSign sign=To ) const {};

    RhoCandidate& Boost ( const RhoCandidate& cand,  BoostSign sign ) const;

  private:

    //
    // private helper functions
    //

    TVector3  fBoostVector; //!
//    TLorentzRotation* fRotateAndBoostTo; //!
//    TLorentzRotation* fRotateAndBoostFrom; //!
    Bool_t fSaveCovMatrix;
//    void clean();

  public:
    ClassDef ( RhoBooster,1 ) // Boost RhoCandidate from and to CMS
};

#endif
