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
  maxSectors=0;
  maxModules=20; 
}

const char* PndGeoFts::getModuleName(Int_t m) 
{
  // Returns the module name of stt number m
  // sprintf(modName,"fts%i",m+1);

  //if ( m < 9 ) sprintf(modName,"fts0%i",m+1);
  //else sprintf(modName,"fts%i",m+1);
 sprintf(modName,"fts0%i",m+1);

  return modName;
}

const char* PndGeoFts::getEleName(Int_t m) 
{
  // Returns the element name of sts number m
  sprintf(eleName,"fts0%i",m+1);

  //if ( m < 9 ) sprintf(eleName,"fts0%i",m+1);
  //else sprintf(eleName,"fts%i",m+1);
  return eleName;

}
