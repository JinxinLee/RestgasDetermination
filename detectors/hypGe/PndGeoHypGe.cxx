/////////////////////////////////////////////////////////////
// PndGeoHyp
//
// Class for geometry of EMC
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoHypGe.h"
#include "FairGeoNode.h"

ClassImp(PndGeoHypGe)

// -----   Default constructor   -------------------------------------------
PndGeoHypGe::PndGeoHypGe() {
  // Constructor
  fName="Ge_Crystal";
  maxSectors=0;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndGeoHypGe::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"Ge_Crystal%i",m+1);
  return modName;
}

const char* PndGeoHypGe::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"Ge_Crystal%i",m+1);
  return eleName;
}
