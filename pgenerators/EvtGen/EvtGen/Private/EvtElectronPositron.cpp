//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtElectronPositron.cc,
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
#include "EvtGenModels/EvtElectronPositron.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"

#define Mp   0.938272

//using std::fstream;
using std::string;
using namespace std;

EvtElectronPositron::EvtElectronPositron() : EvtDecayProb(), _pbarmom(0.), _s(0.), _GeDivGm(-1.)
{
}

EvtElectronPositron::~EvtElectronPositron()
{
}

std::string EvtElectronPositron::getName()
{
  return "ElectronPositron";
}

EvtDecayBase* EvtElectronPositron::clone()
{
  return new EvtElectronPositron;
}

void EvtElectronPositron::initProbMax()
{
// the normalisation factor must be large enough in order 
// to get correct results
// but if this normalisation factor is very large the accept-reject algo
// is very inefficient
  setProbMax(10.0);
}

void EvtElectronPositron::init()
{

    checkNDaug(2);
    checkNArg(1);

    _GeDivGm = getArg(0); // ratio fo Ge/Gm

}

void EvtElectronPositron::decay(EvtParticle* p)
{
    p->initializePhaseSpace(getNDaug(),getDaugs());

    static const EvtId positronID = EvtPDL::getId("e+");
    EvtParticle* positron =  p->getDaug(0);

    if(positron->getId() != positronID )
        positron= p->getDaug(1);
    if(positron->getId() != positronID )
        cout << "EvtElectronPositron::decay():\n wrong id of produced particles!"<<endl;

    EvtVector4R p4 = positron->getP4();
    double x = p4.get(3)/p4.d3mag(); // cos(theta)

    EvtVector4R pbarmom;
    pbarmom = p->getP4Lab();
    cout<<"here is momentum: "<<pbarmom.d3mag()<<endl;

    _s=(Mp+sqrt(pow(pbarmom.d3mag(),2)+pow(Mp,2)))*(Mp+sqrt(pow(pbarmom.d3mag(),2)+pow(Mp,2)))-pow(pbarmom.d3mag(),2);

    double Ge=1.;
    double Gm=1.;
    if(_GeDivGm>0)
    { Gm=Ge/_GeDivGm;}
    else
    { Ge=0;}
    double sum=Ge+Gm;
    Ge=Ge/sum;
    Gm=Gm/sum;

    double prob = (Gm*Gm)*(1+x*x)+4*Mp*Mp*Ge*Ge*(1-x*x)/_s;

    setProb(prob);

    return;
}


