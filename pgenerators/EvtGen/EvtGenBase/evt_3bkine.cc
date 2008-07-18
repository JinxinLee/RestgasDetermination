//-----------------------------------------------------------------------
// File and Version Information: 
//      $Id: evt_3bkine.cc,v 1.7 2004/12/21 19:58:51 ryd Exp $
// 
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations. If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998 Caltech, UCSB
//
// Module creator:
//      Alexei Dvoretskii, Caltech, 2001-2002.
//-----------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"

// Regression tests for two and three body kinematics
// calculations

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "EvtGenBase/EvtDalitzCoord.hh"
#include "EvtGenBase/EvtDalitzPlot.hh"
#include "EvtGenBase/EvtDalitzPoint.hh"
#include "EvtGenBase/EvtCyclic3.hh"

int main(int argc, char* argv[])
{
  using namespace EvtCyclic3;
  
  // A (3.,1.,1.,1.)
  // B (2.,1.,-1.,0.)
  // C (5.,-1.,1.,1.)
  // A+B+C (10.,1.,1.,2.);
  
    double mA(sqrt(6.)), mB(sqrt(2.)), mC(sqrt(22.)), mD(sqrt(94.));
    double qAB(20.), qBC(48), qCA(56);
    
    EvtDalitzPlot dp(mA,mB,mC,mD);
    EvtDalitzCoord coord(AB,qAB,BC,qBC);
    EvtDalitzPoint x(dp,coord);
    
    x.print();
    
    
    // AB rest-frame
    
    printf(" eA(AB) = %f\n",x.e(A,AB));
    printf(" pA(AB) = %f\n",x.p(A,AB));
    printf(" eB(AB) = %f\n",x.e(B,AB));
    printf(" pB(AB) = %f\n",x.p(B,AB));
    printf(" eC(AB) = %f\n",x.e(C,AB));
    printf(" pC(AB) = %f\n",x.p(C,AB));
    printf("qBC(AB) = %f (%f : %f)\n",x.q(BC),x.qMin(BC,AB),x.qMax(BC,AB));
    printf("cBC(AB) = %f\n",x.cosTh(BC,AB));
    printf("qCA(AB) = %f (%f : %f)\n",x.q(CA),x.qMin(CA,AB),x.qMax(CA,AB));
    printf("cCA(AB) = %f\n",x.cosTh(CA,AB));
    
    // BC rest-frame

    printf(" eA(BC) = %f\n",x.e(A,BC));
    printf(" pA(BC) = %f\n",x.p(A,BC));
    printf(" eB(BC) = %f\n",x.e(B,BC));
    printf(" pB(BC) = %f\n",x.p(B,BC));
    printf(" eC(BC) = %f\n",x.e(C,BC));
    printf(" pC(BC) = %f\n",x.p(C,BC));
    printf("qCA(AB) = %f (%f : %f)\n",x.q(CA),x.qMin(CA,BC),x.qMax(CA,BC));
    printf("cCA(BC) = %f\n",x.cosTh(CA,BC));
    printf("qCA(AB) = %f (%f : %f)\n",x.q(AB),x.qMin(AB,BC),x.qMax(AB,BC));
    printf("cAB(BC) = %f\n",x.cosTh(AB,BC));

    // CA rest-frame

    printf(" eA(CA) = %f\n",x.e(A,CA));
    printf(" pA(CA) = %f\n",x.p(A,CA));
    printf(" eB(CA) = %f\n",x.e(B,CA));
    printf(" pB(CA) = %f\n",x.p(B,CA));
    printf(" eC(CA) = %f\n",x.e(C,CA));
    printf(" pC(CA) = %f\n",x.p(C,CA));
    printf("qCA(AB) = %f (%f : %f)\n",x.q(AB),x.qMin(AB,CA),x.qMax(AB,CA));
    printf("cAB(CA) = %f\n",x.cosTh(AB,CA));
    printf("qBC(AB) = %f (%f : %f)\n",x.q(CA),x.qMin(BC,CA),x.qMax(BC,CA));
    printf("cBC(CA) = %f\n",x.cosTh(BC,CA));
} 




