// -------------------------------------------------------------------------
// -----                    PndEvtFilter header file                   -----
// -------------------------------------------------------------------------


/** PndEvtFilter.h

Author: Martin Galuska

Abstract base class for event filters adding PandaRoot specific capabilities (FillList mainly)

Can be used to derive event filters which reject generated events which should not be fed into the transport.

Each concrete filter class derived from this one must implement the abstract
method EventMatches, in which the particular filter criteria are realized.

 **/


#ifndef PndEvtFilter_H
#define PndEvtFilter_H


#include "Rtypes.h"                     // for Bool_t, etc
#include "TClonesArray.h"
#include <iostream>
#include "TParticle.h"
#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "TLorentzVector.h"
#include "FairEvtFilter.h"

std::ostream& operator <<(std::ostream& os, const std::set<Int_t> & set);


class PndEvtFilter : public FairEvtFilter
{

public:

	/** Default constructor. **/
	PndEvtFilter();


	/** Constructor with name and title **/
	PndEvtFilter(const char* name, const char* title="PndEvtFilter");


	/** Destructor. **/
	virtual ~PndEvtFilter();


	//Initialize the filter if needed
	Bool_t Init() { return kTRUE;}


	// useful for event filtering on invariant masses
	// returns a list of particles
	// which have the same charge as charge(inPdgCode)
	// mass hypothesis is set to m(inPdgCode)
	// for better performance pdgCodeCharge can optionally be specified
	// It should be the same as the charge corresponding to inPdgCode
	Bool_t FillList ( RhoCandList& rhoOutList, Int_t inPdgCode, Double_t pdgCodeCharge = kNoChargeSpecified );

	// returning kTRUE indicates that the event satisfies the filter conditions,
	// kFALSE otherwise.
	virtual Bool_t EventMatches(Int_t evtNr)=0;

	virtual Bool_t FilterActive()=0;


private:
	PndEvtFilter(const PndEvtFilter& G);
	PndEvtFilter& operator= (const PndEvtFilter&) {return *this;}



	ClassDef(PndEvtFilter,1);
};

#endif


