// --------------------------------------------------------------------------
// -----                   PndGeoDrcPar source file                     -----
// -----                Created 14/08/06  by A Cecchi                   -----
// --------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoDrcPar.h"
#include "FairParamList.h"

ClassImp(PndGeoDrcPar)

PndGeoDrcPar::PndGeoDrcPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
}

PndGeoDrcPar::~PndGeoDrcPar(void) {
}

void PndGeoDrcPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoDrcPar::putParams(FairParamList* l) {
  if (!l) return;
 //   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoDrcPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
