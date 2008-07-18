/////////////////////////////////////////////////////////////
//
// PndGeoHypPar
//
// Class for geometry parameters of HYP
//
// Created by A.sAnchez
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndGeoHypPar.h"
#include "CbmParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndGeoHypPar)

PndGeoHypPar::PndGeoHypPar(const char* name,const char* title,const char* context)
           : CbmParGenericSet(name,title,context) {

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

void PndGeoHypPar::putParams(CbmParamList* l) {
  if (!l) return;
   l->addBinary("CbmGeoNodes Sensitive List", fGeoSensNodes);
   l->addBinary("CbmGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoHypPar::getParams(CbmParamList* l) {
    if (!l) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
    if (!l->fillBinary("CbmGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
