//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConstraint								//
//                                                                      //
// Set kinematics and geometrical constraints for TCandidates		//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Aug.99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "THashList.h"
#include "TString.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TEventInfo.h"
#include "RhoBase/TConstraint.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/VAbsVertex.h"
#include "RhoMath/TDoubleErr.h"
#include "RhoMath/TPointErr.h"

#include <assert.h>
#include <stddef.h>

ClassImp(TConstraint)

#include <iostream>
using namespace std;

TConstraint::TConstraint( TConstraint::Type type )
: fType(type)
, fParams( 0 )
, fParamsTCandidate( 0 )
{
}

TConstraint::TConstraint(const TConstraint& b )
: fType(b.fType)
, fParams( 0 )
, fParamsTCandidate( 0 )
{  
    if ( b.fParams != 0 ) {
	fParams = new THashList(b.fParams);
    }
    if ( b.fParamsTCandidate != 0 ) {
	fParamsTCandidate = 
	    new THashList(b.fParamsTCandidate);
    }
}

//--------------
// Destructor --
//--------------

TConstraint::~TConstraint( )
{
    if (fParams!=0) { fParams->Delete(); delete fParams; }
    if (fParamsTCandidate!=0) { fParamsTCandidate->Delete();delete fParamsTCandidate; }
}

//--------------------
// Member Functions --
//--------------------


TConstraint::Type 
TConstraint::GetType() const 
{ 
    return fType; 
}

void
TConstraint::AddNewParm( const TString& parmName, Double_t parm )
{
    if ( fParams == 0 ) {
	fParams = new THashList();
    }
    fParams->Add( new TDoubleParm(parmName, parm) );
}

void
TConstraint::AddNewParm( const TString& parmName, TCandidate* parm )
{
    if ( fParamsTCandidate == 0 ) {
	fParamsTCandidate = new THashList();
    }
    fParamsTCandidate->Add( new TCandidateParm(parmName, parm) );
}

Bool_t
TConstraint::GetParmValue( const TString& parmName, Double_t& value ) const
{
    if ( fParams == 0 ) return kFALSE;
    TDoubleParm *parm = (TDoubleParm *)  fParams->FindObject(parmName);
    if (parm!=0) value = parm->GetValue();
    return (parm!=0);
}

Bool_t
TConstraint::GetParmValue( const TString& parmName, TCandidate* value ) const
{
    if ( fParamsTCandidate == 0 ) return kFALSE;
    TCandidateParm *parm = (TCandidateParm *)  fParamsTCandidate->FindObject(parmName);
    if (parm!=0) value = parm->GetValue();
    return (parm!=0);
}


void 
TConstraint::PrintOn(std::ostream& o) const
{
    o << "Constraint Type " << GetType() << endl;
    if ( fParams == 0 ) return;
    TIter iter(fParams);
    for(;iter.Next();)
    {
	TDoubleParm &p = (TDoubleParm &) iter;
	o << "/t" << p.GetName() << "/t" << p.GetValue() << endl;
    }
}

//
// Global functions
//

// Constrain the mass of the candidate to the given mass
void 
SetMassConstraint( TCandidate& cand, Double_t mass )
{
    TConstraint& c = cand.AddConstraint( TConstraint::Mass );
    c.AddNewParm( "mass", mass );
}

// Constrain the mass of the candidate to its type mass
void 
SetMassConstraint( TCandidate& cand )
{
    const TParticlePDG* pdt = cand.PdtEntry();
    Double_t mass = pdt>0 ? pdt->Mass() : 0.;
    SetMassConstraint( cand, mass );
}

// Constrain the lifetime of the candidate (c*tau, in centimeters)
void 
SetLifetimeConstraint( TCandidate& cand, Double_t ctau )
{
    TConstraint& c = cand.AddConstraint( TConstraint::Life );
    c.AddNewParm( "ctau", ctau );  
}

// Constrain the lifetime of the candidate to its type lifetime
void 
SetLifetimeConstraint( TCandidate& cand )
{
    const TParticlePDG* pdt = cand.PdtEntry();
    Double_t ctau = pdt>0 ? pdt->Lifetime() : 0.;
    SetLifetimeConstraint( cand, ctau );  
}

// Constrain the energy of the candidate
void 
SetEnergyConstraint( TCandidate& cand, TDoubleErr& energy, const TVector3& boost )
{
    TConstraint& c = cand.AddConstraint( TConstraint::Energy );
    c.AddNewParm( "energy", energy.Value() );
    c.AddNewParm( "energyCov", energy.Covariance() );
    c.AddNewParm( "boostX", boost.X() );    
    c.AddNewParm( "boostY", boost.Y() );    
    c.AddNewParm( "boostZ", boost.Z() );    
}


void 
SetEnergyConstraint( TCandidate& cand, TVectorErr& eMinusMom, TVectorErr& ePlusMom, Double_t scale)
{
    TConstraint& c = cand.AddConstraint( TConstraint::BeamEnergy );
    c.AddNewParm( "eMinusPx", eMinusMom.X() );    
    c.AddNewParm( "eMinusPy", eMinusMom.Y() );    
    c.AddNewParm( "eMinusPz", eMinusMom.Z() );
    c.AddNewParm( "eMinusCovPxPx", eMinusMom.CovMatrix()(0,0) );
    c.AddNewParm( "eMinusCovPxPy", eMinusMom.CovMatrix()(0,1) );
    c.AddNewParm( "eMinusCovPxPz", eMinusMom.CovMatrix()(0,2) );
    c.AddNewParm( "eMinusCovPyPy", eMinusMom.CovMatrix()(1,1) );
    c.AddNewParm( "eMinusCovPyPz", eMinusMom.CovMatrix()(1,2) );
    c.AddNewParm( "eMinusCovPzPz", eMinusMom.CovMatrix()(2,2) );
    //    
    c.AddNewParm( "ePlusPx", ePlusMom.X() );    
    c.AddNewParm( "ePlusPy", ePlusMom.Y() );    
    c.AddNewParm( "ePlusPz", ePlusMom.Z() );
    c.AddNewParm( "ePlusCovPxPx", ePlusMom.CovMatrix()(0,0) );
    c.AddNewParm( "ePlusCovPxPy", ePlusMom.CovMatrix()(0,1) );
    c.AddNewParm( "ePlusCovPxPz", ePlusMom.CovMatrix()(0,2) );
    c.AddNewParm( "ePlusCovPyPy", ePlusMom.CovMatrix()(1,1) );
    c.AddNewParm( "ePlusCovPyPz", ePlusMom.CovMatrix()(1,2) );
    c.AddNewParm( "ePlusCovPzPz", ePlusMom.CovMatrix()(2,2) );
    //
    c.AddNewParm( "scale", scale );    // scale factor to apply to the total beam energy
}


void 
SetEnergyConstraint( TCandidate& cand,const TEventInfo* ev, Double_t scale)
{
    // retreive TEventInfo pointer
    if( ev==0 )
    {
      cerr << "SetEnergyConstraint TEventInfo* not available " << endl;
      assert(0);
    }
    // get the beam energies
    const TFitParams em = ev->ElectronBeam().GetFitParams();
    TVectorErr eMinusMom(em.P3WErr());
    const TFitParams ep = ev->PositronBeam().GetFitParams();
    TVectorErr ePlusMom(ep.P3WErr());
    // impose constraint
    SetEnergyConstraint( cand, eMinusMom, ePlusMom, scale);
}



// Constraint the momentum of the candidate
void 
SetMomentumConstraint( TCandidate& cand )
{
    TConstraint& c = cand.AddConstraint( TConstraint::PseudoMomentum );
}


void 
SetMomentumConstraint( TCandidate& cand, const TVectorErr& momentum, const TVector3& boost)
{
    TConstraint& c = cand.AddConstraint( TConstraint::Momentum );
    c.AddNewParm( "px", momentum.X() );      
    c.AddNewParm( "py", momentum.Y() );      
    c.AddNewParm( "pz", momentum.Z() );      
    c.AddNewParm( "spxx", momentum.CovMatrix()(0,0) );      
    c.AddNewParm( "spyy", momentum.CovMatrix()(1,1) );      
    c.AddNewParm( "spzz", momentum.CovMatrix()(2,2) );      
    c.AddNewParm( "spxy", momentum.CovMatrix()(0,1) );      
    c.AddNewParm( "spxz", momentum.CovMatrix()(0,2) );      
    c.AddNewParm( "spyz", momentum.CovMatrix()(1,2) );
    c.AddNewParm( "boostX", boost.X() );    
    c.AddNewParm( "boostY", boost.Y() );    
    c.AddNewParm( "boostZ", boost.Z() );
}

// Beam Constrain - 
// Constrain the head of a decay tree to come from a given vertex
void 
SetBeamConstraint( TCandidate& cand, 
		  const TPointErr& beam )
{
    TConstraint& c = cand.AddConstraint( TConstraint::Beam );
    c.AddNewParm( "bx", beam.X() );      
    c.AddNewParm( "by", beam.Y() );      
    c.AddNewParm( "bz", beam.Z() );      
    c.AddNewParm( "sxx", beam.CovMatrix()(0,0) );      
    c.AddNewParm( "syy", beam.CovMatrix()(1,1) );      
    c.AddNewParm( "szz", beam.CovMatrix()(2,2) );      
    c.AddNewParm( "sxy", beam.CovMatrix()(0,1) );      
    c.AddNewParm( "sxz", beam.CovMatrix()(0,2) );      
    c.AddNewParm( "syz", beam.CovMatrix()(1,2) );      
}


void 
SetBeamConstraint( TCandidate& cand,const TEventInfo* ev )
{
    // retreive TEventInfo pointer
    if( ev==0 )
    {
      cerr << "SetBeamConstraint TEventInfo* not available " << endl;
      assert(0);
    }
    // get the beam position
    TPointErr beamSpot(ev->BeamSpot());
    // impose constraint
    SetBeamConstraint( cand, beamSpot);
}


void 
SetGeoConstraint( TCandidate& cand)
{
    SetGeoXYConstraint(cand);
    SetGeoZConstraint(cand);
    SetMomentumConstraint(cand);
}

void 
SetGeoXYConstraint( TCandidate& cand)
{
    TConstraint& c = cand.AddConstraint( TConstraint::GeoXY );
}

void 
SetGeoZConstraint( TCandidate& cand)
{
    TConstraint& c = cand.AddConstraint( TConstraint::GeoZ );
}


void 
SetLineOfFlightConstraint( TCandidate& cand, TVector3 dir, TVector3 point)
{
    TConstraint& c = cand.AddConstraint( TConstraint::LineOfFlight );
    c.AddNewParm( "dirX", dir.Unit().X() );
    c.AddNewParm( "dirY", dir.Unit().Y() );
    c.AddNewParm( "dirZ", dir.Unit().Z() );
    c.AddNewParm( "pointX", point.X() );
    c.AddNewParm( "pointY", point.Y() );
    c.AddNewParm( "pointZ", point.Z() );
}


void 
SetLineOfFlightConstraint( TCandidate& cand, TVector3 dir, const TEventInfo* ev)
{
    // retreive TEventInfo pointer
    if( ev==0 )
    {
       cerr << "TConstraint::SetLineOfFlightConstraint: TEventInfo* not available " << endl;
    }
    SetLineOfFlightConstraint( cand, dir, ev->PrimaryVertex() );
}


void
SetLineOfFlightConstraint( TCandidate& cand, TCandidate* cdir)
{
    TVector3 point;
    if ( cdir->IsComposite() && (cdir->DecayVtx()!=0) && (cdir->DecayVtx()->Status()==VAbsVertex::Success) )
	SetLineOfFlightConstraint( cand, cdir->P3(), cdir->DecayVtx()->Point() );
    else
	SetLineOfFlightConstraint( cand, cdir->P3() );
}



void 
SetMissingMassConstraint( TCandidate& cand, TCandidate* cMissing, Double_t missingMass )
{
    TConstraint& c = cand.AddConstraint( TConstraint::MissingMass );
    TCandListIterator iterDau( cand.DaughterIterator() );
    int iDau(0);
    TCandidate* b=0;
    const TParticlePDG* pdt=0;
    while ( b=iterDau.Next() ) {
	iDau++;
	if (b->Uid()==cMissing->Uid()) {
	    pdt = b->PdtEntry();
	    break;
	}
    }
    assert(iDau<=cand.NDaughters());
    assert(pdt==cMissing->PdtEntry());
    c.AddNewParm( "iDaughter", iDau );
    c.AddNewParm( "missingMass", missingMass );
}



void 
SetMissingMassConstraint( TCandidate& cand, TCandidate* cMissing )
{
    const TParticlePDG* pdt = cMissing->PdtEntry();
    Double_t missingMass = pdt>0 ? pdt->Mass() : 0.;
    SetMissingMassConstraint( cand, cMissing, missingMass);
}

