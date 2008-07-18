// Author: R.Ritman
// Written: Nov 2002
// PndReaction Class Header

#ifndef PNDREACTION_HH
#define PNDREACTION_HH

#include <iostream>
#include <math.h>
#include "TObject.h"
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDecayChannel.h"
#include "TDatabasePDG.h"
#include "TGenPhaseSpace.h"
#include "TString.h"
#include "TRandom.h"

const Float_t hbar = 6.582122e-25; // units of (GeV s)
const Double_t c   = 299792458000.; // speed of light in mm/s
const Double_t MassWindow = 10;    // this times PoleWidth gives the total width of window about pole mass to select a mass
const Int_t numM  = 100;           // number of steps to integrate over the mass distribution
const Int_t nBins = 100;           // number of bins in the histogram


class PndReaction : public TObject  {

public:


  PndReaction( TParticle BT, TVector3 VR = TVector3(), char * OutFile = "jim++output.root");


  void Loop(Int_t nEvts);

  inline Double_t GetWeight()    { return weight; }
  inline void     UseStaticBR()  { StaticBR = 1;  }
  inline void     UsePoleMass()  { PoleMass = 1;  }

  Int_t     UseElasticDXC(); // Use Elastic Differential Cross Section for pbar-b

//private:

  Int_t     Decay                ( TParticle *mother );

  TDecayChannel *  SelectChannel ( TParticle *mother );

  Double_t  BranchRatio          ( TParticle *mother, Int_t channel );

  Double_t  PartialWidth         ( TParticle *mother, Int_t DecChan ); 

  Double_t  PW                   ( TParticle *mother, Int_t DecChan ); 
      
  Double_t  TotalWidth           ( TParticle *mother );

  Double_t  PhaseSpace           ( Double_t ecm, Int_t * array, Int_t nPart,
				   Int_t ch=0, Int_t indx=0, Int_t S=0);

  Double_t  Prob                 ( Int_t PDGCode, Double_t Mass);

  Int_t     InitHisto            ( Int_t PDGCode );

  Double_t  BreitWigner          ( Double_t m0, Double_t m, Double_t Gamma);

  Double_t  SampleMass           ( Int_t PDGCode );

  Double_t  SampleLifetime       ( TParticle *mother );

  Bool_t    Stable               ( Int_t PDGCode );

  Double_t  PoleWidth            ( Int_t PDGCode );

  Double_t  PCM                  ( Double_t Ecm, Double_t m1, Double_t m2 );

  void      DefinePartialWaves   ( Int_t PDGCode, Int_t DecayChannel, Int_t *array);

  Double_t  PWWeight             ( TGenPhaseSpace *event, Int_t PdgCode, Int_t channel, Int_t index);

  void      SetStable            (Long_t PdgCode);

  void      doElastic            (void);

  void      SetPolarization      (Long_t PdgCode, Long_t channel, Float_t Polarization);

  void      SetDecayAsymmetry    (Long_t PdgCode, Long_t channel, Float_t Asymmetry);


//
// class data members
//
private:

  TParticle        BeamTarget;
  TString          filename;
  Double_t         weight;

  TVector3         PrimaryVtx;
  TVector3         VtxResolution;
  Bool_t           VtxResFlag;

  TClonesArray*    fEvt;
  Int_t            activeCnt;

  Bool_t StaticBR;
  Bool_t PoleMass;
  Bool_t Elastic;

  TH1D * MassHistos[100];
  Int_t  PID[100];
  Int_t  numHistos;

  TH1D * PWHistos[100][20];

  Int_t PartialWaves[100][22];
  Int_t PWindex;

  Long_t PartStable[100];
  Long_t PStabindex;

  Long_t PolarIndex;
  Long_t PolarID[100];
  Float_t Polar[100];
  Float_t Asym[100];
  Int_t   DC;

  ClassDef(PndReaction,0) 
};

#endif // PNDREACTION_HH











