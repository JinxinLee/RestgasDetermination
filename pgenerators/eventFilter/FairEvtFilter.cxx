// -------------------------------------------------------------------------
// -----                     FairEvtFilter source file                 -----
// -------------------------------------------------------------------------


#include "FairEvtFilter.h"



// -----   Default constructor   -------------------------------------------
FairEvtFilter::FairEvtFilter()
: TNamed(), fEventNr(0), fVerbose(0), fTestMode(0)
{
	fAcceptedEventNumbers.clear();
	fParticleList = 0;
}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
FairEvtFilter::FairEvtFilter(const char* name, const char* title)
: TNamed(name, title), fEventNr(0), fVerbose(0), fTestMode(0)
{
	fAcceptedEventNumbers.clear();
	fParticleList = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
FairEvtFilter::~FairEvtFilter() { }
// -------------------------------------------------------------------------


std::ostream& operator <<(std::ostream& os, const std::set<Int_t> & set)
{
	std::set<Int_t>::iterator iter;
	os << "{";
	for(iter=set.begin(); iter!=set.end(); ++iter){
		os << *iter;
		if(iter!=--set.end()){os << ",";}
	}
	os << "}\n";

	return os;
}


void FairEvtFilter::PrintAllTParticleInEvent(){
	// sanity checks
	if (0==fParticleList){ return; }
	for (Int_t iPart=0; iPart<fParticleList->GetEntries(); ++iPart) {
		TParticle *particle = (TParticle*)fParticleList->At(iPart);
		particle->Print();
	}
}


ClassImp(FairEvtFilter)
