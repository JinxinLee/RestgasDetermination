/*
 * PndLumiGeoPar.cxx
 *
 *  Created on: Feb 21, 2009
 *      Author: tsito
 */
using namespace std;
#include "PndLumiGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

PndLumiGeoPar::PndLumiGeoPar(const char* name,const char* title,const char* context)
    : FairParGenericSet(name,title,context)
{
    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

PndLumiGeoPar::~PndLumiGeoPar(void)
{
}

void PndLumiGeoPar::clear(void)
{
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndLumiGeoPar::putParams(FairParamList* l)
{
  if (!l) return;
 //  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndLumiGeoPar::getParams(FairParamList* l)
{
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
ClassImp(PndLumiGeoPar)
