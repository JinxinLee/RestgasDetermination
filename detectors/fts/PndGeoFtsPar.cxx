//
//

#include "PndGeoFtsPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoFtsPar)

PndGeoFtsPar::PndGeoFtsPar(const char* name,const char* title,const char* context)
    : FairParGenericSet(name,title,context), 
     fGeoSensNodes(new TObjArray()),
     fGeoPassNodes(new TObjArray()),
     fGeoType(-1),
     fTubeInRad(0), 
     fTubeOutRad(0)
{
}

PndGeoFtsPar::~PndGeoFtsPar(void) 
{
}

void PndGeoFtsPar::clear(void) 
{
     if(fGeoSensNodes) {
         fGeoSensNodes->Delete();
        delete fGeoSensNodes;
      }
     if(fGeoPassNodes) {
        fGeoPassNodes->Delete();
        delete fGeoPassNodes;
     }
}

void PndGeoFtsPar::putParams(FairParamList* l) 
{
  if (!l) return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
  l->add("Tube Innen Radius", fTubeInRad);
  l->add("Tube Outer Radius", fTubeOutRad);
  l->add("Geometry Type", fGeoType);
}

Bool_t PndGeoFtsPar::getParams(FairParamList* l) 
{
  if (!l) return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;
  if (!l->fill("Tube Innen Radius", &fTubeInRad) )  return kFALSE;
  if (!l->fill("Tube Outer Radius", &fTubeOutRad)) return kFALSE;
  if (!l->fill("Geometry Type", &fGeoType)) return kFALSE;

  return kTRUE;
}
