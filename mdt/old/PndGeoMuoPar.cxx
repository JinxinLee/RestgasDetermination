#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoMuoPar.h"
#include "CbmParamList.h"

ClassImp(PndGeoMuoPar)

PndGeoMuoPar::PndGeoMuoPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();
	       
	       barrel_y = 1884.;
	       dplane   =   30.;
	       barrel_z = -350.;
	       dstrip   =   10.;
	       nbox     =   19;
	       dbox     =   82.44;
	       dtube    =   10.;
	       ntube    =   8;
}

PndGeoMuoPar::~PndGeoMuoPar(void) {
}

void PndGeoMuoPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoMuoPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoMuoPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
