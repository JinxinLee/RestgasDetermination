//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterRadius
//      see PndTpcClusterRadius.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcClusterRadius.h"
#include "PndTpcCluster.h"

bool 
PndTpcClusterRadius::operator()(PndTpcCluster* s1, PndTpcCluster* s2)
{
  double r1=s1->pos().Perp();
  double r2=s2->pos().Perp();
  return r1>r2;
}

