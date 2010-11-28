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
  void SetDatafile(const TString& name) {fFile=name;}
  void SetCutSmallPad(Bool_t opt=kTRUE) {_cutsmallpad=opt;}  
  void SetCutBigPad(Bool_t opt=kTRUE) {_cutbigpad=opt;}
  void SetMinNbSamples(unsigned int nb) {fCutoff=nb;}
  //void SetMinDigis(unsigned int md) {fMinDigis=md;}
  //void SetMaxSample(unsigned int md) {fMaxSample=md;}
  //void SetOccMax(unsigned int md) {fCutOcc=md;}
  //void SetNbChip(unsigned int nb) {fNbChip=nb;}
  //void SetOnly800(Bool_t nb=kTRUE) {fOnly800=nb;}

 // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

 private:
  
  TString _digiBranchName;
  TString fFile;
  
  //std::vector<PndTpcSample*>* _di; //!

  TChain* fIntree;  //external data input
  PndTpcEvent* fEv; 
  
  //unsigned int fMinDigis;

  //PndTpcPadPlane* fPadplane;
  PndTpcDigiPar* fPar;
  
  unsigned int fLoop;
  unsigned int fEventNb;
  unsigned int fSpillNb;
  
  //unsigned int fMaxSample;
  //unsigned int fCutOcc;
  //unsigned int fNbChip;

  TClonesArray* _sampleOutArray;
  
  Bool_t _persistence;
  unsigned int fCutoff;
  
  Bool_t _cutsmallpad;  
  Bool_t _cutbigpad;
  //Bool_t fOnly800;


  // Private Data Members ------------

public:
  ClassDef(PndTpcDataReaderTask,2)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
