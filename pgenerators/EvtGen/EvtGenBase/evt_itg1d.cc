#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_itg1d.cc,v 1.8 2004/12/21 19:58:52 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

/* 
 * Calculate integrals over the Dalitz plot with and witout pole compensation.
 *  1 - Flat
 *  2 - Breit-Wigner
 *  3 - Sum
 */

#include <iostream>
#include <stdlib.h>
#include "EvtGenBase/EvtBreitWignerPdf.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtPdfSum.hh"
#include "EvtGenBase/EvtIntervalFlatPdf.hh"

void usage(const char* name) {

  printf("Usage: %s TEST_NUMBER NUMBER_OF EVENTS POLECOMP\n",name);
  exit(0);
}

int main(int argc, char* argv[]) 
{  
  if(argc != 4 ) usage(argv[0]);
  int i = atoi(argv[1]);
  int N = atoi(argv[2]);
  int pc = 0;
  if(atoi(argv[3]) > 0) pc = 1;

  EvtRandomEngine eng;
  EvtRandom::setRandomEngine(&eng);

  double min = 0.;
  double max = 1.;
  EvtIntervalFlatPdf flat(min,max); 
  EvtBreitWignerPdf bw(min,max,0.5,0.01);
  flat.getItg(N);
  bw.getItg(N);
  EvtValError itg;
  
  if(i == 1) {
    if(!pc) itg = flat.compute_mc_integral(flat,N);
    else itg = flat.compute_mc_integral(bw,N);
  }

  if(i == 2) {
    if(!pc) itg = bw.compute_mc_integral(flat,N);
    else itg = bw.compute_mc_integral(bw,N);
  }

  if(i == 3) {
    
    EvtPdfSum<EvtPoint1D> sum;
    sum.addTerm(1.,flat);
    sum.addTerm(1.,bw);
    if(!pc)  itg = sum.compute_mc_integral(flat,N);
    else itg = sum.compute_mc_integral(sum,N);
  }
  
  printf("Itg = %f +/- %f\n",itg.value(),itg.error());
  return 0;
}
