/////////////////////////////////////////////////////////////
//
// PndGeoFTofPar
//
//
/////////////////////////////////////////////////////////////

#include "PndGeoFtofPar.h"
#include "FairParamList.h"
#include "iostream"
#include "iomanip"

ClassImp(PndGeoFtofPar)

PndGeoFtofPar::PndGeoFtofPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();

}

PndGeoFtofPar::~PndGeoFtofPar(void) {
}

// probably the next funtions can be deleted

void PndGeoFtofPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoFtofPar::putParams(FairParamList* l) {
  if (!l) return;
  // l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoFtofPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
