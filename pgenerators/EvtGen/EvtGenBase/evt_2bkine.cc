#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_2bkine.cc,v 1.6 2004/12/21 19:58:51 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// Test two body kinematic calculations. Output should be:
/*
p in AB = 2.58192
p in  A = 17.7046
p in  B = 97.3752
*/

#include <stdio.h>
#include "EvtGenBase/EvtTwoBodyKine.hh"

int main(int argc, char* argv[])
{
  double mB = 5.28;
  double mRho = 0.77;
  double mPi = 0.14;

  EvtTwoBodyKine x(mRho,mPi,mB);
  EvtTwoBodyKine::Index A = EvtTwoBodyKine::A;
  EvtTwoBodyKine::Index B = EvtTwoBodyKine::B;
  EvtTwoBodyKine::Index AB = EvtTwoBodyKine::AB;

  printf("p in AB = %f\n",x.p(AB));
  printf("p in A = %f\n",x.p(A));
  printf("p in B = %f\n",x.p(B));
}
