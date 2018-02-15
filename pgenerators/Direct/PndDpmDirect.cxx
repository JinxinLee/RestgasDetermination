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
    double p[5000];   	
  } lujets;

 #define LUJETS COMMON_BLOCK(LUJETS,lujets)
 COMMON_BLOCK_DEF(lujets,LUJETS);

}

extern "C" int init1_(double* Plab, double* seed, double* Elastic, double* tetmin );//install DPM 
extern "C" int dpm_gen_(double* Generator, double* seed ); //to generate events
extern "C" int chstatus_(int* iPDG, int* iStatus); //to change Particle status

TF1 	 * fDensityFunction;

// -----   Default constructor   ------------------------------------------
PndDpmDirect::PndDpmDirect() {

}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------

PndDpmDirect::PndDpmDirect(Double_t Mom, Int_t Mode, Long_t Seed) {

//
// Calculate ThtMin first. For this we make a cut-off on the value of -t of 1e-2 GeV^2 (~100 MeV/c momentum)
// This estimated from a parametrization found in thesis of Thomas Wuerschig (figure 6.4, page 121): 
// Roughly: 0.4 deg at 15 GeV/c and 4 deg at 1.5 GeV/c, lineair interpolation in double log-scale.
//

  Double_t logangle = TMath::Log(0.4)+(TMath::Log(15.)-TMath::Log(Mom))*(TMath::Log(4)-TMath::Log(0.4))/(TMath::Log(15)-TMath::Log(1.5));
  Double_t ThtMin = TMath::Exp(logangle);

  PndDpmDirect(Mom, Mode, Seed, ThtMin) ;
}

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
// -----Set particle type stable ------------------------------------------
void PndDpmDirect::SetStable(int pdg)
{
	int status=1;
	chstatus_(&pdg, &status);
}

// -----Set particle type unstable ----------------------------------------
void PndDpmDirect::SetUnstable(int pdg)
{
	int status=0;
	chstatus_(&pdg, &status);
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------
Bool_t PndDpmDirect::ReadEvent(FairPrimaryGenerator* primGen) {

  int  npart, i;	
	Double_t fX, fY, fZ, radius;
  double Px[1000],Py[1000],Pz[1000];  //,E[1000],Pm[1000],Wh[1000];  //[R.K. 01/2017] unused variables
  int Id[1000];
  
  double Generator=0.; //  Format in which events are produced (0=pythia, 1=pluto)

  //Double_t weight = 1.0; //[R.K. 01/2017] unused variable?
  //Int_t activeCnt=0; //[R.K. 01/2017] unused variable?

  // run generator
	dpm_gen_(&Generator, &fSeed);
  
	// Loop over all produced particles 
   npart = lujets_.n;
      
   for (i= 0; i< npart; ++i) {        

		Id[i]=lujets_.k[i+1000];
		Px[i]=lujets_.p[i];
    Py[i]=lujets_.p[i+1000];
    Pz[i]=lujets_.p[i+2000];
    //Pm[i]=lujets_.p[i+4000];
    //E[i]=lujets_.p[i+3000];
    //Wh[i]=1.0;

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

