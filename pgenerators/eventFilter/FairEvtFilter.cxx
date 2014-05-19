// -------------------------------------------------------------------------
// -----                     FairEvtFilter source file                 -----
// -------------------------------------------------------------------------


#include "FairEvtFilter.h"


// -----   Default constructor   -------------------------------------------
FairEvtFilter::FairEvtFilter(): fVerbose(4), TNamed(),fEventNr(0){
	fAcceptedEventNumbers.clear();}
// -------------------------------------------------------------------------



// -----   Constructor with name and title   -------------------------------
FairEvtFilter::FairEvtFilter(const char* name, const char* title)
: fVerbose(4), TNamed(name, title),fEventNr(0){
	fAcceptedEventNumbers.clear();}
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

ClassImp(FairEvtFilter)
