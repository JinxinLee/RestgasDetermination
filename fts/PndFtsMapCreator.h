#ifndef PNDFTSMAPCREATOR_H
#define PNDFTSMAPCREATOR_H

#include <iostream>
#include <map>
#include "TObject.h"

class PndFtsTube;
class PndGeoFtsPar;

class PndFtsMapCreator : public TObject
{
 public:

  PndFtsMapCreator();
  PndFtsMapCreator(Int_t geoType);
  PndFtsMapCreator(PndGeoFtsPar *par);
 ~PndFtsMapCreator();

  // general functions
  void SetGeneralParameters();


  // general functions, to be specified depending on geo type
  Int_t GetTubeIDFromPath(TString path);
  //Int_t GetTubeIDFromName(TString name);
  PndFtsTube * GetTubeFromTubeID(Int_t tubeid);
  TClonesArray * FillTubeArray();
  Int_t GetChamberIDFromPath(TString path);
  Int_t GetLayerID(Int_t chamberid, Int_t tubeid, TString path);
  Int_t GetTubeIDTot(Int_t chamberid,Int_t layerid, Int_t tubeid, TString path);

 private:

 // geometry type 1 specific functions
  Int_t GetTubeIDFromPathGeoType1(TString path);
  Int_t GetTubeIDFromNameGeoType1(TString name);
  TString GetNameFromPathGeoType1(TString path);
  TString GetNameFromTubeIDGeoType1(Int_t tubeid, Bool_t isCopy);
  PndFtsTube * GetTubeFromTubeIDGeoType1(Int_t tubeid);
  PndFtsTube * GetTubeFromTubeIDToFillGeoType1(Int_t tubeid);
  TClonesArray* FillTubeArrayGeoType1();

  Int_t fGeoType;
  PndGeoFtsPar *fFtsParameters;
  Double_t fTubeInRad, fTubeOutRad;

  // fGeoType1
  std::map<int, int> copy_map;

  PndFtsMapCreator(const  PndFtsMapCreator& L);
  PndFtsMapCreator& operator= (const  PndFtsMapCreator&) {return *this;}
 
 protected:

  ClassDef(PndFtsMapCreator,1)

};

#endif
