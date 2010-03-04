#include "PndPidMvdPar.h"
#include "TMath.h"
#include "TF1.h"
//___________________________________________________________
PndPidMvdPar::PndPidMvdPar() 
{
  SetElectronPara();
  SetMuonPara();
  SetPionPara();
  SetKaonPara();
  SetProtonPara();
}



Float_t PndPidMvdPar::GetElectronMpv(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetElectronMpvP0(),GetElectronMpvP1(),GetElectronMpvP2());
  TF1 *empv= new TF1("empv",buffer);
  return empv->Eval(momentum);
}

Float_t PndPidMvdPar::GetElectronSigma(Float_t momentum)
{ 
  Text_t buffer[250];
  sprintf(buffer,"%f*x+%f",GetElectronSigmaP0(), GetElectronSigmaP1());
  TF1 *esigma= new TF1("esigma",buffer);
  return esigma->Eval(momentum);
}


Float_t PndPidMvdPar::GetMuonMpv(Float_t momentum)
{ 
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetMuonMpvP0(),GetMuonMpvP1(),GetMuonMpvP2());
  TF1 *empv= new TF1("empv",buffer);
  return empv->Eval(momentum);
}

Float_t PndPidMvdPar::GetMuonSigma(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f*x+%f",GetMuonSigmaP0(), GetMuonSigmaP1());
  TF1 *esigma= new TF1("esigma",buffer);
  return esigma->Eval(momentum);
}

Float_t PndPidMvdPar::GetKaonMpv(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetKaonMpvP0(),GetKaonMpvP1(),GetKaonMpvP2());
  TF1 *empv= new TF1("empv",buffer);
  return empv->Eval(momentum);
}

Float_t PndPidMvdPar::GetKaonSigma(Float_t momentum)
{ 
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetKaonSigmaP0(),GetKaonSigmaP1(),GetKaonSigmaP2());
  TF1 *esigma= new TF1("esigma",buffer);
  return esigma->Eval(momentum);
}

Float_t PndPidMvdPar::GetPionMpv(Float_t momentum)
{ 
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetPionMpvP0(),GetPionMpvP1(),GetPionMpvP2());
  TF1 *empv= new TF1("empv",buffer);
  return empv->Eval(momentum);
}

Float_t PndPidMvdPar::GetPionSigma(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetPionSigmaP0(),GetPionSigmaP1(),GetPionSigmaP2());
  TF1 *esigma= new TF1("esigma",buffer);
  return esigma->Eval(momentum);
}

Float_t PndPidMvdPar::GetProtonMpv(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetProtonMpvP0(),GetProtonMpvP1(),GetProtonMpvP2());
  TF1 *empv= new TF1("empv",buffer);
  return empv->Eval(momentum);
}

Float_t PndPidMvdPar::GetProtonSigma(Float_t momentum)
{
  Text_t buffer[250];
  sprintf(buffer,"%f/x/x*(%f*TMath::Log(x*x)-x*x-%f)",GetProtonSigmaP0(),GetProtonSigmaP1(),GetProtonSigmaP2());
  TF1 *esigma= new TF1("esigma",buffer);
  return esigma->Eval(momentum);
}

//___________________________________________________________
PndPidMvdPar::~PndPidMvdPar() 
{
}

void PndPidMvdPar::SetElectronPara()
{
  fElecMpvP0=-0.003128;
  fElecMpvP1=-9.103e-011;
  fElecMpvP2=0.000278;
  fElecSigmaP0=-6.072E-06;
  fElecSigmaP1=0.0002131;
}
void PndPidMvdPar::SetMuonPara()
{
  fMuonMpvP0=-0.00293;
  fMuonMpvP1=0.01559;
  fMuonMpvP2=-0.04645;
  fMuonSigmaP0=-7.067E-06;
  fMuonSigmaP1=0.002236;

}
void PndPidMvdPar::SetPionPara()
{
  fPionMpvP0=-0.002891;
  fPionMpvP1=0.0212;
  fPionMpvP2=-0.05162;
  fPionSigmaP0=-0.0001952;
  fPionSigmaP1=-0.01729;
  fPionSigmaP2=-0.008938;
}

void PndPidMvdPar::SetKaonPara()
{
  fKaonMpvP0=-0.0027;
  fKaonMpvP1=0.05456;
  fKaonMpvP2=0.1057;
  fKaonSigmaP0=-0.0002245;
  fKaonSigmaP1=-0.0203;
  fKaonSigmaP2=0.1286;

}

void PndPidMvdPar::SetProtonPara()
{
  fProtonMpvP0=-0.002521;
  fProtonMpvP1=0.04519;
  fProtonMpvP2=0.8242;
  fProtonSigmaP0=-0.0002329;
  fProtonSigmaP1=0.0673;
  fProtonSigmaP2=0.3625;
}

ClassImp(PndPidMvdPar)
