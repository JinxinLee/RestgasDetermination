//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: EvtChic2toJpsiGam.cc,v 1.2 2008/05/28 15:45:35 jan Exp $
//
// Description:
// example generator for pbar p -> chi_c2 -> J/Psi gamma          
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
#include "EvtGenModels/EvtChic2toJpsiGam.hh"

using std::string;
using namespace std;


EvtChic2toJpsiGam::EvtChic2toJpsiGam() : EvtDecayNew(), _B0(0.), _a2(0.), _a3(0.), _prob(0.)
{
}

EvtChic2toJpsiGam::~EvtChic2toJpsiGam()
{
}

std::string EvtChic2toJpsiGam::getName()
{
  return "Chic2toJpsiGam";
}

EvtDecayBase* EvtChic2toJpsiGam::clone()
{
  return new EvtChic2toJpsiGam;
}

void EvtChic2toJpsiGam::initProbMax()
{
  setProbMax(20.0);
}

void EvtChic2toJpsiGam::init()
{
  checkNDaug(2);
  checkNArg(3);

  _B0 = getArg(0);
  _a2 = getArg(1);
  _a3 = getArg(2);
}

void EvtChic2toJpsiGam::decay(EvtParticle* p)
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
  double phiPrime=normDecPlaneJpsi.Angle(normDecPlaneChic);

  double a1=sqrt(1.-_a2*_a2-_a3*_a3);
  double A0=a1/sqrt(10.)+_a2/sqrt(2.)+_a3*sqrt(0.4);
  double A1=a1*sqrt(0.3)+_a2/sqrt(6.)-_a3*sqrt(8./15.);
  double A2=a1*sqrt(0.6)-_a2/sqrt(3.)+_a3*sqrt(1./15.);
  double  R=1.-_B0*_B0;

  double T1=1;
  double T2=costheta*costheta;
  double T3=costheta*costheta*costheta*costheta;
  double T4=costhetaPrime*costhetaPrime;
  double T5=T4*T2;
  double T6=T4*T3;
  double T7=sin(thetaPrime)*sin(thetaPrime)*cos(2*phiPrime);
  double T8=T2*T7;
  double T9=T3*T7;
  double T10=sin(2*theta)*sin(2*thetaPrime)*cos(phiPrime);
  double T11=T2*T10;

  double K1=0.125*(2*A0*A0+3*A2*A2-R*(2*A0*A0-4*A1*A1+A2*A2));
  double K2=0.75 *(-2*A0*A0+4*A1*A1-A2*A2+R*(4*A0*A0-6*A1*A1+A2*A2));
  double K3=0.125*(6*A0*A0-8*A1*A1+A2*A2)*(3-5*R);
  double K4=0.125*(2*A0*A0+3*A2*A2-R*(2*A0*A0+4*A1*A1+A2*A2));
  double K5=0.75 *(-2*A0*A0-4*A1*A1-A2*A2+R*(4*A0*A0+6*A1*A1+A2*A2));
  double K6=0.125*(6*A0*A0+8*A1*A1+A2*A2)*(3-5*R);
  double K7=sqrt(1.5)*(R-1.)*A0*A2;
  double K8=sqrt(1.5)*(4-6.*R)*A0*A2;
  double K9=sqrt(1.5)*(5.*R-3.)*A0*A2;
  double K10=-1.*sqrt(0.75)*(A0*A1+sqrt(1.5)*A1*A2-R*(2*A0*A1+sqrt(1.5)*A1*A2) );
  double K11=(-1./sqrt(48.)) * (5*R-3.) * (3*A0*A1+sqrt(1.5)*A1*A2);


  double W = K1*T1+
             K2*T2+
             K3*T3+
             K4*T4+
             K5*T5+
             K6*T6+
             K7*T7+
             K8*T8+
             K9*T9+
             K10*T10+
             K11*T11;
  
  setProb(W);

  return;
}
