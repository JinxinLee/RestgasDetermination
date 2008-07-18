//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcEFieldCylLoader
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
#include "PndTpcEFieldCylLoader.h"

//C++ Headers ----------------------------------------------
#include <fstream>
#include <iostream>

//Collaborating Headers ------------------------------------
#include "PndTpcEFieldCyl.h"
#include "TError.h"

//Class Member Definitions ---------------------------------

PndTpcEFieldCylLoader::PndTpcEFieldCylLoader(PndTpcEFieldCyl* field,
				       const char* const fileName)
{
  _field = field;
  _fileName = fileName;
}


PndTpcEFieldCylLoader::~PndTpcEFieldCylLoader()
{;}

void 
PndTpcEFieldCylLoader::load()
{
  // try to find and open the file
  std::ifstream infile(_fileName, std::fstream::in);
  if (!infile.good()) 
  {
    Fatal("PndTpcEFieldCylLoader::load()","field-file could not be found.");
    return;
  }
  
  int rSteps, zSteps;
  double eFieldNomZ;
  double rMin, dr, zMin, dz;
  infile >> eFieldNomZ >> rMin >> dr >> rSteps >> zMin >> dz >> zSteps;
  // dr = ((rSteps-1)*dr)/rSteps;   
  // dz = ((zSteps-1)*dz)/zSteps;   
  //DOLFIN seems to calculate the Bin-Size not right 
  //so this is an awkward correction     
  //REMOVED FOR MORE CLARITY! BE SURE TO HAVE A PROPER FIELD FILE!!!

  _field->setNominal(TVector3(0.,0.,eFieldNomZ));
  _field->setMinR(rMin);
  _field->setSpacingR(dr);
  _field->setMinZ(zMin);
  _field->setSpacingZ(dz);
  
  // Initialize the field-map 
  _fieldmap = new std::vector<std::vector<TVector3*>*>;  

  // fill the matrix from the file; z-Index runs first
  for (int nr=0; nr< rSteps; nr++)
  {
    _fieldmap->push_back(new std::vector<TVector3*>(zSteps));
    for (int nz=0; nz< zSteps; nz++)
    {
      double eFieldR, eFieldZ;
      infile >> eFieldR >> eFieldZ;
      _fieldmap->at(nr)->at(nz) = new TVector3(eFieldR, 0., eFieldZ);      
    }
  }
  std::cout<<"\n\n\nPndTpcEFieldCylLoader::load(): successfully initialized the E-field"
	   <<std::endl;
  
  //just a test
  int lengthr = _fieldmap->size();
  int lengthz = _fieldmap->at(0)->size();
  std::cout << "fieldmap has " << lengthr <<" bins in r and \n"
       << lengthz << " bins in z direction.\n\n";
  std::cout.flush();
  _field->_pGrid = _fieldmap;       //PndTpcEFieldCyl takes ownership
  infile.close();
}




