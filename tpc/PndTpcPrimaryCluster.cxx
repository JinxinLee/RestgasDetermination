//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPrimaryCluster
//      see PndTpcPrimaryCluster.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPrimaryCluster.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------


ClassImp(PndTpcPrimaryCluster);

PndTpcPrimaryCluster::PndTpcPrimaryCluster()
  : _t(0), _q(0), _mcTrackId(0),_mcHitId(0),_pos(0,0,0)
{}

PndTpcPrimaryCluster::PndTpcPrimaryCluster(const double t, 
				     const int q, 
				     const TVector3& pos,
				     const unsigned int mcTrackId,
				     const unsigned int mcHitId)
  : _t(t), _q(q), _mcTrackId(mcTrackId),_mcHitId(mcHitId),_pos(pos)
{}


PndTpcPrimaryCluster::~PndTpcPrimaryCluster()
{}

unsigned int 
PndTpcPrimaryCluster::mcTrackId() const {
  return _mcTrackId;
}

unsigned int
PndTpcPrimaryCluster::mcHitId() const {
  return _mcHitId;
}
