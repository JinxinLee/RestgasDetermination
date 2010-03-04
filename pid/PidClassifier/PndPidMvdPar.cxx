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
  fElecMpvP0=-0.008357;
  fElecMpvP1=-9.839e-05;
  fElecMpvP2=0.004032;
  fElecSigmaP0=-5.0E-05;
  fElecSigmaP1=0.0005833;
}
void PndPidMvdPar::SetMuonPara()
{
  fMuonMpvP0=-0.002931;
  fMuonMpvP1=0.01618;
  fMuonMpvP2=-0.04723;
  fMuonSigmaP0=-7.439E-06;
  fMuonSigmaP1=0.0002242;
}
void PndPidMvdPar::SetPionPara()
{
  fPionMpvP0=-0.002893;
  fPionMpvP1=0.02134;
  fPionMpvP2=-0.05176;
  fPionSigmaP0=-0.000211;
  fPionSigmaP1=0.007626;
  fPionSigmaP2=-0.008938;
}

void PndPidMvdPar::SetKaonPara()
{
  fKaonMpvP0=-0.002699;
  fKaonMpvP1=0.05342;
  fKaonMpvP2=0.1067;
  fKaonSigmaP0=-0.0002203;
  fKaonSigmaP1=0.005101;
  fKaonSigmaP2=0.1609;
}

void PndPidMvdPar::SetProtonPara()
{
  fProtonMpvP0=-0.00252;
  fProtonMpvP1=0.04429;
  fProtonMpvP2=0.8253;
  fProtonSigmaP0=-0.0002334;
  fProtonSigmaP1=0.07461;
  fProtonSigmaP2=0.3562;
}

ClassImp(PndPidMvdPar)
