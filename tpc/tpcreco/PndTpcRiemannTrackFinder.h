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
//      Johannes Rauch       TUM
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
class GFTrackCand;
class PndTpcAbsHitTrackCorrelator;
class PndTpcAbsTrackTrackCorrelator;
class PndTpcRiemannTrack;

class PndTpcRiemannTrackFinder {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrackFinder();
  ~PndTpcRiemannTrackFinder();

  
  // Accessors -----------------------
  const PndTpcAbsHitTrackCorrelator* getCorrelator(unsigned int i) const
  {return _correlators.at(i);}
  const PndTpcAbsTrackTrackCorrelator* getTTCorrelator(unsigned int i) const
  {return _TTcorrelators.at(i);}

  static int getSorting(){return _sorting;}
  static double getInteractionZ(){return _interactionZ;}

  // Modifiers -----------------------
  void setMinHitsForFit(unsigned int n){_minHitsForFit=n;}

  static void setSorting(int s){_sorting=s;} // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R
  void setSortingMode(bool sortingMode){_sortingMode=sortingMode;} // false: sort only according to _sorting; true: use internal sorting when adding hits to trackcands
  static void setInteractionZ(double z){_interactionZ=z;}


  // Operations ----------------------
  unsigned int buildTracks(std::vector<PndTpcCluster*>& clusters,
			   std::vector<PndTpcRiemannTrack*>& candlist);

  void mergeTracks(std::vector<PndTpcRiemannTrack*>& candlist);

  void addCorrelator(PndTpcAbsHitTrackCorrelator* c);
  void addTTCorrelator(PndTpcAbsTrackTrackCorrelator* c);

  void sortClusters(std::vector<PndTpcCluster*>& cll);

private:

  // Private Data Members ------------
  std::vector<PndTpcAbsHitTrackCorrelator*> _correlators;
  std::vector<bool> _found; // flags which correlator fired
  std::vector<double> _bestMatchQuality;
  std::vector<unsigned int> _bestMatchIndex;

  std::vector<PndTpcAbsTrackTrackCorrelator*> _TTcorrelators;

  static int _sorting;
  bool _sortingMode;
  static double _interactionZ;

  unsigned int _minHitsForFit;

  // Private Methods -----------------
  void resetFlags();

};

// sorting algorithm
bool sortCluster(PndTpcCluster* s1, PndTpcCluster* s2);

bool sortTracksN(PndTpcRiemannTrack* t1, PndTpcRiemannTrack* t2);
bool sortTracksFirstClusterPos(PndTpcRiemannTrack* t1, PndTpcRiemannTrack* t2);

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
