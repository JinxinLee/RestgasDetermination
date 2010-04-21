//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Digitization
//      Abstract interface for Pulse Shape Analyzer
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCABSPSASTRATEGY_HH
#define TPCABSPSASTRATEGY_HH

// Base Class Headers ----------------
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "PndTpcAbsPulseshape.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class PndTpcSample;
class PndTpcDigi;

class PndTpcAbsPSAStrategy {
public:

  // Constructors/Destructors ---------
  PndTpcAbsPSAStrategy(){}
  virtual ~PndTpcAbsPSAStrategy(){;}


 // Operations ----------------------
  virtual PndTpcDigi* ProcessNext(const PndTpcSample* sample) = 0;

  virtual void Process(const std::vector<PndTpcSample*> & samples, 
					   std::vector<PndTpcDigi*>& digis,
					   double padThreshold) = 0;

  // TODO kill Maxence:
  virtual void setPs(PndTpcAbsPulseshape* pulseshape) { std::cerr << "setPs not implemented!" << std::endl; exit(1); };

  virtual void TailCancellation(bool b) { std::cerr << "TailCancellation not implemented!" << std::endl; exit(1); };

  virtual void setFreq(int b) { std::cerr << "Set frequency not implemented!" << std::endl; exit(1); };

protected:

  // Private Data Members ------------
 

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
