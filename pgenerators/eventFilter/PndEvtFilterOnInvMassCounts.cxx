// -------------------------------------------------------------------------
// -----              PndEvtFilterOnInvMassCounts source file                -----
// -------------------------------------------------------------------------

#include "PndEvtFilterOnInvMassCounts.h"
#include "TParticlePDG.h"



// -----   Default constructor   -------------------------------------------
PndEvtFilterOnInvMassCounts::PndEvtFilterOnInvMassCounts() : PndEvtFilter(), fInvMassRangeSet(0), fPgdCodesSet(0), fCountRangeSet(0)

{
	fInvMassSel = 0;
}


// -----   Constructor with name and title   ------------------------------------
PndEvtFilterOnInvMassCounts::PndEvtFilterOnInvMassCounts(const char* name, const char* title) : PndEvtFilter(name, title), fInvMassRangeSet(0), fPgdCodesSet(0), fCountRangeSet(0)
{
	fInvMassSel = 0;
}

// -----   Destructor   ----------------------------------------------------
PndEvtFilterOnInvMassCounts::~PndEvtFilterOnInvMassCounts(){}


Bool_t PndEvtFilterOnInvMassCounts::SetMinMaxCounts( Int_t min, Int_t max )
{
	if ( kTRUE == fCountRangeSet ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts of " << this->GetTitle() << ": " << this->GetName() << ": \n";
		std::cout << "You are trying to set min and max counts for the events more than once! \n";
		std::cout << "I take the first setting of min = " << fCountsMinMax.first << " max = " << fCountsMinMax.second << " and ignore the later ones! \n\n\n";
		std::cout << "Your current input of min = " << min << " max = " << max << " is ignored! \n\n\n";
		return kFALSE;
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0 ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts of " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. You are trying to request that your events should have at least a number <= 0 of some particles. That makes no sense. \n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts  of " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. You are trying to request that your events should have at most a number of some particles which is less than the minimum number that you request. That makes no sense. \n\n\n";
		return kFALSE;
	}

	// set the minimum and maximum number of particle combinations
	fCountsMinMax = std::pair<Int_t, Int_t> (min,max);

	// turn filter on
	fCountRangeSet = kTRUE;

	return kTRUE;
}


Bool_t PndEvtFilterOnInvMassCounts::SetMinMaxInvMass( Double_t min, Double_t max )
{
	if ( kTRUE == fInvMassRangeSet ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts of " << this->GetTitle() << ": " << this->GetName() << ": \n";
		std::cout << "You are trying to set the limits for the invariant mass selection set more than once! \n";
		std::cout << "I take the first setting and ignore the later ones! \n\n\n";
		return kFALSE;
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0. ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. Min inv. mass too low. \n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. Max inv. mass lower than min inv. mass. \n\n\n";
		return kFALSE;
	}

	// set up the inv. mass selector
	Double_t mu = (min + max)/2.0;
	Double_t fullWidth = max - min;

	fInvMassSel = new RhoMassParticleSelector("invMassFilter", mu, fullWidth);

	// turn filter on
	fInvMassRangeSet = kTRUE;

	return kTRUE;
}


Bool_t PndEvtFilterOnInvMassCounts::SetRhoMassParticleSelector( const char* name, Double_t cv, Double_t w, const char* type )
{
	if ( kTRUE == fInvMassRangeSet ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts of " << this->GetTitle() << ": " << this->GetName() << ": \n";
		std::cout << "You are trying to set the limits for the invariant mass selection set more than once! \n";
		std::cout << "I take the first setting and ignore the later ones! \n\n\n";
		return kFALSE;
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( cv <= 0. ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. cv has to be positive. \n\n\n";
		return kFALSE;
	}
	if ( w < 0. ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": Filter could not be added. w has to be non-negative. \n\n\n";
		return kFALSE;
	}

	fInvMassSel = new RhoMassParticleSelector(name, cv, w, type);

	// turn filter on
	fInvMassRangeSet = kTRUE;

	return kTRUE;
}


Bool_t PndEvtFilterOnInvMassCounts::SetPdgCodesToCombine( Int_t pdgCode1, Int_t pdgCode2, Int_t pdgCode3, Int_t pdgCode4, Int_t pdgCode5 ){
	if ( kTRUE == fPgdCodesSet ){
		std::cout << "\n\n\n  -WARNING from PndEvtFilterOnInvMassCounts of " << this->GetTitle() << ": " << this->GetName() << ": \n";
		std::cout << "You are trying to set the pdgCodes to combine into invariant mass combinations more than once! \n";
		std::cout << "I take the first setting and ignore the later ones! \n\n\n";
		return kFALSE;
	}


	// create temp. vector of all input pdg codes
	std::vector< Int_t > pdgCodes;
	pdgCodes.push_back(pdgCode1);
	pdgCodes.push_back(pdgCode2);
	pdgCodes.push_back(pdgCode3);
	pdgCodes.push_back(pdgCode4);
	pdgCodes.push_back(pdgCode5);

	// Copy valid entries from pdgCodes to fPdgCodesCharges
	for (UInt_t iPdgCodes = 0; iPdgCodes < pdgCodes.size(); ++iPdgCodes){
		// skip kInvalidPdgCode entries in pdgCodes
		if ( kInvalidPdgCode == pdgCodes[iPdgCodes] ) { continue; }
		// If we have invalid pdg codes warn user
		Double_t pdgCodeCharge = kNoChargeSpecified;
		if ( kFALSE == GetCharge( pdgCodes[iPdgCodes], &pdgCodeCharge ) ) {
			std::cout << "\n\n\nFATAL ERROR FROM PndEvtFilterOnInvMassCounts::SetPdgCodesToCombine of " << this->GetTitle() << ": " << this->GetName() << " Charge for pdg code " << pdgCodes[iPdgCodes] << " was not found. \n";
			std::cout << "Filter was not set. \n\n\n”";
			fPgdCodesSet = kFALSE;
			return kFALSE;
		}
		// If we get to this point, we have successfully added a pdg code
		fPdgCodesCharges.push_back(std::pair<Int_t, Double_t> (pdgCodes[iPdgCodes], pdgCodeCharge));
	}

	// Check that we had at least two correctly set pdg codes
	if ( 1 < fPdgCodesCharges.size() ){
		fPgdCodesSet = kTRUE;
		return kTRUE;
	} else {
		std::cout << "\n\n\nFATAL ERROR FROM PndEvtFilterOnInvMassCounts::SetPdgCodesToCombine of " << this->GetTitle() << ": " << this->GetName() << " You have to specify at least 2 valid pdg codes. \n";
		std::cout << "Filter was not set. \n\n\n”";
		fPgdCodesSet = kFALSE;
		return kFALSE;
	}
}



Bool_t PndEvtFilterOnInvMassCounts::EventMatches(Int_t evtNr)
{
	if (fVerbose > 3){
		std::cout << "\n\n";
		std::cout << "Internal eventNr:" << evtNr << "\n";
		std::cout << "Nr. of simulated particles:" << fParticleList->GetEntries()<<"\n";
		PrintAllTParticleInEvent();
		std::cout << "PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": Beginning of EventMatches\n";
	}

	// sanity checks
	if (0==fParticleList){
		std::cout << "\n\n\n PndEvtFilterOnInvMassCounts " << this->GetTitle() << ": " << this->GetName() << ": FATAL ERROR: No particle list! Discard this event.\n\n\n";
		return kFALSE;
	}


	// do not accept events that do not have enough particles
	if ( fParticleList->GetEntriesFast() < fPdgCodesCharges.size() ){
		if (0<fVerbose) std::cout << "\n\n\n PndEvtFilterOnInvMassCounts: Event contains less than " << fPdgCodesCharges.size() << " particles. " << this->GetTitle() << ": " << this->GetName() << " will not accept this event.\n\n\n";
		return kFALSE;
	}

	// get rho cand lists of particles and combine them
	RhoCandList p0, p1, p2, p3, p4;
	FillList( p0, fPdgCodesCharges[0].first, fPdgCodesCharges[0].second );
	FillList( p1, fPdgCodesCharges[1].first, fPdgCodesCharges[1].second );

	RhoCandList combinedList;
	switch( fPdgCodesCharges.size() ) {
	case 2:
		combinedList.Combine(p0, p1);
		break;
	case 3:
		FillList( p2, fPdgCodesCharges[2].first, fPdgCodesCharges[2].second );
		combinedList.Combine(p0, p1, p2);
		break;
	case 4:
		FillList( p2, fPdgCodesCharges[2].first, fPdgCodesCharges[2].second );
		FillList( p3, fPdgCodesCharges[3].first, fPdgCodesCharges[3].second );
		combinedList.Combine(p0, p1, p2, p3);
		break;
	case 5:
		FillList( p2, fPdgCodesCharges[2].first, fPdgCodesCharges[2].second );
		FillList( p3, fPdgCodesCharges[3].first, fPdgCodesCharges[3].second );
		FillList( p4, fPdgCodesCharges[4].first, fPdgCodesCharges[4].second );
		combinedList.Combine(p0, p1, p2, p3, p4);
		break;
	default:
		std::cout << "FATAL ERROR in PndEvtFilterOnInvMassCounts::EventMatches of " << this->GetTitle() << ": " << this->GetName() << ". The number of pdg codes is not supported. \n";
		return kFALSE;
		break;
	}


	if (fVerbose > 3){
		std::cout << "Looking for inv. masses of " << fPdgCodesCharges.size() << " particles.\n";
		std::cout << p0.GetLength() << " / " << p1.GetLength() << " / " << p2.GetLength() << " / " << p3.GetLength() << " / " << p4.GetLength();
		std::cout << " particles in initial lists\n";
		std::cout << combinedList.GetLength() << " particles in combined list before mass selection\n";
	}

	// select acceptable invariant mass range
	combinedList.Select(fInvMassSel);

	if (fVerbose > 3){
		std::cout << combinedList.GetLength() << " particles in combined list after mass selection\n";
	}

	// check if number of acceptable combinations is within acceptable limits
	if ( combinedList.GetLength() < fCountsMinMax.first){
		if (fVerbose >9){
			std::cout << "Event is not accepted by " << this->GetTitle() << ": " << this->GetName() << " because there are too few matching inv. mass combinations in the event. \n";
		}
		return kFALSE;
	}
	if ( combinedList.GetLength() > fCountsMinMax.second ){
		if (fVerbose >9){
			std::cout << "Event is not accepted by " << this->GetTitle() << ": " << this->GetName() << " because there are too many matching inv. mass combinations in the event. \n";
		}
		return kFALSE;
	}


	// for QA
	fAcceptedEventNumbers.insert(evtNr);
	if (fVerbose >5){
		std::cout << "\n Event matches " << this->GetTitle() << ": " << this->GetName() << "\n\n";
	}
	return kTRUE;
}


ClassImp(PndEvtFilterOnInvMassCounts)
