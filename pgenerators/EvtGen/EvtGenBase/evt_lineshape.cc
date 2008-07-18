#include "EvtGenBase/EvtPatches.hh"
/*******************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: EvtGenBase
 *    File: $Id: evt_lineshape.cc,v 1.8 2004/12/21 19:58:52 ryd Exp $
 *  Author: Alexei Dvoretskii, dvoretsk@slac.stanford.edu, 2001-2002
 *
 * Copyright (C) 2002 Caltech
 *******************************************************************************/

// test lineshape

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "EvtGenBase/EvtRelBreitWignerBarrierFact.hh"
#include "EvtGenBase/EvtSpinType.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
using std::ifstream;

void usage(char* name) {

  printf("Usage: %s NUMBER_OF EVENTS BW_RADIUS\n",name);
  exit(0);
}

int main(int argc, char* argv[]) 
{
  if(argc != 3 ) usage(argv[0]);
  
  int N = atoi(argv[1]);
  double R = atof(argv[2]);
  
  EvtRandomEngine eng;
  EvtRandom::setRandomEngine(&eng);

  EvtPDL pdl;
  pdl.readPDT("../EvtGenBase/evt.pdl");
  EvtRelBreitWignerBarrierFact bw(0.77, 0.15, 0.3, EvtSpinType::VECTOR);
  bw.reSetBlatt(R);

  EvtId rho = EvtPDL::getId("rho0");
  EvtId pi = EvtPDL::getId("pi+");
  EvtId Bp = EvtPDL::getId("B+");
  EvtId d[2];
  d[0] = pi;
  d[1] = pi;
  double mD[2];
  mD[0]=EvtPDL::getMeanMass(pi);
  mD[1]=EvtPDL::getMeanMass(pi);


  int i;
  for(i=0;i<N;i++) {

    double m = bw.getRandMass(&Bp,2,d,&pi,3.0,mD);
    printf("%f\n",m);
  }
}


// Macro for displaying resutls
/*
TTree* readTree(const char* file, int N) 
{
  double m;
  TTree* t = new TTree();
  t->Branch("m",&m,"m/D");
  
  ifstream f(file);
  
  int i=0;
  while(i < N) {
    i++;
    f >> m;
    t->Fill();
  }

  return t;
}
*/





