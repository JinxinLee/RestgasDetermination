//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      COG calculation for PndTpcClusters
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

#ifndef TPCCLUSTERCOG_HH
#define TPCCLUSTERCOG_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --
class PndTpcCluster;
class PndTpcDigi;


class PndTpcClusterCOG {
public:

  // Constructors/Destructors ---------
  PndTpcClusterCOG(){}
  ~PndTpcClusterCOG(){}

  // Operations ----------------------
  PndTpcCluster*
  cog(std::vector<PndTpcDigi*>* digis);

private:

  // Private Data Members ------------


  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
