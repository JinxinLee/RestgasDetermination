/////////////////////////////////////////////////////////////
//
// PndGeoFSciTPar
//
//
/////////////////////////////////////////////////////////////

#include "PndGeoSciTPar.h"
#include "FairParamList.h"
#include "iostream"
#include "iomanip"

ClassImp(PndGeoSciTPar)

PndGeoSciTPar::PndGeoSciTPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();

}

PndGeoSciTPar::~PndGeoSciTPar(void) {
}

// probably the next funtions can be deleted

void PndGeoSciTPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoSciTPar::putParams(FairParamList* l) {
  if (!l) return;
  // l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoSciTPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
