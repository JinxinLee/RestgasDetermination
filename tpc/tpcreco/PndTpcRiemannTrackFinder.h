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
  PndTpcRiemannTrackFinder(double scale);
  ~PndTpcRiemannTrackFinder();

  
  // Accessors -----------------------
  const PndTpcAbsHitTrackCorrelator* getCorrelator(unsigned int i) const
  {return _correlators.at(i);}
  const PndTpcAbsTrackTrackCorrelator* getTTCorrelator(unsigned int i) const
  {return _TTcorrelators.at(i);}

  int getSorting(){return _sorting;}
  double getInteractionZ(){return _interactionZ;}

  // Modifiers -----------------------
  void setMinHitsForFit(unsigned int n){_minHitsForFit=n;}

  void setSorting(int s){_sorting=s;} // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: Distance to interaction point, 5: Phi
  void setSortingMode(bool sortingMode){_sortingMode=sortingMode;} // false: sort only according to _sorting; true: use internal sorting when adding hits to trackcands
  void setInteractionZ(double z){_interactionZ=z;}
  void setMaxNumHitsForPR(double MaxNumHitsForPR){_MaxNumHitsForPR=MaxNumHitsForPR;} // for debugging

  void SkipCrossingAreas(bool opt=true) {_skipCrossingAreas=opt;}
  void setTTProxcut(double cut){_TTproxcut=cut;} // needed for speeding up the merging
  void setProxcut(double cut){_proxcut=cut;} // needed for speeding up the initialized trackbuilding
  void setHelixcut(double cut){_helixcut=cut;} // needed for excluding hits in crossing areas

  void initTracks(bool initTrks=true, double dip=0, double curv=0) {_initTrks=initTrks; _initDip=dip; _initCurv=curv;}
  void setMaxR(double R){_maxR=R;}
  void setMinHits(unsigned int i){_minHits=i;}
  void SetSkipAndDelete(bool opt=true) {_skipAndDelete=opt;}

  void setScale(double scale){fRiemannScale=scale;}
  double getScale()const {return fRiemannScale;}

  // Operations ----------------------
  unsigned int buildTracks(std::vector<PndTpcCluster*>& clusters,
			   std::vector<PndTpcRiemannTrack*>& candlist);

  void mergeTracks(std::vector<PndTpcRiemannTrack*>& candlist);
  void cleanTracks(std::vector<PndTpcRiemannTrack*>& candlist, double szcut, double planecut);

  void addCorrelator(PndTpcAbsHitTrackCorrelator* c);
  void addTTCorrelator(PndTpcAbsTrackTrackCorrelator* c);

  void sortClusters(std::vector<PndTpcCluster*>& cll);
  void sortTracklets(std::vector<PndTpcRiemannTrack*>& tracklets);

private:

  // Private Data Members ------------
  std::vector<PndTpcAbsHitTrackCorrelator*> _correlators;
  std::vector<bool> _found; // flags which correlator fired
  std::vector<double> _bestMatchQuality;
  std::vector<unsigned int> _bestMatchIndex;

  std::vector<PndTpcAbsTrackTrackCorrelator*> _TTcorrelators;

  int _sorting;
  bool _sortingMode;
  double _interactionZ;
  int _MaxNumHitsForPR; // for debugging

  bool _skipCrossingAreas;

  bool _initTrks;
  bool _skipAndDelete; // skip hits far away from boundary and delete incomplete tracks!
  double _initDip;
  double _initCurv;
  double _maxR;
  unsigned int _minHits;

  unsigned int _minHitsForFit;

  double _proxcut;
  double _helixcut;
  double _TTproxcut;
  double fRiemannScale;

  // Private Methods -----------------
  void resetFlags();

};

// sorting algorithm for clusters
class sortClusterClass{
 public:
  bool operator() (PndTpcCluster* s1, PndTpcCluster* s2);
  void setSorting(int s){sorting=s;}
  void setInteractionZ(double z){interactionZ=z;}
 private:
  int sorting;
  double interactionZ;
};

// sorting algorithm for tracklets
class sortTrackletsClass{
 public:
  bool operator() (PndTpcRiemannTrack* t1, PndTpcRiemannTrack* t2);
  void setSorting(int s){sorting=s;}
 private:
  int sorting;
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
