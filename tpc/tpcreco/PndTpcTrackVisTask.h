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
#include "GFTrack.h"

#include "TApplication.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class PndTpcTrackVisTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcTrackVisTask();
  virtual ~PndTpcTrackVisTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void drawFits(bool flag=true){_fitFlag=flag;}

private:

  void drawHitsAndTracks(std::vector<GFTrack*> tracks);
  void drawLSLHitsAndTracks(std::vector<GFTrack*> tracks);
  void doDraw(std::vector<GFTrack*> tracks);

  // Private Data Members ------------
  //TApplication app;
  TString _trackBranchName;
  TClonesArray* _trackArray;
  TClonesArray* _clusterArray;

  Bool_t _persistence;
  TGeoManager* _geom;

  bool _fitFlag;

  // Private Methods -----------------

public:
  ClassDef(PndTpcTrackVisTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
