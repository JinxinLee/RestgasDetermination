#include "PndRichGeo.h"
#include "FairGeoNode.h"

ClassImp(PndRichGeo)

// -----   Default constructor   -------------------------------------------
PndRichGeo::PndRichGeo()
  : FairGeoSet()
{
  // Constructor
  // fName has to be the name used in the geometry for all volumes.
  // If there is a mismatch the geometry cannot be build.
  fName="rich";
  maxSectors=0;
  maxModules=10;
}

// -------------------------------------------------------------------------

const char* PndRichGeo::getModuleName(Int_t m)
{
  /** Returns the module name of PndRich number m
      Setting PndRich here means that all modules names in the
      ASCII file should start with PndRich otherwise they will
      not be constructed
  */
  sprintf(modName,"rich0%i",m+1);
  return modName;
}

const char* PndRichGeo::getEleName(Int_t m)
{
  /** Returns the element name of Det number m */
  sprintf(eleName,"rich0%i",m+1);
  return eleName;
}
