/////////////////////////////////////////////////////////////
// PndDchGeoPar
//
// Class for geometry parameters of DCH
//
/////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

using namespace std;

#include "PndDchGeoPar.h"
#include "FairParamList.h"

ClassImp(PndDchGeoPar)

PndDchGeoPar::PndDchGeoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndDchGeoPar::~PndDchGeoPar(void) {
}

void PndDchGeoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndDchGeoPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndDchGeoPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
