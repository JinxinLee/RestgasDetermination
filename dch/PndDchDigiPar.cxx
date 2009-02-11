/////////////////////////////////////////////////////////////
// PndDchDigiPar
//
// Class for geometry parameters of DCH
//
/////////////////////////////////////////////////////////////


#include "PndDchDigiPar.h"
#include "FairParamList.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(PndDchDigiPar)

PndDchDigiPar::PndDchDigiPar(const char* name,const char* title,const char* context)
: FairParGenericSet(name,title,context) {

	fVariables=NULL;
}

PndDchDigiPar::~PndDchDigiPar(void) {
}

void PndDchDigiPar::clear(void) {
	if (fVariables)
		delete fVariables;
}

void PndDchDigiPar::putParams(FairParamList* l) {
	if (!l)
		return;
	l->addObject("Drift times and distances for Calibration of Dch", fVariables);
}

Bool_t PndDchDigiPar::getParams(FairParamList* l) {
	if (!l)
		return kFALSE;

	if (!l->fillObject("Drift times and distances for Calibration of Dch",
			fVariables))
		return kFALSE;

	return kTRUE;
}

