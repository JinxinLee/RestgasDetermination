#ifndef CBMGEOPlane_H
#define CBMGEOPlane_H

#include "CbmGeoSet.h"

class  CbmGeoPlane : public CbmGeoSet {
public:
  char modName[10];  // name of module
  char eleName[10];  // substring for elements in module
  CbmGeoPlane();
  ~CbmGeoPlane() {}
  //inline Int_t getModNumInMod(const TString&);
  ClassDef(CbmGeoPlane,0) // Class for Plane
};

/* inline Int_t CbmGeoPlane::getModNumInMod(const TString& name) { */
/*   // returns the module index from module name */
/*   Int_t bla  = (Int_t)(name[6]-'0')-1; */
/*   if(bla == -1) bla = 9; */
/*   return bla; */
/* } */

#endif  /* !CBMGEOPlane_H */



