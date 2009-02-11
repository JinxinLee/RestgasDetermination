//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Event deconvolution on the basis of TpcMCTracklets
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCMCEVTDECONVTASK_HH
#define TPCMCEVTDECONVTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --
class TClonesArray;
class FairGeanePro;
class GeaneTrackRep;
class TVector3;

class TpcMCEvtDeconvTask : public FairTask {
public:

  // Constructors/Destructors ---------
  TpcMCEvtDeconvTask();
  virtual ~TpcMCEvtDeconvTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackletBranchName(const TString& name) {_trackletBranchName=name;}
  void SetOutBranchName(const TString& name) {_trackletOutBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetVDrift(Double_t v){_vdrift=v;}
  void SetTWindow(Double_t t){_dt=0.5*t;} // Specify Window-Width! 
  void SetDx(Double_t dx){_dx=dx;}
  void SetMinNumMVDHits(Int_t n){_minMVDHits=n;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TString _trackletBranchName;
  TString _trackletOutBranchName;
  TClonesArray* _trackletArray;
  TClonesArray* _trackletOutArray;
  TClonesArray* _mvdArray;
    
  FairGeanePro* _geanePro;

  Bool_t _persistence;
  Double_t _vdrift;
  Double_t _dt; // half-width of time window
  Double_t _dx; // cut on distance to MVD hit
  Int_t _minMVDHits;
  
  // Private Methods -----------------
  // returns number of connected VD hits
  std::vector<double> ConnectMVD(GeaneTrackRep* trk);

public:
  // poca between 2 lines (this should go to some utility package
  void linepoca(const TVector3& x1, const TVector3& d1,
		const TVector3& x2, const TVector3& d2,
		TVector3& poca1, TVector3& poca2);

public:
  ClassDef(TpcMCEvtDeconvTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
