//-----------------------------------------------------------
// File and Version Information:
// 
//
// Description:
//      This class is a file processing interface for a
//      PndTpcEFieldCyl class.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM     (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEFIELDCYLLOADER_H
#define TPCEFIELDCYLLOADER_H

//Collaborating Class Headers -------------------------------

class PndTpcEFieldCyl;

#include <vector>
#include "TVector3.h"

class PndTpcEFieldCylLoader
{

public:

  //Constructors / Destructors ------------------
  PndTpcEFieldCylLoader(PndTpcEFieldCyl*, const char* const);
  ~PndTpcEFieldCylLoader();

  //Methods
  void load();

private:

  const char* ffileName;
  PndTpcEFieldCyl* ffield;
  std::vector<std::vector<TVector3*>*>* ffieldmap;
  
};

#endif
  
