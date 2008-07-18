#ifndef PNDGEOMUO_H
#define PNDGEOMUO_H

#include "CbmGeoSet.h"

class  PndGeoMuo : public CbmGeoSet {
protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
public:
  PndGeoMuo();
  ~PndGeoMuo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  //inline Int_t getModNumInMod(const TString&);
  ClassDef(PndGeoMuo,0) // Class for Muo
};

/* inline Int_t PndGeoMuo::getModNumInMod(const TString& name) { */
/*   // returns the module index from module name */
/*   Int_t bla  = (Int_t)(name[6]-'0')-1; */
/*   if(bla == -1) bla = 9; */
/*   return bla; */
/* } */

#endif  /* !PNDGEOMUO_H */



