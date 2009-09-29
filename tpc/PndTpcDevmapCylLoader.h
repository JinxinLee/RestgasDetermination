//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      This class is a file processing interface for a
//      PndTpcDevmapCyl class.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM     (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCDEVMAPCYLLOADER_H
#define TPCDEVMAPCYLLOADER_H

//Collaborating Class Headers -------------------------------

class PndTpcDevmapCyl;

#include <vector>
#include "TVector3.h"

class PndTpcDevmapCylLoader
{

public:

  //Constructors / Destructors ------------------
  PndTpcDevmapCylLoader(PndTpcDevmapCyl*, const char* const);
  ~PndTpcDevmapCylLoader();

  //Methods
  int load();

private:

  const char* ffileName;
  PndTpcDevmapCyl* fdevmap;
  std::vector<std::vector<TVector3*>*>* ffieldmap;
  double fvDrift;
  
};

#endif
  
