//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
//
// Modification history:
//
//    NK     September 3, 1997         Module created
//
//
//  ** Partly used as DS_DALITZ in PandaRoot        July, 2013       Lu Cao
//------------------------------------------------------------------------
// 
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtResonance.hh"
#include "EvtGenBase/EvtResonance2.hh"
#include "EvtGenBase/EvtFlatte.hh"
#include "EvtGenModels/EvtDsDalitz.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
using std::endl;

EvtDsDalitz::EvtDsDalitz() : EvtDecayAmp(), _d1(0), _d2(0), _d3(0), _flag(0), _kkpi_params()
{
}

EvtDsDalitz::~EvtDsDalitz() {}

std::string EvtDsDalitz::getName(){
  
  return "DS_DALITZ";     

}


EvtDecayBase* EvtDsDalitz::clone(){

  return new EvtDsDalitz;

}

void EvtDsDalitz::init(){

  // check that there are 0 arguments

  static EvtId DSP=EvtPDL::getId("D_s+");
  static EvtId DSM=EvtPDL::getId("D_s-");
  static EvtId KM=EvtPDL::getId("K-");
  static EvtId KP=EvtPDL::getId("K+");
  static EvtId PIM=EvtPDL::getId("pi-");
  static EvtId PIP=EvtPDL::getId("pi+");


  static double MPI = EvtPDL::getMeanMass(PIP);
  static double MKP = EvtPDL::getMeanMass(KP);
  
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
  checkSpinDaughter(2,EvtSpinType::SCALAR);

  EvtId parnum=getParentId();
  EvtId d1=getDaug(0);
  EvtId d2=getDaug(1);
  EvtId d3=getDaug(2);
  _flag=0;
  if ( parnum == DSP ) {
     if ( d1==KM && d2==KP && d3==PIP ) { _flag=5; _d1=0; _d2=1; _d3=2; }
     if ( d1==KM && d3==KP && d2==PIP ) { _flag=5; _d1=0; _d2=2; _d3=1; }
     if ( d2==KM && d1==KP && d3==PIP ) { _flag=5; _d1=1; _d2=0; _d3=2; }
     if ( d2==KM && d3==KP && d1==PIP ) { _flag=5; _d1=1; _d2=2; _d3=0; }
     if ( d3==KM && d1==KP && d2==PIP ) { _flag=5; _d1=2; _d2=0; _d3=1; }
     if ( d3==KM && d2==KP && d1==PIP ) { _flag=5; _d1=2; _d2=1; _d3=0; }
  }

  if ( parnum == DSM ) {
     if ( d1==KP && d2==KM && d3==PIM ) { _flag=5; _d1=0; _d2=1; _d3=2; }
     if ( d1==KP && d3==KM && d2==PIM ) { _flag=5; _d1=0; _d2=2; _d3=1; }
     if ( d2==KP && d1==KM && d3==PIM ) { _flag=5; _d1=1; _d2=0; _d3=2; }
     if ( d2==KP && d3==KM && d1==PIM ) { _flag=5; _d1=1; _d2=2; _d3=0; }
     if ( d3==KP && d1==KM && d2==PIM ) { _flag=5; _d1=2; _d2=0; _d3=1; }
     if ( d3==KP && d2==KM && d1==PIM ) { _flag=5; _d1=2; _d2=1; _d3=0; }
  }
  
  if ( _flag==5) {
     _kkpi_params.push_back(EvtFlatteParam(MPI, MPI, 0.406));
     _kkpi_params.push_back(EvtFlatteParam(MKP, MKP, 0.800));
  }

  if ( _flag==0) {
    report(EVT_ERROR,"EvtGen") << "EvtDsDaltiz: Invalid mode."<<endl;
    assert(0);
  }
}

void EvtDsDalitz::initProbMax() {

//probmax for Ds -> K K pi

if ( _flag==5 ) {setProbMax(50000.0);}

}

void EvtDsDalitz::decay( EvtParticle *p){

//same structure for all of these decays

  p->initializePhaseSpace(getNDaug(),getDaugs());
  EvtVector4R moms1 = p->getDaug(_d1)->getP4();
  EvtVector4R moms2 = p->getDaug(_d2)->getP4();
  EvtVector4R moms3 = p->getDaug(_d3)->getP4();

  EvtVector4R p4_p;
  p4_p.set(p->mass(),0.0,0.0,0.0);

  EvtComplex amp(1.0,0.0);


  // Ds -> K K pi
  if(_flag==5) {
     EvtResonance2 DsKKpiRes1(p4_p, moms3, moms1, 1.0, 0.0, 0.0455, 0.8944, 1, true); // K*(892)
     EvtResonance2 DsKKpiRes2(p4_p, moms3, moms1, 1.48, 138., 0.290, 1.414, 0); // K*_0(1430)
     EvtFlatte     DsKKpiRes3(p4_p, moms1, moms2, 5.07, 156., 0.965, _kkpi_params); // f_0(980)
     EvtResonance2 DsKKpiRes4(p4_p, moms1, moms2, 1.15, -10., 0.00426, 1.019455, 1, true); // phi(1020)
     EvtResonance2 DsKKpiRes5(p4_p, moms1, moms2, 1.28, 53., 0.265, 1.350, 0); // f_0(1370)
     EvtResonance2 DsKKpiRes6(p4_p, moms1, moms2, 1.19, 87., 0.137, 1.724, 0); // f_0(1710)
     amp = DsKKpiRes1.resAmpl() + DsKKpiRes2.resAmpl() + DsKKpiRes3.resAmpl()
	+ DsKKpiRes4.resAmpl() + DsKKpiRes5.resAmpl() + DsKKpiRes6.resAmpl();

  }
  
  vertex(amp);

  return ;
}

