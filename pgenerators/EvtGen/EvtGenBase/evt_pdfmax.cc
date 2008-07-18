#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_pdfmax.cc,v 1.8 2004/12/21 19:58:52 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

/*
 * Compute Breit-Wigner pdf maximum by throwing random numbers
 * and compare with correct value. Throwing random numbers uniformly over the interval
 * can give poor results for narrow peaks. Pole-compensation eliminates the problem.
 * 
 * In this example: if the resonance has width/mass of order 10^-3. Twenty pole-compensated
 * samplings are needed to get a reasonable estimate. Millions uniform samplings still
 * fall very short. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtIntervalFlatPdf.hh"
#include "EvtGenBase/EvtBreitWignerPdf.hh"

void usage(const char* name) {

  printf("Usage: %s nPoints\n");
  exit(0);
}

int main(int argc, char* argv[]) 
{
  int N = 0;
  if(argc == 2) N = atoi(argv[1]);
  else usage(argv[0]);
  
  // Random Engine

  EvtRandomEngine eng;
  EvtRandom::setRandomEngine(&eng);

  // Intervals

  double xmin = 0.;
  double xmax = 1.;
  EvtIntervalFlatPdf flat(xmin,xmax); 
  EvtBreitWignerPdf bw(xmin,xmax,0.5,0.001);
  
  printf("BW mass    %f\n",bw.m0());
  printf("BW width   %f\n",bw.g0());
  printf("Iterations %d\n",N);

  EvtPdfMax<EvtPoint1D> max = bw.findMax(flat,N);
  printf("Computed max %f at %f\n",max.value(),max.loc().value());

  EvtPdfMax<EvtPoint1D> max1 = bw.findMax(bw,N);
  printf("Computed max %f at %f\n",max1.value(),max1.loc().value());

  return 0;
}



