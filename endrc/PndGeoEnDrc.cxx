////////////////////////////////////////////////////////////////////////////////
//
//  PndGeoEnDrc
//
//  This is the interface to the geometry file
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////

#include "PndGeoEnDrc.h"

ClassImp(PndGeoEnDrc)

// -----------------------------------------------------------------------------
PndGeoEnDrc::PndGeoEnDrc()
{
  fName = "edrc";
  maxSectors = 0;
  maxModules = 99;
}

// -----------------------------------------------------------------------------
const char *PndGeoEnDrc::getEleName(Int_t m)
{
  // Returns the element name of endrc number m

  if (m < 9) sprintf(eleName,"edrc0%i",m+1);
  else       sprintf(eleName,"edrc%i", m+1);
  return eleName;
}

// -----------------------------------------------------------------------------
const char *PndGeoEnDrc::getModuleName(Int_t m)
{
  // Returns the module name of ddrc number m

  if (m < 9) sprintf(modName,"edrc0%i",m+1);
  else       sprintf(modName,"edrc%i", m+1);
  return modName;
}
