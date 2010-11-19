/*
 * PndMCEntry.cpp
 *
 *  Created on: Dec 22, 2009
 *      Author: stockman
 */

#include "PndMCEntry.h"

ClassImp(PndMCEntry);

PndMCEntry::PndMCEntry():fPos(-1), fSource(-1) {
	SetPersistanceCheck(kFALSE);
}

PndMCEntry::~PndMCEntry() {
	// TODO Auto-generated destructor stub
}

void PndMCEntry::RemoveType(Int_t type)
{
	std::set<FairLink>::iterator endIter = fLinks.end();
	std::set<FairLink>::iterator it = fLinks.begin();
	//std::cout << "PndMCEntry " << *this << std::endl;
	for (; it!=fLinks.end();){
		//it->Print();
		//std::cout << std::endl;
		if (it->GetType() == type){
			fLinks.erase(it);
			it = fLinks.begin();
		}
		else{
			it++;
		}
	}
}

