//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Fitter for clusters in the tpc
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERFITTER_HH
#define TPCCLUSTERFITTER_HH

#include <vector>
#include "PndTpcClusterFitLikeli.h"

class PndTpcCluster;
class PndTpcDigi;

class PndTpcClusterFitter {
 public:
  PndTpcClusterFitter();

  PndTpcCluster* doFit(const std::vector<PndTpcDigi*>& digis);


 private:
  PndTpcClusterFitLikeli fLikeli;

};

#endif
