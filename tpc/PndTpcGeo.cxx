//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcGeo
//      see PndTpcGeo.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcGeo.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairGeoNode.h"

// Class Member definitions -----------

ClassImp(PndTpcGeo)

// -----   Default constructor   -------------------------------------------
PndTpcGeo::PndTpcGeo() {
  // Constructor
  fName="tpc";
  maxSectors=9;
  maxModules=9;
}

// -------------------------------------------------------------------------

const char* PndTpcGeo::getModuleName(Int_t m) {

  // Returns the module name of PndTpc number m

  // Setting tpc here means that all modules names in the ASCII file should start with tpc otherwise they will not be constructed


  sprintf(modName,"tpcChamber%i",m+1); 
  return modName;
}

const char* PndTpcGeo::getEleName(Int_t m) {
  // Returns the element name of Det number m
  sprintf(eleName,"tpc0%i",m+1);
  return eleName;
}

 

