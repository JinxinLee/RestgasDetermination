
//======
// ROOT
//======
#include <TSystem.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>


void run_EffiContiAll()
{
//   TString inFileCore  = "beamtest1106_f250_lens9_gap20_test_theta";
//   TString outFileCore = "beamtest1106_f250_lens9_gap20_test";

//   TString inFileCore  = "beamtest1106_f250_lens9_gap20_test_FSoil_theta";
//   TString outFileCore = "beamtest1106_f250_lens9_gap20_test_FSoil";

//   TString inFileCore  = "beamtest1106_noLens_gap0_test_theta";
//   TString outFileCore = "beamtest1106_noLens_gap0_test";

//   TString inFileCore  = "beamtest1106_noLens_gap0_test_FSoil_theta";
//   TString outFileCore = "beamtest1106_noLens_gap0_test_FSoil";

//   TString inFileCore  = "beamtest1106_noLens_gap11_test_theta";
//   TString outFileCore = "beamtest1106_noLens_gap11_test";

  TString inFileCore  = "beamtest1106_noLens_gap11_test_FSoil_theta";
  TString outFileCore = "beamtest1106_noLens_gap11_test_FSoil";


  gSystem->Load( "/u/rhohler/src/examples/EffiContiAll_cc.so" ); // for batch-jobs

  EffiContiAll *effiContiAll = new EffiContiAll();


  TString inDirectory = "/d/panda02/rhohler/beamtest1106/test/";
  TString inFilename = inDirectory + inFileCore + "0.root";
  TFile *inFile = new TFile( inFilename );
  TTree *infoTree = (TTree*) inFile->Get("info");


  const int pos_size;
  Double_t frontLens_thickness = -666, frontLens_diameter = -666, airgap = -666;
  Double_t slab_width= -666, slab_height = -666, fishtank_width = -666, fishtank_height = -666;
  Int_t refl_limit = -666;

  infoTree->SetBranchAddress( "pos_size"            , &pos_size );
  infoTree->SetBranchAddress( "frontLens_thickness" , &frontLens_thickness );
  infoTree->SetBranchAddress( "frontLens_diameter"  , &frontLens_diameter );
  infoTree->SetBranchAddress( "airgap"              , &airgap );
  infoTree->SetBranchAddress( "slab_width"          , &slab_width );
  infoTree->SetBranchAddress( "slab_height"         , &slab_height );
  infoTree->SetBranchAddress( "fishtank_width"      , &fishtank_width );
  infoTree->SetBranchAddress( "fishtank_height"     , &fishtank_height );
  infoTree->SetBranchAddress( "refl_limit"          , &refl_limit );
  infoTree->GetEntry( 0 );

  inFile->Close();


  if( pos_size == -666 )
  {
    cout << "No photon position list included!" << endl;
    return;
  }


  bool b_frontLens = true;
  bool b_airgap    = true;

  if( frontLens_thickness == -666 )
    b_frontLens = false;

  if( airgap == -666 )
    b_airgap = false;


  Double_t tiny = 0.00001; // unclear when a tiny shift is used


  effiContiAll->_outDirectory        = "/d/panda02/rhohler/beamtest1106/test/"; // inDirectory
  effiContiAll->_outFileCore         = outFileCore;
  effiContiAll->_frontLens_thickness = frontLens_thickness;
  effiContiAll->_frontLens_diameter  = frontLens_diameter;
  effiContiAll->_airgap              = airgap;
  effiContiAll->_slab_width          = slab_width;
  effiContiAll->_slab_height         = slab_height;
  effiContiAll->_fishtank_width      = fishtank_width;
  effiContiAll->_fishtank_height     = fishtank_height;
  effiContiAll->_refl_limit          = refl_limit;
  effiContiAll->_b_frontLens         = b_frontLens;
  effiContiAll->_b_airgap            = b_airgap;
  effiContiAll->_tiny                = tiny;


  TChain * chain = new TChain( "photon" );

  for( int i=0; i<66; i+=5) //  default 66
  {
    TString i_str;
    i_str += i;
    i_str.Remove( TString::kLeading, ' ' );

    TString file_str = inDirectory + inFileCore + i_str + ".root";
    chain->Add( file_str );
  }

  chain->Process( effiContiAll );
}

