using namespace std;
#include "CbmGeoPlanePar.h"
#include "FairParamList.h"
#include "iostream.h"
#include "iomanip.h"

ClassImp(CbmGeoPlanePar)

CbmGeoPlanePar::CbmGeoPlanePar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

CbmGeoPlanePar::~CbmGeoPlanePar(void) {
}

void CbmGeoPlanePar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void CbmGeoPlanePar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t CbmGeoPlanePar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
