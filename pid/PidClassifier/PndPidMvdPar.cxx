
// update 9/02/2010
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
  fElecMpvP0=-0.003129;
  fElecMpvP1=-9.839e-05;
  fElecMpvP2=-0.0008691;
  fElecSigmaP0=-1.152E-05;
  fElecSigmaP1=0.0002087;
}
void PndPidMvdPar::SetMuonPara()
{
  fMuonMpvP0=-0.002869;
  fMuonMpvP1=0.007084;
  fMuonMpvP2=-0.01502;
  fMuonSigmaP0=-5.555E-06;
  fMuonSigmaP1=0.0001891;

}
void PndPidMvdPar::SetPionPara()
{
  fPionMpvP0=-0.002839;
  fPionMpvP1=0.006213;
  fPionMpvP2=-0.005356;
  fPionSigmaP0=-0.0001743;
  fPionSigmaP1=-0.005183;
  fPionSigmaP2=0.03017;
}

void PndPidMvdPar::SetKaonPara()
{
  fKaonMpvP0=-0.002724;
  fKaonMpvP1=0.01344;
  fKaonMpvP2=0.1853;
  fKaonSigmaP0=-0.0001717;
  fKaonSigmaP1=0.008792;
  fKaonSigmaP2=0.1795;

}

void PndPidMvdPar::SetProtonPara()
{
  fProtonMpvP0=-0.00259;
  fProtonMpvP1=0.00559;
  fProtonMpvP2=0.827;
  fProtonSigmaP0=-0.0001326;
  fProtonSigmaP1=-0.2268;
  fProtonSigmaP2=1.142;
}

ClassImp(PndPidMvdPar)
