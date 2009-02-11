#ifndef PNDMDTRECOPAR_H
#define PNDMDTRECOPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndMdtRecoPar : public FairParGenericSet
{
 public :
  //Double_t GetEnergyThresholdBarrel() {return fEnergyThresholdBarrel;};
  
  Float_t GetLayerPos(Int_t mod, Int_t lay) const { return fLayerPos[mod][lay]; };
  Float_t GetAngleCut(Int_t mod)            const { return fAngleCut[mod]; };

  PndMdtRecoPar (const char* name="PndMdtRecoPar",
		 const char* title="Mdt reconstruction parameter",
		 const char* context="TestDefaultContext");
  
  ~PndMdtRecoPar(void){};
  
  void Clear(void);
  
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
  
 private :
		//Double_t fEnergyThresholdBarrel;   // Single crystal energy threshold for barrel EMC
  
  Float_t fLayerPos[2][10];        // Position of the MDT layers [cm]
  Float_t fAngleCut[2];            // Cut selection for correlation [degrees]
  
  ClassDef(PndMdtRecoPar,1);
};

#endif
