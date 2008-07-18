//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track finder using the riemann circle fit
//
//      The algorithm can be configured with correlators 
//      (see PndTpcAbsHitTrackCorrelator)
//      For each hit all track candidates are tested
//      For each track the algorithm is stepping through the 
//      correlators which are in a hirarchical order. 
//      A correlator can be applicable or not. 
//      If it is applicalble to the hit/track combination and
//      the track survives, then the correlator delivers a matchQuality.
//      Finally the hit is added to the track candidate that 
//      reached the deepest correlator level and 
//      if there are ambiguities achieved the best machtQuality
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCRIEMANNTRACKFINDER_HH
#define TPCRIEMANNTRACKFINDER_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector> // remove if you do not need streaming op


// Collaborating Class Declarations --
class PndTpcCluster;
class TrackCand;
class PndTpcAbsHitTrackCorrelator;
class PndTpcRiemannTrack;

class PndTpcRiemannTrackFinder {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrackFinder();
  ~PndTpcRiemannTrackFinder();

  
  // Accessors -----------------------
  const PndTpcAbsHitTrackCorrelator* getCorrelator(unsigned int i) const
  {return _correlators.at(i);}

  // Modifiers -----------------------
  void setMinHitsForFit(unsigned int n){_minHitsForFit=n;}

  // Operations ----------------------
  unsigned int buildTracks(std::vector<PndTpcCluster*>& clusters,
			   std::vector<PndTpcRiemannTrack*>& candlist);

  void addCorrelator(PndTpcAbsHitTrackCorrelator* c);
private:

  // Private Data Members ------------
  std::vector<PndTpcAbsHitTrackCorrelator*> _correlators;
  std::vector<bool> _found; // flags which correlator fired
  std::vector<double> _bestMatchQuality;
  std::vector<unsigned int> _bestMatchIndex;

  unsigned int _minHitsForFit;
  // Private Methods -----------------
  void resetFlags();
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
