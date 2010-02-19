//-------------------------------------------------------//
//     Class PndMvdCreateDefaultApvMap                   //
//                                                       //
// Creates a default Frontend number mapping file        //
//                                                       //
// Created 16.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//
#ifndef PNDMVDCREATEDEFAULTAPVMAP_H
#define PNDMVDCREATEDEFAULTAPVMAP_H


#include "TObject.h"
#include "TList.h"
#include "TGeoManager.h"
#include "TString.h"
#include "PndMvdStripDigiPar.h"

class PndMvdCreateDefaultApvMap : public TObject {
  
public:
  
  PndMvdCreateDefaultApvMap();
  
  ~PndMvdCreateDefaultApvMap();
  
  // setup
  Bool_t Init();
  
  // main function
  Bool_t CreateFile(TString outFile);
  
private:

  // helper
  Bool_t SelectSensorParams(TString detname);

  // helper members
  PndMvdStripDigiPar* fCurrentDigiPar;
  TGeoManager* fGeoMan; //!
  TList* fDigiParameterList; //!
  
  ClassDef(PndMvdCreateDefaultApvMap,0);
  
};

#endif
