//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDriftedElectron
//      see PndTpcDriftedElectron.hh for details
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
#include "PndTpcDriftedElectron.h"

// C/C++ Headers ----------------------
#include <cmath>

// Collaborating Class Headers --------
#include "PndTpcPrimaryCluster.h"

// Class Member definitions -----------

ClassImp(PndTpcDriftedElectron)

PndTpcDriftedElectron::PndTpcDriftedElectron():
  _x(0),_y(0),_t(0),_tpcPClusterp(0)
{;}



PndTpcDriftedElectron::PndTpcDriftedElectron(const double x, 
				       const double y,
				       const double t,
				       PndTpcPrimaryCluster* tPCp)
  : _x(x),_y(y),_t(t),
    _tpcPClusterp(tPCp)
{;}

unsigned int 
PndTpcDriftedElectron::mcTrackId() const {
  if(_tpcPClusterp==0)return 0;
  return _tpcPClusterp->mcTrackId();
}

unsigned int
PndTpcDriftedElectron::mcHitId() const {
  if(_tpcPClusterp==0)return 0;
  return _tpcPClusterp->mcHitId();
}

bool operator== (const PndTpcDriftedElectron& lhs, const PndTpcDriftedElectron& rhs)
{
  double dx=lhs._x - rhs._x;
  double dy=lhs._y - rhs._y;
  double dt=lhs._t - rhs._t;
  double d2=dx*dx+dy*dy;
  return (d2<1E-10) && (fabs(dt)<1E-4);
}
