//
//

using namespace std;
#include "PndGeoFtsPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoFtsPar)

PndGeoFtsPar::PndGeoFtsPar(const char* name,const char* title,const char* context)
    : FairParGenericSet(name,title,context) 
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
    fGeoType = -1;
}

PndGeoFtsPar::~PndGeoFtsPar(void) 
{
}

void PndGeoFtsPar::clear(void) 
{
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoFtsPar::putParams(FairParamList* l) 
{
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoFtsPar::getParams(FairParamList* l) 
{
  if (!l) return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
