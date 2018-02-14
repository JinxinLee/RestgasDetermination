// -------------------------------------------------------------------------
// -----                     PndGeoSciT header file                     -----
// -----               Created 30.03.09 by A. Sanchez                -----
// -------------------------------------------------------------------------

#ifndef PNDGEOSCIT_H
#define PNDGEOSCIT_H

#include "FairGeoSet.h"

class  PndGeoSciT : public FairGeoSet {
protected:
 /*  char modName[20];  // name of module */
/*   char eleName[20];  // substring for elements in module */
  char modName[4];  // name of module
  char eleName[4];  // substring for elements in module

  
public:
  PndGeoSciT();
  ~PndGeoSciT() {}
//  const char* getModuleName(Int_t);
//  const char* getEleName(Int_t);
  
  ClassDef(PndGeoSciT,0) // Class for SciT
};

#endif  /* !PNDGEOSCIT_H */



