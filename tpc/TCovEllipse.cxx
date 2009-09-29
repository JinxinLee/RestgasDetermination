//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TCovEllipse
//      see TCovEllipse.hh for details
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
#include "TCovEllipse.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "TMath.h"

// Class Member definitions -----------
ClassImp(TCovEllipse)

TCovEllipse::TCovEllipse()
  : TEllipse(), fsig(1)
{
  SetFillStyle(0);
}

TCovEllipse::TCovEllipse(const TMatrixT<double>& cov,
			 double x0, double y0,
			 int i, int j)
  : TEllipse(),fcov(cov), fx0(x0), fy0(y0), fi(i), fj(j)
{
  recalc(); 
  SetFillStyle(0);
}

void
TCovEllipse::setCov(const TMatrixT<double>& cov)
{
  fcov=cov;
  recalc();
}

void
TCovEllipse::setMean(double x, double y)
{
  SetX1(x);
  SetY1(y);
}

void
TCovEllipse::recalc()
{
  double sig1=sqrt(fcov[fi][fi]);
  double sig2=sqrt(fcov[fj][fj]);
  double rho=fcov[fi][fj]/(sig1*sig2);
  double theta=0;
  if(fcov[fi][fi]!=fcov[fj][fj]){
    double term=2.*rho*sig1*sig2/(fcov[fi][fi]-fcov[fj][fj]);
    theta=TMath::ATan(term)*0.5;
  }
  //std::cout<<theta<<std::endl;
  double num=fcov[fi][fi]*fcov[fj][fj]*(1-rho*rho);
  double mixed=2*rho*sig1*sig2*sin(theta)*cos(theta);
  double sin2=sin(theta)*sin(theta);
  double cos2=cos(theta)*cos(theta);
  double p12_=fcov[fj][fj]*cos2-mixed+fcov[fi][fi]*sin2;
  double p22_=fcov[fj][fj]*sin2+mixed+fcov[fi][fi]*cos2; 
  double p1=sqrt(num/p12_);
  double p2=sqrt(num/p22_);
  SetTheta(theta*57.2958);
  SetR1(p1);
  SetR2(p2);
  SetX1(fx0);
  SetY1(fy0);
}
