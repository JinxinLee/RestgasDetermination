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
#include "CbmParamList.h"
#include "iostream.h"
#include "iomanip.h"

ClassImp(PndGeoHypGePar)

PndGeoHypGePar::PndGeoHypGePar(const char* name,const char* title,const char* context)
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

PndGeoHypGePar::~PndGeoHypGePar(void) {
}

// probably the next funtions can be deleted

void PndGeoHypGePar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoHypGePar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoHypGePar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
