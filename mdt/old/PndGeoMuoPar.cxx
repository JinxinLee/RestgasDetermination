#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoMuoPar.h"
#include "FairParamList.h"

ClassImp(PndGeoMuoPar)

PndGeoMuoPar::PndGeoMuoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

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

void PndGeoMuoPar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoMuoPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
