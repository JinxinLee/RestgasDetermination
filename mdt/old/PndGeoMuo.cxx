/////////////////////////////////////////////////////////////
// PndGeoMuo
//
// Class for geometry of MUO
//
/////////////////////////////////////////////////////////////

#include "PndGeoMuo.h"
#include "CbmGeoNode.h"

ClassImp(PndGeoMuo)

// -----   Default constructor   -------------------------------------------
PndGeoMuo::PndGeoMuo() {
  // Constructor
  fName="muo";
  maxSectors=0;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndGeoMuo::getModuleName(Int_t m) {
  // Returns the module name of muo number m
/*
  if ( m < 9 ) sprintf(modName,"muo0%i",m+1);
  else sprintf(modName,"muo%i",m+1);
  // cout << "MODNAME: " << modName << endl;
  */
  sprintf(modName,"muo0%i",m+1);
  return modName;
}

const char* PndGeoMuo::getEleName(Int_t m) {
  // Returns the element name of muo number m
  /*
  if ( m < 9 ) sprintf(eleName,"gasstt0%i",m+1);
  else sprintf(eleName,"gasstt%i",m+1);

  // cout << "ELENAME: " << eleName << endl;
  */
sprintf(eleName,"muo0%i",m+1);

  return eleName;
}
