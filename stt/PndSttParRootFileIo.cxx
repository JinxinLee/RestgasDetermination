//*-- AUTHOR : Denis Bertini
//*-- Modified : 06/10/2005 by Denis Bertini

/////////////////////////////////////////////////////////////
//  PndSttParRootFileIo
//
//  Class for Start parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////
#include "PndSttParRootFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"
#include "PndSttDigiPar.h"
#include <iostream> 
#include <iomanip>

ClassImp(PndSttParRootFileIo)

PndSttParRootFileIo::PndSttParRootFileIo(FairParRootFile* f) : FairDetParRootFileIo(f) 
{
  fName="PndSttParIo";
}


PndSttParRootFileIo::~PndSttParRootFileIo() 
{
}

Bool_t PndSttParRootFileIo::init(FairParSet* pPar,Int_t* set) 
{
  const Text_t* name=pPar->GetName();
  if (pFile) 
  {
      if (!strcmp(name,"SttDigiPar")) return read((PndSttDigiPar*)pPar,set);
  }
  std::cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<std::endl;
  return kFALSE;
}


Bool_t PndSttParRootFileIo::read(PndSttDigiPar* pPar,Int_t* set) 
{
  // No params are implemented
  std::cout << "-I- PndSttParRootFileIo : reading " << std::endl;
  return kTRUE;
}

