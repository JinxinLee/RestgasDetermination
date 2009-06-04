//*-- AUTHOR  :
//*-- Created : 20/05/2009

using namespace std;
#include "PndEmcGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndEmcGeoPar)

PndEmcGeoPar::PndEmcGeoPar(const char* name,const char* title,const char* context)
    : FairParGenericSet(name,title,context)
{
   // fGeometryVersion = new TObjArray();
}

PndEmcGeoPar::~PndEmcGeoPar(void)
{
}

void PndEmcGeoPar::clear(void)
{
//  if(fGeometryVersion) delete fGeometryVersion;
}

void PndEmcGeoPar::putParams(FairParamList* l)
{
  if (!l) return;
  //l->addObject("PndEmcMapperVersion", fGeometryVersion);
   l->add("PndEmcMapperVersion", fMapperVersion);
}

Bool_t PndEmcGeoPar::getParams(FairParamList* l)
{
  if (!l) return kFALSE;
//  if (!l->fillObject("PndEmcMapperVersion", fGeometryVersion)) return kFALSE;
    if (!l->fill("PndEmcMapperVersion", &fMapperVersion)) return kFALSE;  //AB

  return kTRUE;
}
