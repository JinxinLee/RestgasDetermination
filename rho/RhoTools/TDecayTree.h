//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDecayTree.h,v 1.3 2002-02-01 03:54:54 marcel Exp $
//
// Description:
//	Probably a temporary class, which should eventually be merged
//      of replace the BetaTools/BtaMode and PDT/DecayMode classes.
//      The idea (as for these classes) is to describe a decay 
//      channel.  Basically a decay channel is described by a TParticlePDG
//      for the mother, and a list of PdtEntries for the daughters.
//      These are provided a construction in a list.
//      A new feature is that a TDecayTree represents a decay 
//      as well as its CP conjugate.
//      The purpose of a TDecayTree is to be given as an argument
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
//      Eric Charles:  Added recursion, matching, sorting and some other jazz
//
// Copyright Information:
//	Copyright (C) 1998		Saclay & LBNL
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#ifndef TDECAYTREE_H
#define TDECAYTREE_H

#include <iostream>
#include "TObject.h"

class TParticlePDG;
class TString;
class TCandidate;
class TCandList;

//		---------------------
// 		-- Class Interface --
//		---------------------

class TDecayTree : public TObject {
    
public: 
    
    // an enum used for comparison operations
    enum compare { less=-1, equal=0, greater=1 };
    
    
    //--------------------
    // Instance Members --
    //--------------------
    
public:
    
    //
    // Constructors
    //
    
    // directly from PdtEntries
    TDecayTree( const TParticlePDG* theMother, 
	const TCandList& theDaughters );
    
    // from Pdt names
    TDecayTree( const TString& theMotherName,
	const TString* theDaughterNames , Int_t n);
    
    // For -> X decays and stable particle ( no daughters )
    TDecayTree( const TParticlePDG* theMother );
    
    // from a TCandidate
    TDecayTree( const TCandidate* cand );
    
    // from the decay mode string
    TDecayTree( const char* = 0 );
    
    // copy constructor ( allow for conjugation )
    TDecayTree( const TDecayTree& , Bool_t conj = kFALSE );
    
    // Destructor
    ~TDecayTree();
    
    // 
    // operators
    // 
    
    // assignment
    TDecayTree& operator= ( const TDecayTree& );
    
    // comparison and sorting
    Bool_t operator==( const TDecayTree& ) const;
    Bool_t operator!=( const TDecayTree& ) const;
    Bool_t operator< ( const TDecayTree& ) const;
    
    // returns true if the mode "matches anything"
    Bool_t IsWildcard( ) const;
    
    // returns true if the mode has no daughters
    Bool_t NoDaughters( ) const;  
    
    // returns true if mother is a CP eigen state
    Bool_t IsSelfConjugate() const;
    
    // returns true if the mode is equal to the other  
    // wildcards are only expanded in this mode, not in the other
    Bool_t IsSubMode( const TDecayTree* other, Bool_t conj = kFALSE,	
	Bool_t recurse = kFALSE ) const;
    
    // returns true if the mode is equal to the other 
    // wildcards are only expanded in the other mode, not in this one
    Bool_t IsSubModeOf( const TDecayTree* other, Bool_t conj = kFALSE,
	Bool_t recurse = true ) const;
    
    // returns true if the mode is equalk to the other
    // wildcards are expanded in both modes
    Bool_t IsMatch( const TDecayTree* other, Bool_t conj = kFALSE, 
	Bool_t recurse = kFALSE ) const;
    
    // print function, allow recursive printing 
    void PrintOn( std::ostream& o=std::cout, Bool_t recurse = kFALSE, Bool_t conj = kFALSE) const;

    // return the lund id of the mother
    Int_t LundId( Bool_t conj = kFALSE ) const;    
    
    //
    // access
    //
    
    // to the mother's TParticlePDG
    const TParticlePDG* Mother( Bool_t conjugate=kFALSE ) const;
    
    // to daughters
    int NDaughters() const; 
    const TParticlePDG* PdtDaughter(const int& i, Bool_t conj=kFALSE) const;
    const TDecayTree* Daughter(const int& i, Bool_t conj=kFALSE) const;
    
    const TDecayTree* FindDaughter(const TParticlePDG* pdt, Bool_t conj = kFALSE) const;
    const TDecayTree* FindDaughter(const TDecayTree* mode, Bool_t conj = kFALSE) const;  
    // RWTPtrSlistIterator<TParticlePDG> DaughterIterator( Bool_t conj=kFALSE ) const;
    
    
    //
    // Modifiers
    // 
    
    // useful for histograms, this is not const b/c it invokes
    // the lazy evaluation of the string.
    const TString& DecayString( Bool_t conj=kFALSE ) const;
    
    // these set the decay of a particular daughter. . .
    const TDecayTree* SetDaughterDecay( const TDecayTree* dMode);
    const TDecayTree* SetDaughterDecay( const char* dModeStr);
    
protected:
    
    // access to daughters
    TList* Daughters( Bool_t conj=kFALSE );
    const TList* Daughters( Bool_t conj=kFALSE ) const;  
    
private:
    
    //
    // May not be the most elegant implementation.
    // The important is the interface for now
    //
    const TParticlePDG* _theMother; //!
    const TParticlePDG* _theAntiMother; //!
    
    TList* _theDaughters; //!
    TList* _theAntiDaughters; //!
    
    // these are made by lazy eval. . .
    TString* _decayString; //!
    TString* _antiString; //!
    
    //
    // helper functions
    //
    
    // modifiers, all of these handle the conjugate modes as well
    Bool_t SetMother( const TParticlePDG* theMother );
    Bool_t AddDaughter( const TParticlePDG* aDaughter );
    Bool_t AddDaughter( const TDecayTree& aDaughter );
    Bool_t AddDaughter( const TDecayTree* aDaughter );
    
    // pdt utilities
    const TParticlePDG* CPConjugate( const TParticlePDG* aPart ) const;
    const TParticlePDG* Lookup( const TString& str ) const; 
    
    // a useful function
    static Bool_t HasWildcard( const TList* list );
    
    // comparision routines
    static Bool_t PtrSubMode( const TDecayTree* m1,
	const TDecayTree* m2,
	Bool_t conj = kFALSE );
    
    static Bool_t ListSubMode( const TList* l1,
	const TList* l2 ); 
    
    static Bool_t DaughterSubMode( const TList* l1,
	const TList* l2,
	Bool_t recurse = kFALSE);
    
    static Bool_t PtrMatch( const TDecayTree* m1,
	const TDecayTree* m2,
	Bool_t conj = kFALSE );
    
    static Bool_t ListMatch( const TList* l1,
	const TList* l2 );
    
    static Bool_t DaughterMatch( const TList* l1,
			     const TList* l2,
			     Bool_t recurse = kFALSE);
    
    static compare PtrCompare( const TDecayTree* m1, 
					   const TDecayTree* m2,
					   Bool_t conj = kFALSE );
    
    static compare ListCompare( const TList* l1,
	const TList* l2 );
    
    static compare DaughterCompare( const TList* l1,
	const TList* l2,
	Bool_t recurse = kFALSE);
public:
    ClassDef(TDecayTree,1) // Represent a decay as well as its CP conjugate
	
};

#endif










