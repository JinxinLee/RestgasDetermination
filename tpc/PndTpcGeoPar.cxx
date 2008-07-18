using namespace std;
#include "PndTpcGeoPar.h"
#include "CbmParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndTpcGeoPar)

PndTpcGeoPar::PndTpcGeoPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) 
{
  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndTpcGeoPar::~PndTpcGeoPar(void) 
{
}

void PndTpcGeoPar::clear(void)
{
  if(fGeoSensNodes) 
    delete fGeoSensNodes;
  if(fGeoPassNodes) 
    delete fGeoPassNodes;
}

void PndTpcGeoPar::putParams(CbmParamList* l) 
{
  if (!l) 
    return;
  
  l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
  l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndTpcGeoPar::getParams(CbmParamList* l) 
{
  if (!l) 
    return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) 
    return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) 
    return kFALSE;

  return kTRUE;
}
