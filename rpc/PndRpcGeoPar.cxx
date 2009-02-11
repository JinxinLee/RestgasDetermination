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
#include "FairParamList.h"

ClassImp(PndRpcGeoPar)

PndRpcGeoPar::PndRpcGeoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndRpcGeoPar::~PndRpcGeoPar(void) {
}

void PndRpcGeoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndRpcGeoPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndRpcGeoPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
