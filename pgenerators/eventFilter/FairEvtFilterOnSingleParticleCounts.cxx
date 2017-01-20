// -------------------------------------------------------------------------
// -----              FairEvtFilterOnSingleParticleCounts source file                -----
// -------------------------------------------------------------------------

#include "FairEvtFilterOnSingleParticleCounts.h"
#include "TParticlePDG.h"

std::ostream& operator <<(std::ostream& os, const std::vector<Int_t>& v)
{
	os << "(";
	Int_t lastIdx = v.size()-1;
	if(lastIdx < 0){
		os << ")" ;
		return os;
	}

	for (Int_t iVec = 0; iVec < lastIdx; ++iVec){
		os << v[iVec] << ", ";
	}
	os << v[lastIdx] << ")";
	return os;
}



std::ostream& operator <<(std::ostream& os, const std::vector< std::pair<Double_t,Double_t> >& vpair)
{
	os << "( ";
	Int_t lastIdx = vpair.size()-1;
	if(vpair.size() == 0){
		os << ")" ;
		return os;
	}

	for (Int_t iVec = 0; iVec < lastIdx; ++iVec){
		os << "{";
		os << vpair[iVec].first << ", ";
		os << vpair[iVec].second;
		os << "},";
	}
	os << "{";
	os << vpair[lastIdx].first << ", ";
	os << vpair[lastIdx].second;
	os << "} )";
	return os;
}


std::ostream& operator <<(std::ostream& os, const std::vector< std::pair<Int_t,Int_t> >& vpair)
{
	os << "( ";
	Int_t lastIdx = vpair.size()-1;
	if(vpair.size() == 0){
		os << ")" ;
		return os;
	}

	for (Int_t iVec = 0; iVec < lastIdx; ++iVec){
		os << "{";
		os << vpair[iVec].first << ", ";
		os << vpair[iVec].second;
		os << "},";
	}
	os << "{";
	os << vpair[lastIdx].first << ", ";
	os << vpair[lastIdx].second;
	os << "} )";
	return os;
}


std::ostream& operator <<(std::ostream& os, const std::map<Int_t, std::vector<Int_t> >& pdgGroupId)
{
	if(pdgGroupId.begin() == pdgGroupId.end()){
		os << "( , [ ] )";
		return os;
	}
	for (std::map<Int_t,std::vector<Int_t> >::const_iterator iPdgGroupId = pdgGroupId.begin(); iPdgGroupId != pdgGroupId.end(); ++iPdgGroupId){
		os << "( "<< iPdgGroupId->first << ", [";
		os << iPdgGroupId->second;
		os << "] )";
	}
	return os;
}


// -----   Default constructor   -------------------------------------------
FairEvtFilterOnSingleParticleCounts::FairEvtFilterOnSingleParticleCounts() : fFilterPdg(kFALSE),
		FairEvtFilter(" ", "FairEvtFilterOnSingleParticleCounts"), fFilterCharge(kFALSE),
		fFilterMom(kFALSE),fFilterGeom(kFALSE)
{
	SetDefaultBoundaries();
}


// -----   Constructor with name and title   ------------------------------------
FairEvtFilterOnSingleParticleCounts::FairEvtFilterOnSingleParticleCounts(const char* name, const char* title)
: fFilterPdg(kFALSE), FairEvtFilter(name, title), fFilterCharge(kFALSE),
  fFilterMom(kFALSE),fFilterGeom(kFALSE)
{
	SetDefaultBoundaries();
}


void FairEvtFilterOnSingleParticleCounts::SetDefaultBoundaries(){
	// initialize a default fChargeCountsMinMax with kChargeLastElement entries
	for (UInt_t icount = 0; icount < FairEvtFilter::kChargeLastElement; ++icount){
		fChargeCountsMinMax.push_back(std::pair<Int_t, Int_t> (0,99999));
	}

	// initialize a default fMomMinMax with kMomLastElement entries
	for (UInt_t icount = 0; icount < FairEvtFilter::kMomLastElement; ++icount){
		fMomMinMax.push_back(std::pair<Double_t, Double_t> (-99999.,99999.));
	}

	// initialize a default fGeomMinMax with kGeomLastElement entries
	for (UInt_t icount = 0; icount < FairEvtFilter::kGeomLastElement; ++icount){
		fGeomMinMax.push_back(std::pair<Double_t, Double_t> (-99999.,99999.));
	}
}

// -----   Destructor   ----------------------------------------------------
FairEvtFilterOnSingleParticleCounts::~FairEvtFilterOnSingleParticleCounts(){}


Bool_t FairEvtFilterOnSingleParticleCounts::AndMinMaxPdgCodes( Int_t min, Int_t max, Int_t pdgCode1, Int_t pdgCode2, Int_t pdgCode3, Int_t pdgCode4, Int_t pdgCode5, Int_t pdgCode6, Int_t pdgCode7, Int_t pdgCode8 )
{
	// construct a vector pdgCodes containing all pdg codes given by caller
	// and call the general AndMinMaxPdgCodes method
	std::vector<Int_t> pdgCodes;
	pdgCodes.push_back(pdgCode1);
	pdgCodes.push_back(pdgCode2);
	pdgCodes.push_back(pdgCode3);
	pdgCodes.push_back(pdgCode4);
	pdgCodes.push_back(pdgCode5);
	pdgCodes.push_back(pdgCode6);
	pdgCodes.push_back(pdgCode7);
	pdgCodes.push_back(pdgCode8);

	return AndMinMaxPdgCodes( min, max, pdgCodes );
}


Bool_t FairEvtFilterOnSingleParticleCounts::AndMinMaxPdgCodes( Int_t min, Int_t max, std::vector<Int_t> &pdgCodes )
{
	// was adding of the filter successful
	Bool_t filterAdded = kFALSE;

	if (fVerbose > 0){
		std::cout << this->GetTitle() << ": " << this->GetName() << "\n";
		std::cout << "fPdgGroupId: " << fPdgGroupId << "\n";
		std::cout << "fGroupIdCountsMinMax: " << fGroupIdCountsMinMax << "\n\n";
	}


	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0 ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at least a number <= 0 of some particles. That makes no sense. Check your AndMinMaxPdgCodes calls!\n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at most a number of some particles which is less than the minimum number that you request. That makes no sense. Check your AndMinMaxPdgCodes calls!\n\n\n";
		return kFALSE;
	}

	// find the groupId for the pdgCodes
	// the current pdgCodes are appended to the pre-existing vector
	Int_t newGroupId = fGroupIdCountsMinMax.size();

	// for every entry in pdgCodes
	// -- save a new entry in the map fPdgGroupId if the entry is not already in map
	// -- add a new groupId to the vector in fPdgGroupId if the entry already exists in the map

	for (UInt_t iPdgCodes = 0; iPdgCodes < pdgCodes.size(); ++iPdgCodes){
		// skip kInvalidPdgCode entries in pdgCodes
		if ( kInvalidPdgCode != pdgCodes[iPdgCodes] ) {
			// check if map has already an entry for the pdgCode
			PdgGroupIdIterator finder = fPdgGroupId.find(pdgCodes[iPdgCodes]);
			if ( fPdgGroupId.end() != finder){
				// pdgCode is already in map, add groupId to vector

				std::vector<Int_t> & groupIdVector = fPdgGroupId[pdgCodes[iPdgCodes]];
				groupIdVector.push_back(newGroupId);
			}
			else {
				// pdgCode is not in map yet, create new entry of Int_t and vector<Int_t>

				std::vector<Int_t> groupIdVector;
				groupIdVector.push_back(newGroupId);
				fPdgGroupId.insert(PdgGroupIdPair(pdgCodes[iPdgCodes],groupIdVector));
			}

			filterAdded = kTRUE;
		}
	}

	if ( kTRUE == filterAdded ){
		// set the minimum and maximum number of particles for the current groupId
		fGroupIdCountsMinMax.push_back(std::pair<Int_t, Int_t> (min,max));

	} else {
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Tried to add filter, but that was not possible as all pdg codes were kInvalidPdgCode!\n\n\n";
		return kFALSE;
	}

	// turn filter on
	fFilterPdg = kTRUE;

	if (fVerbose > 0){
		std::cout << "FairEvtFilterOnSingleParticleCounts: After adding PdgFilter:\n";
		std::cout << "fPdgGroupId: " << fPdgGroupId << "\n";
		std::cout << "fGroupIdCountsMinMax: " << fGroupIdCountsMinMax << "\n\n";
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AndMinMaxCharge( Int_t min, Int_t max, ChargeState charge )
{

	if (fVerbose > 0){
		std::cout << this->GetTitle() << ": " << this->GetName() << "\n";
		std::cout << "fChargeCountsMinMax: " << fChargeCountsMinMax << "\n\n";
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0 ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at least a number <= 0 of some particles. That makes no sense. Check your AndMinMaxCharge calls!\n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at most a number of some particles which is less than the minimum number that you request. That makes no sense. Check your AndMinMaxCharge calls!\n\n\n";
		return kFALSE;
	}

	// saving the lower and upper limit of particle multiplicities for the corresponding ChargeState in fChargeCountsMinMax
	fChargeCountsMinMax[charge]=std::pair<Int_t, Int_t> (min,max);

	// turn filter on
	fFilterCharge=kTRUE;

	if (fVerbose > 0){
		std::cout << "FairEvtFilterOnSingleParticleCounts: After adding ChargeFilter:\n";
		std::cout << "fGroupIdCountsMinMax: " << fChargeCountsMinMax << "\n\n";
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AndMinMaxMom( Double_t min, Double_t max, FairEvtFilter::MomState mom )
{

	if (fVerbose > 0){
		std::cout << this->GetTitle() << ": " << this->GetName() << "\n";
		std::cout << "fMomMinMax { P , Pt , Pz }: " << fMomMinMax << "\n\n";
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0 ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at least a number <= 0 of some particles. Check your AndMinMaxMom calls.\n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at most a number of some particles which is less than the minimum number that you request. Check your AndMinMaxMom calls.\n\n\n";
		return kFALSE;
	}

	// saving the lower and upper limit for the corresponding MomState in fMomMinMax
	fMomMinMax[mom]=std::pair<Double_t, Double_t> (min,max);

	// turn filter on
	fFilterMom=kTRUE;

	if (fVerbose > 0){
		std::cout << "FairEvtFilterOnSingleParticleCounts: After adding MomFilter:\n";
		std::cout << "fMomMinMax { P , Pt , Pz }: " << fMomMinMax << "\n\n";
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AndMinMaxGeom( Double_t min, Double_t max, FairEvtFilter::GeomState geom )
{

	if (fVerbose > 0){
		std::cout << this->GetTitle() << ": " << this->GetName() << "\n";
		std::cout << "fGeomMinMax { Theta , Phi, VertexZ, VertexRho, VertexRadius }: " << fGeomMinMax << "\n\n";
	}

	// check if filter can be applied (make sure that min and max make sense)
	if ( min < 0 ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at least a number <= 0 of some particles. Check your AndMinMaxGeom calls.\n\n\n";
		return kFALSE;
	}
	if ( max < min ){
		std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Filter could not be added. You are trying to request that your events should have at most a number of some particles which is less than the minimum number that you request. Check your AndMinMaxGeom calls.\n\n\n";
		return kFALSE;
	}

	if(geom==0 || geom==1){
		min=min*2*(TMath::Pi())/360;
		max=max*2*(TMath::Pi())/360;
	}

	// saving the lower and upper limit for the corresponding GeomState in fGeomMinMax
	fGeomMinMax[geom]=std::pair<Double_t, Double_t> (min,max);


	// turn filter on
	fFilterGeom=kTRUE;

	if (fVerbose > 0){
		std::cout << "FairEvtFilterOnSingleParticleCounts: After adding GeomFilter:\n";
		std::cout << "fGeomMinMax { Theta , Phi, VertexZ, VertexRho, VertexRadius }: " << fGeomMinMax << "\n\n";
	}

	return kTRUE;
}


void FairEvtFilterOnSingleParticleCounts::InitCounters()
{
	// initialize a 0 fCountGroupId of same size as fGroupIdCountsMinMax
	fCountGroupId.clear();
	for (UInt_t icountPdg = 0; icountPdg < fGroupIdCountsMinMax.size(); ++icountPdg){
		fCountGroupId.push_back(0);
	}

	// fCountGroupId counts up, event matches if all entries in fCountGroupId are between the according entries in fGroupIdCountsMinMax
	if (fVerbose >11){
		std::cout << "fCountGroupId after initialization: " << fCountGroupId << "\n";
	}

	// initialize a 0 fCountCharge of same size as fChargeCountsMinMax
	fCountCharge.clear();
	for (UInt_t icountCharge = 0; icountCharge < FairEvtFilter::kChargeLastElement; ++icountCharge){
		fCountCharge.push_back(0);
	}

	// fCountCharge counts up, event matches if all entries in fCountCharge are between the according entries in fChargeCountsMinMax
	if (fVerbose >11){
		std::cout << "fCountCharge after initialization: " << fCountCharge << "\n";
	}
}


Bool_t FairEvtFilterOnSingleParticleCounts::AcceptMomentum(TParticle* particle)
{
	if (fVerbose > 9){
		std::cout << "{ P , Pt , Pz }: { " << particle->P() << ", " << particle->Pt() << ", " << particle->Pz() << " }\n";
	}

	Double_t momentum;
	std::string type;
	for(UInt_t iMom=0; iMom < FairEvtFilter::kMomLastElement; iMom++){//check every momentum constraint for the considered particle
		switch(iMom){
		case kMomTotal:
			momentum=particle->P();
			type="P";
			break;
		case kMomTrans:
			momentum=particle->Pt();
			type="Pt";
			break;
		case kMomZ:
			momentum=particle->Pz();
			type="Pz";
			break;
		default:
			std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Check your filter calls!\n\n\n";
			return kFALSE;
			if ( momentum < fMomMinMax[iMom].first){
				if (fVerbose >9){
					std::cout << "Set MomOk to kFALSE because of " << type << "< And" << type << "Range() minimum." << "\n";
				}
				return kFALSE;
			}
			if ( momentum > fMomMinMax[iMom].second ){
				if (fVerbose >9){
					std::cout << "Set MomOk to kFALSE because of " << type << " > And" << type << "Range() maximum." << "\n";
				}
				return kFALSE;
			}
		}
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AcceptGeometry(TParticle* particle)
{
	if (fVerbose > 9){
		std::cout << "{ Theta , Phi , Vz, VRho, VRadius }: { " << particle->Theta()*180./TMath::Pi() << ", " << particle->Phi()*180./TMath::Pi();
		std::cout << ", " << particle->Vz() << ", " << particle->Rho() << ", " << particle->R()<< " }\n";
	}

	Double_t geom;
	std::string type;
	for(UInt_t iGeom=0; iGeom < FairEvtFilter::kGeomLastElement; iGeom++){//check every geometry constraint for the considered particle
		switch(iGeom){
		case kTheta:
			geom=particle->Theta();
			type="Theta";
			break;
		case kPhi:
			geom=particle->Phi();
			type="Phi";
			break;
		case kVertexZ:
			geom=particle->Vz();
			type="Vz";
			break;
		case kVertexRho:
			geom=particle->Rho();
			type="VRho";
			break;
		case kVertexRadius:
			geom=particle->R();
			type="VRadius";
			break;
		default:
			std::cout << "\n\n\n  -WARNING from FairEvtFilterOnSingleParticleCounts: Check your filter calls!\n\n\n";
			return kFALSE;
		}
		if ( geom < fGeomMinMax[iGeom].first){
			if (fVerbose >9){
				std::cout << "Set GeomOk to kFALSE because of " << type << " < And" << type << "Range() minimum." << "\n";
			}
			return kFALSE;
		}
		if ( geom > fGeomMinMax[iGeom].second ){
			if (fVerbose >9){
				std::cout << "Set GeomOk to kFALSE because of " << type << " > And" << type << "Range() maximum." << "\n";
			}
			return kFALSE;
		}
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AcceptPdgCounter()
{	// check if event matches the filter
	// event matches if
	// fCountGroupId[icountPdg] >= fGroupIdCountsMinMax[icountPdg].first
	// and
	// fCountGroupId[icountPdg] <= fGroupIdCountsMinMax[icountPdg].second
	for (UInt_t icountPdg = 0; icountPdg < fCountGroupId.size(); ++icountPdg){
		if ( fCountGroupId[icountPdg] < fGroupIdCountsMinMax[icountPdg].first){
			if (fVerbose >9){
				std::cout << "Event does NOT match filter because of fCountGroupId[icountPdg] < fGroupIdCountMinMax[icountPdg].first for icountPdg == " << icountPdg << "\n";
			}
			return kFALSE;
		}
		if ( fCountGroupId[icountPdg] > fGroupIdCountsMinMax[icountPdg].second ){
			if (fVerbose >9){
				std::cout << "Event does NOT match filter because of fCountGroupId[icountPdg] > fGroupIdCountsMinMax[icountPdg].second for icountPdg == " << icountPdg << "\n";
			}
			return kFALSE;
		}
	}
	return kTRUE;
}


Bool_t FairEvtFilterOnSingleParticleCounts::AcceptChargeCounter()
{	// check if event matches the filter
	// event matches if
	// fCountCharge[icountPdg] >= fChargeCountsMinMax[icountPdg].first
	// and
	// fCountCharge[icountPdg] <= fChargeCountsMinMax[icountPdg].second
	for (UInt_t icountCharge = 0; icountCharge < FairEvtFilter::kChargeLastElement; ++icountCharge){
		if ( fCountCharge[icountCharge] < fChargeCountsMinMax[icountCharge].first){
			if (fVerbose >9){
				std::cout << "Event does not match filter because of fCountCharge[icountChart] < fChargeCountMinMax[icountCharge].first for icountCharge == " << icountCharge << "\n";
			}
			return kFALSE;
		}
		if ( fCountCharge[icountCharge] > fChargeCountsMinMax[icountCharge].second ){
			if (fVerbose >9){
				std::cout << "Event does not match filter because of fCountCharge[icountCharge] > fChargeCountsMinMax[icountCharge].second for icountCharge == " << icountCharge << "\n";
			}
			return kFALSE;
		}
	}
	return kTRUE;
}


void FairEvtFilterOnSingleParticleCounts::CountPdg(TParticle* particle){
	// every restriction that is placed on a pdg code is described by a groupId
	// one groupId can belong to several particles assuming that they are indistinguishable
	// fPdgGroupId contains all the user defined constraints on particles with certain pdg codes organized in a map
	// all elements in this map have the pdg code as first entry and a vector of groupId's as second entry
	// if there exists an entry for the considered particle in the map the appropriate entry of fCountGroupId is counted up
	//  whereat the associated groupId's are exactly representing the index of the incremented fCountGroupId vector

	// get pdg code for particle
	Int_t partPdg = particle->GetPdgCode();

	// check if the pdg code is contained in some groupId
	PdgGroupIdIterator it = fPdgGroupId.find(partPdg);
	if ( fPdgGroupId.end() != it ){
		// pdg code was found
		std::vector<Int_t> groupIdVector = it->second;
		for (size_t iGroupId = 0; iGroupId < groupIdVector.size(); ++iGroupId){
			++fCountGroupId[groupIdVector[iGroupId]];
		}
	}

	if (fVerbose >9){
		std::cout << "partPdg:" << partPdg << "    ->    " << "fCountGroupId: " << fCountGroupId << "\n";
	}
}


void FairEvtFilterOnSingleParticleCounts::CountCharge(TParticle* particle){
	// get pdg code for particle
	Int_t partPdg = particle->GetPdgCode();


	// get charge for pdg code
	Double_t pdgCodeCharge = 0;
	if ( kFALSE == GetCharge( partPdg, &pdgCodeCharge ) ) {
		// skip particles of unknown charge
		return;
	}


	// Based on charge increment the according entry in fChargeCountsMinMax
	if(pdgCodeCharge!=0){
		fCountCharge[FairEvtFilter::kCharged]++;
		if(pdgCodeCharge>0){
			fCountCharge[FairEvtFilter::kPlus]++;
		}else{
			fCountCharge[FairEvtFilter::kMinus]++;
		}
	}else{
		fCountCharge[FairEvtFilter::kNeutral]++;
	}
	fCountCharge[FairEvtFilter::kAll]++;

	if (fVerbose >9){
		std::cout << "Charge:" << pdgCodeCharge << "    ->    " << "fCountCharge: " << fCountCharge << "\n";
	}
}


Bool_t FairEvtFilterOnSingleParticleCounts :: EventMatches(Int_t evtNr)
{
	if (fVerbose > 3){
		std::cout << "\n\n";
		std::cout << "Generated event: " << evtNr << "\n";
		std::cout << "FairEvtFilterOnSingleParticleCounts: " << this->GetTitle() << ": " << this->GetName() << " Beginning of EventMatches\n";
		std::cout << "Nr. of simulated particles: " << fParticleList->GetEntries()<<"\n";
		PrintAllTParticleInEvent();
		std::cout << "fPdgGroupId: " << fPdgGroupId << "\n";
		std::cout << "fGroupIdCountsMinMax: " << fGroupIdCountsMinMax << "\n";
		std::cout << "fChargeCountsMinMax: " << fChargeCountsMinMax << "\n";
		std::cout << "fMomMinMax: " << fMomMinMax << "\n";
		std::cout << "fGeomMinMax: " << fGeomMinMax << "\n";
	}


	// Not necessary as FairFilteredPrimaryGenerator already checks this
	//	// event matches if filter is not set
	//	if ( kFALSE == FilterActive() ) {
	//		return kTRUE;
	//	}


	// sanity checks
	if (0==fParticleList){
		std::cout << "\n\n\n FairEvtFilterOnSingleParticleCounts: FATAL ERROR: No particle list! Event does not match.\n\n\n";
		return kFALSE;
	}

	if (0==fParticleList->GetEntriesFast()){
		std::cout << "\n\n\n FairEvtFilterOnSingleParticleCounts: Event contains 0 particles. Event does not match.\n\n\n";
		return kFALSE;
	}

	// reset all counters for each new event
	InitCounters();


	// loop the produced particles and count up
	// the specified "groupIDs"
	// and charge states
	// for particles which satisfy the momentum, angular and vertex constraints
	for (Int_t iPart= 0; iPart< fParticleList->GetEntries(); iPart++) {

		TParticle* particle=(TParticle*)fParticleList->At(iPart);
		if (0 == particle) { continue; }

		// check whether the particle satisfies the momentum constraints
		if(fFilterMom){
			if(!AcceptMomentum(particle)){ continue; } // if the particle doesn't suit the momentum filter settings go ahead to the next particle
		}

		// check whether the particle satisfies the geometric constraints
		if(fFilterGeom){
			if(!AcceptGeometry(particle)){ continue; } // if the particle doesn't suit the geometric filter settings go ahead to the next particle
		}

		// count up the appropriate element of fCountGroupId and fCountCharge
		// important: Do not count before all of the above tests were passed!
		if(fFilterPdg){
			CountPdg(particle);
		}
		if(fFilterCharge){
			CountCharge(particle);
		}

	}// end of the loop over all particles

	// reset PdgOk and ChargeOk to kTRUE for each new event
	// for checking if event matches
	Bool_t PdgOk= kTRUE;
	Bool_t ChargeOk = kTRUE;

	if (fVerbose >9){
		std::cout << "\n Check if event matches the filter \n";
		std::cout << "fCountGroupId: " << fCountGroupId << "\n";
		std::cout <<  "fGroupIdCountsMinMax: " << fGroupIdCountsMinMax << "\n";
		std::cout << "fCountCharge: " << fCountCharge << "\n";
		std::cout <<  "fChargeCountsMinMax: " << fChargeCountsMinMax << "\n";
	}


	if(fFilterPdg){
		PdgOk=AcceptPdgCounter();
	}

	if(fFilterCharge){
		ChargeOk=AcceptChargeCounter();
	}

	// event matches if pdg and charges are ok
	Bool_t evtOk=PdgOk && ChargeOk;

	if ( kTRUE == evtOk ) {
		fAcceptedEventNumbers.insert(evtNr); // for QA
	}

	if (fVerbose >5){
		if ( kTRUE == evtOk ) {
			std::cout << "\n Event matches " << this->GetTitle() << ": " << this->GetName() << "\n\n";
		} else {
			std::cout << "\n Event does NOT match " << this->GetTitle() << ": " << this->GetName() << "\n\n";
		}
	}
	return evtOk;
}


ClassImp(FairEvtFilterOnSingleParticleCounts)
