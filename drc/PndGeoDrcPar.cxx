// --------------------------------------------------------------------------
// -----                   PndGeoDrcPar source file                     -----
// -----                Created 14/08/06  by A Cecchi                   -----
// --------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoDrcPar.h"
#include "CbmParamList.h"

ClassImp(PndGeoDrcPar)

PndGeoDrcPar::PndGeoDrcPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndGeoDrcPar::~PndGeoDrcPar(void) {
}

void PndGeoDrcPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoDrcPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoDrcPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
