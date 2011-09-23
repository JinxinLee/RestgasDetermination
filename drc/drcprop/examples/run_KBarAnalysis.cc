
//======
// ROOT
//======
#include <TSystem.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>



void run_KBarAnalysis()
{
  gSystem->Load( "lib_KBarAnalysis.so" );


  Bool_t effiMode = false; // for single wavelength not necessary

  Double_t resolution = 10; // in mm


  TString inFileCore  = "kBarList_test";
  TString outFileCore = inFileCore;

  TString inDirectory = gSystem->WorkingDirectory();
  TString inFilename = inDirectory + "/" + inFileCore + ".root";

  TFile *inFile = new TFile( inFilename );
  TTree *infoTree = (TTree*) inFile->Get( "info" );


  Double_t fishtank_width, fishtank_height;
  Double_t airgap;

  infoTree->SetBranchAddress( "fishtank_width" , &fishtank_width ); // for checking of the side reflections
  infoTree->SetBranchAddress( "fishtank_height", &fishtank_height );
  infoTree->SetBranchAddress( "airgap"         , &airgap );
  infoTree->GetEntry( 0 );

  inFile->Close();


  KBarAnalysis *kBarAnalysis = new KBarAnalysis( 1 );

  kBarAnalysis->_outDirectory    = inDirectory;
  kBarAnalysis->_outFileCore     = outFileCore;
  kBarAnalysis->_fishtank_width  = fishtank_width;
  kBarAnalysis->_fishtank_height = fishtank_height;
  kBarAnalysis->_airgap          = airgap;
  kBarAnalysis->_effiMode        = effiMode;
  kBarAnalysis->_resolution      = resolution;


  TChain *chain = new TChain( "photon");
  chain->Add( inFilename );
  chain->Process( kBarAnalysis );
}
