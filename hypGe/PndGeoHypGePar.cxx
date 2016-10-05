/////////////////////////////////////////////////////////////
//
// PndGeoHypGePar
//
// Class for geometry parameters of HYPGe
//

//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndGeoHypGePar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoHypGePar)

PndGeoHypGePar::PndGeoHypGePar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
	       
	 
}

PndGeoHypGePar::~PndGeoHypGePar(void) {
}

// probably the next funtions can be deleted

void PndGeoHypGePar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoHypGePar::putParams(FairParamList* l) {
  if (!l) return;
  // l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  // l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoHypGePar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
   // if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
   // if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
