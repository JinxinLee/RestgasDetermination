// -------------------------------------------------------------------------
// -----                     PndGeoDrc header file                     -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRC_H
#define PNDGEODRC_H

#include "CbmGeoSet.h"

class  PndGeoDrc : public CbmGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  PndGeoDrc();
  ~PndGeoDrc() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
 
  ClassDef(PndGeoDrc,0) // Class for Drc
};


#endif  /* !PNDGEODRC_H */



