////////////////////////////////////////////////////////////////
// ----     Interface to start Multfrag code --------         //
// ----     Author : Alicia Sanchez          -------          //
//                                                            // 
////////////////////////////////////////////////////////////////

#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TParticle.h"
#include <vector>

using namespace std;
#include "cfortran.h"
extern "C" {
extern struct {
  int n, AG[1000], ZG[1000], HG[1000];  
  float PXs[1000], PYs[1000], PZs[1000],
   EP[1000],GM[1000] ;   	
} frag_;
}

extern "C" void initia_(double* she,int*an, int*zn, int*hn);//install DPM 
extern "C" void razhyp_gen__(double* shell ); //to generate events

int main()
{

  float Plab, Elastic;                    // Plab  momentum in Lab.Sys.
  double shell,she;
  int ntot, Ieven, npart, i;//,A[1000],Z[1000],H[1000];	
  //float Px[1000],Py[1000],Pz[1000],E[1000],M[1000],Wh[1000];
  //int Id[1000];
  int an,zn,hn;
  
  std::vector<double> fPx;
  std::vector<double> fPy;
  std::vector<double> fPz;
  //Double fPhx[1000],fPhy[1000],fPhz[1000];
  std::vector<double> fMass;
  std::vector<double>fEx; std::vector<int>fPid;
  std::vector<int> A;
  std::vector<int>Z;
  std::vector<int>H;
  //const Char_t* fFileName;

  char* fFileName;
  
  /* TFile f1("Background-micro.root","RECREATE","ROOT_Tree"); 

 float Generator=0.;

 Double_t weight = 1.0;
 Int_t activeCnt=0;
 TTree* fTree = new TTree("data","DPM Background");
 TClonesArray* fEvt;

 fEvt=new TClonesArray("TParticle",100);
 fTree->Branch("Npart",&activeCnt,"Npart/I");
 fTree->Branch("Weigth",&weight,"Weight/D");
 fTree->Branch("Seed",&seed,"Weight/D");

 fTree->Branch("Particles",&fEvt, 32000,99);*/

  
 
  she= 3.;
  //an=13;
  //zn=5;
  //hn=2;
  
 
 std::cout << " Enter  N_Events ";
 std::cin >> ntot;
 std::cout << " Enter  target name ";
 std::cin >> fFileName;
 
 if(strcmp(fFileName,"12C")==0){
   std::cout<<" 12C + Xi- --> 13BLL "<<std::endl;
   an = 13;
   zn = 5;
   hn =2;
 } else if(strcmp(fFileName,"13C")==0){
   std::cout<<" 13C + Xi- --> 14BLL "<<std::endl;
   an = 14;
   zn = 5;
   hn =2;
 } else if(strcmp(fFileName,"10B")==0){
   std::cout<<" 10B + Xi- --> 11BeLL "<<std::endl;
   an = 11;
   zn = 4;
   hn =2;
 } else if(strcmp(fFileName,"11B")==0){
   std::cout<<" 11B + Xi- --> 12BeLL "<<std::endl;
   an = 12;
   zn = 4;
   hn =2;
 } else if(strcmp(fFileName,"9Be")==0){
   std::cout<<" 9Be + Xi- --> 10LiLL "<<std::endl;
   an = 10;
   zn = 3;
   hn =2;
 }

 initia_(&she,&an,&zn,&hn);     // installation of the DPM generator  

 TLorentzVector Mom; 
 TLorentzVector V(0,0,0,0);

// Simulation of events
 for (Ieven = 1; Ieven <= ntot; ++Ieven) {
   //if( (Ieven%100) == 0 ) 
   std::cout << "Event number = " << Ieven << std::endl; 
   
   fPx.clear();fPy.clear();fPz.clear();
   A.clear();Z.clear();H.clear();
   fMass.clear();fEx.clear();
   fPid.clear();



   razhyp_gen__(&she);

   //fEvt->Clear();
   Int_t cnt = 0;

// Loop over all produced particles 
   npart = frag_.n;
   cout<<" npart razhyp "<<npart<<" "<<frag_.AG[0]<<endl;
   for (i= 0; i< npart; ++i) {        // update TClonesArrays

     A.push_back(frag_.AG[i]);
     Z.push_back(frag_.ZG[i]);
     H.push_back(frag_.HG[i]);
     fPx.push_back(frag_.PXs[i]);
     fPy.push_back(frag_.PYs[i]);
     fPz.push_back(frag_.PZs[i]);
     fMass.push_back(frag_.GM[i]);
     fEx.push_back(frag_.EP[i]);

     //Wh[i]=1.0;
     std::cout << " A "<<A[i]<<" "
	       <<" z "<<Z[i]<<" "
	       <<" H "<<H[i]<<std::endl;  

     std::cout << " Px "<< fPx[i]<<" "
	       <<" Py "<<fPy[i]<<" "
	       <<" Pz "<<fPz[i]<<std::endl; 
     //std::cout << " M "<< M[i]<<" "
     //	<<" Ex "<<E[i]<<std::endl;

     /*
      Mom.SetPxPyPzE(fPx[i],fPy[i],fPz[i],fEx[i]);
     TParticle  fparticle(fPid[i],1,0,0,0,0,Mom,V);
     new((*fEvt)[cnt++]) TParticle(fparticle);*/
      
   }

   // activeCnt = cnt;
   
   //fTree->Fill();        //!
 } 

 //return 0;
 //f1.Write();  
}
