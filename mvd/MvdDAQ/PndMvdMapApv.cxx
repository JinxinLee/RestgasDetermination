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
  Info("Init","Begin. ------------------");
  ifstream mapfile(fFileName.Data(),ifstream::in);
  Int_t rw=-1, sw=-1;
  char* detname="";
  if (!mapfile.is_open()){
    // abort on wrong file
    Error("Init","Could not open file %s",fFileName.Data());
    return kFALSE; 
  }
  while(mapfile.good())
  {
    mapfile >> rw >> sw >> detname;
    std::pair<Int_t,const char*> apair(sw,detname);
    fApvNumberMap[rw] = apair;
  }
  mapfile.close();
  Info("Init","Finished. ---------------");
  return kTRUE;
}

// ----------------------------------------------------------
void PndMvdMapApv::DoMapping(Int_t realWorldId, Int_t &onSensorId, const char* detname)
{
  std::pair<Int_t,const char*> apair = fApvNumberMap[realWorldId];
  onSensorId = apair.first;
  detname = apair.second;
  return;
}




ClassImp(PndMvdMapApv);
