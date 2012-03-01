//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

using namespace std;
#include "PndGeoSttPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoSttPar)

PndGeoSttPar::PndGeoSttPar(const char* name,const char* title,const char* context)
    : FairParGenericSet(name,title,context), 
  fGeoSensNodes(new TObjArray()),
  fGeoPassNodes(new TObjArray()),
  fGeoType(-1),
  fTubeInRad(0), 
  fTubeOutRad(0)
{
}

PndGeoSttPar::~PndGeoSttPar(void) 
{
}

void PndGeoSttPar::clear(void) 
{
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoSttPar::putParams(FairParamList* l) 
{
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes); 
  l->add("Tube Inner Radius", fTubeInRad);
  l->add("Tube Outer Radius", fTubeOutRad);
  l->add("Geometry Type", fGeoType);
}

Bool_t PndGeoSttPar::getParams(FairParamList* l) 
{
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE; 
    if (!l->fill("Tube Inner Radius", &fTubeInRad) )  return kFALSE;
    if (!l->fill("Tube Outer Radius", &fTubeOutRad)) return kFALSE;
    if (!l->fill("Geometry Type", &fGeoType)) return kFALSE;
    
    return kTRUE;
}
