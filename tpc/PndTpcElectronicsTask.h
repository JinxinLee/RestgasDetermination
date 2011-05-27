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

#ifndef TPCELECTRONICSTASK_HH
#define TPCELECTRONICSTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcFrontend;
class PndTpcAbsPulseshape;
class PndTpcAbsPSAStrategy;
class PndTpcDigiPar;
class PndTpcSample;
class TH1D;
class QAPlotCollection;

class PndTpcElectronicsTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcElectronicsTask();
  ~PndTpcElectronicsTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {fsignalBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) 	{fpersistence=opt;}
  void SetSamplePersistence(Bool_t opt=kTRUE)	{fsamplePersistence=opt;}
  void SetQAPlotCol(QAPlotCollection* col){fqa=col;}
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

  void WriteHistograms();
  
private:

  unsigned int fCount;  //event counter
  
  // Private Data Members ------------
  TString fsignalBranchName;
  TClonesArray* fsignalArray;
  TClonesArray* fsampleArray;
  TClonesArray* fdigiArray;
  
  Bool_t fpersistence;
  Bool_t fsamplePersistence;
  const PndTpcFrontend* ffrontend;
  PndTpcAbsPulseshape* fpulseshape;
  PndTpcAbsPSAStrategy* fpsa;

  PndTpcDigiPar* fpar;

  //! QA Histograms
  QAPlotCollection* fqa;
  
  //! Sample amplitudes
  TH1D* fsampleAmpH;
  TH1D* fmeanDigiTH;
  //! number samples recorded per number of signals
  TH1D* fnSampRelSig;
  
  Bool_t finitialized; 
  // Private Methods -----------------
  void PresetNullSample(std::vector<PndTpcSample*> *samplelist);
  void StoreSamples(std::vector<PndTpcSample*> *samplelist);

public:
  ClassDef(PndTpcElectronicsTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
