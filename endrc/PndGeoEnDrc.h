#ifndef PNDGEOENDRC_H
#define PNDGEOENDRC_H

////////////////////////////////////////////////////////////////////////////////
//
//  PndGeoEnDrc
//
//  This is the interface to the geometry file
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////

#include "FairGeoSet.h"

class PndGeoEnDrc : public FairGeoSet
{

protected:
  char modName[20];  // name of module
  char eleName[20];  // substring for elementes in module

public:
  PndGeoEnDrc();
  ~PndGeoEnDrc() {}

  const char *getEleName(Int_t number);
  const char *getModuleName(Int_t number);

  ClassDef(PndGeoEnDrc,1)
};

#endif // PNDGEODENDRC_H
