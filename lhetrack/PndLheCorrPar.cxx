/////////////////////////////////////////////////////////////
// PndLheCorrPar
//
// Container class for Digitisation parameters
// class is inherited from CbmParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndLheCorrPar.h"
#include <iostream>

ClassImp(PndLheCorrPar)

PndLheCorrPar::PndLheCorrPar(const char* name,const char* title,
			     const char* context)
    : CbmParGenericSet(name,title,context) 
{
	clear();
}

void PndLheCorrPar::putParams(CbmParamList* list)
{
	if(!list) return;

	list->add("TofRadius",     fTofRadius);
	list->add("TofPhi0",       fTofPhi0);
	list->add("TofZ0",         fTofZ0);
	list->add("TofSigmaPhi",   fTofSigmaPhi);
	list->add("TofSigmaZ",     fTofSigmaZ);
	list->add("TofCut",        fTofCut);	
	list->add("Emc12Radius",   fEmc12Radius);
	list->add("Emc12Phi0",     fEmc12Phi0);
	list->add("Emc12Z0",       fEmc12Z0);
	list->add("Emc12SigmaPhi", fEmc12SigmaPhi);
	list->add("Emc12SigmaZ",   fEmc12SigmaZ);
	list->add("Emc12Cut",      fEmc12Cut);
	list->add("MdtRadius",     fMdtRadius);
	list->add("MdtPhi0",       fMdtPhi0);
	list->add("MdtZ0",         fMdtZ0);
	list->add("MdtSigmaPhi",   fMdtSigmaPhi);
	list->add("MdtSigmaZ",     fMdtSigmaZ);
	list->add("MdtCut",        fMdtCut);
}

Bool_t PndLheCorrPar::getParams(CbmParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("TofRadius",     &fTofRadius))     return kFALSE;
	if (!list->fill("TofPhi0",       &fTofPhi0))       return kFALSE;
	if (!list->fill("TofZ0",         &fTofZ0))         return kFALSE;
	if (!list->fill("TofSigmaPhi",   &fTofSigmaPhi))   return kFALSE;
	if (!list->fill("TofSigmaZ",     &fTofSigmaZ))     return kFALSE;
	if (!list->fill("TofCut",        &fTofCut))        return kFALSE;
	if (!list->fill("Emc12Radius",   &fEmc12Radius))   return kFALSE;
	if (!list->fill("Emc12Phi0",     &fEmc12Phi0))     return kFALSE;
	if (!list->fill("Emc12Z0",       &fEmc12Z0))       return kFALSE;
	if (!list->fill("Emc12SigmaPhi", &fEmc12SigmaPhi)) return kFALSE;
	if (!list->fill("Emc12SigmaZ",   &fEmc12SigmaZ))   return kFALSE;
	if (!list->fill("Emc12Cut",      &fEmc12Cut))      return kFALSE;
	if (!list->fill("MdtRadius",     &fMdtRadius))     return kFALSE;
	if (!list->fill("MdtPhi0",       &fMdtPhi0))       return kFALSE;
	if (!list->fill("MdtZ0",         &fMdtZ0))         return kFALSE;
	if (!list->fill("MdtSigmaPhi",   &fMdtSigmaPhi))   return kFALSE;
	if (!list->fill("MdtSigmaZ",     &fMdtSigmaZ))     return kFALSE;
	if (!list->fill("MdtCut",        &fMdtCut))        return kFALSE;
	
	return kTRUE;
}

