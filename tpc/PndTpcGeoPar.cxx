using namespace std;
#include "PndTpcGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndTpcGeoPar)

PndTpcGeoPar::PndTpcGeoPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) 
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

void PndTpcGeoPar::putParams(FairParamList* l) 
{
  if (!l) 
    return;
  
  //l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 // l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndTpcGeoPar::getParams(FairParamList* l) 
{
  if (!l) 
    return kFALSE;
 // if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))     return kFALSE;
 // if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))     return kFALSE;

  return kTRUE;
}
