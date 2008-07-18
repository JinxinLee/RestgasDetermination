// -------------------------------------------------------------------------
// -----                     PndGeoHyp header file                     -----
// -----               Created by A.Sanchez                            -----
// -------------------------------------------------------------------------

#ifndef PNDGEOHYP_H
#define PNDGEOHYP_H

#include "CbmGeoSet.h"

class  PndGeoHyp : public CbmGeoSet {
protected:
 /*  char modName[20];  // name of module */
/*   char eleName[20];  // substring for elements in module */
  char modName[4];  // name of module
  char eleName[4];  // substring for elements in module

  
public:
  PndGeoHyp();
  ~PndGeoHyp() {}
//  const char* getModuleName(Int_t);
//  const char* getEleName(Int_t);
  
  ClassDef(PndGeoHyp,0) // Class for Hyp
};

#endif  /* !PNDGEOHYP_H */



