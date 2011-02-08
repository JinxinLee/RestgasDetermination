//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      
//      Implementation for crosstalk suppression for the test chamber
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCTCCROSSTALKTASK_HH
#define TPCTCCROSSTALKTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "PndTpcPadPlane.h"
#include "PndTpcDigiPar.h"

#include "PndTpcTCcrossTalk.h"

// Collaborating Class Headers -------
#include <ostream>

// Collaborating Class Declarations --
#include "PndTpcSample.h"

class TClonesArray;
class PndTpcPadPlane;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcTCcrossTalkTask : public FairTask {
 public:

  // Constructors/Destructors ---------

  PndTpcTCcrossTalkTask();
  ~PndTpcTCcrossTalkTask();

  //operator

  void SetSampleBranchName(const TString& name) {_sampleBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetQAPlotCol(QAPlotCollection* col){fqa=col;}
  void SetCalc(Bool_t opt=kTRUE) {_calc=opt;}
  void SetDigi();
  void SetMode(int opt) {_mode=opt;} // 0 is proportional cut
                                     // 1 is vote
                                     // 2 is dummy (time length)

 // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

 private:
  TString _sampleBranchName;


  std::vector<PndTpcSample*>* _di;//!

  PndTpcPadPlane* fpadplane;
  PndTpcDigiPar* fpar;

  int loop;

  TClonesArray* _sampleOutArray;  
  TClonesArray* _sampleInArray;

  Bool_t _persistence;
  Bool_t _calc;
  int _mode;

  PndTpcTCcrossTalk * TCcross;
  QAPlotCollection* fqa;


  // Private Data Members ------------

public:
  ClassDef(PndTpcTCcrossTalkTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
