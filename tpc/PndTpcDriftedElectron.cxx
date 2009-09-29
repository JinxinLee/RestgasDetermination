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
  fx(0),fy(0),ft(0),ftpcPClusterp(0)
{;}



PndTpcDriftedElectron::PndTpcDriftedElectron(const double x, 
				       const double y,
				       const double t,
				       PndTpcPrimaryCluster* tPCp)
  : fx(x),fy(y),ft(t),
    ftpcPClusterp(tPCp)
{;}

unsigned int 
PndTpcDriftedElectron::mcTrackId() const {
  if(ftpcPClusterp==0)return 0;
  return ftpcPClusterp->mcTrackId();
}

unsigned int
PndTpcDriftedElectron::mcHitId() const {
  if(ftpcPClusterp==0)return 0;
  return ftpcPClusterp->mcHitId();
}

bool operator== (const PndTpcDriftedElectron& lhs, const PndTpcDriftedElectron& rhs)
{
  double dx=lhs.fx - rhs.fx;
  double dy=lhs.fy - rhs.fy;
  double dt=lhs.ft - rhs.ft;
  double d2=dx*dx+dy*dy;
  return (d2<1E-10) && (fabs(dt)<1E-4);
}
