#ifndef PNDGEOSTTPAR_H
#define PNDGEOSTTPAR_H

#include "FairParGenericSet.h"
#include "TH1F.h"
#include "TObjArray.h"

class PndGeoSttPar : public FairParGenericSet 
{
 public:
  TObjArray            *fTubeParams;
  Int_t                 fGeoType;
  Double_t              fTubeInRad, fTubeOutRad;
  
  PndGeoSttPar(const char* name="PndGeoSttPar",
	       const char* title="Stt Geometry Parameters",
	       const char* context="TestDefaultContext");
  ~PndGeoSttPar(void);
  void clear(void);
  void putParams(FairParamList*);
  Bool_t getParams(FairParamList*);
    TObjArray             *GetTubeParameters() { return fTubeParams; } 

  // additional function for geometry
  void                  SetGeometryType(Int_t geoType) { fGeoType = geoType; }
  void                  SetTubeInRad(Double_t inrad)   { fTubeInRad = inrad; }
  void                  SetTubeOutRad(Double_t outrad) { fTubeOutRad = outrad; }
  Int_t                 GetGeometryType() { return (Int_t)    fGeoType;};
  Double_t              GetTubeInRad()    { return (Double_t) fTubeInRad;};
  Double_t              GetTubeOutRad()   { return (Double_t) fTubeOutRad;};

  PndGeoSttPar(PndGeoSttPar& par);
  PndGeoSttPar& operator= (const  PndGeoSttPar&) {return *this;};
  
  ClassDef(PndGeoSttPar,1)
};

#endif /* !PNDGEOSTTPAR_H */
