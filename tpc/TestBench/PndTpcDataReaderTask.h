//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Reading data from the t2k-testchamber
//      Write it in a PndTpcSample TCclones array
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCTCREADER_HH
#define TPCTCREADER_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "TVector3.h"
#include "TChain.h"
#include "PndTpcPadPlane.h"
#include "PndTpcDigiPar.h"
#include "PndTpcSample.h"
#include "PndTpcEvent.h"

// Collaborating Class Headers -------
#include <ostream>
#include <vector>

// Collaborating Class Declarations --

class TClonesArray;
class PndTpcPadPlane;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcDataReaderTask : public FairTask {
 public:

  // Constructors/Destructors ---------

  PndTpcDataReaderTask();
  ~PndTpcDataReaderTask();

  //operator

  void SetClusterBranchName(const TString& name) {_digiBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetDatafile(const TString& name) {_file=name;}
  void SetCutSmallPad(Bool_t opt=kTRUE) {_smallpad=opt;}
  void SetMinDigis(unsigned int md) {fMinDigis=md;}

 // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

 private:
  
  TString _digiBranchName;
  TString _file;
  
  std::vector<PndTpcSample*>* _di; //!

  TChain* fIntree;  //external data input

  PndTpcEvent* fEv; 
  
  unsigned int fMinDigis;

  PndTpcPadPlane* fpadplane;

  PndTpcDigiPar* fpar;

  int loop;

  TClonesArray* _sampleOutArray;
  
  Bool_t _persistence;
  Bool_t _smallpad;


  // Private Data Members ------------

public:
  ClassDef(PndTpcDataReaderTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
