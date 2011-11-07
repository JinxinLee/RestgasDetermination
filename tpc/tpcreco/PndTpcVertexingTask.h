//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Vertexing with GFRave
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Johannes Rauch    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCVERTEXINGTASK_HH
#define TPCVERTEXINGTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "GFTrack.h"
class GFRaveVertexFactory;
class GFRaveVertex;
class TClonesArray;


class PndTpcVertexingTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcVertexingTask();
  ~PndTpcVertexingTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName = name;}

  void SetPersistence(Bool_t opt = kTRUE) {_persistence = opt;}

  void SetUseVacuumPropagator(bool opt = true) {fUseVacuumPropagator = opt;} // use the rave::Vacuum propagator instead of GFRavePropagator

  void SetBeamspot(const TVector3 & pos, const TMatrixT<double> & cov3x3); // also sets fUseBeamspot true
  void SetBeamspot(const TVector3 & pos, double err); // also sets fUseBeamspot true
  void SetMethod(const std::string & method) {fMethod = method;}

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  Bool_t _persistence;

  TString _trackBranchName;
  TClonesArray* _trackArray;
  TClonesArray* _vertexArray;

  GFRaveVertexFactory* fVertexFactory;
  std::vector < GFRaveVertex* > * fVertexBuffer;

  bool fUseVacuumPropagator; // use the rave::Vacuum propagator instead of GFRavePropagator
  bool fUseBeamspot;
  TVector3 fBeamPos;
  TMatrixT<double> fBeamCov;
  std::string fMethod; // vertex reconstruction method


  // Private Methods -----------------

public:
  ClassDef(PndTpcVertexingTask,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
