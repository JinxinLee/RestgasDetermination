//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDecayTree.cxx,v 1.6 2004-01-28 15:27:37 marcel Exp $
//
// Description:
//	Class TDecayTree - 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//      Paul Harrison & Luca Lista, original Authors of BtaMode and DecayMode
//
// Copyright Information:
//      Copyright (C) 1998              Saclay & LBNL
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "RhoTools/TDecayTree.h"

//-------------
// C Headers --
//-------------
#include <assert.h>

//---------------
// C++ Headers --
//---------------
#include <math.h>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "TParticlePDG.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

ClassImp(TDecayTree)

TBuffer &operator>>(TBuffer &buf, TDecayTree *&obj)
{
   obj = (TDecayTree *) buf.ReadObject(TDecayTree::Class());
   return buf;
}

#include <iostream>
using namespace std;

//----------------
// Constructors --
//----------------

TDecayTree::TDecayTree( const TParticlePDG* theMother,
			   const TCandList& theDaughters )
			   :_theMother(0),
			   _theAntiMother(0),
			   _theDaughters(0),
			   _theAntiDaughters(0),
			   _decayString(0),
			   _antiString(0){
    assert( SetMother( theMother ) );
    for( size_t i=0; i< theDaughters.GetNumberOfCandidates(); i++ ) {
	AddDaughter( theDaughters.GetConst(i) );
    }
}

TDecayTree::TDecayTree( const TString& theMotherName,
			   const TString* theDaughterNames, Int_t n )
			   :_theMother( 0 ), 
			   _theAntiMother( 0 ),
			   _theDaughters( 0 ),
			   _theAntiDaughters( 0 ),
			   _decayString( 0 ),
			   _antiString( 0 )
{
    assert( SetMother( Lookup( theMotherName ) ) );
    for( size_t i=0; i<n; i++ )
	assert( AddDaughter(  Lookup( theDaughterNames[i] ) ) );
}

TDecayTree::TDecayTree( const TDecayTree& o, Bool_t conj)
:_theMother( 0 ) ,
_theAntiMother( 0 ),
_theDaughters( 0 ),
_theAntiDaughters( 0 ),
_decayString( 0 ),
_antiString( 0 ){
    assert( SetMother( (TParticlePDG*)o.Mother(conj) ) );
    for( size_t i=0; i< o.NDaughters(); i++ )
	assert( AddDaughter( o.Daughter(i,conj) ) );    
}

TDecayTree::TDecayTree( const char* theMode )
:_theMother( 0 ), 
_theAntiMother( 0 ),
_theDaughters( 0 ),
_theAntiDaughters( 0 ),
_decayString( 0 ),
_antiString( 0 ){
    if ( theMode == 0 ) return;
    
    // the initial string
    TString string( theMode );
    string = string.Strip( TString::kBoth );
    
    // find the arrow
    Int_t i;
    i = string.Index("->");
    
    // extract the mother
    TString mother( string, i );
    mother = mother.Strip( TString::kBoth );
    if ( !SetMother( Lookup( mother ) ) ) {
	cout << "Failed to set mother " << mother.Data() << endl;
    }
    
    // now the decay products
    string.Remove( 0, i+2 );
    string = string.Strip( TString::kBoth );
    
    while( 1 )
    {
	i=string.First(" ");
	
	if( i < 0 )
	{
	    if ( !AddDaughter( Lookup( TString( string, i ) ) ) ) {
		cout << "Failed to add daughter " << string.Data() << endl;
	    } 
	}
	else
	{
	    if ( ! AddDaughter( Lookup( string ) ) ) {
		cout << "Failed to add daughter " << string.Data() << endl;
	    } 
	    break;
	}
	
	string.Remove( 0, i+1 );
	string = string.Strip( TString::kBoth );
	
    }
    
}

TDecayTree::TDecayTree( const TParticlePDG* theMother )
:_theMother(0),
_theAntiMother(0),
_theDaughters(0),
_theAntiDaughters(0),
_decayString(0),
_antiString(0){
    assert(SetMother(theMother));     
}

TDecayTree::TDecayTree( const TCandidate* cand )
:_theMother(0),
_theAntiMother(0),
_theDaughters(0),
_theAntiDaughters(0),
_decayString(0),
_antiString(0){
    
    if ( cand == 0 ) return;
    
    const TParticlePDG* pdt = cand->PdtEntry();
    
    // the mother
    assert( SetMother(cand->PdtEntry()) );
    
    // now do the daughters,
    // this will recurse all the way down the tree. . .
    TCandListIterator itr = cand->DaughterIterator();
    
    TCandidate* dau=0;
    int i=0;
    while ( dau = itr.Next() ) {
	if ( dau == 0 ) continue;
	TDecayTree* dauMode = new TDecayTree(dau);
	AddDaughter(dauMode);
    }
}

//--------------
// Destructor --
//--------------

TDecayTree::~TDecayTree() {
    if ( _theDaughters ) {
	_theDaughters->Delete();
    }
    delete _theDaughters;
    if ( _theAntiDaughters ) {
	_theAntiDaughters->Delete();
    }
    delete _theAntiDaughters;
    delete _decayString;
    delete _antiString;
}

//---------------
//   Operators
//---------------

TDecayTree& 
TDecayTree::operator= ( const TDecayTree& other){
    assert ( SetMother ( other.Mother() ) );
    for( size_t i=0; i< other.NDaughters(); i++ )
	assert( AddDaughter( other.Daughter(i) )); 
    return *this;
}

Bool_t 
TDecayTree::operator==( const TDecayTree& other) const{
    // might get lucky
    if ( this == &other ) return kTRUE;
    
    const TDecayTree* m1 = this;
    const TDecayTree* m2 = &other;
    
    // compare this and the other
    switch ( PtrCompare(m1,m2) ) {
    case TDecayTree::less:
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;    
    }
    
    // compare the daughters in order
    switch ( ListCompare(Daughters(),other.Daughters() ) ) {
    case TDecayTree::less:
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;
    }
    
    // recurse 
    switch ( DaughterCompare(Daughters(),other.Daughters(),kTRUE) ) {
    case TDecayTree::less:
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;
    }  
    
    // everything ok, they are equal
    return kTRUE;
}

Bool_t 
TDecayTree::operator!=( const TDecayTree& other) const{
    // do the obvious
    return ! (*this == other);
}

Bool_t 
TDecayTree::operator< (const TDecayTree& other) const{
    
    const TDecayTree* m1 = this;
    const TDecayTree* m2 = &other;
    
    // compare this and the other
    switch ( PtrCompare(m1,m2) ) {
    case TDecayTree::less:
	return kTRUE;
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;    
    }
    
    
    // compare the daughters in order
    switch ( ListCompare(Daughters(),other.Daughters() ) ) {
    case TDecayTree::less:
	return kTRUE;
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;
    }
    
    // recurse 
    switch ( DaughterCompare(Daughters(),other.Daughters(),kTRUE) ) {
    case TDecayTree::less:
	return kTRUE;
    case TDecayTree::greater:
	return kFALSE;
    case TDecayTree::equal:
	break;
    }  
    
    // everything is equal, 
    return TDecayTree::equal;
}

Bool_t
TDecayTree::IsWildcard( ) const {
    return LundId() == 0;
}

Bool_t
TDecayTree::NoDaughters( ) const {
    return _theDaughters == 0 ? kTRUE : _theDaughters->GetSize() == 0;
}

Bool_t 
TDecayTree::IsSelfConjugate() const
{
    return _theMother==_theAntiMother;
}


Bool_t
TDecayTree::IsSubMode(const TDecayTree* other, Bool_t conj, 
			Bool_t recurse) const {
    
    // check the mothers
    if ( ! PtrSubMode(this,other,conj) ) return kFALSE;
    
    // check the daughters
    const TList* dauVect1 = 
	Daughters(conj);
    const TList* dauVect2 = 
	other != 0 ? other->Daughters() : 0;
    
    if ( !ListSubMode(dauVect1,dauVect2) ) return kFALSE;
    
    // and recurse
    if ( recurse ) { 
	if ( !DaughterSubMode(dauVect1,dauVect2,kTRUE) ) return kFALSE;
    }
    
    // everything ok, it's a match
    return kTRUE;
}

Bool_t 
TDecayTree::IsSubModeOf( const TDecayTree* other, Bool_t conj,
			  Bool_t recurse ) const {
    // check the mothers
    if ( ! PtrSubMode(other,this,conj) ) return kFALSE;
    
    // check the daughters
    const TList* dauVect1 = 
	Daughters(conj);
    const TList* dauVect2 = 
	other != 0 ? other->Daughters() : 0;
    
    if ( !ListSubMode(dauVect2,dauVect1) ) return kFALSE;
    
    // and recurse
    if ( recurse ) { 
	if ( !DaughterSubMode(dauVect2,dauVect1,kTRUE) ) return kFALSE;
    }
    
    // everything ok, it's a match
    return kTRUE;
}

Bool_t
TDecayTree::IsMatch(const TDecayTree* other, Bool_t conj, 
		      Bool_t recurse) const {
    
    // check the mothers
    if ( ! PtrMatch(this,other,conj) ) return kFALSE;
    
    // check the daughters
    const TList* dauVect1 = Daughters(conj);
    const TList* dauVect2 = other->Daughters();
    
    if ( !ListMatch(dauVect1,dauVect2) ) return kFALSE;
    
    // and recurse
    if ( recurse ) { 
	if ( !DaughterMatch(dauVect1,dauVect2,kTRUE) ) return kFALSE;
    }
    
    // everything ok, it's a match
    return kTRUE;
}


void
TDecayTree::PrintOn(std::ostream& o, Bool_t recurse, Bool_t conj) const {
    o << DecayString(conj) << endl;  
    if ( ! recurse ) return;
    for (int i=0; i < NDaughters(); ++i) {
	TDecayTree* dau = (TDecayTree*) Daughter(i);
	if ( dau != 0 && dau->NDaughters() != 0 ) {
	    dau->PrintOn(o,kTRUE,conj);
	}
    }
}

Int_t
TDecayTree::LundId( Bool_t conj ) const {
    return Mother(conj) == 0 ? 0 : Mother(conj)->PdgCode();
}

//--------------------
//  Selectors
//--------------------

const TParticlePDG * 
TDecayTree::Mother( Bool_t conjugate ) const
{
    if( conjugate ) return _theAntiMother;
    else            return _theMother;
}


const TParticlePDG* 
TDecayTree::PdtDaughter(const int& i, Bool_t conj) const{  
    const TDecayTree* mode = Daughter(i,conj);
    return mode == 0 ? 0 : mode->Mother();
}

const TDecayTree*
TDecayTree::Daughter(const int& i, Bool_t conj) const{
    if ( NDaughters() == 0 ) return 0;
    if ( i < 0 || i >= NDaughters() ) return 0;
    return (TDecayTree*)(*Daughters(conj)).At(i);
}


int 
TDecayTree::NDaughters() const
{  
    if ( _theDaughters == 0 ) return 0;
    return _theDaughters->GetSize();
}

const TDecayTree*
TDecayTree::FindDaughter(const TParticlePDG* pdt, Bool_t conj) const {
    TDecayTree mode((TParticlePDG*)pdt);
    return FindDaughter(&mode,conj);
}

const TDecayTree*
TDecayTree::FindDaughter(const TDecayTree* mode, Bool_t conj) const {
    const TList* vect = Daughters(conj);
    if ( vect == 0 ) return 0;
    return (TDecayTree*) vect->FindObject(mode);
}

//----------------
//  Modifiers
//----------------

Bool_t
TDecayTree::SetMother( const TParticlePDG* theMother ){
    _theMother = theMother;  
    _theAntiMother = _theMother == 0 ? 0 : CPConjugate( theMother );
    return kTRUE;
}

const TDecayTree*
TDecayTree::SetDaughterDecay( const TDecayTree* dMode ){
    if ( dMode == 0 ) return 0;
    if ( dMode->IsWildcard() ) return 0;
    delete _decayString;
    delete _antiString;
    
    const TParticlePDG* modePdt = dMode->Mother();
    TDecayTree baseMode(modePdt);
    
    // remove, and re-insert ( to insure that it end up in the right place
    TList* vect = Daughters();
    if ( vect == 0 ) return 0;
    TDecayTree* oldMode = (TDecayTree*) vect->Remove(&baseMode);
    if ( ! oldMode ) return 0;  
    delete oldMode;
    vect->Add((TDecayTree*)dMode);
    
    TDecayTree* conjBaseMode = new TDecayTree(CPConjugate(modePdt));
    vect = Daughters(kTRUE);
    if ( vect == 0 ) return 0;
    oldMode = (TDecayTree*) vect->Remove(conjBaseMode);
    if ( ! oldMode ) return 0;  
    delete oldMode;
    TDecayTree* conjMode = new TDecayTree(*dMode,kTRUE);
    vect->Add(conjMode);
    
    return dMode;
}

const TDecayTree*
TDecayTree::SetDaughterDecay( const char* dModeStr){
    TDecayTree* newMode = new TDecayTree(dModeStr);
    return SetDaughterDecay(newMode);
}

Bool_t
TDecayTree::AddDaughter( const  TParticlePDG* aDaughter ){  
    if( aDaughter==0 ) return kFALSE;
    TDecayTree dMode(aDaughter);
    return AddDaughter(dMode);
}

Bool_t 
TDecayTree::AddDaughter( const TDecayTree* mode ){
    if ( mode == 0 ) return kFALSE;
    return AddDaughter(*mode);
}

Bool_t
TDecayTree::AddDaughter( const TDecayTree& mode ){  
    if ( _theDaughters == 0 ) {
	_theDaughters = new TList;
	_theAntiDaughters = new TList;
    }
    TDecayTree* newMode = new TDecayTree(mode);
    TDecayTree* conjMode = new TDecayTree(mode,kTRUE);
    _theDaughters->Add(newMode);
    _theAntiDaughters->Add(conjMode );
    return kTRUE;
}

TList* 
TDecayTree::Daughters( Bool_t conjugate )
{
    return conjugate ? _theAntiDaughters : _theDaughters;
}

const TList* 
TDecayTree::Daughters( Bool_t conjugate ) const
{
    return conjugate ? _theAntiDaughters : _theDaughters;
}


//RWTPtrSlistIterator< TParticlePDG >
//TDecayTree::daughterIterator( Bool_t conjugate ) const
//{
//  if( conjugate ) 
//    return RWTPtrSlistIterator< TParticlePDG > ( const_cast<RWTPtrSlist<TParticlePDG>&>(*_theAntiDaughters) );
///  else            
//    return RWTPtrSlistIterator< TParticlePDG > ( const_cast<RWTPtrSlist<TParticlePDG>&>(*_theDaughters) );
//}


const TString&
TDecayTree::DecayString( Bool_t conjugate ) const {
    
    TString* string = conjugate ? _antiString : _decayString;
    
    if ( string != 0 ) {
	return *string;
    }
    string = new TString;
    (*string) += Mother( conjugate )->GetName();
    (*string) += " ->";  
    
    const int nDau = NDaughters();
    
    for ( int i(nDau-1); i >= 0; --i ) {
	
	//    const TParticlePDG* pdt;
	// pdt=pdtDaughter(i,conjugate);
	//if ( pdt == 0 ) continue;
	//if ( pdt->LundId() == PdtLund::null ) {
	//   (*string) += " X";
	//   continue;
	//}
	//(*string) += " ";
	//(*string) += pdt->name();
	TDecayTree* dau = (TDecayTree*)Daughter(i,conjugate);
	TString dauString = dau->DecayString();
	size_t iDau;
	iDau = dauString.Index("->");
	TString dauName(dauString,iDau);
	(*string) += " ";
	(*string) += dauName;
	if ( dau->NDaughters() > 0 ) {
	    // find the arrow
	    dauString.Remove( 0, iDau+2 );
	    (*string) += "{";
	    (*string) += dauString;
	    (*string) += "}";
	}
	//
	// 
    }
    return (*string);
}


const TParticlePDG* 
TDecayTree::Lookup( const TString& str ) const{
    if ( str == "X" ) return 0;
    return TRho::Instance()->GetPDG()->GetParticle(str);
}

const TParticlePDG*
TDecayTree::CPConjugate( const TParticlePDG* thePart ) const {
    TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    
    TString name = thePart->GetName();
    if (name.Index("-")>0)
	name.ReplaceAll("-","+");
    else
	name.ReplaceAll("+","-");
    
    return pdt->GetParticle(name);
}

Bool_t
TDecayTree::HasWildcard( const TList* list ) {
    if ( list == 0 ) return kFALSE;
    for ( int i=0; i < list->GetSize(); ++i ) {
	if ( ((TDecayTree*)(*list).At(i))->IsWildcard() ) return kTRUE;
    }
    return kFALSE;
}

//-------------------
//  Help Functions
//-------------------

Bool_t 
TDecayTree::PtrSubMode( const TDecayTree* m1,
			 const TDecayTree* m2,
			 Bool_t conj ) {
    // could get lucky
    if ( m1 == m2 ) return kTRUE;
    
    // check for wildcards
    if ( (m1 != 0 && m1->IsWildcard() ) ) return kTRUE; 
    
    // check for nulls
    if ( m1 == 0 || m2 == 0 ) return kFALSE;
    
    // check the lund id's
    return m1->LundId(conj) == m2->LundId();
    
}

Bool_t 
TDecayTree::ListSubMode( const TList* l1,
			  const TList* l2 ) {
    
    // could get lucky
    if ( l1 == l2 ) return kTRUE;
    
    // so iterate list 1, 
    // starting from the end, check the other list in parallel
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {
	--iD1;
	--iD2;
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD2 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	// a wild card matches rest of list, so break
	if ( m1 != 0 && m1->IsWildcard() ) break;
	
	if ( PtrSubMode(m1,m2) ) {
	    continue; 
	} else {
	    return kFALSE;
	}
    }
    
    // everything matches
    return kTRUE;
}

Bool_t 
TDecayTree::DaughterSubMode( const TList* l1,
			      const TList* l2,
			      Bool_t recurse ) {  
    // could get lucky
    if ( l1 == l2 ) return kTRUE;
    
    // so iterate list 1, 
    // starting from the end, check the other list in parallel
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {
	--iD1;
	--iD2;
	
	// get the modes
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD2 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	// could get lucky
	if ( m1 == m2 ) continue;
	
	// a wild card matches rest of list, so break
	if ( m1 != 0 && m1->IsWildcard() ) break;
	
	// get the daughters 
	const TList* d1 = 
	    m1 != 0 ? m1->Daughters() : 0;
	const TList* d2 = 
	    m2 != 0 ? m2->Daughters() : 0;
	
	if ( ! ListSubMode(d1,d2) ) { 
	    return kFALSE; 
	}
	
	if ( recurse ) {
	    if ( ! DaughterSubMode(d1,d2,kTRUE) ) {
		return kFALSE;
	    }
	}
    }
    
    // everything matches 
    return kTRUE;
}

Bool_t 
TDecayTree::PtrMatch( const TDecayTree* m1,
		       const TDecayTree* m2,
		       Bool_t conj ) {
    
    // could get lucky
    if ( m1 == m2 ) return kTRUE;
    
    // check for wildcards
    if ( (m1 != 0 && m1->IsWildcard() ) || 
	(m2 != 0 && m2->IsWildcard() ) ) return kTRUE;
    
    // check for nulls
    if ( m1 == 0 || m2 == 0 ) return kFALSE;
    
    // check the lund id's
    return m1->LundId(conj) == m2->LundId();
    
}

Bool_t 
TDecayTree::ListMatch( const TList* l1,
			const TList* l2 ) {
    
    // could get lucky
    if ( l1 == l2 ) return kTRUE;
    
    // so iterate on the lists in parrallel
    // starting from the end
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {
	--iD1;
	--iD2;
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD2 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	// wildcard match the rest of the list
	if ( m1 != 0 && m1->IsWildcard() ) break;
	if ( m2 != 0 && m2->IsWildcard() ) break;
	
	if ( PtrMatch(m1,m2) ) {
	    continue; 
	} else {
	    return kFALSE;
	}
    }
    
    // everything matches 
    return kTRUE;
}

Bool_t 
TDecayTree::DaughterMatch( const TList* l1,
			    const TList* l2,
			    Bool_t recurse ) {
    // could get lucky
    if ( l1 == l2 ) return kTRUE;
    
    // so iterate list 1, 
    // starting from the end, check the other list in parallel
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {    
	--iD1;
	--iD2;
	
	// get the modes
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD1 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	// could get lucky
	if ( m1 == m2 ) continue;
	
	// a wild card matches rest of list, so break
	if ( m1 != 0 && m1->IsWildcard() ) break;
	if ( m2 != 0 && m2->IsWildcard() ) break;
	
	// get the daughters 
	const TList* d1 = 
	    m1 != 0 ? m1->Daughters() : 0;
	const TList* d2 = 
	    m2 != 0 ? m2->Daughters() : 0;
	
	if ( ! ListMatch(d1,d2) ) { 
	    return kFALSE; 
	}
	
	if ( recurse ) {
	    if ( ! DaughterMatch(d1,d2,kTRUE) ) {
		return kFALSE;
	    }
	}
    }
    
    // everything matches
    return kTRUE;
}

TDecayTree::compare 
TDecayTree::PtrCompare( const TDecayTree* m1, 
			 const TDecayTree* m2,
			 Bool_t conj ) {
    // could get lucky
    if ( m1 == m2 ) return TDecayTree::equal;
    
    // null ptr is alway less than anything else
    if ( m1 == 0 ) return TDecayTree::less;
    if ( m2 == 0 ) return TDecayTree::greater;
    
    // check the abs lund id 
    double l1 = m1->LundId(conj);
    double l2 = m2->LundId();
    
    if ( fabs(l1) < fabs(l2) ) { 
	return TDecayTree::less; 
    } else if ( fabs(l1) > fabs(l2) ) {
	return TDecayTree::greater; 
    } else if ( l1 < l2 ) {
	return TDecayTree::less; 
    } else if ( l1 > l2 ) {
	return TDecayTree::greater; 
    }
    
    // everything is equal 
    return TDecayTree::equal; 
    
}

TDecayTree::compare 
TDecayTree::ListCompare( const TList* l1,
			  const TList* l2 ) {
    // could get lucky
    if ( l1 == l2 ) return TDecayTree::equal;
    
    // null ptr is less that any value
    if ( l1 == 0 ) return TDecayTree::less;
    if ( l2 == 0 ) return TDecayTree::greater;
    
    // both lists exist, 
    // so iterate on the lists in parrallel
    // starting from the end
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {    
	--iD1;
	--iD2;
	
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD2 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	switch ( PtrCompare(m1,m2) ) {
	case TDecayTree::less:
	    return TDecayTree::less;
	case TDecayTree::greater:
	    return TDecayTree::greater;
	case TDecayTree::equal:
	    break;
	}
    }
    
    // everything is equal, 
    return TDecayTree::equal;
    
}

TDecayTree::compare 
TDecayTree::DaughterCompare( const TList* l1,
			      const TList* l2,
			      Bool_t recurse ) {
    
    // could get lucky
    if ( l1 == l2 ) return TDecayTree::equal;
    
    // null ptr is less that any value
    if ( l1 == 0 ) return TDecayTree::less;
    if ( l2 == 0 ) return TDecayTree::greater;
    
    // both lists exist, 
    // so iterate on the lists in parrallel
    // starting from the end
    int iD1 = l1 != 0 ? l1->GetSize() : 0;
    int iD2 = l2 != 0 ? l2->GetSize() : 0;
    
    while ( iD1 >= 0 || iD2 >= 0 ) {    
	--iD1;
	--iD2;
	
	const TDecayTree* m1 = iD1 >= 0 ? (TDecayTree*)(*l1).At(iD1) : 0;
	const TDecayTree* m2 = iD2 >= 0 ? (TDecayTree*)(*l2).At(iD2) : 0;
	
	// could get lucky
	if ( m1 == m2 ) continue;
	
	// null ptr is less that any value
	if ( m1 == 0 ) return TDecayTree::less;
	if ( m2 == 0 ) return TDecayTree::greater;
	
	// check the lund ids
	const TList* d1 = m1->Daughters();
	const TList* d2 = m2->Daughters();
	
	switch ( ListCompare(d1,d2) ) {
	case TDecayTree::less:
	    return TDecayTree::less;
	case TDecayTree::greater:
	    return TDecayTree::greater;
	case TDecayTree::equal:
	    if ( ! recurse ) continue;
	    break;
	}
	
	// if recurse
	switch ( DaughterCompare(d1,d2,kTRUE) ) {
	case TDecayTree::less:
	    return TDecayTree::less;
	case TDecayTree::greater:
	    return TDecayTree::greater;
	case TDecayTree::equal:
	    break;
	}
    }
    
    // everything is equal, 
    return TDecayTree::equal;
    
}








