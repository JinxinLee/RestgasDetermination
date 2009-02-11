/////////////////////////////////////////////////////////////
//
// PndGeoHypGePar
//
// Class for geometry parameters of HYPGe
//

//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndGeoHypGePar.h"
#include "FairParamList.h"
#include "iostream.h"
#include "iomanip.h"

ClassImp(PndGeoHypGePar)

PndGeoHypGePar::PndGeoHypGePar(const char* name,const char* title,const char* context)
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

PndGeoHypGePar::~PndGeoHypGePar(void) {
}

// probably the next funtions can be deleted

void PndGeoHypGePar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoHypGePar::putParams(FairParamList* l) {
  if (!l) return;
   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
   l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoHypGePar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
