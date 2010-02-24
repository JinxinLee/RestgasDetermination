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
#include "FairTask.h"
#include "PndMvdStripDigiPar.h"
#include "PndMvdGeoHandling.h"

class PndMvdCreateDefaultApvMap : public FairTask {
  
public:
  
  PndMvdCreateDefaultApvMap();
  
  ~PndMvdCreateDefaultApvMap();
  
  // setup  
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit(){return kSUCCESS;};
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt){return;};
  
  /** Our main function here **/
  Bool_t CreateFile(TString outFile);
  
private:

  // helper
  Bool_t SelectSensorParams(TString detname);
  void WriteExpandNode(std::ofstream& outfile);
  //TString FindNodePath(TGeoNode* node);
  //void DiveDownToNode(TGeoNode* node);
  
  // helper members
  PndMvdStripDigiPar* fCurrentDigiPar; //!
  PndMvdGeoHandling* fGeoH; //!
  TList* fDigiParameterList; //!
  Int_t fFeCount;
  ClassDef(PndMvdCreateDefaultApvMap,1);
  
};

#endif
