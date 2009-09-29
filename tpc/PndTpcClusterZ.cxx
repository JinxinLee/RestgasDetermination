//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcClusterZ
//      see PndTpcClusterZ.h for details
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
#include "PndTpcClusterZ.h"
#include "PndTpcCluster.h"

bool 
PndTpcClusterZ::operator()(PndTpcCluster* s1, PndTpcCluster* s2)
{
  double r1=s1->pos().Z();
  double r2=s2->pos().Z();
  if(fdir)return r1>r2;
  else return r1<r2;
}

