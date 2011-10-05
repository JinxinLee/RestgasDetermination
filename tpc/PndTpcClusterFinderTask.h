//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc PSA: Pulse shape analysis
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERFINDERTASK_HH
#define TPCCLUSTERFINDERTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "TH2D.h"

// Collaborating Class Headers -------
#include "PndTpcCluster.h"

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsClusterFinder;
class PndTpcDigiPar;
class PndTpcPadPlane;
class PndTpcGem;
class PndTpcFrontend;
class PndTpcGas;

class PndTpcClusterFinderTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcClusterFinderTask();
  ~PndTpcClusterFinderTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetDigiBranchName(const TString& name) {fdigiBranchName=name;}
  void SetClusterBranchName(const TString& name) {fClusterOutName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetDigiPersistence(Bool_t opt=kTRUE) {fDigiPersistence=opt;} // store references to digis in clusters
  void SetSamplePesistence(Bool_t opt=kTRUE) {fSamplePersistence=opt;} // store references to samples in digis
  void SetTrivialClustering(Bool_t opt=kTRUE) {ftrivial=opt;}
  void SetSimpleClustering(Bool_t opt=kTRUE, Bool_t sectorize=kTRUE);
  
  void timeslice(unsigned int t){ftimeslice=t;}
  void SetThreshold(unsigned int t){fthres=t;}
  void SetSingleDigiClusterAmpCut(unsigned int cut){fSDiClAmpCut=cut;}
  void SetClusterAmpCut(double cut){fClAmpCut=cut;}
  void SetErrorPars(double ADCsens,double C){fAdcSens=ADCsens;fC=C;}

  void SetMode(int mode){fmode=mode;}
  void SetDiffFactor(double d){fDiffFactor=d;} // for Cluster splitting
  void SetClusterTimeCut(double d){fClusterTimeCut=d;} // for Cluster splitting
  void SetDataMode(Bool_t opt) {fDataMode=opt;}



  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString fdigiBranchName;
  TString fClusterOutName;
  TClonesArray* fclusterArray;
  TClonesArray* fdigiArray;
  
  Bool_t fpersistence;
  Bool_t fDigiPersistence;
  Bool_t fSamplePersistence;
  Bool_t ftrivial;          // one cluster from each digi
  Bool_t fsimple;           // use PndTpcClusterFinderSimple
  Bool_t fsectorize;        // sectorize simple clustering
  Bool_t fDataMode;         // for PndTpcClusterFinder
 
  int fmode;                 // controls method of sorting into time bins for PndTpcClusterFinder
  unsigned int ftimeslice;   // defines time bucket / gap
  unsigned int fthres;       // amplitude cut on total Cluster amplitude
  unsigned int fSDiClAmpCut; // amplitude cut on Single Digi Cluster amplitude
  double fClAmpCut;          // amplitude cut on mean digi amplitude
  double fDiffFactor;        // defines maximum slope for cluster splitting in XY for PndTpcClusterFinder
  double fClusterTimeCut;    // defines time cut for cluster splitting for PndTpcClusterFinder

  double fAdcSens;     // electrons per adc channel
  double fC;           // scale for error calculation 

  const PndTpcFrontend* ffrontend;
  PndTpcPadPlane* fpadplane;
  const PndTpcGem* fgem;
  const PndTpcGas* fgas;
  double fzGem;
  PndTpcAbsClusterFinder* ffinder;

  PndTpcDigiPar* fpar;
 
  std::vector<PndTpcCluster*>* fcluster_buffer;

  TH2D* nDigivsTime;

  // Private Methods -----------------

public:
  ClassDef(PndTpcClusterFinderTask,1)

};

bool PndTpcDigiIndex(PndTpcDigi* digi1, PndTpcDigi* digi2);

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
