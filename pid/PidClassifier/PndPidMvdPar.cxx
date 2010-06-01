#include "PndPidMvdPar.h"
#include "TMath.h"

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
  fMuonSigmaP1=0.0002236;

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

