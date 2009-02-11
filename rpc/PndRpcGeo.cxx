/////////////////////////////////////////////////////////////
// RpcTofGeo
//
// Class for geometry of RpcTof
//
/////////////////////////////////////////////////////////////

#include "PndRpcGeo.h"
#include "FairGeoNode.h"

ClassImp(PndRpcGeo)

// -----   Default constructor   -------------------------------------------
PndRpcGeo::PndRpcGeo() {
  // Constructor
  fName="rpc";
  maxSectors=0;
  maxModules=24;
 }
// -------------------------------------------------------------------------

const char* PndRpcGeo::getModuleName(Int_t m) {
   // Returns the module name of rpctof number m

  // Setting rpctof here means that all modules names 
  // in the ASCII file should start with rpctof otherwise 
  // they will not be constructed
 
  sprintf(modName,"tof%i",m+1);
  return modName;
  
}

const char* PndRpcGeo::getEleName(Int_t m) {
  // Returns the element name of rpctof number m
 
  sprintf(eleName,"t%i",m+1);
    return eleName;
}
