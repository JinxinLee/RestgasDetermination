//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      GFKalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDHYPDKALMANTASK_HH
#define PNDHYPDKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TH2D;
class TGeoManager; 

class PndHypDKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndHypDKalmanTask();
  ~PndHypDKalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){_hitBranchMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetSmooth(Bool_t opt=kTRUE){_smooth=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _trackBranchName;
  std::map<unsigned int,TString> _hitBranchMap;
  TClonesArray* _trackArray;
  
  Bool_t _persistence;

  GFRecoHitFactory* _theRecoHitFactory;
  

  TH1D* _pH; // momentum histo;
  TH1D* _chi2H; // chi2 histo;
  TH1D* _xresH;
  TH1D* _yresH;
  TH1D* _xresFitH;
  TH1D* _yresFitH;
  TH2D* _pEnd;

  int _trackcount;
  Bool_t _smooth;
  int _evt; // event counter

  // Private Methods -----------------

public:
  ClassDef(PndHypDKalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
