//==========================================================================
// File and Version Information:
// 	$Id: TBVariables.cxx,v 1.7 2008-02-28 09:31:01 marcel Exp $
//
//--------------------------------------------------------------------------
// Description:
//	See TBVariables.h
//
//--------------------------------------------------------------------------
// Sample User Code:
//
//--------------------------------------------------------------------------
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
//--------------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//      Bill Ford        (Added m_ES)
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1999	Colorado State University
//
// ROOT Version by Marcel Kunze, RUB
//==========================================================================
#define RHOFRAMEWORK

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <math.h>
#include <assert.h>
#include <float.h>

#include "TDatabasePDG.h"
#include "TLorentzVector.h"

//----------------
// BaBar Header --
//----------------

//-----------------------
// This Class's Header --
//-----------------------
#include "RhoTools/TBVariables.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "RhoBase/TRho.h"
#ifdef RHOFRAMEWORK
#include "RhoConditions/TConditions.h"
#include "RhoConditions/TBeams.h"
#else
#include "RhoMath/TDoubleErr.h"
class TBeams {
public:
    TBeams() {}
    virtual ~TBeams() {}
    TLorentzVector Total4Momentum() { return TLorentzVector(-0.11,0,5.88,12.108); }
    TLorentzVectorErr Total4MomentumWErr() { return TLorentzVectorErr(Total4Momentum(), TError(4));}
    TVector3 BoostCMtoLab() { return Total4Momentum().BoostVector(); }
    TDoubleErr EnergyCMWErr() { return TDoubleErr(Total4Momentum().T(),1.E-3); } 
};
#endif

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

ClassImp(TBVariables)

TBuffer &operator>>(TBuffer &buf, TBVariables *&obj)
{
   obj = (TBVariables *) buf.ReadObject(TBVariables::Class());
   return buf;
}

#include <iostream>
using namespace std;

#ifndef BBV_CHECK_SQRT
#define BBV_CHECK_SQRT(square) CheckSqrt(square, __FILE__, __LINE__)
#endif

static const char rscid[] = "$Id: TBVariables.cxx,v 1.7 2008-02-28 09:31:01 marcel Exp $";

static inline Double_t Sqr(Double_t d) {return d * d;}

//-----------------------------------------------
//-- Static Data & Function Member Definitions --
//-----------------------------------------------
//--------------------------------------------------------------------
Double_t 
TBVariables::MomErr(const TLorentzVectorErr & p4) const {
    const Double_t p2 = p4.Vect().Mag2();
    if (0 == p2){
	return 0.0;
    }
    
    Double_t cov = 0.0;
    for (int row = 0; row < 3; ++row){
	for (int col = 0; col < 3; ++col){
	    cov += p4.CovMatrix()(row, col) * p4(row) * p4(col);
	}
    }
    cov /= p2;
    
    return BBV_CHECK_SQRT(cov);
}

//--------------------------------------------------------------------
Double_t 
TBVariables::BMass() {
    static TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    static Double_t result = pdt->GetParticle("BfPlus")->Mass();
    return result;
}

//----------------------------------------
//-- Public Function Member Definitions --
//----------------------------------------

//----------------
// Constructors --
//----------------
TBVariables::TBVariables() :
fMHat(0.0),
fM_ES(0.0),
fDeltaE(0.0),
fMHatErr(0.0),
fM_ESErr(0.0),
fDeltaEErr(0.0),
fStatus(TBVariables::NO_BCAND),
fUpsilonWidth(-DBL_MAX),
fQuietMode(kFALSE)
{
    SetDefaultUpsilonWidth();
}

//--------------------------------------------------------------------
TBVariables::TBVariables(const TLorentzVectorErr & p4Lab,
			     Bool_t quiet) :
fStatus(TBVariables::NO_BCAND),
fUpsilonWidth(-DBL_MAX),
fQuietMode(quiet)
{
    SetDefaultUpsilonWidth();
    SetP4Lab(p4Lab);
}

//--------------
// Destructor --
//--------------
TBVariables::~TBVariables() {} 

//-------------
// Accessors --
//-------------
Double_t 
TBVariables::PHat() const {
    return fP4Hat.Vect().Mag();
}

//--------------------------------------------------------------------
Double_t
TBVariables::PHatErr() const {
    return MomErr(fP4Hat);
}

//--------------------------------------------------------------------
Double_t 
TBVariables::EnergySubstitutedMass() const {
    const Double_t p = fP4.Vect().Mag();
    const Double_t m2 = Sqr(fEBeam) - Sqr(p);
    return BBV_CHECK_SQRT(m2);
}

//--------------------------------------------------------------------
Double_t 
TBVariables::EnergySubstitutedMassErr() const {
    Double_t pContribution = 0.0;
    const Double_t mass = EnergySubstitutedMass();
    
    if (0 < mass){
	pContribution = MomErr(fP4) * fP4.Vect().Mag() / mass;
    }
    
    return BBV_CHECK_SQRT(Sqr(pContribution) + Sqr(EBeamErr()));
}

//-------------
// Modifiers --
//-------------
void 
TBVariables::SetUpsilonWidth(Double_t upsWidth) {
    const Double_t oldWidth = fUpsilonWidth;
    fUpsilonWidth = upsWidth;
    
    // If the the Upsilon width really changed and fit was already
    // carried out, refit with the new Upsilon width:
    if (fUpsilonWidth != oldWidth && NO_BCAND != fStatus) {
	ReFit();
    }
}

//--------------------------------------------------------------------
void
TBVariables::SetDefaultUpsilonWidth() {
    static TDatabasePDG *pdt = TRho::Instance()->GetPDG();
    SetUpsilonWidth(pdt->GetParticle("Upsilon(4S)")->Width());
}

//--------------------------------------------------------------------
TBVariables::Status
TBVariables::SetP4Lab(const TLorentzVectorErr & theP4Lab) {
    fP4Lab = theP4Lab;
    
    // Get the beam parameters:
    
#ifdef RHOFRAMEWORK
    TBeams *theBeams = 0;
    
    if (TRho::Instance()->GetConditions()!=0) {
	TConditions *theConditions = TRho::Instance()->GetConditions();
	TRhoTime *theTime = TRho::Instance()->GetTime();
	if (theTime!=0) 
	    theConditions->At(*theTime);	// Read the beam conditions DB
	else
	    theConditions->At(0);
	theBeams = theConditions->GetBeams();
    } 
    else {
	fStatus = NO_BEAMS_INFO;
	return FitStatus();
    }
#else
    static TBeams *theBeams = new TBeams();;
#endif

    // Get the boost parameters:
    const TVector3 boostLabToCM = -(theBeams->BoostCMtoLab());
    const TLorentzRotation theBoost(boostLabToCM);  
    
    // Boost to the CM:
    fP4 = theP4Lab;
    fP4.Transform(theBoost);
    
    // Do the rest of the work:
    return ReFit();
}

//--------------------------------------------------------------------
TBVariables::Status
TBVariables::SetP4CM(const TLorentzVectorErr & theP4CM) {
    fP4 = theP4CM;
    
    // Get the beam parameters:
    
#ifdef RHOFRAMEWORK
    TBeams *theBeams = 0;
    
    if (TRho::Instance()->GetConditions()!=0) {
	TConditions *theConditions = TRho::Instance()->GetConditions();
	TRhoTime *theTime = TRho::Instance()->GetTime();
	if (theTime!=0) 
	    theConditions->At(*theTime);	// Read the beam conditions DB
	else
	    theConditions->At(0);
	theBeams = theConditions->GetBeams();
    } 
    else {
	fStatus = NO_BEAMS_INFO;
	return FitStatus();
    }
#else
    static TBeams *theBeams = new TBeams();;
#endif
    
    // Get the boost parameters:
    const TVector3 boostCMToLab = theBeams->BoostCMtoLab();
    const TLorentzRotation theBoost(boostCMToLab);  
    
    // Boost to the Lab:
    fP4Lab = theP4CM;
    fP4Lab.Transform(theBoost);
    
    return ReFit();
}

//-------------------
// Protected Members:
//-------------------

Double_t 
TBVariables::CheckSqrt(Double_t square, const char * file, int line) const {
    if (0 > square){
	if (!fQuietMode){
	    cerr << "TBVariables: Trying to take the square root of "
		<< square << endl
		<< "     in " << file << ", line " << line << endl;
	}
	// Cast this into non-const to change mutable fStatus:
	((TBVariables&)(*this)).fStatus = SQRT_NEGATIVE;
	return -1000.0;
    }
    return TMath::Sqrt(square);
}

//--------------------------------------------------------------------
TBVariables::Status
TBVariables::ReFit() {
    // Initialize:
    fStatus = GOOD;
    
    // Get the beam parameters:
    
#ifdef RHOFRAMEWORK
    TBeams *theBeams = 0;
    
    if (TRho::Instance()->GetConditions()!=0) {
	TConditions *theConditions = TRho::Instance()->GetConditions();
	TRhoTime *theTime = TRho::Instance()->GetTime();
	if (theTime!=0) 
	    theConditions->At(*theTime);	// Read the beam conditions DB
	else
	    theConditions->At(0);
	theBeams = theConditions->GetBeams();
    } 
    else {
	fStatus = NO_BEAMS_INFO;
	return FitStatus();
    }
#else
    static TBeams *theBeams = new TBeams();
#endif
    
    // Get beams lab 4-momentum
    const TLorentzVectorErr q0 = theBeams->Total4MomentumWErr();
    
    // Set the fEBeam:
    TDoubleErr energyCM = theBeams->EnergyCMWErr();
    fEBeam = energyCM.Value() / 2.0;
    
    // Set the fEBeamErr, incorporating the resonance width 
    // (See BaBar Note 497, Eq. B1):
    const Double_t eBeamErr2 =     
	1.0 / (4.0 / energyCM.Covariance() + 22.0 / Sqr(UpsilonWidth()));
    
    fEBeamErr = BBV_CHECK_SQRT(eBeamErr2);
    if (SQRT_NEGATIVE == fStatus) {
	return fStatus;
    }
    
    // Get the fDeltaE and its error:
    fDeltaE = fP4.T() - fEBeam;
    fDeltaEErr = BBV_CHECK_SQRT(fP4.CovMatrix()(3,3) + eBeamErr2);
    if (SQRT_NEGATIVE == fStatus) {
	return fStatus;
    }
    
    // Get fM_ES
    const TVector3 p0(q0.Vect());
    Double_t E0(q0.T());
    //  const TVector3 p0(theBeams->totalMomentum());
    //  Double_t E0 = theBeams->eMinusEnergy()+theBeams->ePlusEnergy();
    const TVector3 p3Lab = fP4Lab.Vect();
    Double_t E_ES = (.5*Sqr(energyCM.Value())+p0.Dot(p3Lab))/E0;
    Double_t m_ES2 = Sqr(E_ES) - p3Lab.Mag2();
    fM_ES = BBV_CHECK_SQRT(m_ES2);
    if (SQRT_NEGATIVE == fStatus) {
	return fStatus;
    }
    
    // Get fM_ES error:
    // Derivatives of m_ES squared with respect to E0, p0_i, p3Lab_i
    TVectorD A0(4), A1(4);
    for (int i=0; i<3; i++) {
	A0(i) = 2.*(E_ES/E0)*(p3Lab[i] - p0[i]);
	A1(i) = 2.*((E_ES/E0)*p0[i] - p3Lab[i]);
    }
    A0(3) =  2.*E_ES*(1. - E_ES/E0);
    A1(3) = 0.0;
    
    // Multiply momentum error matrices left and right by derivative vectors
    TMatrixD Err0(q0.CovMatrix());
    TMatrixD Err1(fP4Lab.CovMatrix());
    Double_t m_SE2Err2 = 0.0;
    for (int i1=0;i1<4;i1++) {
	for (int i2=0;i2<4;i2++) {
	    m_SE2Err2 += (Err0(i1,i2)*A0(i2) + Err1(i1,i2)*A1(i2));
	}
    }

    Double_t m_ES2Err = BBV_CHECK_SQRT(m_SE2Err2);
    if (SQRT_NEGATIVE == fStatus) {
	return fStatus;
    }
    if (fM_ES > 0.) {
	fM_ESErr = m_ES2Err / 2. / fM_ES;
    } else {
	fM_ESErr = -1000.;
    }
    
    // Conduct the fit. See BaBar Note 497, Eq. B2.
    const TError & covCM = fP4.CovMatrix();
    const Double_t Cee = covCM(3,3);
    const TVector3 Ce(covCM(0,3), covCM(1,3), covCM(2,3));
    
    const Double_t totalSigma2 = eBeamErr2 + Cee;  
    const TVector3 CeCorr = Ce * (fDeltaE / totalSigma2);
    const TVector3 theP3Hat = fP4.Vect() - CeCorr;
    
    const Double_t theEHat =
	(eBeamErr2 * fP4.T() + Cee * fEBeam) / totalSigma2;
    
    // The post-fit 4-momentum:
    const TLorentzVector theP4Hat(theP3Hat, theEHat);
    
    // The post-fit error matrix:
    TError theP4HatErr = covCM;
    for (int row = 0; row < 4; ++row){
	for (int col = row; col < 4; ++col){
	    theP4HatErr(row, col) -= covCM(3, row) * covCM(3, col) / totalSigma2;
	}
    }
    
    // Combine into the fitted p4 + error matrix:
    fP4Hat = TLorentzVectorErr(theP4Hat, theP4HatErr);
    
    // Go on to calculate mHat:    
    const Double_t mHat2 = Sqr(theP4Hat.T()) - theP4Hat.Vect().Mag2();
    fMHat = 0;
    fMHatErr = 0;
    
    if (mHat2 < 0.0) {
	fStatus = BAD_BCAND;
	return fStatus;
    }
    
    fMHat = BBV_CHECK_SQRT(mHat2);
    if (SQRT_NEGATIVE == fStatus) {
	return fStatus;
    }
    
    // And the error on mHat:
    if (0 < fMHat){
	// The momentum part:
	const Double_t mHatErrP = MomErr(fP4Hat) * PHat() / fMHat;
	
	// The energy part:
	const Double_t mHatErrE2 = Sqr(EHat() / fMHat) * fP4Hat.CovMatrix()(3,3);
	
	// The energy-momentum cross terms:
	Double_t mHatErrEP2 = 0.0;
	for (int row = 0; row < 3; ++row){
	    mHatErrEP2 += EHat() * fP4Hat(row) * fP4Hat.CovMatrix()(3, row);
	}
	mHatErrEP2 *= -2.0 / Sqr(fMHat);
	
	const Double_t mHatErr2 = Sqr(mHatErrP) + mHatErrE2 + mHatErrEP2;
	fMHatErr = BBV_CHECK_SQRT(mHatErr2);
	if (SQRT_NEGATIVE == fStatus) {
	    return fStatus;
	}
    }
    
    // Done:
    fStatus = GOOD;
    return fStatus;
}

Double_t 
TBVariables::MHat() const {
  return fMHat;
}
			     
//--------------------------------------------------------------------
Double_t 
TBVariables::M_ES() const {
  return fM_ES;
}
			     
//--------------------------------------------------------------------
Double_t 
TBVariables::DeltaE() const {
  return fDeltaE;
}
			     
//--------------------------------------------------------------------
Double_t
TBVariables::MHatErr() const {
  return fMHatErr;
}
			     
//--------------------------------------------------------------------
Double_t
TBVariables::M_ESErr() const {
  return fM_ESErr;
}
			     
//--------------------------------------------------------------------
Double_t
TBVariables::DeltaEErr() const {
  return fDeltaEErr;
}
			     
//--------------------------------------------------------------------
const TLorentzVectorErr &
TBVariables::P4() const {
  return fP4;
}

//--------------------------------------------------------------------
const TLorentzVectorErr &
TBVariables::P4Hat() const {
  return fP4Hat;
}

//--------------------------------------------------------------------
Double_t 
TBVariables::EHat() const {
  return fP4Hat.T();
}

//--------------------------------------------------------------------
Double_t
TBVariables::EHatErr() const {
  return fP4Hat.CovMatrix()(3,3);
}

//--------------------------------------------------------------------
Double_t 
TBVariables::EBeam() const {
  return fEBeam;
}

//--------------------------------------------------------------------
Double_t 
TBVariables::EBeamErr() const {
  return fEBeamErr;
}

//--------------------------------------------------------------------
Double_t
TBVariables::UpsilonWidth() const {
  return fUpsilonWidth;
}
