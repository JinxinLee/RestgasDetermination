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
#include "CbmParamList.h"

ClassImp(PndDchGeoPar)

PndDchGeoPar::PndDchGeoPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndDchGeoPar::~PndDchGeoPar(void) {
}

void PndDchGeoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndDchGeoPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndDchGeoPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
