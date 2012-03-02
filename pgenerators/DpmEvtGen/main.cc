//  edition 2.03.2012  A.Galoyan
//  main.cc,v 1.18 2004/03/04 15:09:11 ritman Exp $
//
// C++ program to call DPM event generator for PBAP P interactions 
// Original author:   A.Galoyan        
// change to TParticle output format J.Ritman May 2003
/* ------------------------------------------------------- */

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "TROOT.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TParticle.h"


extern struct {
    int n, k[2000];  
    float p[5000];   	
} lujets_;

// n   - number of produced particles, 
// k[] - Pythia particle identifiers
// p[] - kinematical characteristics of particles

extern "C" int init1_(float* Plab, double* seed, float* Elastic, 
float* tetmin);      // to install DPM generator
extern "C" int dpm_gen__(float* Generator, double* seed);  //to generate events

 
 int main()
{
 float Plab, Elastic, tetmin;          // Plab - PBAP momentum in Lab.Sys. 
 double seed;
 int ntot, Ieven, npart, i;	
 double Px[1000],Py[1000],Pz[1000],E[1000],Pm[1000],Wh[1000];
 int Id[1000];

//	Elastic=0.;	 // No elastic scattering, only inelastic
	Elastic=1. ;     // Elastic and inelastic interactions
//	Elastic=2.;	 // Only elastic scattering, no inelastic one


 //   Root initialization 
 TFile f1("Background-micro.root","RECREATE","ROOT_Tree"); 

 float Generator=0.;
 Double_t weight = 1.0;
 Int_t activeCnt=0;
 TTree* fTree = new TTree("data","DPM Background");
 TClonesArray* fEvt;

 fEvt=new TClonesArray("TParticle",100);
 fTree->Branch("Npart",&activeCnt,"Npart/I");
 fTree->Branch("Weigth",&weight,"Weight/D");
 fTree->Branch("Seed",&seed,"Weight/D");
 fTree->Branch("Particles",&fEvt, 32000,99);


 std::cout<<" Give as seed a large float number (eg. 123456.): ";
 std::cin>>seed;
 if (!seed){  // if the seed is 0 then take the time
   Long_t Time = time(NULL);
   int a = Time/100000;
   seed = Time - a*100000 + a/100000.;
 }
 
 std::cout << " Enter  P_lab(GeV/c), ";   
 std::cin >> Plab ;    

 std::cout << " Enter  Elastic : 0., 1. or 2. " << "\n" << 
        "0. - No elastic scattering, only inelastic"<< "\n" <<
	"1. - Elastic and inelastic interactions" << "\n" <<     
	"2. - Only elastic scattering, no inelastic one"<< "\n";   
 std::cin >> Elastic ;    
 if((Elastic==1.) || (Elastic==2.)) 
 {  
    std::cout << " Teta_min (degree) ";   
    std::cin >> tetmin;    
  }
 else  {tetmin=0;}
 init1_(&Plab,&seed,&Elastic, &tetmin);  // installation of the DPM generator  
 
std::cout << " Enter  N_Events ";
 std::cin >> ntot;

 TStopwatch timer;                        // time loop
 timer.Start();

 TLorentzVector Mom; 
 TLorentzVector V(0,0,0,0);
  
// Simulation of events
 for (Ieven = 1; Ieven <= ntot; ++Ieven) {
   if( (Ieven%100) == 0 ) 
     std::cout << "Event number = " << Ieven << std::endl; 

   dpm_gen__(&Generator, &seed);
   fEvt->Clear();
   Int_t cnt = 0;

// Loop over all produced particles 
   npart = lujets_.n;
      
   for (i= 0; i< npart; ++i) {        // update TClonesArrays

//     std::cout << i  <<"  " << lujets_.k[i+1000] << "   " 
//	  << lujets_.p[i] << "  " << lujets_.p[i+1000]  <<"   "
//	  << lujets_.p[i+2000] << "  " << lujets_.p[i+3000] << "   "
//	  << lujets_.p[i+4000]  << " \n" ;
     
// i - order number of particle
// lujets_.k[i+1000] - identifier of i-th particle
// lujets_.p[i]       - Px (GeV/c) of i-th particle
// lujets_.p[i+1000] - Py (GeV/c) of i-th particle
// lujets_.p[i+2000] - Pz (GeV/c) of i-th particle
// lujets_.p[i+3000] - Energy (GeV) of i-th particle
// lujets_.p[i+4000] - Mass   (GeV) of i-th particle

     Id[i]=lujets_.k[i+1000];
     Px[i]=lujets_.p[i];
     Py[i]=lujets_.p[i+1000];
     Pz[i]=lujets_.p[i+2000];
     Pm[i]=lujets_.p[i+4000];
     E[i]=lujets_.p[i+3000];
     Wh[i]=1.0;

     Mom.SetPxPyPzE(Px[i],Py[i],Pz[i],E[i]);
     TParticle  fparticle(Id[i],1,0,0,0,0,Mom,V);
     new((*fEvt)[cnt++]) TParticle(fparticle);
}
   activeCnt = cnt;

   fTree->Fill();                     
 } 

 timer.Stop(); 

 std::cout << " ----- Realtime: "<<timer.RealTime()<<"sec"<<std::endl;
 std::cout << " ----- Cputime:  "<<timer.CpuTime()<<"sec"<<std::endl;
 std::cout << " ----- Time/Event:"<<timer.CpuTime()/ntot<<"sec"<<std::endl;
 std::cout << " ----- Speed:    "<<ntot/timer.CpuTime()<<"Hz"<<std::endl;
 std::cout << std::endl;

  f1.Write();    

}
