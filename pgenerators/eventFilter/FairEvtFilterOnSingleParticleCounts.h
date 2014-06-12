// -------------------------------------------------------------------------
// -----             FairEvtFilterOnSingleParticleCounts header file                 -----
// -------------------------------------------------------------------------


/** FairEvtFilterOnSingleParticleCounts.h

 Authors: Martin Galuska, Katja Kleeberg
 Martin dot J dot Galuska at physik dot uni minus giessen dot de

 Purpose:
 This class is used for an event filter expansion of FairPrimaryGenerator.
 It filters events right after event generation and before transport through the detector model
 events will either be accepted (and further simulated)
 or rejected completely.

 This filter class implements such decisions based on particle multiplicities in the events
 This filter class can count multiplicities of
   particles whose pdg code are in a user defined list of pdg codes (called a group ID)
   particles with desired electrical charge
   all particles
 Multiple constraints of the above criteria are supported in one filter.

 The standard is that all particles in the event are counted.
 Optionally, only particles that satisfy certain constraints on momentum / angles or vertices are counted.
 Only one range of such constraints are supported for one filter.

 The user has to specify the minimum or maximum multiplicities for each group of pdg codes / charge state / all particles to be investigated
 (Multiple conditions for the same PDG code are supported)

 **/



#ifndef FairEvtFilterOnSingleParticleCounts_H_
#define FairEvtFilterOnSingleParticleCounts_H_

#include "FairEvtFilter.h"
#include "TParticle.h"
#include "TMath.h"
#include<map>
#include<vector>
#include<iostream>


typedef std::map<Int_t, std::vector<Int_t> >  PdgGroupId;
typedef std::map<Int_t,std::vector<Int_t> >::iterator PdgGroupIdIterator;

std::ostream& operator <<(std::ostream& os, const std::vector<Int_t>& v);
std::ostream& operator <<(std::ostream& os, const std::vector< std::pair<Double_t, Double_t> >& vpair);
std::ostream& operator <<(std::ostream& os, const std::vector< std::pair<Int_t, Int_t> >& vpair);
std::ostream& operator <<(std::ostream& os, const std::map<Int_t, std::vector<Int_t> >& PdgGroupId);


class FairEvtFilterOnSingleParticleCounts : public FairEvtFilter
{

public:

	//Default constructor
	FairEvtFilterOnSingleParticleCounts();

	//Constructor with name and title
	FairEvtFilterOnSingleParticleCounts(const char* name, const char* title="FairEvtFilterOnSingleParticleCounts");

	// Destructor
	virtual ~FairEvtFilterOnSingleParticleCounts();

	//////////////////
	// User interfaces -- Pdg Code Min and Max
	//////////////////
	// Use this for grouping up to 8 pdgCodes into 1 groupId
	// all particles belonging to the groupId are regarded as being indistinguishable
	// min defines how many particles you want in your events AT LEAST
	// max defines how many particles you want in your events AT MOST
	// the min and max numbers are used for all particles with one of the above pdgCodes
	// returns kTRUE if the filter was added, otherwise returns kFALSE
	Bool_t AndMinMaxPdgCodes( Int_t min, Int_t max, Int_t pdgCode1, Int_t pdgCode2=kInvalidPdgCode, Int_t pdgCode3=kInvalidPdgCode, Int_t pdgCode4=kInvalidPdgCode, Int_t pdgCode5=kInvalidPdgCode, Int_t pdgCode6=kInvalidPdgCode, Int_t pdgCode7=kInvalidPdgCode, Int_t pdgCode8=kInvalidPdgCode );


	// use if you want to group more than 8 pdgCodes into 1 groupId
	// all particles belonging to the groupId are regarded as being indistinguishable
	// min defines how many particles you want in your events AT LEAST
	// max defines how many particles you want in your events AT MOST
	// the min and max numbers are used for all particles with pdgCodes in the vector pdgCodes
	// returns kTRUE if the filter was added, otherwise returns kFALSE
	// all entries containing invalidPdgCode in the pdgCodes vector are skipped (as there is no pdgCode invalidPdgCode)
	Bool_t AndMinMaxPdgCodes( Int_t min, Int_t max, std::vector<Int_t> &pdgCodes );


	//////////////////
	// User interfaces -- Pdg Code Min only
	//////////////////
	Bool_t AndMinPdgCodes(Int_t min, Int_t pdgCode1, Int_t pdgCode2=kInvalidPdgCode, Int_t pdgCode3=kInvalidPdgCode, Int_t pdgCode4=kInvalidPdgCode, Int_t pdgCode5=kInvalidPdgCode, Int_t pdgCode6=kInvalidPdgCode, Int_t pdgCode7=kInvalidPdgCode, Int_t pdgCode8=kInvalidPdgCode){
		return AndMinMaxPdgCodes( min, 9999, pdgCode1, pdgCode2, pdgCode3, pdgCode4, pdgCode5, pdgCode6, pdgCode7, pdgCode8  );
	};
	Bool_t AndMinPdgCodes(Int_t min, std::vector<Int_t> &pdgCodes ){
		return AndMinMaxPdgCodes( min, 9999, pdgCodes );
	};


	//////////////////
	// User interfaces -- Pdg Code Max only
	//////////////////
	Bool_t AndMaxPdgCodes(Int_t max, std::vector<Int_t> &pdgCodes ){
		return AndMinMaxPdgCodes( 0, max, pdgCodes );
	};
	Bool_t AndMaxPdgCodes(Int_t max, Int_t pdgCode1, Int_t pdgCode2=kInvalidPdgCode, Int_t pdgCode3=kInvalidPdgCode, Int_t pdgCode4=kInvalidPdgCode, Int_t pdgCode5=kInvalidPdgCode, Int_t pdgCode6=kInvalidPdgCode, Int_t pdgCode7=kInvalidPdgCode, Int_t pdgCode8=kInvalidPdgCode){
		return AndMinMaxPdgCodes( 0, max, pdgCode1, pdgCode2, pdgCode3, pdgCode4, pdgCode5, pdgCode6, pdgCode7, pdgCode8 );
	};


	//////////////////
	// User interfaces -- Charge
	//////////////////
	// min defines how many particles you want in your events AT LEAST
	// max defines how many particles you want in your events AT MOST
	// Use for setting multiplicity constraints based on particle charges
	// ChargeState
	// kNeutral - neutral particles
	// kPlus - positively charged particles
	// kMinus - negatively charged particles
	// kCharged - positively or negatively charged particles
	// kAll - all particles (neutral or charged)
	Bool_t AndMinMaxCharge( Int_t min, Int_t max, ChargeState charge );


	//use the methods above with fixed minimum or maximum number of particles
	Bool_t AndMinCharge(Int_t min, ChargeState charge){return AndMinMaxCharge( min, 9999, charge);};
	Bool_t AndMaxCharge(Int_t max, ChargeState charge){return AndMinMaxCharge( 0, max, charge);};


	//////////////////
	// User interfaces -- All particles in event
	//////////////////
	// Use if you want to put constraints on the number of particles in the event
	// Functionality is implemented in AndMinMaxCharge
	Bool_t AndMinMaxAllParticles(Int_t min, Int_t max){ return AndMinMaxCharge(min, max, kAll); };


	//use the methods above with fixed minimum or maximum number of particles
	Bool_t AndMinAllParticles(Int_t min){ return AndMinMaxCharge( min, 9999, kAll); };
	Bool_t AndMaxAllParticles(Int_t max){ return AndMinMaxCharge( 0, max, kAll); };


	//////////////////
	// User interfaces -- Momentum constraints
	//////////////////
	//use if you want to consider particles which fulfill certain momentum constraints
	//could be used in combination with any other filter, if you want to set momentum limits for several particles
	Bool_t AndPRange(Double_t min, Double_t max){return AndMinMaxMom( min, max, kMomTotal);};
	Bool_t AndPtRange(Double_t min, Double_t max){return AndMinMaxMom( min, max, kMomTrans);};
	Bool_t AndPzRange(Double_t min, Double_t max){return AndMinMaxMom( min, max, kMomZ);};


	//////////////////
	// User interfaces -- Angular constraints
	//////////////////
	//use if you want to consider particles in certain angle or vertex ranges
	//could be used in combination with any other filter, if you want to set geometric limits for several particles
	Bool_t AndThetaRange(Double_t min, Double_t max){return AndMinMaxGeom( min, max, kTheta);};
	Bool_t AndPhiRange(Double_t min, Double_t max){return AndMinMaxGeom( min, max, kPhi);};


	//////////////////
	// User interfaces -- Vertex constraints
	//////////////////
	//use if you want to consider particles in certain angle or vertex ranges
	//could be used in combination with any other filter, if you want to set geometric limits for several particles
	Bool_t AndVzRange(Double_t min, Double_t max){return AndMinMaxGeom( min, max, kVertexZ);};
	Bool_t AndRhoRange(Double_t min, Double_t max){return AndMinMaxGeom( min, max, kVertexRho);};
	Bool_t AndRadiusRange(Double_t min, Double_t max){return AndMinMaxGeom( min, max, kVertexRadius);};


	// checks if the particles in the event (fParticleList) suit the filter settings
	// kTRUE if event matches, kFALSE otherwise
	Bool_t EventMatches(Int_t evtNr);


	// returns kTRUE if any filter is turned on
	Bool_t FilterActive(){return (fFilterPdg || fFilterCharge || fFilterMom || fFilterGeom);};


private:
	// in these methods the functions of the user interfaces from above are realized
	// all user interfaces are only calling this two methods with different arguments
	Bool_t AndMinMaxMom(Double_t min, Double_t max, MomState mom);
	Bool_t AndMinMaxGeom(Double_t min, Double_t max, GeomState geom);


	// initialize the counters for pdg codes or charge states
	void InitCounters();


	// Set default boundaries for momenta, angles and vertices
	// that are always fulfilled
	// this implements the default case of not filtering on momenta, angles or vertices
	void SetDefaultBoundaries();


	// count up the appropriate element of fCountCharge
	void CountCharge(TParticle* particle);
	// count up the appropriate element of fCountGroupId
	void CountPdg(TParticle* particle);

	// check whether the particle fulfills the momentum constraints
	Bool_t AcceptMomentum(TParticle* particle);
	// check whether the particle fulfills the geometry constraints
	Bool_t AcceptGeometry(TParticle* particle);

	// check whether the event satisfies the constraints on pdg multiplicities (fCountGroupId vector has to be filled already)
	Bool_t AcceptPdgCounter();
	//check whether the event satisfies the constraints on charge state multiplicities (fCountCharge vector has to be filled already)
	Bool_t AcceptChargeCounter();


	// first input = pdg code (as used by event generators)
	// second input = "vector of group IDs that the pdg code is assigned to"
	// one pdg code can be assigned to mutliple group IDs
	// all pdg codes within the same group ID are treated as indistinguishable
	// group IDs are automatically assigned; they are not known to the user and have to start with 0 and be consecutive as they are used as the index for fPartCounts
	PdgGroupId fPdgGroupId;

	// for counting the multiplicities based on group IDs
	std::vector<Int_t> fCountGroupId;
	// for counts the multiplicities based on electrical charge
	std::vector<Int_t> fCountCharge;


	// vector defining the minimum and maximum acceptable multiplicities for each group ID
	std::vector< std::pair<Int_t,Int_t> > fGroupIdCountsMinMax;
	// vector containing the minimum and maximum acceptable multiplicities for each electrical charge state
	// check ChargeState in FairEvtFilter.h for translating the index to an electrical charge
	std::vector< std::pair<Int_t,Int_t> > fChargeCountsMinMax;


	// only particles with a momentum within the min and max value will be counted
	// check MomState in FairEvtFilter.h for translating the index to an electrical charge
	std::vector< std::pair<Double_t,Double_t> > fMomMinMax;
	// only particles with angles and vertices within the min and max value will be counted
	// check GeomState in FairEvtFilter.h for translating the index to an electrical charge
	std::vector< std::pair<Double_t,Double_t> > fGeomMinMax;


	Bool_t fFilterPdg; // is kTRUE if any pdg / group ID filter is set
	Bool_t fFilterCharge; // is kTRUE if any filter on electrical charge is set (also kTRUE if filter on the total number of particles is set - charge state kAll)
	Bool_t fFilterMom; // is kTRUE if any momentum filter is set
	Bool_t fFilterGeom; // is kTRUE if any geometry filter is set


	// constant holding an integer number which is not used as a pdg code
	// this serves as a place holder when constructing a vector from integers given by the user
	static const Int_t kInvalidPdgCode = 0;


	ClassDef(FairEvtFilterOnSingleParticleCounts,1);

};


#endif /* FairEvtFilterOnSingleParticleCounts_H_ */
