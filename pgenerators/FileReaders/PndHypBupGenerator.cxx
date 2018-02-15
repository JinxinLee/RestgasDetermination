// -------------------------------------------------------------------------
// -----            PndHypBupGenerator source file                 -----
// -----                Created by A. Sanchez              -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"

#include "TFile.h"
#include "TF1.h"
#include "TRandom.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "THParticle.h" //detectors/hyp
#include "PndHypBupGenerator.h"
#include "FairPrimaryGenerator.h"

using namespace std;



// -----   Default constructor   ------------------------------------------
PndHypBupGenerator::PndHypBupGenerator() {
 iEvent     = 0;
 fInputFile = NULL;
 fInputTree = NULL;
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
PndHypBupGenerator::PndHypBupGenerator(const char* fileName) {

 
  fFileName  = fileName;
   iEvent     = 0;
   fInputFile = new TFile(fFileName);
  fInputTree = (TTree*) fInputFile->Get("data");
  fParticles = new TClonesArray("THParticle",100);
  fInputTree->SetBranchAddress("Particles", &fParticles);

 
   
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
PndHypBupGenerator::~PndHypBupGenerator() {
  CloseInput();
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t PndHypBupGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for input file
  if ( ! fInputFile ) {
    cout << "-E- PndHypBupGenerator: Input file not open!" << endl;
    return kFALSE;
  }
 // Check for number of events in input file
  if ( iEvent > fInputTree->GetEntries() ) {
    cout << "-E PndDpmGenerator: No more events in input file!" << endl;
    CloseInput();
    return kFALSE;
  }
  TFile  *g=gFile;
  fInputFile->cd();
  fInputTree->GetEntry(iEvent++);
  g->cd();

   // Get number of particle in TClonesrray
  Int_t nParts = fParticles->GetEntriesFast();

  // Loop over particles in TClonesArray
  
  for (Int_t iPart=0; iPart < nParts; iPart++) {
    THParticle* part = (THParticle*) fParticles->At(iPart);
    Int_t pdgType = part->GetPdgCode();
    
    // Check if particle type is known to database
    if ( ! pdgType ) {
      cout << "-W PndDpmGenerator: Unknown type " << part->GetPdgCode()
	   << ", skipping particle." << endl;
      continue;
    }

    Double_t px = part->Px();
    Double_t py = part->Py();
    Double_t pz = part->Pz();

    // Double_t vx = part->Vx();
    //     Double_t vy = part->Vy();
    //     Double_t vz = part->Vz();//default vertex 0,0,-76.5

    Double_t vx ;
    Double_t vy ;
    Double_t vz ;

     vx = part->Vx(); 
     vy = part->Vy();
     vz = part->Vz();

    /* cout<<" track "<<iPart<<" "
	<<" pdgCode "<<pdgType<<" "
	<<"  energy "<<part->Energy()<<endl;*/
    
    //parametrisation of secondary vertex in second.target.
    //reference 0.5 mm of silicon old(geo)

    /* TF1* Zdist = new TF1("Zdist","gaus(0)",-77,-73.5);
    Zdist->SetParameters(10.1355,-75.51,0.76);
    vz = Zdist->GetRandom();
    
    TF1* totaly = new TF1("totaly","gaus(0)+gaus(3)+gaus(6)",-2.,2);
    totaly->SetParameters(8.9131,-0.635,0.6219,1.201,87.17,194,10.7,0.934,0.300);
    TF1* totalx = new TF1("totalx","gaus(0)+gaus(3)+gaus(6)",-2.,2);
    totalx->SetParameters(15.111,-1,-0.27,1,23,45,13,0.5,0.7);

    vx = totalx->GetRandom();
    vy = totaly->GetRandom();
    */
     //if(pdgType==1020030090){
       
     cout<<" Secondary vertex for stopping Xi - "<<pdgType<<" "<<vx<<" "
    <<vy<<" "<<vz<<endl;
    
    // Give track to PrimaryGenerator
    //primGen->AddTrack(pdgType, px, py, pz, 0., 0., -76.5);
     primGen->AddTrack(pdgType, px, py, pz, vx, vy,vz);
     //}
     

  }


  return kTRUE;
}
// ------------------------------------------------------------------------



// -----   Private method CloseInput   ------------------------------------
void PndHypBupGenerator::CloseInput() {
 
    if ( fInputFile) {
       cout << "-I- CbmHypBupGenerator: Closing input file " 
	    << fFileName << endl;
       fInputFile->Close();
    
       delete fInputFile;
    }
    
    fInputFile = NULL;
  }

// ------------------------------------------------------------------------



// ------------------------------------------------------------------------
ClassImp(PndHypBupGenerator)

