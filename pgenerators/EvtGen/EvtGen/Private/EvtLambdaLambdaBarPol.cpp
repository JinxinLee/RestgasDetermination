//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtLambdaLambdaBarPol.cc,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> LambdaBar Lambda events
//            based on the parametrised experimental data
//
// Author List:
// diff cross section parametrisation taken from EvtLambdaLambdaBar.cc written by Aleksandra Wronska, UJ Cracow 
// Jan Zhong  (add polarisation, no spin correlation applied yet)
// Ralf Kliemt, TU Dresden (ported to pandaroot use)
//------------------------------------------------------------------------
// #include "Experiment/Experiment.hh"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string>
#include "EvtGenBase/EvtPatches.hh"

#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtVector3R.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtLambdaLambdaBarPol.hh"
#include "EvtGenBase/EvtRandom.hh"
// #include "ErrLogger/ErrLog.hh"
using std::string;
using namespace std;


EvtLambdaLambdaBarPol::EvtLambdaLambdaBarPol() : EvtDecayProb(), _a0(0.), _a1(0.), _a2(0.), _a3(0.), _a4(0.),
                                           _a5(0.), _a6(0.), _a7(0.), _a8(0.), _a9(0.), _pbeam(0.),
                                           _polarisation(0.)
{
}

EvtLambdaLambdaBarPol::~EvtLambdaLambdaBarPol()
{ }

std::string EvtLambdaLambdaBarPol::getName()
{
  return "LambdaLambdaBarPol";
}

EvtDecayBase* EvtLambdaLambdaBarPol::clone()
{
  return new EvtLambdaLambdaBarPol;
}

void EvtLambdaLambdaBarPol::initProbMax()
{
  setProbMax(10.0);
}

void EvtLambdaLambdaBarPol::init()
{

  checkNDaug(2);
  checkNArg(2);

  _pbeam = getArg(0);
  _polarisation = getArg(1);   
  if(_pbeam > 2.5)
  {
    std::cerr << "Warning: EvtLambdaLambdaBar::init(): I generate the distribution as for pbeam = 2.5 GeV/c" << std::endl;
    _pbeam = 2.5;
  }

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
}

void EvtLambdaLambdaBarPol::decay(EvtParticle* p)
{
  p->initializePhaseSpace(getNDaug(),getDaugs());

  static const EvtId LambdaBarID = EvtPDL::getId("anti-Lambda0");
  EvtParticle* lambdabar =  p->getDaug(0);
  EvtParticle* lambda    =  p->getDaug(1);

  if(lambdabar->getId() != LambdaBarID )
  {
    lambdabar= p->getDaug(1);
    lambda   = p->getDaug(0);
  }
  if(lambdabar->getId() != LambdaBarID ){
    std::cerr << "Error: wrong id of produced particles!" << endl;
    abort();
  }

  EvtVector4R p4 = lambdabar->getP4();
  double x = p4.get(3)/p4.d3mag(); // cos(theta)
  double prob = _a0+
    _a1*x+
    _a2*(0.5*(3.*x*x-1.))+ 
    _a3*(0.5*(5.*x*x*x-3.*x))+
    _a4*(1./8.*(35.*x*x*x*x-30.*x*x+3.))+
    _a5*(1./8.*(63.*x*x*x*x*x-70.*x*x*x+15.*x))+
    _a6*(1./16.*(231.*x*x*x*x*x*x-315.*x*x*x*x+105.*x*x-5.))+
    _a7*(1./16.*(429.*x*x*x*x*x*x*x-693.*x*x*x*x*x+315.*x*x*x-35.*x))+
    _a8*(1./128.*(6435.*x*x*x*x*x*x*x*x-12012.*x*x*x*x*x*x+6930.*x*x*x*x-1260.*x*x+35.))+
    _a9*(1./152.*(109395.*x*x*x*x*x*x*x*x*x-231660.*x*x*x*x*x*x*x+162162.*x*x*x*x*x-41580.*x*x*x+2835.*x));


	      
  // calculate the polarization vector perp to scattering plane
    EvtVector4R p4LamBar = lambdabar->getP4();
    EvtVector3R prodnLamBar = cross(EvtVector3R(0.,0.,1.),EvtVector3R( p4LamBar.get(1),p4LamBar.get(2),p4LamBar.get(3) ));
    EvtVector3R polvLamBar = (prodnLamBar/prodnLamBar.d3mag());
    polvLamBar = polvLamBar*_polarisation;

    EvtSpinDensity rhoLamBar;
    rhoLamBar.setDim(2);
    //calc spindesity matrix= 0.5*(1+P_vec*pauli_vect)
    EvtComplex rhoLamBar00 = 0.5*EvtComplex(1.+polvLamBar.get(2),0.);
    EvtComplex rhoLamBar01 = 0.5*EvtComplex(polvLamBar.get(0),-polvLamBar.get(1));
    EvtComplex rhoLamBar10 = 0.5*EvtComplex(polvLamBar.get(0), polvLamBar.get(1));
    EvtComplex rhoLamBar11 = 0.5*EvtComplex(1.-polvLamBar.get(2),0.);

    rhoLamBar.set(0,0,rhoLamBar00);
    rhoLamBar.set(0,1,rhoLamBar01);
    rhoLamBar.set(1,0,rhoLamBar10);
    rhoLamBar.set(1,1,rhoLamBar11);
	      
    lambdabar->setSpinDensityForwardHelicityBasis(rhoLamBar);
  //use same polarization for lambda
    lambda->setSpinDensityForwardHelicityBasis(rhoLamBar);

    setProb(prob);
}
