//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correlate fitted TPC track with MVD hits 
//      and append to Track 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCMVDCORRELATORTASK_HH
#define TPCMVDCORRELATORTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"
#include "PndTpcCluster.h"
#include "FairGeanePro.h"

#include "GFDetPlane.h"

#include <vector>
#include <map>

class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TGraph;


class PndTpcMVDCorrelatorTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcMVDCorrelatorTask();
  virtual ~PndTpcMVDCorrelatorTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetOutTrackBranchName(const TString& name) {fOutTrackBranchName=name;}
  void SetPixelBranchName(const TString& name) {fPixelBranchName=name;}
  void SetStripBranchName(const TString& name) {fStripBranchName=name;}
  void SetClusterBranchNamer(const TString& name) {fClusterBranchName=name;}
  
  void SetMatchDistance(double d) {fMatchDistance=d;}
  void SetAngleCut(double a){fAngleCut=a;}
  void SetMinMVDHits(unsigned int n) {fMinMVDHits = n;}
  //void SetScanTargetCut(bool opt=true) {fScan=opt;}
  void SetScanStepping(unsigned int nSteps, double min, double max);
  void SetNExpectedPhysicsTracks(unsigned int n) {fNPhys=n;}  //only needed for scanning
  void SetMVDTimeWindow(double tw, double t0=0.) {fWindow=tw; ft0 = t0;}  //ns; MVD "mixing" timeframe
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& fname);
  
  
 private:

  double fMatchDistance;   // lookup distance in cm
  double fAngleCut; // only use hits within this angular range around the track position
  
  // Private Data Members ------------
  TString fTrackBranchName;
  TString fOutTrackBranchName;
  TString fPixelBranchName;
  TString fStripBranchName;
  TString fClusterBranchName;
  
  bool fPersistence;
  bool fMergeHits;    //merge found MVD hits into output track 
  bool fScan;         //check background purities as a function of the roawidth

  unsigned int fScanSteps;
  unsigned int fMinMVDHits;
  unsigned int fNPhys;   //number of physics tracks (>=) required to fill purity data
  unsigned int fTot;
    
  
  TClonesArray* fTrackArray;
  TClonesArray* fOutTrackArray;
  TClonesArray* fPixelArray;
  TClonesArray* fStripArray;
  TClonesArray* fClusterArray;

  GFRecoHitFactory* fTheRecoHitFactory;

  TH1D* fResHistU;
  TH1D* fResHistV;
  TH1D* fResHistUBkg;
  TH1D* fResHistVBkg;
  TH1D* fTotHist;
  TGraph* fPurityGraph;
  TGraph* fEffGraph;



  std::map<unsigned int, std::vector<double> > fGlobalPurities; //<roadwidth stepping, list of purities>
  std::map<unsigned int, unsigned int> fEffMap;   //<roadwidth, number of corr. found tracks>
  double fWindow;
  double ft0;
  double fScanMin;
  double fScanMax;
  double fInterval;
    

  ClassDef(PndTpcMVDCorrelatorTask,2);
  
};

class DetPlaneWrapper {
 public:
  DetPlaneWrapper(const GFDetPlane&);
  
  const GFDetPlane& getPlane() const {return fPl;}
  
  friend bool operator== (const DetPlaneWrapper& lhs, const DetPlaneWrapper& rhs);
  friend bool operator< (const DetPlaneWrapper& lhs, const DetPlaneWrapper& rhs);
 

 private:
  GFDetPlane fPl;  
};


#endif

