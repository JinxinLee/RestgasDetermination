//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Simple noise generator
//      Puts additional charge into the drift volume
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert        TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCNOISETASK_H
#define TPCNOISETASK_H

// Base Class Headers ---------------------------------------
#include "FairTask.h"

// External Headers -----------------------------------------
#include <string>
#include "TVector3.h"



class TClonesArray;
class PndTpcDigiPar;
class PndTpcGas;

class PndTpcNoiseTask : public FairTask {

 public:

// Constructors ---------------------------------------------
  PndTpcNoiseTask();
  ~PndTpcNoiseTask();

// ----------------------------------------------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void SetParContainers();

// Accessors ------------------------------------------------

// Modifiers ------------------------------------------------

 
  void setOccupancy(double occ){fOcc=occ;}
  void setMeanEnergy(double b){fmeanEnergy=b;} // keV

  
 private:

  TClonesArray* fprimArray;
  double fmeanEnergy; // keV
  //double fstn; // signal to noise ratio
  double fOcc;  //occupancy in terms of fVox

  PndTpcDigiPar* fpar;  
  const PndTpcGas* fGas;

  unsigned int fVox;  //number of voxels (nSamples * nPads) for normalization
  
public:
  ClassDef(PndTpcNoiseTask,1)

};

#endif




