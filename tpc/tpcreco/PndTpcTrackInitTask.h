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

#ifndef TPCTRACKINITTASK_HH
#define TPCTRACKINITTASK_HH

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
class GFRecoHitFactory;
class TH1I;
class TH1D;
class PndTrackCand;
class PndTrack;
class PndTpcDigiPar;

class PndTpcTrackInitTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcTrackInitTask();
  ~PndTpcTrackInitTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetRiemannBranchName(const TString& name) {_riemannBranchName=name;}
  void SetOutBranchNames(const TString& tracks,
			 const TString& cands,
			 const TString& pndcands) 
  {_trackBranchName=tracks;
    _trackCandBranchName=cands;
    _pndTrackBranchName=pndcands;}

  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  void SetSortingParameters(
                   bool sortingMode=false, // false: sort only according to _sorting; true: use internal sorting when adding hits to trackcands
                   int sorting=3,  // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to interaction point
                   double interactionZ=0); // set if you use sorting = 4

  void SetTrkFinderParameters(
                   double proxcut,
                   double _helixcut,
                   unsigned int minpointsforfit);

  void SetMergeTracks(bool mergeTracks=true){_mergeTracks = mergeTracks;}
  void SetSmoothing(bool s=true) {_smoothing=s;}

  void SetTrkMergerParameters(
                   double TTproxcut,
                   double TTdipcut,
                   double TThelixcut,
                   double TTplanecut);

 
  
 
  // Operations ----------------------
  virtual InitStatus Init();

  virtual void SetParContainers();

  virtual void Exec(Option_t* opt);
  
  void useGeane(Bool_t geane=kTRUE) {_geane=geane;}

  void SetStoreHistograms(TString file);

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TString _riemannBranchName;
  TString _trackBranchName;
  TString _trackCandBranchName;
  TString _pndTrackBranchName;
 


  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;
  TClonesArray* _trackCandArray;
  TClonesArray* _pndTrackArray;

  PndTpcDigiPar* fpar;

 
  Bool_t _persistence;
 
  Bool_t _geane;

  double fMins[4];
  double fMaxs[4];
  double fAmpCut;

  double Bz; //mag field

  int counter;

  // tuning parameters for Conformal Map TrackFinder
  bool _sortingMode;
  
  double _riemannscale;

  int _sorting;
  double _proxcut;
  double _helixcut;
  unsigned int _minpoints;

  bool _mergeTracks;
  bool _smoothing;

  double _TTproxcut;
  double _TTdipcut;
  double _TThelixcut;
  double _TTplanecut;

  double _interactionZ;


  GFRecoHitFactory* _theRecoHitFactory;

  FairGeanePro* gPro;


  // Private Methods -----------------

public:
  ClassDef(PndTpcTrackInitTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
