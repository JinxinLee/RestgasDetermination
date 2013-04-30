#ifndef PNDSTTMAPCREATOR_H
#define PNDSTTMAPCREATOR_H

#include <iostream>
#include <map>
#include "TObject.h"

class PndSttTubeParameters;
class PndSttTube; 
class PndGeoSttPar;
class FairGeoNode;

class PndSttMapCreator : public TObject
{
 public:

  PndSttMapCreator(); 
  PndSttMapCreator(Int_t geoType);
  PndSttMapCreator(PndGeoSttPar *par); 
  ~PndSttMapCreator();
  
  void SetGeneralParameters();
  
  // TO BE IMPLEMENTED WITH GEO TYPE n
  PndSttTubeParameters *CreateTubeParameters(FairGeoNode *pnode);
  Int_t FillSttTubeParameters(PndGeoSttPar *par, TList* volList);
  
  TClonesArray* FillTubeArray();
  Int_t GetTubeIDFromName(TString name);
  PndSttTube * GetTubeFromParametersToFill(PndSttTubeParameters *parms);
  TString GetPathFromTubeID(Int_t tubeid, Bool_t isCopy);
  TString GetNameFromPath(TString path);
  Int_t GetTubeIDFromPath(TString path);
    PndSttTube * GetTubeFromTubeID(Int_t tubeid);

 private:
 
  Int_t fGeoType;
  PndGeoSttPar *fSttParameters;
  Double_t fTubeInRad, fTubeOutRad;

  // fGeoType1
  std::map<int, int> copy_map;

  PndSttMapCreator(const  PndSttMapCreator& L);
  PndSttMapCreator& operator= (const  PndSttMapCreator&) {return *this;};

  // GEO TYPE 1
  PndSttTubeParameters *CreateTubeParametersGeoType1(FairGeoNode *pnode);
  Int_t FillSttTubeParametersGeoType1(PndGeoSttPar *par, TList* volList);
  TClonesArray* FillTubeArrayGeoType1();
  TString GetNameFromPathGeoType1(TString path);
  Int_t GetTubeIDFromNameGeoType1(TString name);
  PndSttTube * GetTubeFromParametersToFillGeoType1(PndSttTubeParameters *parms);
  TString GetPathFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy);
  Int_t GetTubeIDFromPathGeoType1(TString path);
  PndSttTube * GetTubeFromTubeIDGeoType1(Int_t tubeid);

 protected:
  
  
  ClassDef(PndSttMapCreator,1) 
    
    };

#endif
