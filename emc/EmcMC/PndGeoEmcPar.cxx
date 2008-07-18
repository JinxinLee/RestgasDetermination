/////////////////////////////////////////////////////////////
//
// PndGeoEmcPar
//
// Class for geometry parameters of EMC
//
// Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndGeoEmcPar.h"
#include "CbmParamList.h"
#include "iomanip"

ClassImp(PndGeoEmcPar)

PndGeoEmcPar::PndGeoEmcPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndGeoEmcPar::~PndGeoEmcPar(void) {
}

// probably the next funtions can be deleted

void PndGeoEmcPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoEmcPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoEmcPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
