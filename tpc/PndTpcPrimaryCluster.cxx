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
  : ft(0), fq(0), fmcTrackId(0),fmcHitId(0),fpos(0,0,0)
{}

PndTpcPrimaryCluster::PndTpcPrimaryCluster(const double t, 
				     const int q, 
				     const TVector3& pos,
				     const unsigned int mcTrackId,
				     const unsigned int mcHitId)
  : ft(t), fq(q), fmcTrackId(mcTrackId),fmcHitId(mcHitId),fpos(pos)
{}


PndTpcPrimaryCluster::~PndTpcPrimaryCluster()
{}

unsigned int 
PndTpcPrimaryCluster::mcTrackId() const {
  return fmcTrackId;
}

unsigned int
PndTpcPrimaryCluster::mcHitId() const {
  return fmcHitId;
}
