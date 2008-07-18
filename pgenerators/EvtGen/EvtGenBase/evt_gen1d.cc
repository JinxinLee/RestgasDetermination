#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_gen1d.cc,v 1.9 2004/12/21 19:58:52 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

/*
 * Generate 1D distributions with and without pole compensation
 * 1 - Flat
 * 2 - Breit-Wigner
 */

#include <assert.h>
#include <stdlib.h>
#include "EvtGenBase/EvtBreitWignerPdf.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtIntervalFlatPdf.hh"
#include "EvtGenBase/EvtPoint1D.hh"

template <class Iterator>
void print(Iterator it, Iterator end) {

  while( !(it == end)) {

    typename Iterator::value_type x = *it++;
    printf("%f\n",x.value());
  }  
}

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
  typedef EvtPoint1D P;
  typedef EvtPredGen<EvtPdfGen<P>,EvtPdfPred<P> > Gen;

  if(i == 1) {
    if(!pc) {
      Gen gen = flat.accRejGen(flat,N);
      print(iter(gen,N),iter(gen));    
    }
    else {
      Gen gen = flat.accRejGen(bw,N);
      print(iter(gen,N),iter(gen));    
    }
  }
  
  if(i == 2) {
    
    if(!pc) {
      Gen gen = bw.accRejGen(flat,N);
      print(iter(gen,N),iter(gen));         
    }
    else {
      Gen gen = bw.accRejGen(bw,N);
      print(iter(gen,N),iter(gen));    
    }
  }
}


