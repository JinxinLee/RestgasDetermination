//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Functor for comparison of pointers to PndTpcClusters
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

#ifndef TPCCLUSTERDIST_HH
#define TPCCLUSTERDIST_HH

class PndTpcCluster;

class PndTpcClusterDist {
public:

  // Constructors/Destructors ---------
  PndTpcClusterDist(bool dir=1): fdir(dir){;}
  ~PndTpcClusterDist(){;}

  // Operators
  bool operator()(PndTpcCluster* s1, PndTpcCluster* s2);
  
private:
  bool fdir; // true=> sorting from back to front; false=> forward sorting

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
