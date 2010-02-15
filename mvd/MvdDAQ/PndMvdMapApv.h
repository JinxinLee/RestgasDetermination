//-------------------------------------------------------//
//     Class PndMvdMapApv                                //
//                                                       //
// Maps Software treated APV numbers to real world.      //
//                                                       //
// Created 15.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//
#ifndef PNDMVDMAPAPV_H
#define PNDMVDMAPAPV_H

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include "TString.h"
#include "TObject.h" // contains message printing methods

class PndMvdMapApv : public TObject {
public:
  // Constructor/destructor
  PndMvdMapApv() {};
  PndMvdMapApv(TString name) {SetMappingFile(name); Init();};
  ~PndMvdMapApv() {};
  
  // Printing
  void Print();
  
  // Initialization & setup
  void SetMappingFile(TString name) {fFileName = name;};
  Bool_t Init();
  
  // Retrieveing information
  void DoMapping(Int_t realWorldId, Int_t &onSensorId, const char* detname);
  
  // Members
private:
  // the lookup table: ( RW Id | SW Id | Detname )
  std::map< Int_t , std::pair<Int_t,const char*> > fApvNumberMap; // Maps the Apv Numbers
  TString fFileName; // file containing the association table

  ClassDef(PndMvdMapApv,1);
};


#endif
