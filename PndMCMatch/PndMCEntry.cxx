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

