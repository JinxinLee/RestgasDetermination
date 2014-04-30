// -------------------------------------------------------------------------
// -----            PndCorrDistGenerator source file                 -----
// -----          Created 23/05/07 by Galoyan Aida                     -----
// -------------------------------------------------------------------------


#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TRandom.h"
#include "TMath.h"
#include "TH2F.h"
#include "TVector3.h"
//#include "THParticle.h"
#include "PndCorrDistGenerator.h"
#include "FairPrimaryGenerator.h"


using namespace std;

// -----   Default constructor   ------------------------------------------
PndCorrDistGenerator::PndCorrDistGenerator() {
  iEvent     = 0;
  fInputFile = NULL;
  fInputHist = NULL;
  fPdgType   = 0;
}
// ------------------------------------------------------------------------


// 
// -----   Standard constructor   -----------------------------------------
PndCorrDistGenerator::PndCorrDistGenerator(const Char_t* fileName) {
  iEvent     = 0;
  fFileName  = fileName;
  fInputFile = new TFile(fFileName);
  fInputHist = (TH2F*) fInputFile->Get("source");
  
}
// ------------------------------------------------------------------------


// -----   Destructor   ---------------------------------------------------
PndCorrDistGenerator::~PndCorrDistGenerator() {
  CloseInput();
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------
Bool_t PndCorrDistGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

  // Check for input file
  if ( ! fInputFile ) {
    cout << "-E PndCorrDistGenerator: Input file nor open!" << endl;
    return kFALSE;
  }
  
  //defining kinematics variables, angle is expressed in degrees.

  Double_t phi, p=0.0,theta=0.0;
  
  //getting p and theta randomly from a TH2F object
  //both variables are correlated to each other
  //phi is independtly generated over the full angular range.

  fInputHist->GetRandom2(theta,p);

  phi = gRandom->Uniform(0,360)* TMath::DegToRad();

    Double_t pz = p*TMath::Cos(theta* TMath::DegToRad());
    Double_t py = p*TMath::Sin(theta* TMath::DegToRad())*TMath::Sin(phi);
    Double_t px = p*TMath::Sin(theta* TMath::DegToRad())*TMath::Cos(phi);

    // vertex coordinates given during generation via prim generator

    // Give track to PrimaryGenerator
    // multiplocity equal to 1
    // pdg is -3312, corresponding to a Xi minus
    
    fPdgType==3312;

    //cout<<" generating Xi minus "<<px<<" "<<py<<" "<<pz<<endl;
    
    primGen->AddTrack(3312, px, py, pz, 0., 0., -55.5);



  return kTRUE;

}
// ------------------------------------------------------------------------


// -----   Private method CloseInput   ------------------------------------
void PndCorrDistGenerator::CloseInput() {
  if ( fInputFile ) {
    cout << "-I PndCorrDistGenerator: Closing input file " << fFileName
	 << endl;
    fInputFile->Close();
    delete fInputFile;
  }
  fInputFile = NULL;
}
// ------------------------------------------------------------------------


ClassImp(PndCorrDistGenerator)
