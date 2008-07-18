//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class KalmanTester
//      see KalmanTester.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "KalmanTester.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "Kalman.h"
#include "TMatrixT.h"
#include "boost/test/test_tools.hpp"
#include "boost/test/floating_point_comparison.hpp"
#include "SltHit.h"
#include "SLTrackRep.h"
#include "Track.h"

// Class Member definitions -----------


void
KalmanTester::test_gain(){
  Kalman k;
  TMatrixT<double> cov(2,2);
  cov[0][0]=1;
  cov[1][1]=2;
  TMatrixT<double> hitcov(2,2);
  hitcov[0][0]=3;
  hitcov[1][1]=3;
  TMatrixT<double> H(2,2);
  H[0][0]=1;
  H[1][1]=1;
  TMatrixT<double> Gain=k.gain(cov,hitcov,H);
  Gain.Print();
  BOOST_CHECK_EQUAL(Gain[0][0],0.25);
  BOOST_CHECK_EQUAL(Gain[1][1],0.4);
}

void
KalmanTester::test_processHit(){
  Kalman K;
  // simplest hit (1D hit y(z) z free parameter s=z)
  SltHit hit(3.01,2,0.1);
  SLTrackRep rep; // y=az+b
  TMatrixT<double> initial(2,1);
  initial[0][0]=1.0;
  initial[1][0]=1.0;
  rep.setState(initial);
  TMatrixT<double> initialCov(2,2);
  initialCov[0][0]=1.1;
  initialCov[0][1]=-0.;
  initialCov[1][0]=-0.;
  initialCov[1][1]=1.1;
  rep.setCov(initialCov);
  K.processHit(&hit,&rep);
  rep.Print();
  SltHit hit2(4.0,3,0.1);
  K.processHit(&hit2,&rep);
  rep.Print();
  // check if inside 1 sigma
  double a=rep.getState()[0][0];
  double b=rep.getState()[1][0];
  BOOST_CHECK_CLOSE(a,1.,TMath::Sqrt(rep.getCov()[0][0])*100/a);
  BOOST_CHECK_CLOSE(b,1.,TMath::Sqrt(rep.getCov()[1][1])*100/b);
}



void
KalmanTester::test_processTrack(){
  Kalman K;
  // setup track
  SLTrackRep rep(1.,1.,0.2,0.2);
  Track trk(&rep);
  SltHit hit(3.01,2,0.1);
  SltHit hit2(4.0,3,0.1);
  trk.addHit(&hit);
  trk.addHit(&hit2);
  K.processTrack(&trk);
  rep.Print();
  double a=rep.getState()[0][0];
  double b=rep.getState()[1][0];
  BOOST_CHECK_CLOSE(a,1.,TMath::Sqrt(rep.getCov()[0][0])*100/a);
  BOOST_CHECK_CLOSE(b,1.,TMath::Sqrt(rep.getCov()[1][1])*100/b);
}
