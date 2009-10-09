//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track finder using a hough transform on the riemann sphere
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCRIEMANNHOUGH_HH
#define TPCRIEMANNHOUGH_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector> // remove if you do not need streaming op


// Collaborating Class Declarations --
class PndTpcCluster;
class GFTrackCand;
class PndTpcAbsHitTrackCorrelator;
class PndTpcRiemannTrack;
class TH3D;


class PndTpcRiemannHough {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannHough();
  ~PndTpcRiemannHough();

  
  // Accessors -----------------------

  // Modifiers -----------------------
  //void setMinHitsForFit(unsigned int n){_minHitsForFit=n;}

  // Operations ----------------------
  unsigned int buildTracks(std::vector<PndTpcCluster*>& clusters,
			   std::vector<PndTpcRiemannTrack*>& candlist);

  
private:

  // Private Data Members ------------
  std::vector<bool> _found; // flags which correlator fired
  std::vector<double> _bestMatchQuality;
  std::vector<unsigned int> _bestMatchIndex;

  TH3D* histo;

  unsigned int _minHitsForFit;
  // Private Methods -----------------
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
