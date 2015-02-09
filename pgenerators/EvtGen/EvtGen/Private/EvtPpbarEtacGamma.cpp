//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarEtacGamma.cc,v 1.4 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> h_c->etac+gamma events
//
// Author List:
//
//------------------------------------------------------------------------
//#include "Experiment/Experiment.hh"
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtPpbarEtacGamma.hh"
#include "EvtGenBase/EvtRandom.hh"
//using std::fstream;
using std::string;
using namespace std;


EvtPpbarEtacGamma::EvtPpbarEtacGamma() : EvtDecayProb(), _pbarmom(0.)
{
}

EvtPpbarEtacGamma::~EvtPpbarEtacGamma()
{
}

std::string EvtPpbarEtacGamma::getName()
{
  return "PpbarEtacGamma";
}

EvtDecayBase* EvtPpbarEtacGamma::clone()
{
  return new EvtPpbarEtacGamma;
}

void EvtPpbarEtacGamma::initProbMax()
{
// the normalisation factor must be large enough in order 
// to get correct results
// but if this normalisation factor is very large the accept-reject algo
// is very inefficient
  setProbMax(1.0);
}

void EvtPpbarEtacGamma::init()
{
  checkNDaug(2);
  //  checkNArg(1);

  //_pbarmom = getArg(0);
}

void EvtPpbarEtacGamma::decay(EvtParticle* p)
{
  p->initializePhaseSpace(getNDaug(),getDaugs());

  static const EvtId etacID = EvtPDL::getId("eta_c");
  EvtParticle* etac =  p->getDaug(0);
  if(etac->getId() != etacID )
    etac= p->getDaug(1);
  if(etac->getId() != etacID )
    cout << "EvtPpbarEtacGamma::decay():\n wrong id of produced particles!"<<endl;

  EvtVector4R p4 = etac->getP4();
  double x = p4.get(3)/p4.d3mag(); // cos(theta)
  double prob = 1-x*x; //sin^2(theta)

  setProb(prob);

  return;
}


