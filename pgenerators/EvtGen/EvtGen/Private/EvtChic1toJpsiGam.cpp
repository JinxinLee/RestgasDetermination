//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: EvtChic1toJpsiGam.cc,v 1.1 2008/05/14 19:11:21 jan Exp $
//
// Description:
// example generator for pbar p -> chi_c1 -> J/Psi gamma          
// formula taken from note written by PANDA Ferrara group 
//
// todo: check if everything is correct ( especially chi angle)
//            
// Author List:
// Jan Zhong  Ruhr-Uni-Bochum
//      PANDA Ferrara group
//
//------------------------------------------------------------------------
#include <math.h>
#include <TVector3.h>

#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtChic1toJpsiGam.hh"

using std::string;
using namespace std;


EvtChic1toJpsiGam::EvtChic1toJpsiGam() : EvtDecayNew(), _a2(0.), _prob(0.)
{
}

EvtChic1toJpsiGam::~EvtChic1toJpsiGam()
{
}

std::string EvtChic1toJpsiGam::getName()
{
  return "Chic1toJpsiGam";
}

EvtDecayBase* EvtChic1toJpsiGam::clone()
{
  return new EvtChic1toJpsiGam;
}

void EvtChic1toJpsiGam::initProbMax()
{
  setProbMax(10.0);
}

void EvtChic1toJpsiGam::init()
{
  checkNDaug(2);
  checkNArg(1);

  _a2 = getArg(0);
}

void EvtChic1toJpsiGam::decay(EvtParticle* p)
{
  EvtParticle* jpsi =  p->getDaug(0);
  EvtParticle* gamma =  p->getDaug(1);
  if ((jpsi->getId() != EvtPDL::getId("J/psi")) || (gamma->getId() != EvtPDL::getId("gamma")) )
    cout << "EvtChic1toJpsiGam::decay():\n wrong id of produced particles!"<<endl;
  
  EvtParticle* lepton = p->getDaug(0)->getDaug(1);
  EvtId JpsiDaugs[2];
  if ((lepton->getId() ==EvtPDL::getId("e-")) || (lepton->getId() == EvtPDL::getId("e+")))
    {
      JpsiDaugs[0]=EvtPDL::getId("e-");
      JpsiDaugs[1]=EvtPDL::getId("e+");
    }
  else
    if ((lepton->getId() ==EvtPDL::getId("mu-")) || (lepton->getId() == EvtPDL::getId("mu+")))
      {
	JpsiDaugs[0]=EvtPDL::getId("mu-");
	JpsiDaugs[1]=EvtPDL::getId("mu+");
      }
    else
      cout << "EvtChic1toJpsiGam::decay():\n wrong id of produced daughter particles!"<<endl;  
  
  p->initializePhaseSpace(2,getDaugs());
  p->getDaug(0)->initializePhaseSpace(2,JpsiDaugs);
  
  TVector3 jpsiMom( jpsi->getP4().get(1), jpsi->getP4().get(2), jpsi->getP4().get(3) );
  double costheta=jpsiMom.z()/jpsiMom.Mag();
  double theta=acos(costheta);  

  EvtParticle* positron = p->getDaug(0)->getDaug(1);
  TVector3  positronMom ( positron->getP4().get(1), positron->getP4().get(2), positron->getP4().get(3) );
  double thetaPrime    = positronMom.Angle(jpsiMom);
  double costhetaPrime = cos(thetaPrime);

  TVector3 z(0,0,1);  
  TVector3 normDecPlaneChic = jpsiMom.Cross(z);
  TVector3 normDecPlaneJpsi = positronMom.Cross(jpsiMom);
  double chiPrime=normDecPlaneJpsi.Angle(normDecPlaneChic);

  double a1=sqrt(1-_a2*_a2);
  double A0=a1/sqrt(2)-_a2/sqrt(2);
  double A1=a1/sqrt(2)+_a2/sqrt(2);

  double T1=1;
  double T2=costheta*costheta;
  double T3=costhetaPrime*costhetaPrime;
  double T4=T2*T3;
  double T5=sin(2*theta)*sin(2*thetaPrime)*cos(chiPrime);

  double K1=0.5;
  double K2=0.5*(A1*A1-A0*A0);
  double K3= -1.0*K2;
  double K4=-0.5;
  double K5= -0.25*A0*A1;

  double W = K1*T1+K2*T2+K3*T3+K4*T4+K5*T5;
  
  setProb(W);

  return;
}

