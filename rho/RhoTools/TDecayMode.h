//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDecayMode.h,v 1.3 2002-02-01 03:54:53 marcel Exp $
//
// Description:
//	Probably a temporary class, which should eventually be merged
//      of replace the BetaTools/TMode and PDT/DecayMode classes.
//      The idea (as for these classes) is to describe a decay 
//      channel.  Basically a decay channel is described by a TParticlePDG
//      for the mother, and a list of PdtEntries for the daughters.
//      These are provided a construction in a list.
//      A new feature is that a TDecayMode represents a decay 
//      as well as its CP conjugate.
//      The purpose of a TDecayMode is to be given as an argument
//      to a CompositeSelector constructor.  The DecayMode will define
//      the number and the types of the daughters.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault, from earlier work by
//                                    Paul Harrison & Luca Lista
//
// Copyright Information:
//	Copyright (C) 1998		Saclay & LBNL
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TDECAYMODE_H
#define TDECAYMODE_H

#include <iostream>

#include "TString.h"
#include "TParticlePDG.h"
#include "TList.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

class TDecayMode : public TObject {
    
    //--------------------
    // Instance Members --
    //--------------------
    
public:
    
    //
    // Constructors
    //
    
    // directly from PdtEntries
    TDecayMode( TParticlePDG* theMother, 
	const TList& theDaughters );
    
    // from Pdt names
    TDecayMode( const TString& theMotherName,
	const TString theDaughterNames[5] );
    
    
    // from the decay mode
    TDecayMode( const char* = "D0 -> K+ pi-" );
    
    // copy constructor
    TDecayMode( const TDecayMode& );
    
    // Destructor
    ~TDecayMode();
    
    //
    // access
    //
    const TParticlePDG* Mother( Bool_t conjugate=kFALSE ) const;
    
    // returns true if mother is a CP eigen state
    Bool_t IsSelfConjugate() const;
    
    // access to daughters
    int NDaughters() const; 
    const TList&  Daughters( Bool_t conj=kFALSE ) const;
    TIter DaughterIterator( Bool_t conj=kFALSE ) const;
    
    // useful for histograms
    TString DecayString( Bool_t conj=kFALSE ) const;
    
protected:
    
private:
    
    //
    // May not be the most elegant implementation.
    // The important is the interface for now
    //
    TParticlePDG* _theMother; //! Do not stream
    TParticlePDG* _theAntiMother; //! Do not stream
    TList _theDaughters; //! Do not stream
    TList _theAntiDaughters; //! Do not stream
    
    // helper functions
    Bool_t SetMother( TParticlePDG* theMother );
    Bool_t AddDaughter( TParticlePDG* aDaughter );
    TParticlePDG* CPConjugate( TParticlePDG* aPart );
    
public:
    ClassDef(TDecayMode,1) // Store particle decays
};

#endif

