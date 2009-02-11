#ifndef PNDRPCGEO_H
#define PNDRPCGEO_H

#include "FairGeoSet.h"

class  PndRpcGeo : public FairGeoSet {

protected:
 char eleName[20];  // substring for elements in module 
 char modName[20];  // name of module

public:
  PndRpcGeo();
  ~PndRpcGeo() {}
  const char* getModuleName(Int_t);
  const char* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);

  ClassDef(PndRpcGeo,0) // Class for RpcTof
};

inline Int_t PndRpcGeo::getModNumInMod(const TString& name) {
  // returns the module index from module name
  return (Int_t)(name[3]-'0')-1;
}

#endif  /* !PNDRPCGEO_H */




