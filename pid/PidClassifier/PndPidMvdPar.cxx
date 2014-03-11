
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
  SetProtonLowPara();
  SetProtonHighPara();
}



Double_t PndPidMvdPar::GetElectronMpv(Double_t momentum)
{
  return (GetElectronMpvP0()/(momentum*momentum) + (GetElectronMpvP1()*TMath::Log(momentum) + GetElectronMpvP2()));
}

Double_t PndPidMvdPar::GetElectronSigma(Double_t momentum)
{ 
  return (GetElectronSigmaP0()/(momentum*momentum) + (GetElectronSigmaP1()*TMath::Log(momentum) + GetElectronSigmaP2()));
}

Double_t PndPidMvdPar::GetMuonMpv(Double_t momentum)
{ 
  return (GetMuonMpvP0()/(momentum*momentum) + (GetMuonMpvP1()*TMath::Log(momentum) + GetMuonMpvP2()));
}

Double_t PndPidMvdPar::GetMuonSigma(Double_t momentum)
{
  return (GetMuonSigmaP0()/(momentum*momentum) + (GetMuonSigmaP1()*TMath::Log(momentum) + GetMuonSigmaP2()));
}

Double_t PndPidMvdPar::GetKaonMpv(Double_t momentum)
{
  return (GetKaonMpvP0()/(momentum*momentum) + (GetKaonMpvP1()*TMath::Log(momentum) + GetKaonMpvP2()));
}

Double_t PndPidMvdPar::GetKaonSigma(Double_t momentum)
{
    return (GetKaonSigmaP0()/(momentum*momentum) + (GetKaonSigmaP1()*TMath::Log(momentum) + GetKaonSigmaP2()));
}                             

Double_t PndPidMvdPar::GetPionMpv(Double_t momentum)
{
    return (GetPionMpvP0()/(momentum*momentum) + (GetPionMpvP1()*TMath::Log(momentum) + GetPionMpvP2()));
}

Double_t PndPidMvdPar::GetPionSigma(Double_t momentum)
{
    return (GetPionSigmaP0()/(momentum*momentum) + (GetPionSigmaP1()*TMath::Log(momentum) + GetPionSigmaP2()));
}

Double_t PndPidMvdPar::GetProtonLowMpv(Double_t momentum)
{
    return (GetProtonLowMpvP0()/(momentum*momentum) + (GetProtonLowMpvP1()*TMath::Log(momentum) + GetProtonLowMpvP2()));
}

Double_t PndPidMvdPar::GetProtonLowSigma(Double_t momentum)
{
    return (GetProtonLowSigmaP0()/(momentum*momentum) + (GetProtonLowSigmaP1()*TMath::Log(momentum) + GetProtonLowSigmaP2()));
}
// momenta above 0.6
Double_t PndPidMvdPar::GetProtonHighMpv(Double_t momentum)
{
    return (GetProtonHighMpvP0()/(momentum*momentum) + (GetProtonHighMpvP1()*TMath::Log(momentum) + GetProtonHighMpvP2()));
}

Double_t PndPidMvdPar::GetProtonHighSigma(Double_t momentum)
{
    return (GetProtonHighSigmaP0()/(momentum*momentum) + (GetProtonHighSigmaP1()*TMath::Log(momentum) + GetProtonHighSigmaP2()));

}

//___________________________________________________________
PndPidMvdPar::~PndPidMvdPar() 
{
}

void PndPidMvdPar::SetElectronPara()
{
  fElecMpvP0   = -2.25190e-06;
  fElecMpvP1   = -7.02849e-06;
  fElecMpvP2   =  3.12928e-03;
  fElecSigmaP0 = -5.52054e-08;
  fElecSigmaP1 = -1.82793e-06;
  fElecSigmaP2 =  2.05758e-04;
}
void PndPidMvdPar::SetMuonPara()
{
  fMuonMpvP0   = 2.08403e-05;
  fMuonMpvP1   = 8.17176e-05;
  fMuonMpvP2   = 2.81340e-03;
  fMuonSigmaP0 = 2.48457e-06;
  fMuonSigmaP1 = 8.31745e-06;
  fMuonSigmaP2 = 1.82576e-04;

}
void PndPidMvdPar::SetPionPara()
{
  fPionMpvP0    = 4.18813e-05;
  fPionMpvP1    = 9.03605e-05;
  fPionMpvP2    = 2.78408e-03;
  fPionSigmaP0  = 4.29608e-06;
  fPionSigmaP1  = 8.24673e-06;
  fPionSigmaP2  = 1.80471e-04;
}

void PndPidMvdPar::SetKaonPara()
{
  fKaonMpvP0   = 5.38291e-04;
  fKaonMpvP1   = 7.84611e-05;
  fKaonMpvP2   = 2.67446e-03;
  fKaonSigmaP0 = 3.77741e-05;
  fKaonSigmaP1 = 2.76088e-06;
  fKaonSigmaP2 = 1.75833e-04;

}

void PndPidMvdPar::SetProtonLowPara()
{
  fProtonLowMpvP0   = 7.92461e-05;
  fProtonLowMpvP1   =-1.20438e-02;
  fProtonLowMpvP2   = 1.63654e-03;
  fProtonLowSigmaP0 = 7.96982e-05;
  fProtonLowSigmaP1 = 1.81096e-04;
  fProtonLowSigmaP2 = 3.30778e-04 ;
}
void PndPidMvdPar::SetProtonHighPara()
{
  fProtonHighMpvP0   = 2.02904e-03 ;
  fProtonHighMpvP1   = 6.83480e-05;
  fProtonHighMpvP2   = 2.61251e-03;
  fProtonHighSigmaP0 = 1.08272e-04;
  fProtonHighSigmaP1 =-2.06436e-06;
  fProtonHighSigmaP2 = 1.80472e-04;
}

ClassImp(PndPidMvdPar)
