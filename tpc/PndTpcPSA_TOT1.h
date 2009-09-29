//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Digitization
//      Implementation of a concrete pulse shape analyzer
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Christian Hoeppner   TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPSA_TOT1_HH
#define TPCPSA_TOT1_HH

// Base Class Headers ----------------
#include "PndTpcAbsPSAStrategy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "McIdCollection.h"
// Collaborating Class Declarations --



class PndTpcPSA_TOT1 : public PndTpcAbsPSAStrategy {
public:

  // Constructors/Destructors ---------
  PndTpcPSA_TOT1();
  virtual ~PndTpcPSA_TOT1(){;}

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual PndTpcDigi* ProcessNext(const PndTpcSample* sample);

  virtual void Process(const std::vector<PndTpcSample*> & samples, 
					   std::vector<PndTpcDigi*>& digis,
					   double padThreshold);



private:

  // Private Data Members ------------
  unsigned int fcurrentPadID;
  bool finprogress;
  unsigned int famp;
  unsigned int ft;
  unsigned int fthreshold;
  
  McIdCollection fmcid;
  // Private Methods -----------------
  void processPulse(std::vector<PndTpcSample*>,double& t0,double& A,double& length);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
