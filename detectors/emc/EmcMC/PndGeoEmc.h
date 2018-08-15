// -------------------------------------------------------------------------
// -----                     PndGeoEmc header file                     -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDGEOEMC_H
#define PNDGEOEMC_H

#include "FairGeoSet.h"

class  PndGeoEmc : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  PndGeoEmc();
  ~PndGeoEmc() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(PndGeoEmc,0) // Class for Emc
};

#endif  /* !PNDGEOEMC_H */



