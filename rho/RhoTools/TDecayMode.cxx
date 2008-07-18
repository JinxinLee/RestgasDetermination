//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TDecayMode.cxx,v 1.3 2002-02-01 23:00:09 marcel Exp $
//
// Description:
//	Class TDecayMode - 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//      Paul Harrison & Luca Lista, original Authors of TMode and DecayMode
//
// Copyright Information:
//      Copyright (C) 1998              Saclay & LBNL
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TDecayMode.h"
#include <math.h>
#include "TIterator.h"
#include "RhoBase/TRho.h"

ClassImp(TDecayMode)

TBuffer &operator>>(TBuffer &buf, TDecayMode *&obj)
{
   obj = (TDecayMode *) buf.ReadObject(TDecayMode::Class());
   return buf;
}

#include <iostream>
using namespace std;

TDecayMode::TDecayMode( TParticlePDG* theMother,const TList& theDaughters )
{
    SetMother( theMother );
    for( size_t i=0; i<theDaughters.GetSize(); i++ ) {
	TParticlePDG *p = (TParticlePDG*) theDaughters.At(i);
	AddDaughter( p );
    }
}

TDecayMode::TDecayMode( const TString& theMotherName,const TString theDaughterNames[5] )
{
    TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    SetMother( pdt->GetParticle( theMotherName ) );
    for( size_t i=0; i<5; i++ )
	if (theDaughterNames[i]!="")
	AddDaughter(  pdt->GetParticle( theDaughterNames[i] ) );
    //    assert( makeSense() );
}

TDecayMode::TDecayMode( const TDecayMode& o )
:
_theMother( o._theMother ),
_theAntiMother( o._theAntiMother )
{
    TList *l1 = const_cast<TList *> (&o._theDaughters);
    TList *l2 = const_cast<TList *> (&o._theAntiDaughters);
    for( size_t i=0; i<_theDaughters.GetSize(); i++ ) {
	l1->Add(_theDaughters.At(i));
	l2->Add(_theAntiDaughters.At(i));
    }
}

TDecayMode::TDecayMode( const char* theMode )
{
    TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    
    // the initial string
    TString string( theMode );
    string = string.Strip(TString::kBoth);
    
    // find the arrow
    Int_t i;
    i = string.Index("->");
    
    // extract the mother
    TString mother( string, i );
    mother = mother.Strip(TString::kBoth);
    SetMother( pdt->GetParticle( mother ) );
    
    // now the decay products
    string.Remove( 0, i+2 );
    string = string.Strip(TString::kBoth);
    
    while( 1 )
    {
	i=string.First(" ");
	
	if( i > 0 )
	{
	    AddDaughter( pdt->GetParticle( TString( string, i ) ) );
	}
	else
	{
	    AddDaughter( pdt->GetParticle( string ) );
	    break;
	}
	
	string.Remove( 0, i+1 );
	string = string.Strip(TString::kBoth);
	
    }
    
}

Bool_t
TDecayMode::SetMother( TParticlePDG* theMother )
{
    if( theMother==0 ) return kFALSE;
    _theMother = theMother;
    _theAntiMother = CPConjugate( theMother );
    return kTRUE;
}

TParticlePDG*
TDecayMode::CPConjugate( TParticlePDG* thePart )
{
    TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    
    TString name = thePart->GetName();
    if (name.Index("-")>0)
	name.ReplaceAll("-","+");
    else
	name.ReplaceAll("+","-");
    
    return pdt->GetParticle(name);
}

Bool_t
TDecayMode::AddDaughter( TParticlePDG* aDaughter )
{
    if( aDaughter==0 ) return kFALSE;
    _theDaughters.Add( aDaughter );
    _theAntiDaughters.Add( CPConjugate( aDaughter ) );
    return kTRUE;
}

const TParticlePDG * 
TDecayMode::Mother( Bool_t conjugate ) const
{
    if( conjugate ) return _theAntiMother;
    else            return _theMother;
}

Bool_t 
TDecayMode::IsSelfConjugate() const
{
    return _theMother==_theAntiMother;
}

// access to daughters
int 
TDecayMode::NDaughters() const
{
    return _theDaughters.GetSize();
}

const TList& 
TDecayMode::Daughters( Bool_t conjugate ) const
{
    if( conjugate ) return _theAntiDaughters;
    else            return _theDaughters;
}

TIter
TDecayMode::DaughterIterator( Bool_t conjugate ) const
{
    if( conjugate ) 
	return TIter(&_theAntiDaughters);
    else            
	return TIter(&_theDaughters);
}


TString 
TDecayMode::DecayString( Bool_t conjugate ) const
{
    TString s;
    s += Mother( conjugate )->GetName();
    s += " ->";
    TIter iter( DaughterIterator( conjugate ) );
    TParticlePDG* pdt;
    while( pdt=(TParticlePDG*) iter.Next() )
    {
	s += " ";
	s += pdt->GetName();
    }
    
    //
    // 
    return s;
}

//--------------
// Destructor --
//--------------

TDecayMode::~TDecayMode() 
{
}



