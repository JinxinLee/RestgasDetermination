//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TBooster.h,v 1.3 2001-12-17 18:46:04 Marcel Exp $
//
// Description:
//	TBooster - 
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
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Sept, 1997  - creation
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//	Copyright (C) 1997	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TBOOSTER_H
#define TBOOSTER_H

#include "RhoTools/TUnaryOperator.h"

class TCandidate;
class TCandMap;
class TCandList;
class TLorentzVector;
class TLorentzRotation;
class TLorentzVectorErr;

class TBooster 
:  public TUnaryOperator
{
    
    //--------------------
    // Instance Members --
    //--------------------
    
    // The moving particle
    TCandidate*  fParticle;  
    
    // cache
    TLorentzVector*       fLorentzVector;
    TLorentzRotation*     fRotateAndBoostTo;
    TLorentzRotation*     fRotateAndBoostFrom;
    
public:
    
    
    //--------------------
    // Public interface --
    //--------------------
    
    enum BoostSign{ To, From };
    
    //
    // constructors  - either from a TCandidate or from a 4-Vector
    //
    
    // takes a 4-vector
    TBooster( const TLorentzVector& cmFrame, Bool_t saveCovMatrix = kFALSE );
    
    // takes the particle which defined the moving system as an argument
    TBooster( const TCandidate& cand, Bool_t saveCovMatrix = kFALSE );
    
    // default constructor
    TBooster( const TCandidate* cand=0, Bool_t saveCovMatrix = kFALSE );
    
    //
    // virtual destructor
    //
    virtual ~TBooster();
    
    //
    // assignment 
    //
    void SetLorentzVector( const  TLorentzVector& cmFrame );
    void SetParticle( const TCandidate& cand );
    
    //
    // accessors
    //
    const TCandidate& RestParticle() const;
    const TLorentzVector& LorentzVector() const;
    const TLorentzRotation& RotateAndBoost( BoostSign sign=To ) const;
    
    // 
    // Disasllow copies, assignment
    //
    // Don't allow copies, assignment
    //TBooster( const TBooster& ) { ::abort(); }
    //TBooster& operator= ( const TBooster& ) {::abort(); return *this;}
    
    //
    // public member functions
    //
    
    // Note : the boost preserves genealogy. 
    
    
    TCandidate& BoostTo(   const TCandidate& cand) const;
    TCandidate& BoostFrom( const TCandidate& cand) const;
    
    void BoostTo(TCandList& initialList, 
	TCandList& boostedList,
	Bool_t cleanList=kTRUE) const;
    void BoostFrom(TCandList& initialList, 
	TCandList& boostedList,
	Bool_t cleanList=kTRUE) const;
    
    // Boost and sort according to momentum   (returns a owned map)
    TCandMap*   BoostAndSort( TCandList&,
	TCandList&,
	BoostSign sign=To);
    
    TLorentzVector BoostedP4( const TCandidate& cand, BoostSign sign=To ) const;
    TLorentzVectorErr BoostedError(const TCandidate& cand, BoostSign sign=To) const;
    
    TCandidate& Boost( const TCandidate& cand,  BoostSign sign) const; 
    
    // virtual function of TUnaryOperator
    TCandidate* Create( const TCandidate& input );
    
private:
    
    //
    // private helper functions
    //
    
    void clean();
    
    Bool_t fSaveCovMatrix;
    
    // mutable trick
    TBooster* myself_() const;
    
public:
    ClassDef(TBooster,1) // Boost TCandidate from and to CMS
};

#endif
