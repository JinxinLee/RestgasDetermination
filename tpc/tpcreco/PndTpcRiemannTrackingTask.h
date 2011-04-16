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

  void SetSortingParameters(
                   bool sortingMode=false, // false: sort only according to _sorting; true: use internal sorting when adding hits to trackcands
                   int sorting=3,  // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to interaction point
                   double interactionZ=0); // set if you use sorting = 4

  void SetTrkFinderParameters(
                   double proxcut,
                   double riproxcut,
                   double planecut,
                   double szcut,
                   unsigned int minpointsforfit);

  void SetMergeTracks(bool mergeTracks=true){_mergeTracks = mergeTracks;}

  void SetTrkMergerParameters(
                   double TTproxcut,
                   double TTszcut,
                   double TTplanecut);

  void SetRiemannScale(double riemannscale=8.7) {_riemannscale = riemannscale;}

  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void useGeane(Bool_t geane=kTRUE) {_geane=geane;}

  void SetStoreHistograms(TString file);

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _mvdArray;
  TClonesArray* _trackArray;
  TClonesArray* _riemannTrackArray;
  TClonesArray* _riemannHitArray;
  TClonesArray* _trackCandArray;
  TClonesArray* _pndTrackArray;


  Bool_t fPersistence;
  Bool_t fStore;
  Bool_t fDebug;



  Bool_t _persistence;
  Bool_t _geane;

  double fMins[4];
  double fMaxs[4];
  double fAmpCut;

  TString fHistoFileName;

  TFile* fHistoFile;

  unsigned fClLimit; //limit nClusters an event may have

  TClonesArray* fClusterArray;
  TClonesArray* fTrackArray;
  TClonesArray* fMonitorArray;

  //PndTpcDigiPar* fPar;
 // TF1* fRep; //Hough space representation
  FairField* fField;

  int counter;
  unsigned int fZStackLimit;
  unsigned int fMomScale;

  std::vector<Color_t> colors;
  std::map<std::string, TH1*> fHistCont;   //container for histograms


  // tuning parameters for Conformal Map TrackFinder
  bool _sortingMode;
  
  double _riemannscale;

  int _sorting;
  double _proxcut;
  double _riproxcut;
  double _planecut;      
  double _szcut;
  unsigned int _minpoints;

  bool _mergeTracks;

  double _TTproxcut;
  double _TTszcut;
  double _TTplanecut;
  double _interactionZ;


  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;
  TH1D* _trackMcIdsH;

  PndTpcRiemannTrackFinder* _trackfinder;

  GFRecoHitFactory* _theRecoHitFactory;


  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannTrackingTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
