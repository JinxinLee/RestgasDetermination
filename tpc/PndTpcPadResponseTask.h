//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc PadResponse reads in MCPoints and produces primary electrons
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

#ifndef TPCPADRESPONSETASK_HH
#define TPCPADRESPONSETASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "TH1.h"
#include "TH2.h"

// Collaborating Class Headers -------
#include <map>

// Collaborating Class Declarations --
class TClonesArray;
class PndTpcPadPlane;
class PndTpcGem;
class PndTpcDigiPar;
class QAPlotCollection;

class PndTpcPadResponseTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcPadResponseTask();
  ~PndTpcPadResponseTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetAvalancheBranchName(const TString& name) {favalancheBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
   void SelectSector(unsigned int id){fsecids[id]=true;}
   void SetQAPlotCol(QAPlotCollection* col){fqa=col;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
  
  void FillHistograms(double, double, double, double);
  void WriteHistograms();

private:

  // Private Data Members ------------
  TString favalancheBranchName;
  TClonesArray* favalancheArray;
  TClonesArray* fsignalArray;

  Bool_t fpersistence;
  Double_t fminSignalAmp;
  Double_t frmin, frmax;

  const PndTpcPadPlane* fpadPlane;
  const PndTpcGem* fgem;
  
  PndTpcDigiPar* fpar;  

  Int_t feventcounter;

  TH1D* fxVariation;
  TH1D* fyVariation;
  TH2D* f2DHisto;

  std::map<unsigned int,bool> fsecids;
  bool fselected; // true if only selected sectors are processed

  Bool_t finitialized;
  QAPlotCollection* fqa;
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcPadResponseTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
