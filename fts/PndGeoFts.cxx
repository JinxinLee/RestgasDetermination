/////////////////////////////////////////////////////////////
// PndGeoFts
//
// Class for geometry of FTS
//
/////////////////////////////////////////////////////////////

#include "PndGeoFts.h"
#include "FairGeoNode.h"

ClassImp(PndGeoFts)

PndGeoFts::PndGeoFts() 
{
  // Constructor
  fName="fts";
  maxSectors=40;
  maxModules=0; 
}

const char* PndGeoFts::getModuleName(Int_t m) 
{
  // Returns the module name of fts number m

  if ( m < 9 ) sprintf(modName,"fts0%i",m+1);
  else sprintf(modName,"fts%i",m+1);
  //std::cout << "MODNAME: " << modName << std::endl;

  return modName;
}

const char* PndGeoFts::getEleName(Int_t m) 
{
  // Returns the element name of fts number m

  if ( m < 9 ) sprintf(eleName,"fts0%i",m+1);
  else sprintf(eleName,"fts%i",m+1);
  //std::cout << "ELENAME: " << eleName << std::endl;

  return eleName;

}
