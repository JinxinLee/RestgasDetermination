using namespace std;
#include "PndMvdGeoPar.h"
#include "CbmParamList.h"
#include <iostream>
#include <iomanip>
#include "TObjArray.h"

ClassImp(PndMvdGeoPar)

PndMvdGeoPar::PndMvdGeoPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) 
{
  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndMvdGeoPar::~PndMvdGeoPar(void) 
{
}

void PndMvdGeoPar::clear(void)
{
  if(fGeoSensNodes) 
    delete fGeoSensNodes;
  if(fGeoPassNodes) 
    delete fGeoPassNodes;
}

void PndMvdGeoPar::putParams(CbmParamList* l) 
{
  if (!l) 
    return;
  
  l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
  l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndMvdGeoPar::getParams(CbmParamList* l) 
{
  if (!l) 
    return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) 
    return kFALSE;
  if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) 
    return kFALSE;

  return kTRUE;
}
