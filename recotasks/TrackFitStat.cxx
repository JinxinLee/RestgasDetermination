//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TrackFitStat
//      see TrackFitStat.hh for details
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
#include "TrackFitStat.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(TrackFitStat)

  TrackFitStat::TrackFitStat()
    : _hasMc(false), _mccharge(0), _nPDG(0),_motherID(-100),
      _p(0.), _pstart(0), _mom(TVector3(0.,0.,0.)), _sigp(0.),
      _failedHits(0)
{}



