/////////////////////////////////////////////////////////////
// CbmGeoSciT
//
// Class for geometry of SCIT
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoSciT.h"
#include "FairGeoNode.h"

ClassImp(PndGeoSciT)

// -----   Default constructor   -------------------------------------------
PndGeoSciT::PndGeoSciT() {
  // Constructor

  fName="ft";
  maxSectors=0;
  maxModules=1;
  strcpy(modName,"ft");
  strcpy(eleName,"ft");
 }
// -------------------------------------------------------------------------
/*
const char* PndGeoSciT::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"stg0%i",m+1);
  return modName;
 cout << "MODNAME: " << modName << endl;
}

const char* PndGeoSciT::getEleName(Int_t m) {
  // Returns the element name of muo number m
 
  sprintf(eleName,"stglay%i",m+1);
  return eleName;
 cout << "ELENAME: " << eleName << endl;
}
*/





