//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TTreeNavigator.cxx,v 1.3 2002-02-01 23:00:23 marcel Exp $
//
// Description:
//	Class TTreeNavigator
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   Oct, 1998  - creation
//
// Copyright Information:
//	Copyright (C) 1998	       CEA - Centre d'Etude de Saclay
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TTreeNavigator.h"
#include "RhoTools/TPrintTree.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"

ClassImp(TTreeNavigator)

TBuffer &operator>>(TBuffer &buf, TTreeNavigator *&obj)
{
   obj = (TTreeNavigator *) buf.ReadObject(TTreeNavigator::Class());
   return buf;
}

#include <iostream>
using namespace std;

//
//  Constructor
//
TTreeNavigator::TTreeNavigator(TCandidate& tree )
{
    _theTree = &tree;
    fillLists(*_theTree);
}


//
// Recursive functions
//

void
TTreeNavigator::fillLists( const TCandidate& c ) 
{
    const VAbsVertex* decayVtx=0;
    if( c.NDaughters()!=0)
    {
	_unstableCands.Add( (TObject*)&c );
	if (decayVtx=c.DecayVtx()) {
	    if( !_vertices.Contains( (TObject*)decayVtx ) )
		_vertices.Add( (TObject*)decayVtx );
	}
	TCandidate* dau=0;
	TCandListIterator iterDau=c.DaughterIterator();
	while( dau=iterDau.Next() )
	{
	    fillLists( *dau );
	}
    }
    else
    {
	_finalCands.Add( (TObject*)&c );
    }
}

void 
TTreeNavigator::PrintCand( const TCandidate& c )
{
    TPrintTree prtTree;
    cout << (prtTree.Print(c)).Data();
    cout <<  ( (c.IsComposite() ) ? "Tree UID:" : "Cand UID:" );
    cout << c.Uid(); //  << " local cand:" << c.thisCandidate();
    cout << " mass: " << c.Mass() << " p: " << c.P() << " pv " ;
    cout << " th/ph " << c.P3().Theta() << "/" << c.P3().Phi() ;
    
    if (c.ProductionVtx()!=0)
	cout << *(c.ProductionVtx());
    else 
	cout << "(none)";
    
    cout << " mom UID: " << (c.TheMother() ? c.TheMother()->Uid() : 0 );
    const TParticlePDG* pdt;
    if( pdt=c.PdtEntry() ) cout << " " << pdt->GetName();
    cout << endl;
    if( c.DecayVtx()!=0 ) {
	cout << "    Decay ";
	PrintVertex( *c.DecayVtx() );
    }
}

void 
TTreeNavigator::PrintTree( const TCandidate& c )
{
    PrintCand( c );
    TCandidate* dau=0;
    TCandListIterator iterDau=c.DaughterIterator();
    while( dau=iterDau.Next() )       
	PrintTree( *dau );
}

void 
TTreeNavigator::PrintVertex( const VAbsVertex& v )
{
    if (&v==0) return;
    TLorentzVector pos = v.V4();
    cout << "Vtx at (" << pos.X() << "," << pos.Y() << "," << pos.Z() << ";" << pos.T()  << ") incoming ";
    const TCandidate* inComing = v.InComingCand();
    if ( inComing!=0 ) 
    {
	cout << (v.InComingCand()->Uid());
	cout << " M " << v.Mass() << " p " << v.P();
    }
    else cout << 0 ;
    cout << " nout " << v.NOutGoing();
    cout << " res " << v.NResonance();
    cout << endl;
}

void 
TTreeNavigator::PrintOn( std::ostream& o ) const
{
    size_t i;
    o << "Number of vertices : " << NVertex() << endl;
    o << "Number of final stable candidates : " << NFinalCand();
    for( i=0; i<_finalCands.GetSize(); i++ )
    {
	const TCandidate* c = (TCandidate*)_finalCands.At(i);
	const TParticlePDG* pdt=0;
	o << " | ";
	if( pdt=c->PdtEntry() )
	    o << pdt->GetName();
	else
	    o << pdt->Mass();
    }
    o << " | " << endl;
    o << "Number of intermediate candidates : " << NUnstableCand();
    for( i=0; i<_unstableCands.GetSize(); i++ )
    {
	const TCandidate* c = (TCandidate*)_unstableCands.At(i);
	const TParticlePDG* pdt=0;
	o << " | ";
	if( pdt=c->PdtEntry() )
	    o << pdt->GetName();
	else
	    o << pdt->Mass();
    }
    o << " | " << endl;
    //  printTree( _theTree, o );
}


Bool_t
TTreeNavigator::IsCloneOf(const TCandidate& theOtherTree, Bool_t checkType)
{
    return _theTree->IsCloneOf(theOtherTree,checkType);
}

  
//
// inline functions
//
int 
TTreeNavigator::NVertex() const
{
    return _vertices.GetSize();
}

TIterator&
TTreeNavigator::VertexIterator()
{
    return *_vertices.MakeIterator();
}

int 
TTreeNavigator::NFinalCand() const
{
    return _finalCands.GetSize();
}

TIterator&
TTreeNavigator::FinalCandIterator()
{
    return *_finalCands.MakeIterator();
}

int 
TTreeNavigator::NUnstableCand() const
{
    return _unstableCands.GetSize();
}

TIterator& 
TTreeNavigator::UnstableCandIterator() 
{
    return *_unstableCands.MakeIterator();
}




