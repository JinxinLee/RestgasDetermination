/////////////////////////////////////////////////////////////
// PndDchGeo
//
// Class for geometry of DCH
//
/////////////////////////////////////////////////////////////

#include "PndDchGeo.h"
#include "FairGeoNode.h"

ClassImp(PndDchGeo)

// -----   Default constructor   -------------------------------------------
PndDchGeo::PndDchGeo() {
  // Constructor
  fName="dch";
  maxSectors=20;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndDchGeo::getModuleName(Int_t m) {
   // Returns the module name of Tpc number m

  // Setting tpc here means that all modules names 
  // in the ASCII file should start with tpc otherwise 
  // they will not be constructed
 
  sprintf(modName,"dchVol%i",m+1);
  return modName;
  //  return "dchChamber";
}

const char* PndDchGeo::getEleName(Int_t m) {
  // Returns the element name of Det number m
 
  sprintf(eleName,"dchSplaneVol%i",m+1);
  //   sprintf(eleName,"dchModule%i",m+1);
  
  return eleName;
}
