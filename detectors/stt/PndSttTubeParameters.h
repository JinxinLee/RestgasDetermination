#ifndef PNDSTTTUBEPARAMETERS_H
#define PNDSTTTUBEPARAMETERS_H

#include "TObject.h" 

class PndSttTubeParameters : public TObject
{

 public:

  /** Default constructor **/
  PndSttTubeParameters();
  PndSttTubeParameters(PndSttTubeParameters &parms);

  ~PndSttTubeParameters();
  PndSttTubeParameters(Int_t tubeID, Double_t hl);

  Int_t GetTubeID();
  Double_t GetHalfLength();
 private:
  Int_t fTubeID;
  Double_t fHalfLength;

 
  ClassDef(PndSttTubeParameters,1);
};


#endif
