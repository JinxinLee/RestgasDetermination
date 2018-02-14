#ifndef PNDGEOSTT_H
#define PNDGEOSTT_H

#include "FairGeoSet.h"

class  PndGeoStt : public FairGeoSet 
{
 protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elements in module
 public:
  PndGeoStt();
  ~PndGeoStt() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(PndGeoStt,0) // Class for Stt
};

// TODO: we don't need this?

inline Int_t PndGeoStt::getModNumInMod(const TString& name) {
  // returns the module index from module name

 
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !PNDGEOSTT_H */
