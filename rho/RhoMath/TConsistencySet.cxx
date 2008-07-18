//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConsistencySet			    				//
//                                                                      //
// Combined consistency class						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TConsistencySet.h"

ClassImp(TConsistencySet)

TBuffer &operator>>(TBuffer &buf, TConsistencySet *&obj)
{
   obj = (TConsistencySet *) buf.ReadObject(TConsistencySet::Class());
   return buf;
}

//-------------
// C Headers --
//-------------
#include <assert.h>

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <iomanip>
using namespace std;

//----------------
// Constructors --
//----------------
TConsistencySet::TConsistencySet()
{
}


//--------------
// Destructor --
//--------------
TConsistencySet::~TConsistencySet()
{
}

//-------------
// Methods   --
//-------------
size_t
TConsistencySet::NParents() const
{
    return fConsistencyList.GetSize();
}

TConsistency*        
TConsistencySet::GetConsistency( size_t index )
{
    if( index>=fConsistencyList.GetSize() ) return 0;
    return (TConsistency*) fConsistencyList.At(index);
}

TConsistency*        
TConsistencySet::GetLabelConsistency( const char* label )
{
    return (TConsistency*) fConsistencyList.FindObject( label );
}

const char* 
TConsistencySet::GetLabel( size_t index )
{
    if( index>=fConsistencyList.GetSize() ) return 0;
    return fConsistencyList.At(index)->GetName();
}

Double_t      
TConsistencySet::WorstSignificance()
{
    size_t index;
    if( !WorstSignificanceIndex_( index ) ) return 0.;
    TConsistency* cons = GetConsistency( index );
    assert( cons!=0 );
    return cons->SignificanceLevel();
}

const char* 
TConsistencySet::WorstSignificanceLabel()
{
    size_t index;
    if( !WorstSignificanceIndex_( index ) ) return 0;
    return GetLabel( index );
}

TConsistencySet* 
TConsistencySet::Overlap(TConsistencySet& other)
{
    TConsistencySet* result = 0;
    
    for (size_t i=0; i<this->NParents(); i++ ) {
	if ( 0 != other.GetLabelConsistency(this->GetLabel(i)) ) {
	    if ( 0 == result ) {
		result = new TConsistencySet;
	    }
	    
	    result->Add(this->GetLabel(i), *(this->GetConsistency(i)) );
	}
    }
    
    return result;
}



//-------------
// Operators --
//-------------

Bool_t 
TConsistencySet::operator==(TConsistencySet& rhs) {
    Bool_t retval = kFALSE;
    
    // just in case
    if ( this != &rhs ) {
	
	if ( this->NParents() == rhs.NParents() ) {
	    
	    retval = kTRUE;
	    for (size_t i=0;i<NParents();i++) {
		// order is not guaranteed to be the same even for equal objects
		if ( 0 == rhs.GetLabelConsistency(this->GetLabel(i)) ) {
		    retval = kFALSE;
		    return retval;
		}
	    }
	}
    }
    
    return retval;
}

Bool_t 
TConsistencySet::operator!=(TConsistencySet& rhs) {
    return !(*this == rhs);
}

//-------------
// Selectors --
//-------------

//-------------
// Modifiers --
//-------------

void 
TConsistencySet::Reset()
{
    fConsistencyList.Delete();
}

Bool_t
TConsistencySet::Add( const char* label, TConsistency& c )
{
    Bool_t retval = kFALSE;
    
    // some idiot-proofing
    
    if ( 0 != label ) {
	TString d( label );
	d.ToLower();
	
	// test for overlaps
	size_t index;
	
	if( fConsistencyList.FindObject( label ) == 0 ) {
	    c.SetName(label);
	    fConsistencyList.Add( (TObject*) &c );
	    retval = kTRUE;
	}
    }
    
    return retval;
}

Bool_t 
TConsistencySet::Combine(TConsistencySet& other) {
    Bool_t retval = kTRUE;
    for (size_t i=0;i<other.NParents();i++) {
	retval = retval && Add(other.GetLabel(i),*(other.GetConsistency(i)));
    }
    
    return retval;
}


//
//  Internal protected functions
//

Bool_t   
TConsistencySet::WorstSignificanceIndex_( size_t& index )
{
    Double_t worst( 999. );
    const TConsistency* cons;
    size_t i=0;
    while( cons=GetConsistency(i) )
    {
	Double_t value=cons->SignificanceLevel();
	if( value<worst )
	{
	    index=i;
	    worst=value;
	}
	i++;
    }
    return worst<=1.;
}


void
TConsistencySet::PrintOn(std::ostream& os) const
{
    os << NParents() << " parents:" << endl;
    for (size_t i=0;i<NParents();i++) {
	TConsistency *c = (TConsistency *) fConsistencyList.At(i);
	os << setw(3) << i;
	os << setw(10) << c->GetName() << " : ";
	c->PrintOn(os);
    }
}

