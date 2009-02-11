//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class MCTruthAnnex
//      see MCTruthAnnex.hh for details
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
#include "MCTruthAnnex.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "PndMCTrack.h"

// Class Member definitions -----------


ClassImp(MCTruthAnnex);

MCTruthAnnex::MCTruthAnnex()
  : _isReco(false)
{}


MCTruthAnnex::~MCTruthAnnex(){}


