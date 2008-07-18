/////////////////////////////////////////////////////////////
// PndDchDigiPar
//
// Class for geometry parameters of DCH
//
/////////////////////////////////////////////////////////////


#include "PndDchDigiPar.h"
#include "CbmParamList.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

ClassImp(PndDchDigiPar)

PndDchDigiPar::PndDchDigiPar(const char* name,const char* title,const char* context)
: CbmParGenericSet(name,title,context) {

	fVariables=NULL;
}

PndDchDigiPar::~PndDchDigiPar(void) {
}

void PndDchDigiPar::clear(void) {
	if (fVariables)
		delete fVariables;
}

void PndDchDigiPar::putParams(CbmParamList* l) {
	if (!l)
		return;
	l->addBinary("Drift times and distances for Calibration of Dch", fVariables);
}

Bool_t PndDchDigiPar::getParams(CbmParamList* l) {
	if (!l)
		return kFALSE;

	if (!l->fillBinary("Drift times and distances for Calibration of Dch",
			fVariables))
		return kFALSE;

	return kTRUE;
}

