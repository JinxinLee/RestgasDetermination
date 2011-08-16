//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pattern recognition = track finding in the TPC
//      Using a riemann track fit
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCRIEMANNTRACKINGTASK_HH
#define TPCRIEMANNTRACKINGTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"
#include "PndTpcCluster.h"
#include "FairGeanePro.h"
class TClonesArray;
class PndTpcFrontend;
class PndTpcRiemannTrackFinder;
class GFRecoHitFactory;
class TH1I;
class TH1D;
class PndTrackCand;
class PndTrack;
class PndTpcDigiPar;

class PndTpcRiemannTrackingTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrackingTask();
  ~PndTpcRiemannTrackingTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;} // store RiemannTracks and RiemannHits

  void SetSortingParameters(
                   bool sortingMode=true,  // false: sort only according to _sorting; true: use internal sorting when adding hits to trackcands
                   int sorting=3,          // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to interaction point, 5: Phi, -5: -Phi
                   double interactionZ=0); // set if you use sorting = 4

  void SetMultistepParameters(bool doMultistep,   // do a multistep approach: 1. find steep tracks (presort clusters along z)
                                                  //                          2. find circle tracks  (presort clusters by angle)
                                                  //                          3. find all other tracks (presort clusters by decreasing radius)
                   unsigned int minHitsZ = 20,    // minimum number of hits for a track to be found in step 1
                   unsigned int minHitsPhi = 15); // minimum number of hits for a track to be found in step 2

  void SetTrkFinderParameters(
                   double proxcut,  // proximity cut in 3D
                   double helixcut, // distance to helix cut
                   unsigned int minpointsforfit, // minimum number of hits in track before a helix is fitted
                   double zStretch = 1.); // stretch proximity cut in z direction

  void SetMergeTracks(bool mergeTracks=true){_mergeTracks = mergeTracks;} // merge tracklets

  void SetMergeCurlers(bool mergeCurlers=true, double blowUp=5.){_MergeCurlers=mergeCurlers; _blowUp=blowUp;} // merge curlers

  void SetTrkMergerParameters(
                   double TTproxcut,  // proximity cut in 3D
                   double TTdipcut,   // cut on difference of dip angles of tracklets
                   double TThelixcut, // distance of the two helices
                   double TTplanecut);// cut on rms of distances of the riemann hits to intersection of the plane with the sphere of a combined fit

  void SetRiemannScale(double riemannscale=8.7) {_riemannscale = riemannscale;} // blow up factor of the riemann sphere
  
  void SkipCrossingAreas(bool opt=true) {_skipCrossingAreas=opt;} // skip and remove hits which would match to more than one track at highest correlator level


  // Operations ----------------------
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _mvdArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;

  PndTpcDigiPar* fpar;

  std::map<unsigned int, std::vector<PndTpcCluster*>*> fbuffermap;
  std::vector<PndTpcCluster*>* fcluster_buffer;
  std::vector<PndTpcRiemannTrack*> friemannlist;

  PndTpcRiemannTrackFinder* _trackfinder;
  PndTpcRiemannTrackFinder* _trackfinderCurl;

  Bool_t _persistence;

  unsigned int fnsectors;
  double _maxRadius; // outer radius of padplane

  int counter;

  // tuning parameters for Conformal Map TrackFinder
  bool _sortingMode;
  int _sorting;
  double _interactionZ;
  
  double _riemannscale;

  unsigned int _minpoints;
  double _proxcut;
  double _proxZstretch;
  double _helixcut;

  bool _mergeTracks;
  double _TTproxcut;
  double _TTdipcut;
  double _TThelixcut;
  double _TTplanecut;

  bool _MergeCurlers;
  double _blowUp;

  bool _skipCrossingAreas;

  // parameters for multistep approach
  bool _doMultistep;
  unsigned int _minHitsZ;
  unsigned int _minHitsPhi;

  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannTrackingTask,3)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
