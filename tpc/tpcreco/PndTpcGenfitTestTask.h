//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      GENFIT-based dEdx
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Christian Hoeppner   TUM
//
//
//-----------------------------------------------------------

#ifndef GENFITTESTTASK_HH
#define GENFITTESTTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include"TTree.h"
#include"TFile.h"
#include"TMatrixT.h"
#include"FairGeanePro.h"
// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class TH1D;
class PndTpcDigiPar;

class PndTpcGenfitTestTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcGenfitTestTask();
  ~PndTpcGenfitTestTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  

  // Operations ----------------------
  
  virtual InitStatus Init();
  
  virtual void SetParContainers();

  virtual void Exec(Option_t* opt);
  
  void WriteTree();

private:

  // Private Data Members ------------
  TClonesArray* _pointArray;


  TTree *outtree;
  TFile *outfile;

  TMatrixT<double> *stMCT;
  TMatrixT<double> *covMCT;
  TMatrixT<double> *stREC;
  TMatrixT<double> *covREC;
  double chi2;
  int ndf;
  int nfail;
  
  FairGeanePro* _geanePro;

  // Private Methods -----------------

public:
  ClassDef(PndTpcGenfitTestTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
