
#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoMdtPar.h"
#include "CbmParamList.h"

ClassImp(PndGeoMdtPar)

PndGeoMdtPar::PndGeoMdtPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

    fSensNodes = new TObjArray();
    fPassNodes = new TObjArray();
}

PndGeoMdtPar::~PndGeoMdtPar(void) {
}

void PndGeoMdtPar::clear(void) {
    if(fSensNodes) delete fSensNodes;
    if(fPassNodes) delete fPassNodes;
}

void PndGeoMdtPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fPassNodes);
}

Bool_t PndGeoMdtPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fPassNodes)) return kFALSE;

  return kTRUE;
}
