#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_gendal.cc,v 1.9 2004/12/21 19:58:52 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

/*
 * Generate 1D distributions with and without pole compensation
 * 1 - NR
 * 2 - Resonant
 * 3 - Sum
 */

#include <assert.h>
#include <stdlib.h>
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtDalitzFlatPdf.hh"
#include "EvtGenBase/EvtCyclic3.hh"
#include "EvtGenBase/EvtDalitzPlot.hh"
#include "EvtGenBase/EvtPropBreitWigner.hh"
#include "EvtGenBase/EvtFlatAmp.hh"
#include "EvtGenBase/EvtPto3PAmp.hh"
#include "EvtGenBase/EvtAmpPdf.hh"
#include "EvtGenBase/EvtDalitzResPdf.hh"
#include "EvtGenBase/EvtAmplitudeSum.hh"
#include "EvtGenBase/EvtPdfSum.hh"

template <class Iterator>
void print(Iterator it, Iterator end) {

  using namespace EvtCyclic3;
  while( !(it == end)) {

    typename Iterator::value_type x = *it++;
    printf("%f %f %f %f %f %f\n",x.m(A),x.m(B),x.m(C),x.q(AB),x.q(BC),x.q(CA));
  }  
}

void usage(const char* name) {

  printf("Usage: %s TEST_NUMBER NUMBER_OF_EVENTS POLECOMP\n",name);
  exit(0);
}


int main(int argc, char* argv[]) 
{
  typedef EvtDalitzPoint P;

  if(argc != 4 ) usage(argv[0]);
  
  int i = atoi(argv[1]);
  int N = atoi(argv[2]);
  int pc = 0;
  if(atoi(argv[3]) > 0) pc = 1;
  
  EvtRandomEngine eng;
  EvtRandom::setRandomEngine(&eng);
  
  double mPi = 0.14;
  double mB  = 5.28;
  EvtDalitzPlot dp(mPi,mPi,mPi,mB);
  
  EvtCyclic3::Pair RES = EvtCyclic3::CA;
  double m0 = 3.4;
  double g0 = 0.1;
  EvtPropBreitWigner bw(m0,g0);

  EvtDalitzFlatPdf flat(dp);  
  flat.getItg(1000);
  EvtDalitzResPdf res(dp,m0,g0,RES);    
  res.getItg(1000);

  EvtFlatAmp<P> amp0; 
  EvtComplex c0(1.0,0.0);
  EvtPto3PAmp amp1(dp,EvtCyclic3::BC,RES,EvtSpinType::SCALAR,bw,EvtPto3PAmp::NBW);
  EvtComplex c1(1.0,0.0);
  
  typedef EvtPredGen<EvtPdfGen<P>,EvtPdfPred<P> > Gen;
  
  if(i == 1) {

    EvtAmpPdf<P> pdf(amp0);
    Gen gen = pdf.accRejGen(flat,N);
    print(iter(gen,N),iter(gen));    
  }
  if(i == 2) {
    
    EvtAmpPdf<P> pdf(amp1);
    if(!pc) {
      Gen gen = pdf.accRejGen(flat,N);
      print(iter(gen,N),iter(gen));    
    }
    else {

      EvtPdfSum<P> pcsum;
      pcsum.addTerm(1./flat.getItg().value(),flat);
      pcsum.addTerm(1./res.getItg().value(),res);
      Gen gen = pdf.accRejGen(pcsum,1000);
      print(iter(gen,N),iter(gen));    
    }
  }  
  if(i == 3) {
    
    EvtAmplitudeSum<P> sum;
    sum.addTerm(c0,amp0);
    sum.addTerm(c1,amp1);
    
    EvtAmpPdf<P> pdf(sum);
    if(!pc) {
      Gen gen = pdf.accRejGen(flat,1000);
      print(iter(gen,N),iter(gen));    
    }
    else {
      EvtPdfSum<P> pcsum;
      pcsum.addTerm(abs2(c0),flat);
      pcsum.addTerm(abs2(c1),res);
      
      Gen gen = pdf.accRejGen(pcsum,1000);
      print(iter(gen,N),iter(gen));    
    }
  }
  
}


