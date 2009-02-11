//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Monte Carlo Point in the TPC
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

#ifndef TPCPOINT_HH
#define TPCPOINT_HH

// Base Class Headers ----------------
#include "FairMCPoint.h"
#include "TMath.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class PndTpcPoint : public FairMCPoint {
public:

  // Constructors/Destructors ---------
  PndTpcPoint();
  PndTpcPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
             Double_t tof, Double_t length, Double_t eLoss);
  ~PndTpcPoint(){;}

  // Operators
  

  // Accessors -----------------------
  double p() const {return TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);}

  // Modifiers -----------------------


  // Operations ----------------------
  virtual void Print(const Option_t* opt=0) const ;


private:

  // Private Data Members ------------


  // Private Methods -----------------

public:
  ClassDef(PndTpcPoint,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
