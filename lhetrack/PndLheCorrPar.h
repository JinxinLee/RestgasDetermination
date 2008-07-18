#ifndef PNDLHECORRPAR_H
#define PNDLHECORRPAR_H

#include "CbmParGenericSet.h"
#include "CbmParamList.h"

class PndLheCorrPar : public CbmParGenericSet
{
 public:

  Float_t GetTofRadius()     { return fTofRadius; };
  Float_t GetTofPhi0()       { return fTofPhi0; };
  Float_t GetTofZ0()         { return fTofZ0; };
  Float_t GetTofSigmaPhi()   { return fTofSigmaPhi; };
  Float_t GetTofSigmaZ()     { return fTofSigmaZ; };
  Float_t GetTofCut()        { return fTofCut; }; 
  Float_t GetEmc12Radius()   { return fEmc12Radius; };
  Float_t GetEmc12Phi0()     { return fEmc12Phi0; };
  Float_t GetEmc12Z0()       { return fEmc12Z0; };
  Float_t GetEmc12SigmaPhi() { return fEmc12SigmaPhi; };
  Float_t GetEmc12SigmaZ()   { return fEmc12SigmaZ; };
  Float_t GetEmc12Cut()      { return fEmc12Cut; };
  Float_t GetMdtRadius()     { return fMdtRadius; };
  Float_t GetMdtPhi0()       { return fMdtPhi0; };
  Float_t GetMdtZ0()         { return fMdtZ0; };
  Float_t GetMdtSigmaPhi()   { return fMdtSigmaPhi; };
  Float_t GetMdtSigmaZ()     { return fMdtSigmaZ; };
  Float_t GetMdtCut()        { return fMdtCut; }; 
  
  PndLheCorrPar (const char* name="PndLheCorrPar",
		 const char* title="Lhe correlation parameter",
		 const char* context="TestDefaultContext");
  ~PndLheCorrPar(void){};
  void clear(void){};
  void putParams(CbmParamList* list);
  Bool_t getParams(CbmParamList* list);
	
 private :
   
  Float_t fTofRadius;      // radius TOF detector
  Float_t fTofPhi0;        // centroid phi(TRACK-TOF)
  Float_t fTofZ0;          // centroid Z(TRACK-TOF)
  Float_t fTofSigmaPhi;    // sigma    phi(TRACK-TOF)
  Float_t fTofSigmaZ;      // sigma    Z(TRACK-TOF)
  Float_t fTofCut;         // cut chi2 for TOF 
  Float_t fEmc12Radius;    // radius EMC12 detector (barrel)
  Float_t fEmc12Phi0;      // centroid phi(TRACK-EMC12)
  Float_t fEmc12Z0;        // centroid Z(TRACK-EMC12)
  Float_t fEmc12SigmaPhi;  // sigma    phi(TRACK-EMC12)
  Float_t fEmc12SigmaZ;    // sigma    Z(TRACK-EMC12)
  Float_t fEmc12Cut;       // cut chi2 for EMC12
  Float_t fMdtRadius;      // radius MDT detector
  Float_t fMdtPhi0;        // centroid phi(TRACK-MDT)
  Float_t fMdtZ0;          // centroid Z(TRACK-MDT)
  Float_t fMdtSigmaPhi;    // sigma    phi(TRACK-MDT)
  Float_t fMdtSigmaZ;      // sigma    Z(TRACK-MDT)
  Float_t fMdtCut;         // cut chi2 for MDT 
 
  ClassDef(PndLheCorrPar,2);
};

#endif
