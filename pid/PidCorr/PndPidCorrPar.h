#ifndef PNDPIDCORRPAR_H
#define PNDPIDCORRPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndPidCorrPar : public FairParGenericSet
{
 public:

  Float_t GetTofRadius()     { return fTofRadius; };
  Float_t GetTofCut()        { return fTofCut; };  
  Float_t GetEmc12Thr()      { return fEmc12Thr; };
  Float_t GetEmc12Radius()   { return fEmc12Radius; };
  Float_t GetEmc12Cut()      { return fEmc12Cut; };
  Float_t GetMdtRadius()     { return fMdtRadius; };
  Float_t GetMdtCut()        { return fMdtCut; }; 
  Float_t GetDrcRadius()     { return fDrcRadius; };
  Float_t GetDrcCut()        { return fDrcCut; }; 
  Float_t GetZLastPlane()    { return fZLastPlane; };
  Float_t GetFtofZ()         { return fFtofZ; };
  Float_t GetRichZ()         { return fRichZ; };  

  PndPidCorrPar (const char* name="PndPidCorrPar",
		 const char* title="Pid correlation parameter",
		 const char* context="TestDefaultContext");
  ~PndPidCorrPar(void){};
  void clear(void){};
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
	
 private :
   
  Float_t fTofRadius;      // radius TOF detector
  Float_t fTofCut;         // cut chi2 for TOF 
  Float_t fEmc12Thr;       // EMC12 cluster energy threshold (barrel)
  Float_t fEmc12Radius;    // radius EMC12 detector (barrel)
  Float_t fEmc12Cut;       // cut chi2 for EMC12
  Float_t fMdtRadius;      // radius MDT detector
  Float_t fMdtCut;         // cut chi2 for MDT  
  Float_t fDrcRadius;      // radius DRC detector
  Float_t fDrcCut;         // cut chi2 for DRC 
  Float_t fZLastPlane;     // position of last point for fwd correlation
  Float_t fFtofZ;          // Z of Ftof
  Float_t fRichZ;          // Z of RICH

  ClassDef(PndPidCorrPar, 2);
};

#endif
