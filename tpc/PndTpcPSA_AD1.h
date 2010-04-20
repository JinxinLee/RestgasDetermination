//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Digitization
//      Implementation of a concrete pulse shape analyzer using
//      CF signal detection and tail cancellation
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPSA_AD1_HH
#define TPCPSA_AD1_HH

// Base Class Headers ----------------
#include "PndTpcAbsPSAStrategy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include "McIdCollection.h"
#include "PndTpcAbsPulseshape.h"

// Collaborating Class Declarations --



class PndTpcPSA_AD1 : public PndTpcAbsPSAStrategy {
public:

  // Constructors/Destructors ---------
  PndTpcPSA_AD1();//const PndTpcAbsPulseshape* pulseshape);
  virtual ~PndTpcPSA_AD1(){;}

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual PndTpcDigi* ProcessNext(const PndTpcSample* sample);//dummy

  virtual void Process(const std::vector<PndTpcSample*> & samples, 
		       std::vector<PndTpcDigi*>& digis,
		       double padThreshold);

  void TailCancellation(bool opt=kTRUE) {fTAIL=opt;}
  void setPs(PndTpcAbsPulseshape* pulseshape) {fpulseShape=pulseshape;}
  void setFreq(int f){f=fFreq;}

private:

  // Private Data Members ------------
  unsigned int fcurrentPadID;
  bool finprogress;
  unsigned int famp;
  unsigned int ft;
  unsigned int fthreshold;

  PndTpcAbsPulseshape* fpulseShape;

  int fdtc;      //digit is characterized by time
  double fdtp; //phase
  double fdamp;//amplitude
  int fN;
  int fLastPulse;
  int fFreq;
  
  bool NEWPULSE;
  bool fTAIL;//tail cancellation if true

  McIdCollection fmcid;

  void tailSuppr(int i ,std::vector<PndTpcSample*> & samples);
  void saveSig(std::vector<PndTpcSample*> & samples,std::vector<PndTpcDigi*>& digis);
  void detectSigCF(int i ,std::vector<PndTpcSample*> & samples);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
