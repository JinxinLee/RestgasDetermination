/////////////////////////////////////////////////////////////
// PndGeoEmc
//
// Class for geometry of EMC
//
// Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "PndGeoEmc.h"
#include "FairGeoNode.h"

ClassImp(PndGeoEmc)

// -----   Default constructor   -------------------------------------------
PndGeoEmc::PndGeoEmc() {
  // Constructor
  fName="emc";
  maxSectors=0;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndGeoEmc::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"emc%i",m+1);
  return modName;
}

const char* PndGeoEmc::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"emc%i",m+1);
  return eleName;
}
