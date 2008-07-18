// -------------------------------------------------------------------------
// -----                     PndGeoTof header file                     -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEOTOF_H
#define PNDGEOTOF_H

#include "CbmGeoSet.h"

class  PndGeoTof : public CbmGeoSet {
protected:
 /*  char modName[20];  // name of module */
/*   char eleName[20];  // substring for elements in module */
  char modName[4];  // name of module
  char eleName[4];  // substring for elements in module

  
public:
  PndGeoTof();
  ~PndGeoTof() {}
//  const char* getModuleName(Int_t);
//  const char* getEleName(Int_t);
  
  ClassDef(PndGeoTof,0) // Class for Tof
};

#endif  /* !PNDGEOTOF_H */



