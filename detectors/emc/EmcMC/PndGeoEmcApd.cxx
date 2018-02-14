/////////////////////////////////////////////////////////////
// PndGeoEmcApd
//
// Class for geometry of EMCAPD
//
// Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "PndGeoEmcApd.h"
#include "FairGeoNode.h"

ClassImp(PndGeoEmcApd)

// -----   Default constructor   -------------------------------------------
PndGeoEmcApd::PndGeoEmcApd() {
  // Constructor
  fName="apd";
  maxSectors=0;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndGeoEmcApd::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"apd%i",m+1);
  return modName;
}

const char* PndGeoEmcApd::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"apd%i",m+1);
  return eleName;
}
