//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Simple event generator for replacing dummy MC events
//      Current application: Iron55 calibration.
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCSIMPLEEVTGEN_H
#define TPCSIMPLEEVTGEN_H

// Base Class Headers ---------------------------------------
#include "FairTask.h"

// External Headers -----------------------------------------
#include <string>
#include "TVector3.h"



class TClonesArray;
class PndTpcDigiPar;

class PndTpcSimpleEvtGen : public FairTask {

 public:

// Constructors ---------------------------------------------
  PndTpcSimpleEvtGen();
  ~PndTpcSimpleEvtGen();

// ----------------------------------------------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void SetParContainers();

// Accessors ------------------------------------------------

// Modifiers ------------------------------------------------

  void setMode(std::string mode) {fmode = mode;}
  void setPos(TVector3 pos) {fpos = pos;}
  

  
 private:

  TClonesArray* fprimArray;
  std::string fmode;
  TVector3 fpos;

  PndTpcDigiPar* fpar;  
    
  
public:
  ClassDef(PndTpcSimpleEvtGen,1)

};

#endif




