/////////////////////////////////////////////////////////////
// PndGeoHyp
//
// Class for geometry of EMC
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoHyp.h"
#include "FairGeoNode.h"

ClassImp(PndGeoHyp)

// -----   Default constructor   -------------------------------------------
PndGeoHyp::PndGeoHyp() {
  // Constructor
  fName="stg";
  maxSectors=0;
  maxModules=1;
  strcpy(modName,"s");
  strcpy(eleName,"s");
 }
// -------------------------------------------------------------------------
/*
const char* PndGeoHyp::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"stg0%i",m+1);
  return modName;
 cout << "MODNAME: " << modName << endl;
}

const char* PndGeoHyp::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"stglay%i",m+1);
  return eleName;
 cout << "ELENAME: " << eleName << endl;
}
*/





