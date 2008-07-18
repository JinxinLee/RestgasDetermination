//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertex								//
//                                                                      //
// Abstract Definition of the vertex					//
//                                                                      //
// Author: Marcel Kunze, RUB, Mar. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsVertex.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"

ClassImp(VAbsVertex)

#include <iostream>
using namespace std;

VAbsVertex::VAbsVertex( )
: fStatus(UnFitted), fInComingCand(0)
{
    nOutGoingCands = 0;
    nResonances = 0;
}

VAbsVertex::VAbsVertex( const VAbsVertex& other )
: fStatus(other.fStatus), fInComingCand(0)
{
    nOutGoingCands = 0;
    nResonances = 0;
}

//--------------
// Destructor --
//--------------

VAbsVertex::~VAbsVertex( )
{
}

//--------------
// Operations --
//--------------

const TCandidate*
VAbsVertex::TheMother() const
{
    // return the incoming candidate, if any
    if( fInComingCand!=0 ) return fInComingCand;
    
    // else, loop over the resonances
    TCandidate* mother=0;
    for( Int_t i=0; i<nResonances; i++ )
    {
	TCandidate* cand =  fResonances[i];
	if( cand->TheMother()==0 )
	{
	    if( mother==0 )
	    {
		mother=cand;
	    }
	    else
	    {
		cout << "several candidates with no mother" 
		    << endl;
	    }
	}
    }
    if( mother==0 ) cout << "no candidate with a mother" 
	<< endl;
    return mother;
}

// 4-momentum of the incoming particle
TLorentzVector 
VAbsVertex::P4() const
{
    const TCandidate* mother = TheMother();
    if (mother==0) return TLorentzVector(0.,0.,0.,0.);
    return mother->P4();
}

// mass of the incoming particle
double 
VAbsVertex::Mass() const
{
    const TCandidate* mother = TheMother();
    if (mother==0) return 0.;
    return mother->Mass();
}

// momentum of the incoming particle
double 
VAbsVertex::P() const
{
    const TCandidate* mother = TheMother();
    if (mother==0) return 0.;
    return mother->P();
}

void 
VAbsVertex::PrintOn(std::ostream& o) const 
{
    o << Point().X() << '\t' << Point().Y() << '\t' << Point().Z() << endl;
}

std::ostream&  
operator << (std::ostream& o, const VAbsVertex& a) 
{
    a.PrintOn(o); return o;
}

TMatrixD& 
VAbsVertex::XXWeight() const 
{
    static TMatrixD x(3,3);
    return x;
}

TMatrixD&  
VAbsVertex::PPWeight() const 
{
    static TMatrixD x(3,3);
    return x;
}

TMatrixD&   
VAbsVertex::XPWeight() const 
{
    static TMatrixD x(3,3);
    return x;
}

void 
VAbsVertex::GetAllWeights(TVector3& xv,
			  TVector3& pv,
			  TMatrixD&  xxWeight,
			  TMatrixD&  ppWeight,
			  TMatrixD&     xpWeight) const 
{
    int i,j;
    for(i=0;i<3;i++)
	for(j=i;j<3;j++) {
	    xxWeight(i,j)=0;
	    ppWeight(i,j)=0;
	    xpWeight(i,j)=0;
	    xpWeight(j,i)=0;
	}
	
	for(i=0;i<3;i++)
	{
	    xv[i]=0;
	    pv[i]=0;
	}
	
}

TMatrixD& 
VAbsVertex::XXCov() const 
{
    static TMatrixD x(3,3);
    return x;
}

TMatrixD& 
VAbsVertex::PPCov() const 
{
    static TMatrixD in(3,3), out(3,3);
    if( fInComingCand ) 
	in = fInComingCand->GetFitParams().P4Err();
    out = in - out;
    return out;
}

TMatrixD&    
VAbsVertex::XPCov() const 
{
    static TMatrixD x(4,3);
    return x;
}

/*
*
*/
VAbsVertex::VtxStatus
VAbsVertex::Status() const
{
    return fStatus;
}

VAbsVertex::VtxType
VAbsVertex::Type() const
{
    return fType;
}


TVector3 
VAbsVertex::Point() const 
{ 
    return TVector3(V4().X(), V4().Y(), V4().Z()); 
}

/*
* Former inline funcs
*/
void
VAbsVertex::SetStatus(VAbsVertex::VtxStatus s) 
{
    fStatus=s;
}

void
VAbsVertex::SetType(VAbsVertex::VtxType type) 
{
    fType=type;
}

// number of out going candidates
int 
VAbsVertex::NOutGoing() const
{
    return nOutGoingCands;
}

// number of resonances (states produced and decayed at that vertex)
int 
VAbsVertex::NResonance() const
{
    return nResonances;
}

const TCandidate* 
VAbsVertex::InComingCand() const
{
    return fInComingCand;
}
