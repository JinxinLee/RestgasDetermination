//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConsistency				    				//
//                                                                      //
// Consistency base class						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <assert.h>

#include "RhoMath/TConsistency.h"

ClassImp(TConsistency)

TBuffer &operator>>(TBuffer &buf, TConsistency *&obj)
{
   obj = (TConsistency *) buf.ReadObject(TConsistency::Class());
   return buf;
}

#include <iostream>
#include <iomanip>
using namespace std;

TConsistency::TConsistency() 
: fStat(noMeasure), fValue(0), fLikelihood(1.), fSign(unknown)
{
}

TConsistency::TConsistency(Double_t v, Double_t l)
: fStat(OK), fValue(v), fLikelihood(l), fSign(unknown)
{
}

TConsistency::TConsistency(const TConsistency& rhs) 
: fStat(rhs.fStat), fValue(rhs.fValue)
, fLikelihood(rhs.fLikelihood), fSign(rhs.fSign)
{
}

// operators
TConsistency& 
TConsistency::operator=(const TConsistency& rhs)
{
    fStat=rhs.fStat;
    fValue=rhs.fValue;
    fLikelihood=rhs.fLikelihood;
    fSign=rhs.fSign;
    
    return *this;
}

Bool_t
TConsistency::operator==(const TConsistency& rhs) const
{
    Bool_t answer = kFALSE;
    if (fStat       == rhs.fStat &&
	fValue      == rhs.fValue &&
	fLikelihood == rhs.fLikelihood &&
	fSign       == rhs.fSign) {
	answer = kTRUE;
    }
    
    return answer;
}

Bool_t
TConsistency::operator<(const TConsistency& rhs) const
{
    //
    // this is not unique...
    //
    
    Bool_t answer = kFALSE;
    if (fStat == OK && rhs.fStat == OK ) {
	//
	// Dare to use consistency for comparison. 
	// Likelihood may give a different answer
	//
	if (fValue < rhs.fValue) {
	    answer = kTRUE;
	}
    } else {
	if (rhs.fStat == OK) {
	    answer = kTRUE;
	}
    }
    
    return answer;
}

Bool_t
TConsistency::operator>(const TConsistency& rhs) const
{
    // ghm
    if( *this==rhs ) return kFALSE;
    return( ! (*this<rhs) );
}


const TConsistencySet* 
TConsistency::Genealogy() const
{
    return 0;
}


void
TConsistency::PrintOn(std::ostream& os) const
{
    os << "Likelihood = ";
    os <<  setw(7) << setprecision(4);
    os << fLikelihood;
    os << "\t SignificanceLevel = ";
    os << setw(7) << setprecision(4);
    os << fValue;
    os << "\t Status ";
    switch( Status() ) {
    case TConsistency::OK  :
	os << "OK"; 
	break;
    case  TConsistency::noMeasure  :
	os << "noMeasure"; 
	break;
    case  TConsistency::underFlow  :
	os << "underFlow"; 
	break;
    case  TConsistency::unPhysical  :
	os << "unPhysical"; 
	break;
    default:
	os << "unknown";
    }
    os << endl;
}

//------------------
// Static methods --
//------------------

const TConsistency&
TConsistency::BadMeasurement() 
{
    static TConsistency bad(1.,1.);
    bad.SetStatus(noMeasure);
    
    return bad;
}

Double_t TConsistency::Consistency() const 
{ return fValue; }

Double_t TConsistency::SignificanceLevel() const 
{ return fValue; }

Double_t TConsistency::Likelihood() const 
{ return  fLikelihood;}

TConsistency::EConsistentStatus TConsistency::Status() const 
{return fStat;}

void TConsistency::SetStatus(EConsistentStatus s ) 
{ fStat = s; }

TConsistency::EConsistentSign TConsistency::Sign() const 
{return fSign; }

void TConsistency::SetSign(EConsistentSign s ) 
{ fSign = s; }
