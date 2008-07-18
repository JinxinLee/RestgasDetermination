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
//
//
//-----------------------------------------------------------

#ifndef TPCSIMPLEPSASTRATEGY_HH
#define TPCSIMPLEPSASTRATEGY_HH

// Base Class Headers ----------------
#include "PndTpcAbsPSAStrategy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "McIdCollection.h"
// Collaborating Class Declarations --



class PndTpcSimplePSAStrategy : public PndTpcAbsPSAStrategy {
public:

  // Constructors/Destructors ---------
  PndTpcSimplePSAStrategy(const unsigned int threshold);
  virtual ~PndTpcSimplePSAStrategy(){;}

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual PndTpcDigi* ProcessNext(const PndTpcSample* sample);

  virtual void Process(const std::vector<PndTpcSample*> & samples, 
					   std::vector<PndTpcDigi*>& digis,
					   double padThreshold);



private:

  // Private Data Members ------------
  unsigned int _currentPadID;
  bool _inprogress;
  unsigned int _amp;
  unsigned int _t;
  unsigned int _threshold;
  
  McIdCollection _mcid;
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
