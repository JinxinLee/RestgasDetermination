/////////////////////////////////////////////////////////////
//
// PndGeoHypPar
//
// Class for geometry parameters of HYP
//
// Created by A.sAnchez
//
/////////////////////////////////////////////////////////////

#include "PndGeoHypPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoHypPar)

PndGeoHypPar::PndGeoHypPar(const char* name,const char* title,const char* context)
           : FairParGenericSet(name,title,context) {

               fGeoSensNodes = new TObjArray();
               fGeoPassNodes = new TObjArray();

}

PndGeoHypPar::~PndGeoHypPar(void) {
}

// probably the next funtions can be deleted

void PndGeoHypPar::clear(void) {
    if(fGeoSensNodes) delete fGeoSensNodes;
    if(fGeoPassNodes) delete fGeoPassNodes;
}

void PndGeoHypPar::putParams(FairParamList* l) {
  if (!l) return;
 //  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
 //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoHypPar::getParams(FairParamList* l) {
    if (!l) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
 //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
