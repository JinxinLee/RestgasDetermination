#ifndef TCONSTRAINT_H
#define TCONSTRAINT_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConstraint	    							//
//                                                                      //
// Set kinematics and geometrical constraints for TCandidates		//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Aug.99						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TObject.h"
#include "TNamed.h"
#include "TVector3.h"

class TCandidate;
class THashList;
class TString;

class TConstraint : public TObject
{
    
public:
    
    //
    //  possible types of constraints
    //
    
    enum Type
    { 
	Mass, 
	Momentum, 
	PseudoMomentum,
	Energy, 
	BeamEnergy, 
	Beam, 
	Life,
	GeoXY, 
	GeoZ,
	LineOfFlight,
	MissingMass
    };
    
private:
    
    //--------------------
    // Instance Members --
    //--------------------
    
    // type
    Type fType;
    
    // parameters
    THashList* fParams;
    THashList* fParamsTCandidate;
    
public:
    
    //
    // Public interface
    //
    
    // the type must be specified
    TConstraint( Type type );
    
    // copy
    TConstraint(const TConstraint& );
    
    // operators
    Bool_t operator==(const TConstraint& o ) 
    { return fType==o.fType; }
    
    // Destructor
    virtual ~TConstraint();
    
    // modifiers
    void AddNewParm( const TString& parmName, Double_t parm );
    void AddNewParm( const TString& parmName, TCandidate* parm );
    
    // access
    Type GetType() const ;
    Bool_t GetParmValue( const TString& parmName, Double_t& ) const;
    Bool_t GetParmValue( const TString& parmName, TCandidate* ) const;
    
    // printings
    void PrintOn( std::ostream& o=std::cout ) const;

private:
    
    // mutable trick
    TConstraint* myself_() const { return (TConstraint*) this; }
    
public:
    ClassDef(TConstraint,1) //Set fitter constraints
};

//
//  Global functions to make the setting of Constraints easier
//  Note : the list is not exhaustive
//

//class TCandidate;
class TVector3;
class TPointErr;
class TVectorErr;
class TDoubleErr;
class TEventInfo;

// Constrain the mass of the candidate to the given mass
void SetMassConstraint( TCandidate& cand, Double_t mass );

// Constrain the mass of the candidate to its type mass
void SetMassConstraint( TCandidate& cand );

// Constrain the lifetime of the candidate (c*tau, in centimeters)
void SetLifetimeConstraint( TCandidate& cand, Double_t ctau );

// Constrain the lifetime of the candidate to its type lifetime
void SetLifetimeConstraint( TCandidate& cand );

// Constrain the energy of the candidate
void SetEnergyConstraint( TCandidate& cand, TDoubleErr& energy,const TVector3& boost=TVector3(0,0,0) );

void SetEnergyConstraint( TCandidate& cand, TVectorErr& eMinusMom, TVectorErr& ePlusMom, Double_t scale=0.5);

void SetEnergyConstraint( TCandidate& cand, const TEventInfo* ev, Double_t scale=0.5);


// Constraint the momentum of the candidate
void SetMomentumConstraint( TCandidate& cand );

void SetMomentumConstraint( TCandidate& cand, 
			   const TVectorErr& momentum, 
			   const TVector3& boost=TVector3(0,0,0) );



// Beam Constrain - 
// Constrain the head of a decay tree to come from a given vertex
void SetBeamConstraint( TCandidate& c, 
		       const TPointErr& beam );

void SetBeamConstraint( TCandidate& c, const TEventInfo* ev);


// Geometric Constraint -
// forces the daughters of a candidate to have a common vertex (only defined in Paul Avery approach)
void SetGeoXYConstraint( TCandidate& c);
void SetGeoZConstraint( TCandidate& c);
void SetGeoConstraint( TCandidate& c);

// Line of flight constraint
void SetLineOfFlightConstraint( TCandidate& c, TVector3 dir, TVector3 point=TVector3(0,0,0));
void SetLineOfFlightConstraint( TCandidate& c, TVector3 dir, const TEventInfo* ev);
void SetLineOfFlightConstraint( TCandidate& c, TCandidate* cdir);

// Constrain the missing mass
void SetMissingMassConstraint( TCandidate& cand, TCandidate* cMissing, Double_t missingMass );
void SetMissingMassConstraint( TCandidate& cand, TCandidate* cMissing );

class TDoubleParm : public TNamed
{
public:
    TDoubleParm(const char* name, Double_t val) : TNamed(name,name)
    { fValue = val; }
    Double_t GetValue() { return fValue; }
private:
    Double_t fValue;
};

class TCandidateParm : public TNamed
{
public:
    TCandidateParm(const char* name, TCandidate *val) : TNamed(name,name)
    { fValue = val; }
    TCandidate* GetValue() { return fValue; }
private:
    TCandidate* fValue;
};

#endif




