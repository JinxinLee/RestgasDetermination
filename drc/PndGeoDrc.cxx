/////////////////////////////////////////////////////////////
// PndGeoDrc
//
// Class for geometry of DRC
//
// Created 13/11/07 by A. Cecchi, Ferrara University 
//
/////////////////////////////////////////////////////////////

#include "PndGeoDrc.h"
#include "FairGeoNode.h"

ClassImp(PndGeoDrc)

// -----   Default constructor   -------------------------------------------
PndGeoDrc::PndGeoDrc() {
  // Constructor
  fName="drc";
  maxSectors=0;
  maxModules=20;
 }
// -------------------------------------------------------------------------

const char* PndGeoDrc::getModuleName(Int_t m) {
  // Returns the module name of drc number m
  // sprintf(modName,"drc0%i",m+1);
  if ( m < 9 ) sprintf(modName,"drc0%i",m+1);
  else sprintf(modName,"drc%i",m+1);
  return modName;
}

const char* PndGeoDrc::getEleName(Int_t m) {
  // // Returns the element name of drc number m
  //sprintf(eleName,"drc0%i",m+1);
  if ( m < 9 ) sprintf(eleName,"drc0%i",m+1);
  else sprintf(eleName,"drc%i",m+1);

  return eleName;
}
