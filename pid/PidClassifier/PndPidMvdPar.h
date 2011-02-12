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

  Float_t GetElectronMpv(Float_t momentum);
  Float_t GetElectronSigma(Float_t momentum);

  Float_t GetMuonMpv(Float_t momentum);
  Float_t GetMuonSigma(Float_t momentum);

  Float_t GetPionMpv(Float_t momentum);
  Float_t GetPionSigma(Float_t momentum);

  Float_t GetProtonMpv(Float_t momentum);
  Float_t GetProtonSigma(Float_t momentum);

  Float_t GetKaonMpv(Float_t momentum);
  Float_t GetKaonSigma(Float_t momentum);

  void SetElectronPara();
  void SetMuonPara();
  void SetPionPara();
  void SetKaonPara();
  void SetProtonPara();

  Float_t GetElectronMpvP0()   const {return fElecMpvP0;};
  Float_t GetElectronMpvP1()   const {return fElecMpvP1;};
  Float_t GetElectronMpvP2()   const {return fElecMpvP2;};
  Float_t GetElectronSigmaP0() const {return fElecSigmaP0;};
  Float_t GetElectronSigmaP1() const {return fElecSigmaP1;};

  Float_t GetMuonMpvP0()   const {return fMuonMpvP0;};
  Float_t GetMuonMpvP1()   const {return fMuonMpvP1;};
  Float_t GetMuonMpvP2()   const {return fMuonMpvP2;};
  Float_t GetMuonSigmaP0() const {return fMuonSigmaP0;};
  Float_t GetMuonSigmaP1() const {return fMuonSigmaP1;};
  
  Float_t GetPionMpvP0()   const {return fPionMpvP0;};
  Float_t GetPionMpvP1()   const {return fPionMpvP1;};
  Float_t GetPionMpvP2()   const {return fPionMpvP2;};
  Float_t GetPionSigmaP0() const {return fPionSigmaP0;};
  Float_t GetPionSigmaP1() const {return fPionSigmaP1;};
  Float_t GetPionSigmaP2() const {return fPionSigmaP2;};

  Float_t GetKaonMpvP0()   const {return fKaonMpvP0;};
  Float_t GetKaonMpvP1()   const {return fKaonMpvP1;};
  Float_t GetKaonMpvP2()   const {return fKaonMpvP2;};
  Float_t GetKaonSigmaP0() const {return fKaonSigmaP0;};
  Float_t GetKaonSigmaP1() const {return fKaonSigmaP1;};
  Float_t GetKaonSigmaP2() const {return fKaonSigmaP2;};

  Float_t GetProtonMpvP0()   const {return fProtonMpvP0;};
  Float_t GetProtonMpvP1()   const {return fProtonMpvP1;};
  Float_t GetProtonMpvP2()   const {return fProtonMpvP2;};
  Float_t GetProtonSigmaP0() const {return fProtonSigmaP0;};
  Float_t GetProtonSigmaP1() const {return fProtonSigmaP1;};
  Float_t GetProtonSigmaP2() const {return fProtonSigmaP2;};

 protected:

  Float_t fElecMpvP0;
  Float_t fElecMpvP1;
  Float_t fElecMpvP2;
  Float_t fElecSigmaP0;
  Float_t fElecSigmaP1;
 
  Float_t fPionMpvP0;
  Float_t fPionMpvP1;
  Float_t fPionMpvP2;
  Float_t fPionSigmaP0;
  Float_t fPionSigmaP1;
  Float_t fPionSigmaP2;
 
  Float_t fProtonMpvP0;
  Float_t fProtonMpvP1;
  Float_t fProtonMpvP2;
  Float_t fProtonSigmaP0;
  Float_t fProtonSigmaP1;
  Float_t fProtonSigmaP2;
 
  Float_t fMuonMpvP0;
  Float_t fMuonMpvP1;
  Float_t fMuonMpvP2;
  Float_t fMuonSigmaP0;
  Float_t fMuonSigmaP1;
 
  Float_t fKaonMpvP0;
  Float_t fKaonMpvP1;
  Float_t fKaonMpvP2;
  Float_t fKaonSigmaP0;
  Float_t fKaonSigmaP1;
  Float_t fKaonSigmaP2;

  ClassDef(PndPidMvdPar,1)   
  
    };

#endif
