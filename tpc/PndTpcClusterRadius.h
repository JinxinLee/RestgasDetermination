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

#ifndef TPCCLUSTERRADIUS_HH
#define TPCCLUSTERRADIUS_HH

class PndTpcCluster;

class PndTpcClusterRadius {
public:

  // Constructors/Destructors ---------
  PndTpcClusterRadius(){;}
  ~PndTpcClusterRadius(){;}

  // Operators
  bool operator()(PndTpcCluster* s1, PndTpcCluster* s2);
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
