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
    : FairParGenericSet(name,title,context),
      fTofRadius(-1.), fTofCut(-1.), fEmc12Thr(-1.), fEmc12Radius(-1.), fEmc12Cut(-1.), fEmcNeutralQCut(-1.),
      fMdtRadius(-1.), fMdtCut(-1.), fDrcRadius(-1.), fDrcCut(-1.), fDskCut(-1.), fZLastPlane(-1.), fFtofZ(-1.),
      fFTofCut(-1.), fRichZ(-1.), fFMdtCut(-1.) 
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
        list->add("EmcNeutralQCut",fEmcNeutralQCut);
	list->add("MdtRadius",     fMdtRadius);
	list->add("MdtCut",        fMdtCut);
	list->add("DrcRadius",     fDrcRadius);
	list->add("DrcCut",        fDrcCut);
	list->add("DskCut",        fDskCut);
        list->add("ZLastPlane",    fZLastPlane);
        list->add("FtofZ",         fFtofZ);
	list->add("FTofCut",       fFTofCut);
        list->add("RichZ",         fRichZ);
	list->add("FMdtCut",       fFMdtCut);
}

Bool_t PndPidCorrPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("TofRadius",     &fTofRadius))     return kFALSE;
	if (!list->fill("TofCut",        &fTofCut))        return kFALSE;
	if (!list->fill("Emc12Thr",      &fEmc12Thr))      return kFALSE;
	if (!list->fill("Emc12Radius",   &fEmc12Radius))   return kFALSE;
	if (!list->fill("Emc12Cut",      &fEmc12Cut))      return kFALSE;
        if (!list->fill("EmcNeutralQCut",&fEmcNeutralQCut))return kFALSE;
	if (!list->fill("MdtRadius",     &fMdtRadius))     return kFALSE;
	if (!list->fill("MdtCut",        &fMdtCut))        return kFALSE;
	if (!list->fill("DrcRadius",     &fDrcRadius))     return kFALSE;
	if (!list->fill("DrcCut",        &fDrcCut))        return kFALSE;
	if (!list->fill("DskCut",        &fDskCut))        return kFALSE;
        if (!list->fill("ZLastPlane",    &fZLastPlane))    return kFALSE;
        if (!list->fill("FtofZ",         &fFtofZ))         return kFALSE;
	if (!list->fill("FTofCut",       &fFTofCut))       return kFALSE;
        if (!list->fill("RichZ",         &fRichZ))         return kFALSE;
	if (!list->fill("FMdtCut",       &fFMdtCut))       return kFALSE;
	
	return kTRUE;
}

