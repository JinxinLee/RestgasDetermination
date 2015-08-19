// -------------------------------------------------------------------------
// -----                     PndGeoEmcApd header file                     -----
// -----               Created 07/04/08  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDGEOEMCAPD_H
#define PNDGEOEMCAPD_H

#include "FairGeoSet.h"

class  PndGeoEmcApd : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  PndGeoEmcApd();
  ~PndGeoEmcApd() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(PndGeoEmcApd,0) // Class for EmcApd
};

#endif  /* !PNDGEOEMCAPD_H */



