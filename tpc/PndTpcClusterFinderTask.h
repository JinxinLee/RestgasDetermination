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
  void SetTrivialClustering(Bool_t opt=kTRUE) {ftrivial=opt;}
  void SetSimpleClustering(Bool_t opt=kTRUE) {fsimple=opt;}
  void SetMode(int mode){fmode=mode;}
  
  void timeslice(unsigned int t){ftimeslice=t;}
  void SetThreshold(unsigned int t){fthres=t;}
  void SetDiffFactor(double d){fDiffFactor=d;} // for Cluster splitting
  void SetErrorPars(double ADCsens,double C){fAdcSens=ADCsens;fC=C;} 
  void SetDataMode(Bool_t opt) {fDataMode=opt;}
  void SetSingleDigiClusterAmpCut(unsigned int cut){fSDiClAmpCut=cut;}


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
  Bool_t ftrivial;
  Bool_t fsimple;
  Bool_t fDataMode;   
 
  int fmode;
  unsigned int ftimeslice;
  unsigned int fthres;
  unsigned int fSDiClAmpCut;
  double fDiffFactor;
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
  // Private Methods -----------------

public:
  ClassDef(PndTpcClusterFinderTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
