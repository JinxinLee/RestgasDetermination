/////////////////////////////////////////////////////////////
// PndPidCorrPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndPidCorrPar.h"
#include <iostream>


ClassImp(PndPidCorrPar)

PndPidCorrPar::PndPidCorrPar(const char* name,const char* title,
			     const char* context)
    : FairParGenericSet(name,title,context) 
{
	clear();
}

void PndPidCorrPar::putParams(FairParamList* list)
{
	if(!list) return;

	list->add("TofRadius",     fTofRadius);
	list->add("TofCut",        fTofCut);	
	list->add("Emc12Thr",      fEmc12Thr);
	list->add("Emc12Radius",   fEmc12Radius);
	list->add("Emc12Cut",      fEmc12Cut);
	list->add("MdtRadius",     fMdtRadius);
	list->add("MdtCut",        fMdtCut);
	list->add("DrcRadius",     fDrcRadius);
	list->add("DrcCut",        fDrcCut);
	list->add("DskCut",        fDskCut);
        list->add("ZLastPlane",    fZLastPlane);
        list->add("FtofZ",         fFtofZ);
        list->add("RichZ",         fRichZ);
}

Bool_t PndPidCorrPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("TofRadius",     &fTofRadius))     return kFALSE;
	if (!list->fill("TofCut",        &fTofCut))        return kFALSE;
	if (!list->fill("Emc12Thr",      &fEmc12Thr))      return kFALSE;
	if (!list->fill("Emc12Radius",   &fEmc12Radius))   return kFALSE;
	if (!list->fill("Emc12Cut",      &fEmc12Cut))      return kFALSE;
	if (!list->fill("MdtRadius",     &fMdtRadius))     return kFALSE;
	if (!list->fill("MdtCut",        &fMdtCut))        return kFALSE;
	if (!list->fill("DrcRadius",     &fDrcRadius))     return kFALSE;
	if (!list->fill("DrcCut",        &fDrcCut))        return kFALSE;
	if (!list->fill("DskCut",        &fDskCut))        return kFALSE;
        if (!list->fill("ZLastPlane",    &fZLastPlane))    return kFALSE;
        if (!list->fill("FtofZ",         &fFtofZ))         return kFALSE;
        if (!list->fill("RichZ",         &fRichZ))         return kFALSE;

	return kTRUE;
}

