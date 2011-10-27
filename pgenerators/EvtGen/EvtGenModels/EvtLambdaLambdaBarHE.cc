//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtLambdaLambdaBar.cc,
//
// Description:
//            Generator of pbar p -> electron positron events
//          
// Author List: Gosia Sudol
// Data: 10.05.09
//------------------------------------------------------------------------
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtResonance.hh"
#include "EvtGenBase/EvtResonance2.hh"
#include "EvtGenModels/EvtLambdaLambdaBarHE.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
#include "TMath.h"

#define Mp   0.938272

//using std::fstream;
using std::string;
using namespace std;


EvtLambdaLambdaBarHE::~EvtLambdaLambdaBarHE()
{
}

void EvtLambdaLambdaBarHE::getName(std::string& model_name)
{
  model_name="LambdaLambdaBarHE";
}

EvtDecayBase* EvtLambdaLambdaBarHE::clone()
{
  return new EvtLambdaLambdaBarHE;
}

void EvtLambdaLambdaBarHE::initProbMax()
{
// the normalisation factor must be large enough in order 
// to get correct results
// but if this normalisation factor is very large the accept-reject algo
// is very inefficient
  setProbMax(3000.0);
}

void EvtLambdaLambdaBarHE::init()
{

    checkNDaug(2);
    checkNArg(0);

}

void EvtLambdaLambdaBarHE::decay(EvtParticle* p)
{
    p->initializePhaseSpace(getNDaug(),getDaugs()); // p is pbarpSystem

    static const EvtId lambdabarID = EvtPDL::getId("anti-Lambda0");
    EvtParticle* lambdabar =  p->getDaug(0);

    if(lambdabar->getId() != lambdabarID )
        lambdabar= p->getDaug(1);
    if(lambdabar->getId() != lambdabarID )
        cout << "EvtLambdaLambdaBarHE::decay():\n wrong id of produced particles!"<<endl;

    EvtVector4R p4_lambdabar = lambdabar->getP4(); // 4 mometum in LAB

    EvtVector4R p4_pbar;
    p4_pbar = p->getP4(); // 4 momentum in LAB

    EvtVector4R vboost = p4_pbar; // boost vector

    vboost.set(1,-vboost.get(1)); // px
    vboost.set(2,-vboost.get(2)); // py
    vboost.set(3,-vboost.get(3)); // pz

    vboost.applyBoostTo(p4_pbar);       // boosting pbar to CM
    vboost.applyBoostTo(p4_lambdabar);  // boosting Lbar to CM

    p_pbar      = p4_pbar.d3mag();      // total momentum of anti-proton in CM
    p_lambdabar = p4_lambdabar.d3mag(); // total momentum of anti-lambda in CM

    double x = p4_lambdabar.get(3)/p4_lambdabar.d3mag(); // cos(theta) in CM



    double a=24.2; // +/- 2.2 mu barn
    double b=10.1; // +/- 0.6 GeV^-2
    double c=16.5; // 2.1 mu barn
    double d=3.0;  // 0.3 GeV^-2

    // momenta are in GeV, so the prob comes in mu barn
    // in this equation momentum of pbar and also of anti-lampda are in CM
    // also cosTheta is in CM

    double prob = 2*p_pbar*p_lambdabar*(a*b*TMath::Exp(2*b*p_pbar*p_lambdabar*(x-1))+c*d*TMath::Exp(2*d*p_pbar*p_lambdabar*(x-1)));

    setProb(prob);

    return;
}
