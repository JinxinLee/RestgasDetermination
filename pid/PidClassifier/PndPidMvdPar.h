#ifndef PNDPIDMVDPAR_H
#define PNDPIDMVDPAR_H

#include "TObject.h"

// **************************************************************************
//  Author: Laura Zotti e-mail: zotti@to.infn.it
//
//  params for MVD Associator
//
// Created: 02-03-10
// Modified:
//
// **************************************************************************

class PndPidMvdPar : public TObject
{
 public:
  
 
  PndPidMvdPar();
  virtual ~PndPidMvdPar();

  Double_t GetElectronMpv(Double_t momentum);
  Double_t GetElectronSigma(Double_t momentum);

  Double_t GetMuonMpv(Double_t momentum);
  Double_t GetMuonSigma(Double_t momentum);

  Double_t GetPionMpv(Double_t momentum);
  Double_t GetPionSigma(Double_t momentum);

  Double_t GetProtonMpv(Double_t momentum);
  Double_t GetProtonSigma(Double_t momentum);

  Double_t GetKaonMpv(Double_t momentum);
  Double_t GetKaonSigma(Double_t momentum);

  void SetElectronPara();
  void SetMuonPara();
  void SetPionPara();
  void SetKaonPara();
  void SetProtonPara();

  Double_t GetElectronMpvP0()   const {return fElecMpvP0;};
  Double_t GetElectronMpvP1()   const {return fElecMpvP1;};
  Double_t GetElectronMpvP2()   const {return fElecMpvP2;};
  Double_t GetElectronSigmaP0() const {return fElecSigmaP0;};
  Double_t GetElectronSigmaP1() const {return fElecSigmaP1;};

  Double_t GetMuonMpvP0()   const {return fMuonMpvP0;};
  Double_t GetMuonMpvP1()   const {return fMuonMpvP1;};
  Double_t GetMuonMpvP2()   const {return fMuonMpvP2;};
  Double_t GetMuonSigmaP0() const {return fMuonSigmaP0;};
  Double_t GetMuonSigmaP1() const {return fMuonSigmaP1;};
  
  Double_t GetPionMpvP0()   const {return fPionMpvP0;};
  Double_t GetPionMpvP1()   const {return fPionMpvP1;};
  Double_t GetPionMpvP2()   const {return fPionMpvP2;};
  Double_t GetPionSigmaP0() const {return fPionSigmaP0;};
  Double_t GetPionSigmaP1() const {return fPionSigmaP1;};
  Double_t GetPionSigmaP2() const {return fPionSigmaP2;};

  Double_t GetKaonMpvP0()   const {return fKaonMpvP0;};
  Double_t GetKaonMpvP1()   const {return fKaonMpvP1;};
  Double_t GetKaonMpvP2()   const {return fKaonMpvP2;};
  Double_t GetKaonSigmaP0() const {return fKaonSigmaP0;};
  Double_t GetKaonSigmaP1() const {return fKaonSigmaP1;};
  Double_t GetKaonSigmaP2() const {return fKaonSigmaP2;};

  Double_t GetProtonMpvP0()   const {return fProtonMpvP0;};
  Double_t GetProtonMpvP1()   const {return fProtonMpvP1;};
  Double_t GetProtonMpvP2()   const {return fProtonMpvP2;};
  Double_t GetProtonSigmaP0() const {return fProtonSigmaP0;};
  Double_t GetProtonSigmaP1() const {return fProtonSigmaP1;};
  Double_t GetProtonSigmaP2() const {return fProtonSigmaP2;};

 protected:

  Double_t fElecMpvP0;
  Double_t fElecMpvP1;
  Double_t fElecMpvP2;
  Double_t fElecSigmaP0;
  Double_t fElecSigmaP1;
 
  Double_t fPionMpvP0;
  Double_t fPionMpvP1;
  Double_t fPionMpvP2;
  Double_t fPionSigmaP0;
  Double_t fPionSigmaP1;
  Double_t fPionSigmaP2;
 
  Double_t fProtonMpvP0;
  Double_t fProtonMpvP1;
  Double_t fProtonMpvP2;
  Double_t fProtonSigmaP0;
  Double_t fProtonSigmaP1;
  Double_t fProtonSigmaP2;
 
  Double_t fMuonMpvP0;
  Double_t fMuonMpvP1;
  Double_t fMuonMpvP2;
  Double_t fMuonSigmaP0;
  Double_t fMuonSigmaP1;
 
  Double_t fKaonMpvP0;
  Double_t fKaonMpvP1;
  Double_t fKaonMpvP2;
  Double_t fKaonSigmaP0;
  Double_t fKaonSigmaP1;
  Double_t fKaonSigmaP2;

  ClassDef(PndPidMvdPar,1)   
  
    };

#endif
