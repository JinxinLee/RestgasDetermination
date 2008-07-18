//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClustershapeCorrelator
//      see PndTpcClustershapeCorrelator.hh for details
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
#include "PndTpcClustershapeCorrelator.h"

// C/C++ Headers ----------------------
#include "TVector3.h"

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"
#include "DebugLogger.h"

// Class Member definitions -----------

PndTpcClustershapeCorrelator::PndTpcClustershapeCorrelator(double cut)
  : _proxcut(cut)
{}


bool
PndTpcClustershapeCorrelator::corr(PndTpcRiemannTrack* trk,
				  PndTpcRiemannHit* rhit,
				  bool& survive,
				  double& matchQuality)
{
  const PndTpcCluster* cl=rhit->cluster();
  if(cl==NULL)return false; // not applicable
  // get closest hit from track
  double l=1000;
  TVector3 dir;
  trk->getClosestHit(rhit,l,dir);
  DebugLogger::Instance()->Histo2D("shape_dir_theta",cl->axis().CosTheta(),
				 dir.CosTheta(),
				 -1,1,100,-1,1,100);
 
  return false;
}


