//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtLambdaLambdaBar.cc,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> LambdaBar Lambda events
//            based on the parametrised experimental data
//
// Author List:
//     Aleksandra Wronska UJ Cracow            (original author)
//     Ralf Kliemt, Tu Dresden                 (ported for pandaroot use)
//     Ralf Kliemt, Uni Bonn                   (Added high momentum part on behalf of Erik Thome, Uppsala)
//------------------------------------------------------------------------
// #include "Experiment/Experiment.hh"
#include "EvtGenBase/EvtPatches.hh"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string>
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtLambdaLambdaBar.hh"
#include "EvtGenBase/EvtRandom.hh"
//using std::fstream;
using std::string;
using namespace std;

EvtLambdaLambdaBar::EvtLambdaLambdaBar() : EvtDecayProb(), _a0(0.), _a1(0.), _a2(0.), _a3(0.), _a4(0.), 
                                           _a5(0.), _a6(0.), _a7(0.), _a8(0.), _a9(0.), _pbeam(0.)
{
}

EvtLambdaLambdaBar::~EvtLambdaLambdaBar(){
}

std::string EvtLambdaLambdaBar::getName()
{
  return "LambdaLambdaBar";
}

EvtDecayBase* EvtLambdaLambdaBar::clone()
{
  return new EvtLambdaLambdaBar;
}

void EvtLambdaLambdaBar::initProbMax(){
  //setProbMax();
}

void EvtLambdaLambdaBar::init(){
  
  checkNDaug(2);
  checkNArg(1);
  
  _pbeam = getArg(0);
  //if(_pbeam > 2.5){
  //  cout<< "EvtLambdaLambdaBar::init(): I generate the distribution as for pbeam = 2.5 GeV/c"<<endl;
  //_pbeam = 2.5;
  //}
  if(_pbeam<2.5){
    _a0 = 1;
    _a1 = -5.053 + 5.701*_pbeam - 1.131*_pbeam*_pbeam;
    _a2 = -7.078 + 7.484*_pbeam - 1.490*_pbeam*_pbeam;
    _a3 = -7.439 + 7.298*_pbeam - 1.399*_pbeam*_pbeam;
    _a4 = (_pbeam>1.667 ? -13.12 + 12.64*_pbeam - 2.748*_pbeam*_pbeam : 0.);
    _a5 = (_pbeam>1.725 ? -10.05 + 9.351*_pbeam - 2.030*_pbeam*_pbeam : 0.);
    _a6 = (_pbeam>1.950 ?  0 : (_pbeam>1.725 ? -2.025+1.18*_pbeam : 0));
    _a7 = 0.;
    _a8 = 0.;
    _a9 = 0.;
  } else {
    _a0=24.2;
    _a1=10.1;
    _a2=16.5;
    _a3=3.0;
    _a4=0.;
    _a5=0.;
    _a6=0.;
    _a7=0.;
    _a8=0.;
    _a9=0.;
  }
  
}

void EvtLambdaLambdaBar::decay(EvtParticle* p){
  
  p->initializePhaseSpace(getNDaug(),getDaugs());
  
  static const EvtId LambdaBarID = EvtPDL::getId("anti-Lambda0");
  EvtParticle* lambdabar =  p->getDaug(0);
  if(lambdabar->getId() != LambdaBarID )
    lambdabar= p->getDaug(1);
  if(lambdabar->getId() != LambdaBarID )
    cout << "EvtLambdaLambdaBar::decay():\n wrong id of produced particles!"<<endl;
  
  EvtVector4R p4 = lambdabar->getP4();
  double x = p4.get(3)/p4.d3mag(); // cos(theta)
  double plbar = p4.d3mag(); // threemomentum
  double prob = 0.;
  if(_pbeam<2.5){
    prob = _a0+
    _a1*x+
    _a2*(0.5*(3.*x*x-1.))+ 
    _a3*(0.5*(5.*x*x*x-3.*x))+
    _a4*(1./8.*(35.*x*x*x*x-30.*x*x+3.))+
    _a5*(1./8.*(63.*x*x*x*x*x-70.*x*x*x+15.*x))+
    _a6*(1./16.*(231.*x*x*x*x*x*x-315.*x*x*x*x+105.*x*x-5.))+
    _a7*(1./16.*(429.*x*x*x*x*x*x*x-693.*x*x*x*x*x+315.*x*x*x-35.*x))+
    _a8*(1./128.*(6435.*x*x*x*x*x*x*x*x-12012.*x*x*x*x*x*x+6930.*x*x*x*x-1260.*x*x+35.))+
    _a9*(1./152.*(109395.*x*x*x*x*x*x*x*x*x-231660.*x*x*x*x*x*x*x+162162.*x*x*x*x*x-41580.*x*x*x+2835.*x));
  }else{
    
    //d sigma / d cos(theta) =  
    prob = 2*_pbeam*plbar*(_a0*_a1*exp(2*_a1*_pbeam*plbar*(x-1))+_a2*_a3*exp(2*_a3*_pbeam*plbar*(x-1)));
    
    
  }
  setProb(prob);
  
  return;
}


//--------------------------------------------------------------
// $Log: EvtLambdaLambdaBar.cc,v $
// Revision 1.2  2008/01/08 17:15:11  steinke
// merged with new version from BaBar
//
// Revision 1.1  2006/11/06 14:06:09  wronska
// initial import
//
//--------------------------------------------------------------

