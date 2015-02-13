//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtPpbarGammaGamma.cc,v 1.2 2008/01/08 17:15:11 steinke Exp $
//
// Description:
//            Generator of pbar p -> gamma + gamma
//
// Author List:
// Cross-section is parametrised accroding to the data of E760 experiment 
// "T.A.Armstrong et al. Two-body neutral final states produced antiproton -protin annihilation". Parametrization describes data at Ecm=3526 MeV which correspond to h_c mass.
// So the given decay model is suitable only to energy Ecm =3526 MeV.
//
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtPpbarGammaGamma.hh"
#include "EvtGenBase/EvtRandom.hh"
//using std::fstream;
using std::string;
using namespace std;


EvtPpbarGammaGamma::EvtPpbarGammaGamma() : EvtDecayProb()
{
}

EvtPpbarGammaGamma::~EvtPpbarGammaGamma()
{
}



std::string EvtPpbarGammaGamma::getName()
{
            return "PpbarGammaGamma";
}

EvtDecayBase* EvtPpbarGammaGamma::clone()
{
  return new EvtPpbarGammaGamma;
}

void EvtPpbarGammaGamma::initProbMax()
{
// the normalisation factor must be large enough in order 
// to get correct results
// but if this normalisation factor is very large the accept-reject algo
// is very inefficient
  setProbMax(1.0);
}

void EvtPpbarGammaGamma::init()
{
  checkNDaug(2);
  //  checkNArg(1);

  //_pbarmom = getArg(0);
}

void EvtPpbarGammaGamma::decay(EvtParticle* p)
{
  p->initializePhaseSpace(getNDaug(),getDaugs());

  static const EvtId gammaID = EvtPDL::getId("gamma");
  EvtParticle* gamma =  p->getDaug(0);
  
  if(gamma->getId() != gammaID )
    gamma= p->getDaug(1);
  if(gamma->getId() != gammaID )
    cout << "EvtPpbarGammaGamma::decay():\n wrong id of produced particles!"<<endl;

  EvtVector4R p4 = gamma->getP4();
  double x = p4.get(3)/p4.d3mag(); // cos(theta)
  
  // Parametrization of experimental data in the cos(theta) range 0-0.6375
	double prob;
	double maxprob=50.963125;
	if ((x>0.6)||(x<-0.6))
		prob =0;
	else
	//  prob =0.5184-0.3077*x-4.6771*x*x+25.570*pow(x,3)+40.692*pow(x,4)-190.463*pow(x,5)-5.561*pow(x,6)+354.568*pow(x,7); //sin^2(theta)
	
	prob=(8.2*8.2*pow(x,2)+2.9*2.9)/1-pow(x,2);
	/*(1/(1-pow(x,2))*(pow(((2.9*(pow(p4.d3mag(),2)/10.4),(-1.1))/pow(p4.d3mag(),4)),2)+pow((8.2*pow((pow(p4.d3mag(),2)/10.4),(-1.1))/pow(p4.d3mag(),4)),2)*pow(x,2)))*/;
	prob*=1./maxprob; // normalization factor (maximum value of the function)
	setProb(prob);

  return;
}
