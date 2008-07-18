//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndMvdPidCand
//      see PndMvdPidCand.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM        (original author)
//	Tobias Baldauf	     TUD	(adapted for MVD Pid)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndMvdPidCand.h"
#include "TObject.h"
#include "TROOT.h"
#include <iostream>
#include "TString.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <vector>
#include "TMath.h"


// Class Member definitions -----------

ClassImp(PndMvdPidCand);

void PndMvdPidCand::SetLikelihood(int lundId, double likelihood) {
  this->flikelihood[lundId]=likelihood;
}

double PndMvdPidCand::GetLikelihood(int lundId) {
  return flikelihood[lundId];
}

double PndMvdPidCand::GetMvdHitdE(int mvdhit) {
  return fdE[mvdhit];
}

double PndMvdPidCand::GetMvdHitdx(int mvdhit) {
  return fdx[mvdhit];
}

double PndMvdPidCand::GetMvdHitMomentum(int mvdhit) {
  return fmomentum[mvdhit];
}

int PndMvdPidCand::GetMvdHits() {
  return fmvdhits;
}

void PndMvdPidCand::AddMvdHit(double dE, double dx, double p) {
  fmomentum.push_back(p);
  fdE.push_back(dE);
  fdx.push_back(dx);
  fmvdhits++;
}

