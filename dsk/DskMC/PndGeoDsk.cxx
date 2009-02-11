////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PndGeoDsk
//
//  This is the interface to the geometry file
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PndGeoDsk.h"
#include "FairGeoNode.h"

ClassImp(PndGeoDsk);


// standard constructor
// -------------------------------------------------------------------------------------------------
PndGeoDsk::PndGeoDsk()
{
  // the fixed name of our detector
  fName = "dsk";
  // we dont use sectors
  maxSectors = 0;
  // the max numer XX of modules used in geo-file (dskXX)
  maxModules = 49;
}


// return element name
// -------------------------------------------------------------------------------------------------
const char*
PndGeoDsk::getEleName(Int_t m)
{
  // Returns the element name of dsk number m
  sprintf(fEleName,"dsk%02i",m+1);
  return fEleName;
}

// return module name
// -------------------------------------------------------------------------------------------------
const char*
PndGeoDsk::getModuleName(Int_t m)
{
  // Returns the module name of dsk number m
  sprintf(fModName,"dsk%02i",m+1);
  return fModName;
}


// testing, maybe not needed?
void PndGeoDsk::addRefNodes() {
  // Adds the reference node
  FairGeoNode* volu=getVolume(fName);
  if (volu) masterNodes->Add(new FairGeoNode(*volu));
}
