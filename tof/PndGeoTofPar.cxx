/////////////////////////////////////////////////////////////
//
// PndGeoTofPar
//
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndGeoTofPar.h"
#include "FairParamList.h"
#include "iostream"
#include "iomanip"

ClassImp(PndGeoTofPar)

PndGeoTofPar::PndGeoTofPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();

}

PndGeoTofPar::~PndGeoTofPar(void) {
}

// probably the next funtions can be deleted

void PndGeoTofPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoTofPar::putParams(FairParamList* l) {
  if (!l) return;
  // l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoTofPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
