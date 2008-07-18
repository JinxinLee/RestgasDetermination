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
#include "CbmTask.h"
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

class PndTpcPadResponseTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcPadResponseTask();
  ~PndTpcPadResponseTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetAvalancheBranchName(const TString& name) {_avalancheBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
   void SelectSector(unsigned int id){_secids[id]=true;}
   void SetQAPlotCol(QAPlotCollection* col){_qa=col;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
  
  void FillHistograms(double, double, double, double);
  void WriteHistograms();


private:

  // Private Data Members ------------
  TString _avalancheBranchName;
  TClonesArray* _avalancheArray;
  TClonesArray* _signalArray;
  
  Bool_t _persistence;
  Double_t _minSignalAmp;
  Double_t _rmin, _rmax;

  const PndTpcPadPlane* _padPlane;
  const PndTpcGem* _gem;
  
  PndTpcDigiPar* _par;  

  Int_t _eventcounter;

  TH1D* _xVariation;
  TH1D* _yVariation;
  TH2D* _2DHisto;

  std::map<unsigned int,bool> _secids;
  bool _selected; // true if only selected sectors are processed

  Bool_t _initialized;
  QAPlotCollection* _qa;
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcPadResponseTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
