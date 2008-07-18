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
#include "CbmTask.h"

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

class PndTpcElectronicsTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcElectronicsTask();
  ~PndTpcElectronicsTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetSignalBranchName(const TString& name) {_signalBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) 	{_persistence=opt;}
  void SetSamplePersistence(Bool_t opt=kTRUE)	{_samplePersistence=opt;}
  void SetQAPlotCol(QAPlotCollection* col){_qa=col;}
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

  void WriteHistograms();
  
private:

  // Private Data Members ------------
  TString _signalBranchName;
  TClonesArray* _signalArray;
  TClonesArray* _sampleArray;
  TClonesArray* _digiArray;
  
  Bool_t _persistence;
  Bool_t _samplePersistence;
  const PndTpcFrontend* _frontend;
  PndTpcAbsPulseshape* _pulseshape;
  PndTpcAbsPSAStrategy* _psa;

  PndTpcDigiPar* _par;

  //! QA Histograms
  QAPlotCollection* _qa;
  
  //! Sample amplitudes
  TH1D* _sampleAmpH;
  TH1D* _meanDigiTH;
  //! number samples recorded per number of signals
  TH1D* _nSampRelSig;
  
  Bool_t _initialized; 
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
