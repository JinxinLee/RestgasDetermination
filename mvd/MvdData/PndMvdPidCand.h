//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track candidate. Basically a list of hit indices.
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    	TUM     (original author)
//	Tobias Baldauf		TUD	(adapted for MVD Pid)
//
//-----------------------------------------------------------

#ifndef PNDMVDPIDCAND_HH
#define PNDMVDPIDCAND_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include <map>

// Collaborating Class Declarations --

class PndMvdPidCand : public TObject {
public:

  // Constructors/Destructors ---------
  PndMvdPidCand() {
    fmvdhits=0;
  }

  // CopyConstructor
  PndMvdPidCand(PndMvdPidCand& point) { *this = point; };

  // operators 

  // Setting -----------------------
  void SetLikelihood(int lundId, double likelihood);
  void AddMvdHit(double dE, double dx, double p);

  // Getting -----------------------
  double GetMvdHitdE(int mvdhit);
  double GetMvdHitdx(int mvdhit);
  double GetMvdHitMomentum(int mvdhit);
  int GetMvdHits();
  double GetLikelihood(int lundId);

private:

  // Private Data Members ------------
  int fmvdhits;
  std::vector<double> fdE;
  std::vector<double> fdx;
  std::vector<double> fmomentum;
  std::map<int, double> flikelihood;
	
public:
  ClassDef(PndMvdPidCand,1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
