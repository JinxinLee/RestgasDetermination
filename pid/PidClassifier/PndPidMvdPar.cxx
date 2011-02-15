
// update 15/02/2011
// pid parameter for the Mvd-2.1_FullVersion.root geometry files

#include "PndPidMvdPar.h"
#include "TMath.h"

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
  return (GetElectronMpvP0()/(momentum*momentum)*(GetElectronMpvP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetElectronMpvP2()));
}

Float_t PndPidMvdPar::GetElectronSigma(Float_t momentum)
{ 
  return (GetElectronSigmaP0()*momentum+GetElectronSigmaP1());
}

Float_t PndPidMvdPar::GetMuonMpv(Float_t momentum)
{ 
  return (GetMuonMpvP0()/(momentum*momentum)*(GetMuonMpvP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetMuonMpvP2()));
}

Float_t PndPidMvdPar::GetMuonSigma(Float_t momentum)
{
  return (GetMuonSigmaP0()*momentum+GetMuonSigmaP1());
}

Float_t PndPidMvdPar::GetKaonMpv(Float_t momentum)
{
  return (GetKaonSigmaP0()/(momentum*momentum)*(GetKaonSigmaP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetKaonSigmaP2()));
}

Float_t PndPidMvdPar::GetKaonSigma(Float_t momentum)
{ 
  return (GetKaonSigmaP0()/(momentum*momentum)*(GetKaonSigmaP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetKaonSigmaP2()));
}

Float_t PndPidMvdPar::GetPionMpv(Float_t momentum)
{ 
  return (GetPionMpvP0()/(momentum*momentum)*(GetPionMpvP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetPionMpvP2()));
}

Float_t PndPidMvdPar::GetPionSigma(Float_t momentum)
{
  return (GetPionSigmaP0()/(momentum*momentum)*(GetPionSigmaP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetPionSigmaP2()));
}

Float_t PndPidMvdPar::GetProtonMpv(Float_t momentum)
{
  return (GetProtonMpvP0()/(momentum*momentum)*(GetProtonMpvP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetProtonMpvP2()));
}

Float_t PndPidMvdPar::GetProtonSigma(Float_t momentum)
{
  return (GetProtonSigmaP0()/(momentum*momentum)*(GetProtonSigmaP1()*TMath::Log(momentum*momentum)-momentum*momentum-GetProtonSigmaP2()));
}

//___________________________________________________________
PndPidMvdPar::~PndPidMvdPar() 
{
}

void PndPidMvdPar::SetElectronPara()
{
  fElecMpvP0=-0.00312858;
  fElecMpvP1=-1.507E-6;
  fElecMpvP2=-1.07387E-3;
  fElecSigmaP0=-1.23101E-05;
  fElecSigmaP1=0.000209078;
}
void PndPidMvdPar::SetMuonPara()
{
  fMuonMpvP0=-2.87821E-3;
  fMuonMpvP1=7.37147E-3;
  fMuonMpvP2=-1.60083E-2;
  fMuonSigmaP0=-5.80053E-06;
  fMuonSigmaP1=1.89061E-4;

}
void PndPidMvdPar::SetPionPara()
{
  fPionMpvP0=-0.00284155;
  fPionMpvP1=0.00648687;
  fPionMpvP2=-0.00606107;
  fPionSigmaP0=-0.000174549;
  fPionSigmaP1=-0.00520594;
  fPionSigmaP2=2.97958E-2;
}

void PndPidMvdPar::SetKaonPara()
{
  fKaonMpvP0=-0.00272739;
  fKaonMpvP1=0.0137811;
  fKaonMpvP2=0.184134;
  fKaonSigmaP0=-0.000170837;
  fKaonSigmaP1=0.00831504;
  fKaonSigmaP2=0.181227;

}

void PndPidMvdPar::SetProtonPara()
{
  fProtonMpvP0=-2.56891E-3;
  fProtonMpvP1=-3.84895E-4;
  fProtonMpvP2=0.842041;
  fProtonSigmaP0=-0.000138299;
  fProtonSigmaP1=-0.235109;
  fProtonSigmaP2=1.06521;
}

ClassImp(PndPidMvdPar)
