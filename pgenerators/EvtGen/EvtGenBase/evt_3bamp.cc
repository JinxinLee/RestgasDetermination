#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_3bamp.cc,v 1.7 2004/12/21 19:58:51 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Test three body amplitude calculations

#include <stdlib.h>
#include <stdio.h>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include "EvtGenBase/EvtCyclic3.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"
#include "EvtGenBase/EvtPto3PAmp.hh"
#include "EvtGenBase/EvtDalitzPoint.hh"
#include "EvtGenBase/EvtDalitzPlot.hh"
#include "EvtGenBase/EvtDalitzCoord.hh"
#include "EvtGenBase/EvtPropBreitWigner.hh"
#include "EvtGenBase/EvtPropBreitWignerRel.hh"

int main(int argc, char* argv[])
{
  using namespace EvtCyclic3;

  double mA(sqrt(6.)), mB(sqrt(2.)), mC(sqrt(22.)), mD(sqrt(94.));
  double qAB(20.), qBC(48), qCA(56);
  
  EvtDalitzPlot dp(mA,mB,mC,mD);
  EvtDalitzCoord coord(AB,qAB,BC,qBC);
  EvtDalitzPoint x(dp,coord);
  
  double m0 = 6.2;
  double g0 = 1.5;
  EvtSpinType::spintype spin = EvtSpinType::VECTOR;
  
  EvtPropBreitWigner      bw(m0,g0);
  EvtPropBreitWignerRel  bwR(m0,g0);

  // Note that for RBW, the sign is flipped. In the CLEO paper
  // the numerator is: 
  // Reduces to -cos(BC) in the r.f. of AB.
  // However, Zemach amplitude is normally written 
  // in such a way that it is proportional to 
  // cos(BC) in the r.f. of AB.
  
  EvtPto3PAmp amp0(dp,BC,AB,spin,bw,EvtPto3PAmp::NBW);
  EvtComplex c = amp0.evaluate(x);
  printf("(%f,%f)\n",real(c),imag(c));

  EvtPto3PAmp amp1(dp,BC,AB,spin,bwR,EvtPto3PAmp::RBW_CLEO);
  amp1.set_fd(5.0/0.197);
  c = amp1.evaluate(x);
  printf("(%f,%f)\n",real(c),imag(c));
  
  amp1.set_fd(2.0/0.197);
  c = amp1.evaluate(x);
  printf("(%f,%f)\n",real(c),imag(c));
  
  amp1.set_fd(1.5);
  amp1.set_fb(5.0);
  c = amp1.evaluate(x);
  printf("(%f,%f)\n",real(c),imag(c));
  
  return 0;
}






