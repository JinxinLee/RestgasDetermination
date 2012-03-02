//*-- AUTHOR  :
//*-- Created : 20/05/2009

using namespace std;
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndEmcGeoPar)

PndEmcGeoPar::PndEmcGeoPar(const char* name,const char* title,const char* context)
: FairParGenericSet(name,title,context), fMapperVersion(0), fGeometryVersion(0)
{
}

PndEmcGeoPar::~PndEmcGeoPar(void)
{
}

void PndEmcGeoPar::clear(void)
{
}

void PndEmcGeoPar::putParams(FairParamList* l)
{
	if (!l) return;
	l->add("PndEmcMapperVersion", fMapperVersion);
	l->add("PndEmcGeometryVersion", fGeometryVersion);
}

Bool_t PndEmcGeoPar::getParams(FairParamList* l)
{
	if (!l) return kFALSE;
	if (!l->fill("PndEmcMapperVersion", &fMapperVersion)) return kFALSE;  //AB
	if (!l->fill("PndEmcGeometryVersion", &fGeometryVersion)) return kFALSE;

  return kTRUE;
}

void PndEmcGeoPar::InitEmcMapper()
{
	PndEmcMapper::Init(fMapperVersion);
}
