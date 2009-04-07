// -------------------------------------------------------------------------
// -----                     PndGeoFtof header file                     -----
// -----               Created 30.03.09 by A. Sanchez                -----
// -------------------------------------------------------------------------

#ifndef PNDGEOFTOF_H
#define PNDGEOFTOF_H

#include "FairGeoSet.h"

class  PndGeoFtof : public FairGeoSet {
protected:
 /*  char modName[20];  // name of module */
/*   char eleName[20];  // substring for elements in module */
  char modName[4];  // name of module
  char eleName[4];  // substring for elements in module

  
public:
  PndGeoFtof();
  ~PndGeoFtof() {}
//  const char* getModuleName(Int_t);
//  const char* getEleName(Int_t);
  
  ClassDef(PndGeoFtof,0) // Class for Ftof
};

#endif  /* !PNDGEOFTOF_H */



