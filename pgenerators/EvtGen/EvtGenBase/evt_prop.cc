#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_prop.cc,v 1.7 2004/12/21 19:58:53 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Test propagator and vertex calculations

#include <stdio.h>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include "EvtGenBase/EvtTwoBodyVertex.hh"
#include "EvtGenBase/EvtPropBreitWigner.hh"
#include "EvtGenBase/EvtPropBreitWignerRel.hh"

int main(int argc, char* argv[])
{
  // B -> Rho, Pi dalitz plot
  
  double mB = 5.28;
  double mRho = 0.77;
  double m = 0.80;
  double width = 0.15;
  double mPi = 0.14;
  EvtSpinType::spintype spin = EvtSpinType::VECTOR;

  // Decay and birth vertices
  
  EvtTwoBodyVertex vD(mPi,mPi,mRho,spin);
  EvtTwoBodyVertex vB(mRho,mPi,mB,spin);
  EvtTwoBodyKine vD1(mPi,mPi,m);
  EvtTwoBodyKine vB1(m,mPi,mB);

  // Blatt-Weisskopf form factors
  
  vD.set_f(1.5);
  vB.set_f(5.0);
  
  EvtTwoBodyKine::Index A = EvtTwoBodyKine::A;
  EvtTwoBodyKine::Index AB = EvtTwoBodyKine::AB;
  
  // Form factors 
  
  printf("   F(D) = %f\n",vD.formFactor(vD1));
  printf("   F(B) = %f\n",vB.formFactor(vB1));
  
  // Vertex factors
  
  printf("AmpF(D) = %f\n",pow(2.,spin) * vD.phaseSpaceFactor(vD1,AB)*vD.formFactor(vD1));
  printf("WidthF(D) = %f\n",vD.widthFactor(vD1));
  printf("AmpF(B) = %f\n",pow(2.,spin) * vB.phaseSpaceFactor(vB1,A)*vB.formFactor(vB1));
  
  
  // Propagators
    
  EvtPropBreitWigner      bw(mRho,width);
  EvtPropBreitWignerRel  bwR(mRho,width);
  
  EvtComplex c = bw.evaluate(m);
  printf("  BW-NR = (%f,%f)\n",real(c),imag(c));
  c = bwR.evaluate(m);
  printf("  BW-REL = (%f,%f)\n",real(c),imag(c));

  return 0;
}
  
