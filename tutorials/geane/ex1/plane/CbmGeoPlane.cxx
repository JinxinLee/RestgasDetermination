/////////////////////////////////////////////////////////////
// CbmGeoplane
//
// Class for geometry of plane
//
/////////////////////////////////////////////////////////////

#include "CbmGeoPlane.h"
#include "FairGeoNode.h"

ClassImp(CbmGeoPlane)

// -----   Default constructor   -------------------------------------------
CbmGeoPlane::CbmGeoPlane() {
  // Constructor
  fName="m";
  maxSectors=20;
  maxModules=1;
  strcpy(modName,"mL");
  strcpy(eleName,"mL");

 }


