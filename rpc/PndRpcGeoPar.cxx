/////////////////////////////////////////////////////////////
// PndRpcGeoPar
//
// Class for geometry parameters of RpcTof
//
/////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

using namespace std;

#include "PndRpcGeoPar.h"
#include "CbmParamList.h"

ClassImp(PndRpcGeoPar)

PndRpcGeoPar::PndRpcGeoPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndRpcGeoPar::~PndRpcGeoPar(void) {
}

void PndRpcGeoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndRpcGeoPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndRpcGeoPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
