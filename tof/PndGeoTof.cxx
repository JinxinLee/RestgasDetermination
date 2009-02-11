/////////////////////////////////////////////////////////////
// CbmGeoHyp
//
// Class for geometry of EMC
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoTof.h"
#include "FairGeoNode.h"

ClassImp(PndGeoTof)

// -----   Default constructor   -------------------------------------------
PndGeoTof::PndGeoTof() {
  // Constructor
  fName="t";
  maxSectors=0;
  maxModules=1;
  strcpy(modName,"t");
  strcpy(eleName,"t");
 }
// -------------------------------------------------------------------------
/*
const char* PndGeoTof::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"stg0%i",m+1);
  return modName;
 cout << "MODNAME: " << modName << endl;
}

const char* PndGeoTof::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"stglay%i",m+1);
  return eleName;
 cout << "ELENAME: " << eleName << endl;
}
*/





