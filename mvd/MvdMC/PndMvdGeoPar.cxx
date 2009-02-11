using namespace std;
#include "PndMvdGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>
#include "TObjArray.h"

ClassImp(PndMvdGeoPar)

PndMvdGeoPar::PndMvdGeoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) 
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

void PndMvdGeoPar::putParams(FairParamList* l) 
{
  if (!l) 
    return;
  
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndMvdGeoPar::getParams(FairParamList* l) 
{
  if (!l) 
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) 
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) 
    return kFALSE;

  return kTRUE;
}
