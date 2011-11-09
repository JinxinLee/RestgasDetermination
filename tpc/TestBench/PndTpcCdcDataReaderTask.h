//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Reading data from the T2K-based prototype and FOPI data
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
#include "PndTpcEventIdentifier.h"

// Collaborating Class Headers -------
#include <ostream>
#include <vector>

// Collaborating Class Declarations --

class TClonesArray;
class PndTpcPadPlane;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcCdcDataReaderTask : public FairTask {
 public:

  // Constructors/Destructors ---------

  PndTpcCdcDataReaderTask();
  ~PndTpcCdcDataReaderTask();

  //operator
  void SetSampleBranchName(const TString& name) {fSampleBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetDatafile(const TString& name) {fFile=name;}
  void SetCdcDataFile(const TString& name) {fCdcFile=name;}
  void SetMinNbSamples(unsigned int nb) {fCutoff=nb;}
  void SetNevent(unsigned int nEvent){nEvents=nEvent;}
  void SetRunNr(unsigned int run){runNr=run;}
 // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

 private:
  unsigned int nEvents;
  unsigned int runNr;
  TString fSampleBranchName;
  TString fCdcEvBranchName;
  TString fCdcHitBranchName;
  TString fCdcTrackBranchName;
  TString fRpcTrackBranchName;

  TString fFile;
  TString fCdcFile;
  
  //std::vector<PndTpcSample*>* _di; //!

  TChain* fIntree;  //external data input
  PndTpcEvent* fEv; 
  PndTpcEventIdentifier* fEventId;
  TChain* fCdcIntree;
  
  TClonesArray *fCdcEv;
  TClonesArray *fCdcTrack;
  TClonesArray *fRpcTrack;
  TClonesArray *fCdcHit;
  

  //PndTpcPadPlane* fPadplane;
  PndTpcDigiPar* fPar;
  
  unsigned int fLoop;
  unsigned int fCdcLoop;
  unsigned int fEventNb;
  unsigned int fSpillNb;
  unsigned int fRunNum;
  
  //unsigned int fMaxSample;
  //unsigned int fCutOcc;
  //unsigned int fNbChip;

  TClonesArray* fSampleOutArray;
  TClonesArray* fEventIdOutArray;
  TClonesArray* fCdcEventOutArray;
  TClonesArray* fCdcTrackOutArray;
  TClonesArray* fRpcTrackOutArray;
  TClonesArray* fCdcHitOutArray;
  
  Bool_t _persistence;
  unsigned int fCutoff;
  
   // Private Data Members ------------

public:
  ClassDef(PndTpcCdcDataReaderTask,4)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
