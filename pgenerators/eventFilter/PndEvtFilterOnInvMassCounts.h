// -------------------------------------------------------------------------
// -----             PndEvtFilterOnInvMassCounts header file           -----
// -------------------------------------------------------------------------


/** PndEvtFilterOnInvMassCounts.h

 Author: Martin Galuska
 Martin dot J dot Galuska at physik dot uni minus giessen dot de

 Purpose:
 This class is used to filter events right after event generation and before transport through the detector model
 events will either be accepted (and further simulated)
 or rejected completely.

 This filter class implements such decisions based on multiplicities of invariant mass combinations within a certain mass region

 The user has to specify the minimum or maximum multiplicities for the invariant mass combinations
 and which pdgCodes the particles should correspond to.

 only particles which were produced by the event generators are taken into account for the combinations
 and which have the same charge as the charges corresponding to the input pdgCoded
 The mass hypotheses of matching particles will be set to the input pdgCodes given by the user

 **/



#ifndef PndEvtFilterOnInvMassCounts_H_
#define PndEvtFilterOnInvMassCounts_H_

#include "PndEvtFilter.h"
#include "TMath.h"
#include <map>
#include <vector>
#include <iostream>
#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoMassParticleSelector.h"
#include "TLorentzVector.h"
#include "RhoMassParticleSelector.h"


class PndEvtFilterOnInvMassCounts : public PndEvtFilter
{

public:

	//Default constructor
	PndEvtFilterOnInvMassCounts();

	//Constructor with name and title
	PndEvtFilterOnInvMassCounts(const char* name, const char* title="PndEvtFilterOnInvMassCounts");

	// Destructor
	virtual ~PndEvtFilterOnInvMassCounts();

	//////////////////
	// User interfaces
	//////////////////
	// returns kTRUE if setting was added, otherwise returns kFALSE
	Bool_t SetMinMaxCounts( Int_t min, Int_t max );
	Bool_t SetMinMaxInvMass( Double_t min, Double_t max );
	Bool_t SetRhoMassParticleSelector( const char* name, Double_t cv, Double_t w, const char* type ); // same arguments as constructor of RhoMassParticleSelector
	Bool_t SetPdgCodesToCombine( Int_t pdgCode1, Int_t pdgCode2, Int_t pdgCode3=kInvalidPdgCode, Int_t pdgCode4=kInvalidPdgCode, Int_t pdgCode5=kInvalidPdgCode );




	// checks if the particles in the event (fParticleList) suit the filter settings
	// kTRUE if event matches, kFALSE otherwise
	Bool_t EventMatches(Int_t evtNr);


	// returns kTRUE if any filter is turned on
	Bool_t FilterActive(){ return ( fInvMassRangeSet && fPgdCodesSet && fCountRangeSet ); };


private:
	// minimum and maximum acceptable multiplicities for the particle combinations
	std::pair<Int_t,Int_t> fCountsMinMax;
	// inv. mass selector which implements the minimum and maximum acceptable invariant mass range for the particle combinations
	RhoMassParticleSelector *fInvMassSel;
	// vector containing the pdg codes which should be combined along with its corresponding charges
	std::vector< std::pair< Int_t, Double_t > > fPdgCodesCharges;



	Bool_t fInvMassRangeSet; // is kTRUE if invariant mass is set
	Bool_t fPgdCodesSet; // is kTRUE if pdg codes are set
	Bool_t fCountRangeSet; // is kTRUE if min and max counts are set


	// constant holding an integer number which is not used as a pdg code
	// this serves as a place holder when constructing a vector from integers given by the user
	static const Int_t kInvalidPdgCode = 0;

	ClassDef(PndEvtFilterOnInvMassCounts,1);

};


#endif /* PndEvtFilterOnInvMassCounts_H_ */
