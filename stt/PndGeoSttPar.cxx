//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

using namespace std;
#include "PndGeoSttPar.h"
#include "CbmParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoSttPar)

PndGeoSttPar::PndGeoSttPar(const char* name,const char* title,const char* context)
    : CbmParGenericSet(name,title,context) 
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

PndGeoSttPar::~PndGeoSttPar(void) 
{
}

void PndGeoSttPar::clear(void) 
{
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoSttPar::putParams(CbmParamList* l) 
{
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoSttPar::getParams(CbmParamList* l) 
{
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
