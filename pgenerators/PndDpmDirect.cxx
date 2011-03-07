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
#include "FairPrimaryGenerator.h"
#include "TRandom.h"

using namespace std;

#include "cfortran.h"
extern "C" {
// COMMON /LUJETS/ N,K(1000,2),P(1000,5)
// n   - number of produced particles, 
// k[] - Pythia particle identifiers
// p[] - kinematical characteristics of particles


 typedef struct {
    int n, k[2000];  
    float p[5000];   	
  } lujets;

 #define LUJETS COMMON_BLOCK(LUJETS,lujets)
 COMMON_BLOCK_DEF(lujets,LUJETS);

}

extern "C" int init1_(float* Plab, double* seed, float* Elastic, float* tetmin );//install DPM 
extern "C" int dpm_gen__(float* Generator, double* seed ); //to generate events

TF1 	 * fDensityFunction;

// -----   Default constructor   ------------------------------------------
PndDpmDirect::PndDpmDirect() {

}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndDpmDirect::PndDpmDirect(Double_t Mom, Int_t Mode, Long_t Seed, Double_t ThtMin) {
   fMom    = Mom;
   fMode   = Mode;
   fSeed   = Seed;
   fThtMin = ThtMin;
      
   if (fSeed < 0)
   {
     Long_t iSeed = gRandom->GetSeed();
     int a = iSeed/100000;   
     fSeed=iSeed - a*100000 + a/100000.;
   }
   
   fGasmode = 0;
   fRsigma = 0.;
   fDensityFunction = new TF1();

   cout << "<I> PndDpmDirect initialization" << endl;
   cout << "<I> Momentum  = " << fMom << endl;
   cout << "<I> Seed      = " << fSeed << endl;
   cout << "<I> Mode      = " << fMode << endl; 
   cout << "<I> Theta min = " << fThtMin<<endl;

   init1_(&fMom,&fSeed,&fMode, &fThtMin);     // init the DPM generator  
}
// ------------------------------------------------------------------------

// -----   Gas mode constructor   -----------------------------------------
PndDpmDirect::PndDpmDirect(Double_t Mom, Int_t Mode, Double_t Rsigma, TF1* DensityFunction, Long_t Seed, Double_t ThtMin) {
   fMom    = Mom;
   fMode   = Mode;
   fSeed   = Seed;
   fThtMin = ThtMin;
   
   if (fSeed < 0)
   {
   	 Long_t iSeed = gRandom->GetSeed();
     int a = iSeed/100000;   
     fSeed=iSeed - a*100000 + a/100000.;
   }
   
   fGasmode = 1;
   fRsigma = Rsigma;
   fDensityFunction = DensityFunction;

   cout << "<I> PndDpmDirect initialization" << endl;
   cout << "<I> Momentum  = " << fMom << endl;
   cout << "<I> Seed      = " << fSeed << endl;
   cout << "<I> Mode      = " << fMode << endl;
   cout << "<I> Gasmode   = " << fGasmode << endl;
   cout << "<I> Theta min = " << fThtMin<<endl;

   init1_(&fMom,&fSeed,&fMode, &fThtMin);     // init the DPM generator     
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndDpmDirect::~PndDpmDirect() {

}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndDpmDirect::ReadEvent(FairPrimaryGenerator* primGen) {

  int  npart, i;	
	Double_t fX, fY, fZ, radius;
  double Px[1000],Py[1000],Pz[1000],E[1000],Pm[1000],Wh[1000];
  int Id[1000];
  
  float Generator=0.; //  Format in which events are produced (0=pythia, 1=pluto)

  Double_t weight = 1.0;
  Int_t activeCnt=0;

  // run generator
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

    /* Check if fGasmode is set */
    fX = 0.;
    fY = 0.;
    fZ = 0.;
		if (fGasmode == 1) {
     
     	// define position of track start
     	// Random 2D point in a circle of radius r (simple beamprofile)     	
     	radius = gRandom->Gaus(0,fRsigma);
			gRandom->Circle(fX, fY, radius);
        
			// calculate fZ according to some (probability) density function of the gas
    	fZ=fDensityFunction->GetRandom();
		     	
    }
		
		// add track
		//printf("- I -: new particle at: %f, %f, %f ...\n", fX, fY, fZ);
		primGen->AddTrack(Id[i], Px[i], Py[i], Pz[i], fX, fY, fZ);

   }

  return kTRUE;

}
// ------------------------------------------------------------------------

ClassImp(PndDpmDirect)

