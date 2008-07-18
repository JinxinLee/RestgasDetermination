//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// CbmGeoTof
//
// Class for geometry of PndMvd
//
/////////////////////////////////////////////////////////////

#include "PndMvdGeo.h"
#include "CbmGeoNode.h"

ClassImp(PndMvdGeo)

PndMvdGeo::PndMvdGeo() 
{
  // Constructor
  fName="mvd";
  maxSectors=99;
  maxModules=99;
  //maxKeepinVolumes=99;
}

const char* PndMvdGeo::getModuleName(Int_t m) 
{
  if ( m < 0 ) {
  	std::cout <<"-E- CbmGeoSts::getModuleName:: Module number "
  	       << m << " not known!" << std::endl;
	 return "";
	 }
  if ( m < 9 ) sprintf(modName,"mvdstation0%i",m+1);
  else  sprintf(modName,"mvdstation%i",m+1);
  return modName;
}

const char* PndMvdGeo::getEleName(Int_t m) 
{
  // Returns the element name of PndMvd number m
  if (m < 9) 
  	sprintf(eleName,"mvd0%i",m+1);
  else sprintf(eleName,"mvd%i",m+1);
  
  return eleName;
}

const char* PndMvdGeo::getKeepinName(Int_t i, Int_t j)
{
   sprintf(keepinName,"MVDKeepin0%i",i+1);
   return keepinName;
}
