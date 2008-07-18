//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TFastCombiner.cxx,v 1.3 2002-02-01 23:00:13 marcel Exp $
//
// Description:
//	Class TFastCombiner - 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Gautier Hamel de Monchenault
//      Fernando Martinez-Vidal     
//
// Copyright Information:
//      Copyright (C) 1999              Saclay & Paris VI
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TFastCombiner.h"
#include <assert.h>
#include <math.h>
#include "TLorentzVector.h"
#include "RhoBase/VAbsPidSelector.h"
#include "RhoTools/TDecayMode.h"
#include "RhoTools/TOpMakeTree.h"

ClassImp(TFastCombiner)

TBuffer &operator>>(TBuffer &buf, TFastCombiner *&obj)
{
   obj = (TFastCombiner *) buf.ReadObject(TFastCombiner::Class());
   return buf;
}

#include <iostream>
using namespace std;

TFastCombiner::TFastCombiner(TDecayMode& mode, VAbsPidSelector* theSelector)
: _theDecayMode( &mode ), _theSelector( theSelector ), _sameLists( kFALSE ), 
  _checkType( kTRUE ), _doWrongChargeOnly( kFALSE ),  _theIterator(0), _theList(0)
{
}

TFastCombiner::TFastCombiner( TDecayMode& mode,	
				 TCandList* list0,
				 TCandList* list1,
				 TCandList* list2,
				 TCandList* list3,
				 TCandList* list4,
				 TCandList* list5,
				 VAbsPidSelector* theSelector)
				 : _theDecayMode( &mode ), _theSelector( theSelector ), _sameLists( kFALSE ), 
				   _checkType( kTRUE ), _doWrongChargeOnly( kFALSE ), _theIterator(0), _theList(0)
{
    TList input;
    if (list0!=0) input.Add(list0);
    if (list1!=0) input.Add(list1);
    if (list2!=0) input.Add(list2);
    if (list3!=0) input.Add(list3);
    if (list4!=0) input.Add(list4);
    if (list5!=0) input.Add(list5);
    Combine( input );
}

TFastCombiner::TFastCombiner( TDecayMode& mode,	
				 TList& ll,VAbsPidSelector* theSelector)
				 : _theDecayMode( &mode ), _theSelector( theSelector ), 
				   _checkType( kTRUE ), _sameLists( kFALSE ), _doWrongChargeOnly( kFALSE ), _theIterator(0)
{
    Combine( ll );
}

TFastCombiner::~TFastCombiner() 
{
    Reset();
}

void 
TFastCombiner::Reset()
{
    delete _theList;
    _theList = 0;
    delete _theIterator;
    _theIterator=0;
}

void
TFastCombiner::Combine( TList& ll )
{
    if (_theList==0) _theList = new TCandList;

    TCandidate* cand0=0;
    TCandidate* cand1=0;
    TCandidate* cand2=0;
    TCandidate* cand3=0;
    TCandidate* cand4=0;
    TCandidate* cand5=0;
    
    Int_t nLists = ll.GetSize();
    TCandList* candList0=0;
    TCandList* candList1=0;
    TCandList* candList2=0;
    TCandList* candList3=0;
    TCandList* candList4=0;
    TCandList* candList5=0;
    if( nLists>0 ) {
	candList0 = (TCandList*) ll.At(0);
	_sameLists = kTRUE;
    }
    assert( candList0!=0 );
    if( nLists>1 ) {
	candList1 = (TCandList*) ll.At(1);
	_sameLists = _sameLists && candList1==candList0;
    }
    if( nLists>2 ) {
	candList2 = (TCandList*) ll.At(2);
	_sameLists = _sameLists && candList2==candList1;
    }
    if( nLists>3 ) {
	candList3 = (TCandList*) ll.At(3);
	_sameLists = _sameLists && candList3==candList2;
    }
    if( nLists>4 ) {
	candList4 = (TCandList*) ll.At(4);
	_sameLists = _sameLists && candList4==candList3;
    }
    if( nLists>5 ) {
	candList5 = (TCandList*) ll.At(5);
	_sameLists = _sameLists && candList5==candList4;
    }
    
    TCandListIterator iter0(*candList0);
    while ( cand0 = iter0.Next() ) {
	if (candList1 != 0) {
	    TCandListIterator iter1(iter0);  
	    if (candList1 != candList0) iter1 = TCandListIterator(*candList1);
	    while ( cand1 = iter1.Next() ) {
		if ( cand1->Overlaps(*cand0) ) continue;
		if (candList2 != 0) {
		    TCandListIterator iter2(iter1);  
		    if (candList2 != candList1) iter2 = TCandListIterator(*candList2);
		    while ( cand2 = iter2.Next() ) {
			if ( cand2->Overlaps(*cand0) ) continue;
			if ( cand2->Overlaps(*cand1) ) continue;
			if (candList3 != 0) {
			    TCandListIterator iter3(iter2);  
			    if (candList3 != candList2) iter3 = TCandListIterator(*candList3);
			    while ( cand3 = iter3.Next() ) {
				if ( cand3->Overlaps(*cand0) ) continue;
				if ( cand3->Overlaps(*cand1) ) continue;
				if ( cand3->Overlaps(*cand2) ) continue;
				if (candList4 != 0) {
				    TCandListIterator iter4(iter3);  
				    if (candList4 != candList3) iter4 = TCandListIterator(*candList4);
				    while ( cand4 = iter4.Next() ) {
					if ( cand4->Overlaps(*cand0) ) continue;
					if ( cand4->Overlaps(*cand1) ) continue;
					if ( cand4->Overlaps(*cand2) ) continue;
					if ( cand4->Overlaps(*cand3) ) continue;
					if (candList5 != 0) {
					    TCandListIterator iter5(iter4);  
					    if (candList5 != candList4) iter5 = TCandListIterator(*candList5);
					    while ( cand5 = iter5.Next() ) {
						if ( cand5->Overlaps(*cand0) ) continue;
						if ( cand5->Overlaps(*cand1) ) continue;
						if ( cand5->Overlaps(*cand2) ) continue;
						if ( cand5->Overlaps(*cand3) ) continue;
						if ( cand5->Overlaps(*cand4) ) continue;
						Combination_(cand0,cand1,cand2,cand3,cand4,cand5);
					    }
					} else {
					    Combination_(cand0,cand1,cand2,cand3,cand4);
					}
				    }
				} else {
				    Combination_(cand0,cand1,cand2,cand3);
				}
			    }
			} else {
			    Combination_(cand0,cand1,cand2);
			}
		    }
		} else {
		    Combination_(cand0,cand1);
		}      
	    }
	} else {
	    Combination_(cand0);
	}
    }

    _theList->RemoveClones(); // Remove second order double counting entries
}

void 
TFastCombiner::Combination_( TCandidate* cand0, 
			      TCandidate* cand1,
			      TCandidate* cand2,
			      TCandidate* cand3,
			      TCandidate* cand4,
			      TCandidate* cand5   )
{
    TLorentzVector m = cand0->P4();
    double charge = cand0->Charge();
    if (cand1!=0) { m+=cand1->P4(); charge+=cand1->Charge(); }
    if (cand2!=0) { m+=cand2->P4(); charge+=cand2->Charge(); }
    if (cand3!=0) { m+=cand3->P4(); charge+=cand3->Charge(); }
    if (cand4!=0) { m+=cand4->P4(); charge+=cand4->Charge(); }
    if (cand5!=0) { m+=cand5->P4(); charge+=cand5->Charge(); }
    
    if (!CheckChargeAndMass(m,charge,0.0,10.0)) return;
    
    //
    assert( cand0!=0 );
    TCandList theList;
    theList.Append( *cand0 );
    if (cand1!=0) theList.Append( *cand1 );
    if (cand2!=0) theList.Append( *cand2 );
    if (cand3!=0) theList.Append( *cand3 );
    if (cand4!=0) theList.Append( *cand4 );
    if (cand5!=0) theList.Append( *cand5 );
 
    // Check the daughter types
    if (_checkType) {
	Int_t nOK = 0;
	TCandListIterator iter(theList);
	TCandidate *c;
	while (c=iter.Next()) {
	    TIter daug = _theDecayMode->DaughterIterator();		
	    TParticlePDG* d;
	    while (d=(TParticlePDG*)daug.Next()) {
		const TParticlePDG* cand = c->PdtEntry();
		if (d==cand) nOK++;
	    }
	}

	if (nOK != _theDecayMode->NDaughters()) return;
    }

    // build the candidate
    TCandListIterator listIter( theList );
    TOpMakeTree comb;
    TCandidate* tempCand = comb.CreateFromList( listIter );

    if (_theSelector!=0)
	if (!_theSelector->Accept(*tempCand)) { return; }

    // check if the charge is correct
    if (_doWrongChargeOnly) 
    {
	// add to the list
	_theList->Append( *tempCand );
    }
    else
    {

	if ( TMath::Abs( tempCand->Charge() ) == TMath::Abs(_theDecayMode->Mother()->Charge()) ) 
	{ 
	    if ( tempCand->Charge() == _theDecayMode->Mother()->Charge() ) 
		tempCand->SetType(_theDecayMode->Mother());
	    else
		tempCand->SetType(_theDecayMode->Mother(kTRUE));        
	    // add to the list
	    _theList->Append( *tempCand );
	}
    }
}



TCandidate*
TFastCombiner::Next()
{
    if( _theIterator==0 ) 
	_theIterator = new TCandListIterator( *_theList );
    return _theIterator->Next();
}

void 
TFastCombiner::Rewind()
{
    delete _theIterator;
    _theIterator = new TCandListIterator( *_theList );
}


Bool_t
TFastCombiner::CheckChargeAndMass(const TLorentzVector& pQ, double charge,
				    double lowMass, double hiMass)
{
    // check charge
    int decayModeCharge = int(_theDecayMode->Mother()->Charge());
    if (_doWrongChargeOnly) 
    {
	if (charge == decayModeCharge) return kFALSE;
    }
    else
    {
	if (TMath::Abs(charge) != TMath::Abs(decayModeCharge)) return kFALSE;
    }
    
    // check mass
    double mass = pQ.M();
    if (mass<lowMass || mass>hiMass) return kFALSE;
    
    return kTRUE;
}


