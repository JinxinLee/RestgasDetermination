#ifndef PNDSTTMAPCREATOR_H
#define PNDSTTMAPCREATOR_H

#include <iostream>
#include "TObject.h"

class PndSttTube; 
class PndGeoSttPar;

class PndSttMapCreator : public TObject
{
 public:

  PndSttMapCreator(); 
  PndSttMapCreator(Int_t geoType);
  PndSttMapCreator(PndGeoSttPar *par); 
  ~PndSttMapCreator();

  // general functions
  void SetGeneralParameters();

  // general functions, to be specified depending on geo type
  Int_t GetTubeIDFromPath(TString path);
  Int_t GetTubeIDFromName(TString name);
  PndSttTube * GetTubeFromTubeID(Int_t tubeid);
  TClonesArray * FillTubeArray();
  TString GetNameFromTubeID(Int_t tubeid);

 private:

  // geometry type 1 specific functions
  Int_t GetTubeIDFromPathGeoType1(TString path);
  Int_t GetTubeIDFromNameGeoType1(TString name);
  TString GetNameFromPathGeoType1(TString path);
  TString GetNameFromTubeIDGeoType1(Int_t tubeid);
  PndSttTube * GetTubeFromTubeIDGeoType1(Int_t tubeid);
  TClonesArray* FillTubeArrayGeoType1();
 
  Int_t fGeoType;
  PndGeoSttPar *fSttParameters;
  Double_t fTubeInRad, fTubeOutRad;

 protected:

  ClassDef(PndSttMapCreator,1) 

};

#endif
