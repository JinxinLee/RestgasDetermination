//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Performs Eventdeconvoluiton of Mixed TPC data  
//      based on 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTDECONVTASK_HH
#define TPCEVTDECONVTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <map>
#include <vector>

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"



class TClonesArray;
class TH1D;
class TH1I;

class PndTpcEvtDeconvTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtDeconvTask();
  virtual ~PndTpcEvtDeconvTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetOutTrackBranchName(const TString& name) {fOutTrackBranchName=name;}
  
    
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void SetCuts(Double_t rcut, Double_t zcut, Double_t endcapcut){fRCut=rcut;fZCut=zcut;fendcapcut=endcapcut;}
  void SetNExpectedTracks(unsigned n){fNExpectedTracks=n;}
  void SetZCutBinning(unsigned int n, double step){fnz=n;fdz=step;}

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

  
  
  
 private:

   // Private Data Members ------------
  TString fTrackBranchName;
  TString fOutTrackBranchName;
   
  Bool_t fPersistence;
  Double_t fRCut;
  Double_t fZCut;
  Double_t fendcapcut;

  TClonesArray* fTrackArray;
  TClonesArray* fOutTrackArray;

  // number of tracks /full events kept for different z-cuts
  std::vector<double> fZCuts; // values of zcuts
  std::vector<unsigned int> fNSingleTrackPhys;
  std::vector<unsigned int> fNSingleTrackBkg;
  std::vector<unsigned int> fNFullEvent;
  // prepare lists of tracks surviving different z cuts
  std::vector<std::vector<unsigned int>* > fsurvivormap; // key: z-cut value: survivors
  unsigned int fnz; // number of different values for zcut
  double fdz; // distance between zcut values
  unsigned int fNExpectedTracks;
  unsigned int fevtcounter;
  
  TH1I* hRetained;
  TH1I* hFoundPhysics;
  TH1I* hFoundIDs;
  TH1D* hPocaZ;
  TH1D* hPocaR;

  ClassDef(PndTpcEvtDeconvTask,1);
  
};

#endif

