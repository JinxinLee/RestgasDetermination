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
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetRiemannPersistence(Bool_t opt=kTRUE) {_riemannPersistence=opt;}

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

  void SetRiemannScale(double riemannscale=8.7) {_riemannscale = riemannscale;}
  
  void SetMCPid(Bool_t opt=kTRUE) {_mcPid = opt;} // use MC information for particle identification
  void SetPDG(int pdg) {_pdg=pdg;}

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
  TClonesArray* _mcTrackArray;
  TClonesArray* _clusterArray;
  TClonesArray* _mvdArray;
  TClonesArray* _trackArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;
  TClonesArray* _trackCandArray;
  TClonesArray* _pndTrackArray;

  PndTpcDigiPar* fpar;

  std::map<unsigned int, std::vector<PndTpcCluster*>*> fbuffermap;
  std::vector<PndTpcCluster*>* fcluster_buffer;
  std::vector<PndTpcRiemannTrack*> friemannlist;
  unsigned int fnsectors;

  Bool_t _persistence;
  Bool_t _riemannPersistence;
  
  Bool_t _geane;
  
  Bool_t _mcPid;
  int _pdg;

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


  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;
  TH1D* _trackMcIdsH;

  PndTpcRiemannTrackFinder* _trackfinder;

  GFRecoHitFactory* _theRecoHitFactory;

  FairGeanePro* gPro;


  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannTrackingTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
