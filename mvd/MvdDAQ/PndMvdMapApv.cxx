//-------------------------------------------------------//
//     Class PndMvdMapApv                                //
//                                                       //
// Maps Software treated APV numbers to real world.      //
//                                                       //
// Created 15.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//

#include "PndMvdMapApv.h"
#include <fstream>

// ----------------------------------------------------------
void PndMvdMapApv::Print()
{
  Info("Print","Filename = %s",fFileName.Data());
  return;
}

// ----------------------------------------------------------
Bool_t PndMvdMapApv::Init()
{
  ifstream mapfile(fFileName.Data(),ifstream::in);
  Int_t rw=-1, sw=-1;
  TString detPathName;
  if (!mapfile.is_open()){
    // abort on wrong file
    Error("Init","Could not open file %s",fFileName.Data());
    return kFALSE; 
  }
  while(mapfile.good())
  {
    mapfile >> rw >> sw >> detPathName;
    if(mapfile.eof()) break;
    Info("Init","Read line: %i %i %s",rw,sw,detPathName.Data());
    std::pair<Int_t,TString> apair(sw,detPathName);
    fApvNumberMap[rw] = apair;
  }
  mapfile.close();
  return kTRUE;
}

// ----------------------------------------------------------
void PndMvdMapApv::DoMapping(Int_t realWorldId, Int_t &onSensorId, TString &detpath)
{
  std::pair<Int_t,TString> apair = fApvNumberMap[realWorldId];
  onSensorId = apair.first;
  detpath = apair.second;
  //std::cout << detpath.Data() << std::endl;
  return;
}




ClassImp(PndMvdMapApv);
