//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarPi0Gamma.cc,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> pi0 + gamma
//
// Author List:
// Cross-section is parametrised accroding to the data of E760 experiment 
// "T.A.Armstrong et al. Two-body neutral final states produced antiproton -protin annihilation". Parametrization describes data at Ecm=3526 MeV which correspond to h_c mass.
// So the given decay model is suitable only to energy Ecm =3526 MeV.
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
#include "EvtGenModels/EvtPpbarPi0Gamma.hh"
#include "EvtGenBase/EvtRandom.hh"
//using std::fstream;
//using std::fstream;
using std::string;
using namespace std;


EvtPpbarPi0Gamma::EvtPpbarPi0Gamma() : EvtDecayProb()
{
}

EvtPpbarPi0Gamma::~EvtPpbarPi0Gamma()
{
}

std::string EvtPpbarPi0Gamma::getName()
{
 return "PpbarPi0Gamma";
}

EvtDecayBase* EvtPpbarPi0Gamma::clone()
{
  return new EvtPpbarPi0Gamma;
}

void EvtPpbarPi0Gamma::initProbMax()
{
// the normalisation factor must be large enough in order 
// to get correct results
// but if this normalisation factor is very large the accept-reject algo
// is very inefficient
  setProbMax(1.0);
}

void EvtPpbarPi0Gamma::init()
{
  checkNDaug(2);
  //  checkNArg(1);

  //_pbarmom = getArg(0);
}

void EvtPpbarPi0Gamma::decay(EvtParticle* p)
{
  p->initializePhaseSpace(getNDaug(),getDaugs());

  static const EvtId pi0ID = EvtPDL::getId("pi0");
  EvtParticle* pi0 =  p->getDaug(0);
  
  if(pi0->getId() != pi0ID )
    pi0= p->getDaug(1);
  if(pi0->getId() != pi0ID )
    cout << "EvtPpbarPi0Gamma::decay():\n wrong id of produced particles!"<<endl;

  EvtVector4R p4 = pi0->getP4();
  double x = p4.get(3)/p4.d3mag(); // cos(theta)

  // Parametrization of experimental data in the cos(theta) range 0-0.6375
	double prob;
	if ((x>0.6)||(x<-0.6)){
		prob =0;
		}
	else{

prob = (3.42*3.42+pow(x,2.)*9.67*9.67)/pow(1.-pow(x,2.),2.);//sin^2(theta)
	prob*=1./110.741;
}	 
 //prob =0.5184-0.3077*x-4.6771*x*x+25.570*pow(x,3)+40.692*pow(x,4)-190.463*pow(x,5)-5.561*pow(x,6)+354.568*pow(x,7); //sin^2(theta)

	//prob*=1./5.29; // normalization factor (maximum value of the function)
	setProb(prob);

	return;
}
