
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
//   TString inFileCore  = "kBarList_center_400nm_2000000_f250_lens9_gap11";
//   TString inFileCore  = "kBarList_center_400nm_2000000_f250_lens9_gap11_z6.3_TEST";
  TString inFileCore  = "kBarList_center_400nm_2000000_woLens";
//   TString outFileCore = "kBarList_center_400nm_2000000_f250_lens9_gap11_res20";
  TString outFileCore = "TEST_woLens";

  Bool_t mcpMode  = false;
  Bool_t effiMode = true;

  Double_t resolution = 10; // resolution valid for no mcpMode

  Double_t gap1 = 40; // redefine gap
  Double_t gap2 = 40;

  Int_t n_mcp = 9; // number of MCPs

  if( !mcpMode )
    n_mcp = 1;


//     gSystem->CompileMacro( "KBarAnalysis.cc" );
  gSystem->Load( "/u/rhohler/src/examples/KBarAnalysis_cc.so" ); // for batch-jobs

  KBarAnalysis *kBarAnalysis = new KBarAnalysis( n_mcp );


  TString inDirectory = "/d/panda02/rhohler/beamtest1106/sim/";
  TString inFilename = inDirectory + inFileCore + "_001.root";
  TFile *inFile = new TFile( inFilename );
  TTree *infoTree = (TTree*) inFile->Get("info");


  Double_t fishtank_width, fishtank_height;
  Double_t airgap;

  infoTree->SetBranchAddress( "fishtank_width" , &fishtank_width );
  infoTree->SetBranchAddress( "fishtank_height", &fishtank_height );
  infoTree->SetBranchAddress( "airgap"         , &airgap );
  infoTree->GetEntry( 0 );

  inFile->Close();


  kBarAnalysis->_outDirectory    = "/d/panda02/rhohler/beamtest1106/sim/"; // inDirectory
  kBarAnalysis->_outFileCore     = outFileCore;
  kBarAnalysis->_fishtank_width  = fishtank_width;
  kBarAnalysis->_fishtank_height = fishtank_height;
  kBarAnalysis->_airgap          = airgap;
  kBarAnalysis->_mcpMode         = mcpMode;
  kBarAnalysis->_effiMode        = effiMode;

  if( mcpMode )
  {
    // XP85012 1
    kBarAnalysis->_mcp_dim[0]    = 59; // MCP case
    kBarAnalysis->_mcp_active[0] = 52; // MCP active area (anode) defined by pixel pitch; cathode: 53
    kBarAnalysis->_minX_dim[0]   = -99 - gap1; // position: left, lower corner
    kBarAnalysis->_minY_dim[0]   = -202;
    kBarAnalysis->_x_bins[0]     = 8;
    kBarAnalysis->_y_bins[0]     = 8;

    // XP85012 2
    kBarAnalysis->_mcp_dim[1]    = 59;
    kBarAnalysis->_mcp_active[1] = 52; // cathode: 53
    kBarAnalysis->_minX_dim[1]   = -74 - gap1;
    kBarAnalysis->_minY_dim[1]   = -133;
    kBarAnalysis->_x_bins[1]     = 8;
    kBarAnalysis->_y_bins[1]     = 8;

    // XP85012 3
    kBarAnalysis->_mcp_dim[2]    = 59;
    kBarAnalysis->_mcp_active[2] = 52; // cathode: 53
    kBarAnalysis->_minX_dim[2]   = -59 - gap1;
    kBarAnalysis->_minY_dim[2]   = -64;
    kBarAnalysis->_x_bins[2]     = 8;
    kBarAnalysis->_y_bins[2]     = 8;

    // XP85012 4
    kBarAnalysis->_mcp_dim[3]    = 59;
    kBarAnalysis->_mcp_active[3] = 52; // cathode: 53
    kBarAnalysis->_minX_dim[3]   = -59 - gap1;
    kBarAnalysis->_minY_dim[3]   =  +5;
    kBarAnalysis->_x_bins[3]     = 8;
    kBarAnalysis->_y_bins[3]     = 8;

    // SiPM
    kBarAnalysis->_mcp_dim[4]    = 120;
    kBarAnalysis->_mcp_active[4] = 84; // light catcher is used
    kBarAnalysis->_minX_dim[4]   = 0 + gap2;
    kBarAnalysis->_minY_dim[4]   = +75;
    kBarAnalysis->_x_bins[4]     = 8;
    kBarAnalysis->_y_bins[4]     = 8;

    // SL10 1
    kBarAnalysis->_mcp_dim[5]    = 27.5;
    kBarAnalysis->_mcp_active[5] = 20.32; // cathode: 22
    kBarAnalysis->_minX_dim[5]   = +15 + gap2;
    kBarAnalysis->_minY_dim[5]   = +5;
    kBarAnalysis->_x_bins[5]     = 4;
    kBarAnalysis->_y_bins[5]     = 4;

    // SL10 2
    kBarAnalysis->_mcp_dim[6]    = 27.5;
    kBarAnalysis->_mcp_active[6] = 20.32; // cathode: 22
    kBarAnalysis->_minX_dim[6]   = +15 + gap2;
    kBarAnalysis->_minY_dim[6]   = -32.5;
    kBarAnalysis->_x_bins[6]     = 4;
    kBarAnalysis->_y_bins[6]     = 1;

    // XP85012 5
    kBarAnalysis->_mcp_dim[7]    = 59;
    kBarAnalysis->_mcp_active[7] = 52; // cathode: 53
    kBarAnalysis->_minX_dim[7]   = +15 + gap2;
    kBarAnalysis->_minY_dim[7]   = -133;
    kBarAnalysis->_x_bins[7]     = 8;
    kBarAnalysis->_y_bins[7]     = 8;

    // H8500
    kBarAnalysis->_mcp_dim[8]    = 52;
    kBarAnalysis->_mcp_active[8] = 48.64; // cathode: 49
    kBarAnalysis->_minX_dim[8]   = +40 + gap2;
    kBarAnalysis->_minY_dim[8]   = -195;
    kBarAnalysis->_x_bins[8]     = 8;
    kBarAnalysis->_y_bins[8]     = 8;


    cout << "MCP gap is : " << gap1 << " \& " << gap2 << "mm" << endl;

    TString gap1_str;
    gap1_str += gap1;
    TString gap2_str;
    gap2_str += gap2;
    gap1_str.Remove( TString::kLeading, ' ' );
    gap2_str.Remove( TString::kLeading, ' ' );
    TString gap_str = gap1_str + "_" + gap2_str;
    kBarAnalysis->_gap_str = gap_str;
  }
  else
    kBarAnalysis->_resolution = resolution; // in mm


  TChain * chain = new TChain( "photon" );

  for( int i=1; i<2; i++) // default 50 files
  {
    TString i_str;
    i_str += i;
    i_str.Remove( TString::kLeading, ' ' );

    if( i_str.Length() == 1 )
      i_str = "00" + i_str;
    else if( i_str.Length() == 2 )
      i_str = "0" + i_str;

    TString file_str = inDirectory + inFileCore + "_" + i_str + ".root";
    chain->Add( file_str );
  }

  chain->Process( kBarAnalysis );

    //     gSystem->Unload("KBarAnalysis_cc.so"); // problem to unload the shared library
}
