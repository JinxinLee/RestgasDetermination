// -------------------------------------------------------------------------
// -----                     CbmGeoHypGe header file                     -----

// -------------------------------------------------------------------------

#ifndef PNDGEOHYPGE_H
#define PNDGEOHYPGE_H

#include "FairGeoSet.h"

class  PndGeoHypGe : public FairGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
  
public:
  PndGeoHypGe();
  ~PndGeoHypGe() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  
  ClassDef(PndGeoHypGe,0) // Class for HypGe
};

#endif  /* !PNDGEOHYPGE_H */



