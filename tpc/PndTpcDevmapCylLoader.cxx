//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDevmapCylLoader
//      See class header for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


//This class' Header ---------------------------------------
#include "PndTpcDevmapCylLoader.h"

//C++ Headers ----------------------------------------------
#include <fstream>
#include <iostream>

//Collaborating Headers ------------------------------------
#include "PndTpcDevmapCyl.h"
#include "TError.h"

//Class Member Definitions ---------------------------------

PndTpcDevmapCylLoader::PndTpcDevmapCylLoader(PndTpcDevmapCyl* devmap,
				       const char* const fileName)
{
  fdevmap = devmap;
  ffileName = fileName;
  fvDrift = fdevmap->vDrift();
}


PndTpcDevmapCylLoader::~PndTpcDevmapCylLoader()
{;}

int 
PndTpcDevmapCylLoader::load()
{
  // try to find and open the file
  std::ifstream infile(ffileName, std::fstream::in);
  if (!infile.good()) 
  {
    Error("PndTpcDevmapCylLoader::load()","devmap-file could not be found.");
    return 1;
  }
  
  int rSteps, zSteps;
  double rMin, rMax, rWidth, zMin, zMax, zWidth;
 
  infile>>rSteps>>zSteps>>rMin>>rMax>>zMin>>zMax; 
  rWidth = (double)(rMax - rMin)/rSteps;
  zWidth = (double)(zMax - zMin)/zSteps;
  
  fdevmap->setNominal(TVector3(0.,0.,0.));
  fdevmap->setMinR(rMin);
  fdevmap->setSpacingR(rWidth);
  fdevmap->setMinZ(zMin);
  fdevmap->setSpacingZ(zWidth);

  fdevmap->fxBins = rSteps;
  fdevmap->fzBins = zSteps;
  
  
  // Initialize the field-map 
  ffieldmap = new std::vector<std::vector<TVector3*>*>;  

  // fill the matrix from the file; z-Index runs first
  for (int nr=0; nr<rSteps; nr++)
  {
    ffieldmap->push_back(new std::vector<TVector3*>(zSteps));
    for (int nz=0; nz<zSteps; nz++)
    {
      double devX, devY, time, dump, devZ, straight_way;
      infile>>devX>>devY>>time>>dump>>dump;
      straight_way = (nz + 0.5) * zWidth;
      devZ = time*fvDrift - straight_way;    //actual z-deviation
      ffieldmap->at(nr)->at(nz) = new TVector3(devX, devY, devZ);      
    }
  }
  std::cout<<"\n\n\nPndTpcDevmapCylLoader::load(): successfully initialized the Dev-field"<<std::endl;
  
  //just a test
  int lengthr = ffieldmap->size();
  int lengthz = ffieldmap->at(0)->size();
  std::cout << "fieldmap has " << lengthr <<" bins in r and \n"
	    << lengthz << " bins in z direction.\n\n";
  std::cout.flush();
  fdevmap->fpGrid = ffieldmap;       //PndTpcDevmapCyl takes ownership
  infile.close();
  return 0;
}


