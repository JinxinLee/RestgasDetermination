//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      example task for the usage of the genfit tracking farmework
//      inside FAIRRoot
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TRACKVISTASK_HH
#define TRACKVISTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "Track.h"

#include "TApplication.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class TrackVisTask : public FairTask {
public:

  // Constructors/Destructors ---------
  TrackVisTask();
  virtual ~TrackVisTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}

private:

  void drawHitsAndTracks(std::vector<Track*> tracks);
  void drawLSLHitsAndTracks(std::vector<Track*> tracks);
  void doDraw(std::vector<Track*> tracks);

  // Private Data Members ------------
  //TApplication app;
  TString _trackBranchName;
  TClonesArray* _trackArray;
  

  Bool_t _persistence;
  TGeoManager* _geom;

  // Private Methods -----------------

public:
  ClassDef(TrackVisTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
