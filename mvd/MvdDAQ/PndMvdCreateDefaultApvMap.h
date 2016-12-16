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
#include "PndSdsStripDigiPar.h"
#include "PndGeoHandling.h"
#include <utility>
#include <string>
#include <vector>
#include <map>

typedef std::pair<TString,std::pair<Int_t,Int_t> > Map_Entry;

class PndMvdCreateDefaultApvMap : public FairTask {
  
public:
  
  PndMvdCreateDefaultApvMap();
  
  ~PndMvdCreateDefaultApvMap();
  PndMvdCreateDefaultApvMap(const PndMvdCreateDefaultApvMap& o) :
    fSortedMapping(o.fSortedMapping),
    fCurrentDigiPar(o.fCurrentDigiPar),
    fDigiParameterList(o.fDigiParameterList),
    fFeCount(o.fFeCount),
    fFakeCount(o.fFakeCount)
  {};
  PndMvdCreateDefaultApvMap& operator=(const PndMvdCreateDefaultApvMap& o)
  {
    fSortedMapping=o.fSortedMapping;
    fCurrentDigiPar=o.fCurrentDigiPar;
    fDigiParameterList=o.fDigiParameterList;
    fFeCount=o.fFeCount;
    fFakeCount=o.fFakeCount;
    return *this;  
  };
  
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
  void WriteExpandNode();
  std::map<Double_t, std::vector<Map_Entry> > fSortedMapping;  //!
  //TString FindNodePath(TGeoNode* node);
  //void DiveDownToNode(TGeoNode* node);
  
  // helper members
  PndSdsStripDigiPar* fCurrentDigiPar; //!
  TList* fDigiParameterList; //!
  Int_t fFeCount; // Running number of rw frontends
  Int_t fFakeCount; // Running number of fake frontends
  ClassDef(PndMvdCreateDefaultApvMap,1);
  
};

#endif
