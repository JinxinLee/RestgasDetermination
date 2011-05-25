//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pattern recognition = track finding in the TPC
//      Using a riemann track follower
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPATTERNRECOTASK_HH
#define TPCPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include <map>


// Collaborating Class Declarations --
#include "PndTpcCluster.h"
class TClonesArray;
class PndTpcFrontend;
class PndTpcRiemannTrackFinder;
class PndTpcRiemannTrack;
class PndTpcDigiPar;
class TH1I;
class TH1D;
class TGraph;

class PndTpcPatternRecoTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcPatternRecoTask();
  ~PndTpcPatternRecoTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetTrkFinderParameters(double RiemannScale,
			      double proxcut, double riproxcut, double szcut,
			      double planecut,
			      double TTproxcut, 
			      double TTplanecut, 
			      double TTszcut,
			      unsigned int minpointsforfit,
			      unsigned int maxpointsforPR);
  void SetTrkFinderOptions(bool dosorting, int sortingmode,
			   bool doClean, bool doMerge){
    _sorting=dosorting; _sortingmode=sortingmode; 
    _doClean=doClean; _doMerge=doMerge;}

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
   virtual void SetParContainers();

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;

  std::map<unsigned int, std::vector<PndTpcCluster*>*> fbuffermap;
  std::vector<PndTpcCluster*>* fcluster_buffer;
  std::vector<PndTpcRiemannTrack*> friemannlist;
  unsigned int fnsectors;

  Bool_t _persistence;

  double _proxcut;
  double _riproxcut;
  double _szcut;
  double _planecut;
  double _TTproxcut;
  double _TTplanecut;
  double _TTszcut;

  unsigned int _minpoints; // min points for fit
  unsigned int _maxpoints; // max points in pr

  bool _sorting;
  int _sortingmode;
  bool _doMerge;
  bool _doClean;
  double fRiemannScale;

  TH1I* _multiplicityHisto;
  TH1I* _trackSizeH;
  TH1D* _trackPurityH;
  TGraph* _gpurity;
  unsigned int _nbins;

  PndTpcRiemannTrackFinder* _trackfinder;
  PndTpcDigiPar* fpar;

   // Private Methods -----------------

public:
  ClassDef(PndTpcPatternRecoTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
