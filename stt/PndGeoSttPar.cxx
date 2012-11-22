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

PndGeoSttPar::PndGeoSttPar(PndGeoSttPar& par) :
  fGeoSensNodes(par.GetGeoSensitiveNodes()),
  fGeoPassNodes(par.GetGeoPassiveNodes()),
  fGeoType(par.GetGeometryType()),
  fTubeInRad(par.GetTubeInRad()), 
  fTubeOutRad(par.GetTubeOutRad())
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
  l->add("Tube_Inner_Radius", fTubeInRad);
  l->add("Tube_Outer_Radius", fTubeOutRad);
  l->add("Geometry_Type", fGeoType);
}

Bool_t PndGeoSttPar::getParams(FairParamList* l) 
{
    if (!l) return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE; 
    if (!l->fill("Tube_Inner_Radius", &fTubeInRad) )  return kFALSE;
    if (!l->fill("Tube_Outer_Radius", &fTubeOutRad)) return kFALSE;
    if (!l->fill("Geometry_Type", &fGeoType)) return kFALSE;
    
    return kTRUE;
}
