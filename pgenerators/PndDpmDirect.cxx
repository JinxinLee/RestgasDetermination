// -------------------------------------------------------------------------
// -----                PndDpmDirect source file                  -----
// -----                                                          -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "TParticle.h"
#include "PndDpmDirect.h"
#include "CbmPrimaryGenerator.h"
#include "TRandom.h"

using namespace std;

#include "cfortran.h"
extern "C" {
// COMMON /LUJETS/ N,K(1000,2),P(1000,5)
// n   - number of produced particles, 
// k[] - Pythia particle identifiers
// p[] - kinematical characteristics of particles


 struct {
    int n, k[2000];  
    float p[5000];   	
  } lujets_;

// #define LUJETS COMMON_BLOCK(LUJETS,lujets)
// COMMON_BLOCK_DEF(lujets,LUJETS);

}

extern "C" int init1_(float* Plab, double* seed, float* Elastic );//install DPM 
extern "C" int dpm_gen__(float* Generator, double* seed ); //to generate events


// -----   Default constructor   ------------------------------------------
PndDpmDirect::PndDpmDirect() {

}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndDpmDirect::PndDpmDirect(Double_t Mom, Int_t Mode) {
   fMom=Mom;
   fMode=Mode;

   Long_t iSeed = gRandom->GetSeed();
   int a = iSeed/100000;   
   fSeed=iSeed - a*100000 + a/100000.;

   cout << "<I> PndDpmDirect initialization" << endl;
   cout << "<I> Momentum = " << fMom << endl;
   cout << "<I> Seed     = " << fSeed << endl;
   cout << "<I> Mode     = " << fMode << endl;

   init1_(&fMom,&fSeed,&fMode);     // init the DPM generator  
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndDpmDirect::~PndDpmDirect() {

}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndDpmDirect::ReadEvent(CbmPrimaryGenerator* primGen) {

  int  npart, i;	
  double Px[1000],Py[1000],Pz[1000],E[1000],Pm[1000],Wh[1000];
  int Id[1000];
  
  float Generator=0.; //  Format in which events are produced (0=pythia, 1=pluto)

  Double_t weight = 1.0;
  Int_t activeCnt=0;

  dpm_gen__(&Generator, &fSeed);
  // Loop over all produced particles 
   npart = lujets_.n;
      
   for (i= 0; i< npart; ++i) {        

    Id[i]=lujets_.k[i+1000];
     Px[i]=lujets_.p[i];
     Py[i]=lujets_.p[i+1000];
     Pz[i]=lujets_.p[i+2000];
     Pm[i]=lujets_.p[i+4000];
     E[i]=lujets_.p[i+3000];
     Wh[i]=1.0;

  
     primGen->AddTrack(Id[i], Px[i], Py[i], Pz[i], 0, 0, 0);

   }

  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(PndDpmDirect)

