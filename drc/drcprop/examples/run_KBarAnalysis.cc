
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
  TString inFileCore  = "kBarList_center_400nm_2000000_f250_lens9_gap20";
//   TString inFileCore  = "kBarList_center_400nm_2000000_f250_lens9_gap11_z6.3_TEST";
//   TString inFileCore  = "kBarList_center_400nm_2000000_woLens";
//   TString outFileCore = "kBarList_center_400nm_2000000_f250_lens9_gap11_res20";
  TString outFileCore = inFileCore;


  Bool_t mcpMode  = true;
  Bool_t effiMode = true;

  Double_t resolution = 10; // resolution valid for no mcpMode

  Double_t gap1 = 45; // redefine gap
  Double_t gap2 = 40;

  Int_t n_mcp = 11; // number of MCPs

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
    // right mask (back view)
    kBarAnalysis->_mcpLabel_str[5] = "XP85012 #1 (MCP 5)";
    kBarAnalysis->_mcp_dim[5] = 59;
    kBarAnalysis->_mcp_act[5] = 53;
    kBarAnalysis->_mcp_det[5] = 52;
    kBarAnalysis->_x_bins[5] = 8;
    kBarAnalysis->_y_bins[5] = 8;
    kBarAnalysis->_minX_dim[5] = +50 + gap1;
    kBarAnalysis->_minY_dim[5] = -230;
    kBarAnalysis->_shiftX[5] = ( kBarAnalysis->_mcp_dim[5] - kBarAnalysis->_mcp_det[5] ) / 2;
    kBarAnalysis->_shiftY[5] = ( kBarAnalysis->_mcp_dim[5] - kBarAnalysis->_mcp_det[5] ) / 2;

    kBarAnalysis->_mcpLabel_str[4] = "XP85012 #2 (MCP 4)";
    kBarAnalysis->_mcp_dim[4] = 59;
    kBarAnalysis->_mcp_act[4] = 53;
    kBarAnalysis->_mcp_det[4] = 52;
    kBarAnalysis->_x_bins[4] = 8;
    kBarAnalysis->_y_bins[4] = 8;
    kBarAnalysis->_minX_dim[4] = +21 + gap1;
    kBarAnalysis->_minY_dim[4] = -148;
    kBarAnalysis->_shiftX[4] = ( kBarAnalysis->_mcp_dim[4] - kBarAnalysis->_mcp_det[4] ) / 2;
    kBarAnalysis->_shiftY[4] = ( kBarAnalysis->_mcp_dim[4] - kBarAnalysis->_mcp_det[4] ) / 2;

    kBarAnalysis->_mcpLabel_str[3] = "XP85012 #3 (MCP 3)";
    kBarAnalysis->_mcp_dim[3] = 59;
    kBarAnalysis->_mcp_act[3] = 53;
    kBarAnalysis->_mcp_det[3] = 52;
    kBarAnalysis->_x_bins[3] = 8;
    kBarAnalysis->_y_bins[3] = 8;
    kBarAnalysis->_minX_dim[3] = +0 + gap1;
    kBarAnalysis->_minY_dim[3] = -69;
    kBarAnalysis->_shiftX[3] = ( kBarAnalysis->_mcp_dim[3] - kBarAnalysis->_mcp_det[3] ) / 2;
    kBarAnalysis->_shiftY[3] = ( kBarAnalysis->_mcp_dim[3] - kBarAnalysis->_mcp_det[3] ) / 2;

    kBarAnalysis->_mcpLabel_str[2] = "XP85012 #4 (MCP 2)";
    kBarAnalysis->_mcp_dim[2] = 59;
    kBarAnalysis->_mcp_act[2] = 53;
    kBarAnalysis->_mcp_det[2] = 52;
    kBarAnalysis->_x_bins[2] = 8;
    kBarAnalysis->_y_bins[2] = 8;
    kBarAnalysis->_minX_dim[2] = +0 + gap1;
    kBarAnalysis->_minY_dim[2] = +10;
    kBarAnalysis->_shiftX[2] = ( kBarAnalysis->_mcp_dim[2] - kBarAnalysis->_mcp_det[2] ) / 2;
    kBarAnalysis->_shiftY[2] = ( kBarAnalysis->_mcp_dim[2] - kBarAnalysis->_mcp_det[2] ) / 2;

    kBarAnalysis->_mcpLabel_str[1] = "XP85012 #5 (MCP 1)";
    kBarAnalysis->_mcp_dim[1] = 59;
    kBarAnalysis->_mcp_act[1] = 53;
    kBarAnalysis->_mcp_det[1] = 52;
    kBarAnalysis->_x_bins[1] = 8;
    kBarAnalysis->_y_bins[1] = 8;
    kBarAnalysis->_minX_dim[1] = +21 + gap1;
    kBarAnalysis->_minY_dim[1] = +89;
    kBarAnalysis->_shiftX[1] = ( kBarAnalysis->_mcp_dim[1] - kBarAnalysis->_mcp_det[1] ) / 2;
    kBarAnalysis->_shiftY[1] = ( kBarAnalysis->_mcp_dim[1] - kBarAnalysis->_mcp_det[1] ) / 2;


    kBarAnalysis->_mcpLabel_str[0] = "XP85012 #6 (MCP 0)";
    kBarAnalysis->_mcp_dim[0] = 59;
    kBarAnalysis->_mcp_act[0] = 53;
    kBarAnalysis->_mcp_det[0] = 52;
    kBarAnalysis->_x_bins[0] = 8;
    kBarAnalysis->_y_bins[0] = 8;
    kBarAnalysis->_minX_dim[0] = +50 + gap1;
    kBarAnalysis->_minY_dim[0] = +168;
    kBarAnalysis->_shiftX[0] = ( kBarAnalysis->_mcp_dim[0] - kBarAnalysis->_mcp_det[0] ) / 2;
    kBarAnalysis->_shiftY[0] = ( kBarAnalysis->_mcp_dim[0] - kBarAnalysis->_mcp_det[0] ) / 2;


// // left mask (back view) (ask Greg for measured values)
    kBarAnalysis->_mcpLabel_str[6] = "H8500 (MCP 6)";
    kBarAnalysis->_mcp_dim[6] = 52;
    kBarAnalysis->_mcp_act[6] = 49;
    kBarAnalysis->_mcp_det[6] = 48.64;
    kBarAnalysis->_x_bins[6] = 8;
    kBarAnalysis->_y_bins[6] = 8;
    kBarAnalysis->_minX_dim[6] = -107 - gap2;
    kBarAnalysis->_minY_dim[6] = -225;
    kBarAnalysis->_shiftX[6] = ( kBarAnalysis->_mcp_dim[6] - kBarAnalysis->_mcp_det[6] ) / 2;
    kBarAnalysis->_shiftY[6] = ( kBarAnalysis->_mcp_dim[6] - kBarAnalysis->_mcp_det[6] ) / 2;

//     kBarAnalysis->_mcpLabel_str[7] = "XP85012 #7 (MCP 7)";
//     kBarAnalysis->_mcp_dim[7] = 59;
//     kBarAnalysis->_mcp_act[7] = 53;
//     kBarAnalysis->_mcp_det[7] = 52;
//     kBarAnalysis->_x_bins[7] = 8;
//     kBarAnalysis->_y_bins[7] = 8;
//     kBarAnalysis->_minX_dim[7] = -81 - gap2;
//     kBarAnalysis->_minY_dim[7] = -149;
//     kBarAnalysis->_shiftX[7] = ( kBarAnalysis->_mcp_dim[7] - kBarAnalysis->_mcp_det[7] ) / 2;
//     kBarAnalysis->_shiftY[7] = ( kBarAnalysis->_mcp_dim[7] - kBarAnalysis->_mcp_det[7] ) / 2;
//
//     kBarAnalysis->_mcpLabel_str[8] = "SL10 #1 (MCP 8)";
//     kBarAnalysis->_mcp_dim[8] = 60; // big electronic
//     kBarAnalysis->_mcp_act[8] = 22;
//     kBarAnalysis->_mcp_det[8] = 20.32;
//     kBarAnalysis->_x_bins[8] = 4;
//     kBarAnalysis->_y_bins[8] = 4;
//     kBarAnalysis->_minX_dim[8] = -61. - gap2;
//     kBarAnalysis->_minY_dim[8] = -69.;
//     kBarAnalysis->_shiftX[8] = 19.84;
//     kBarAnalysis->_shiftY[8] = 31.09;

    kBarAnalysis->_mcpLabel_str[9] = "SL10 #2 (MCP 9)";
    kBarAnalysis->_mcp_dim[9] = 27.5;
    kBarAnalysis->_mcp_act[9] = 22;
    kBarAnalysis->_mcp_det[9] = 20.32;
    kBarAnalysis->_x_bins[9] = 4;
    kBarAnalysis->_y_bins[9] = 4;
    kBarAnalysis->_minX_dim[9] = -39. - gap2;
    kBarAnalysis->_minY_dim[9] = +22;
    kBarAnalysis->_shiftX[9] = ( kBarAnalysis->_mcp_dim[9] - kBarAnalysis->_mcp_det[9] ) / 2;
    kBarAnalysis->_shiftY[9] = ( kBarAnalysis->_mcp_dim[9] - kBarAnalysis->_mcp_det[9] ) / 2;

    kBarAnalysis->_mcpLabel_str[10] = "SiPM (MCP 10)";
    kBarAnalysis->_mcp_dim[10] = 112;
    kBarAnalysis->_mcp_act[10] = 56;
    kBarAnalysis->_mcp_det[10] = 56;
    kBarAnalysis->_x_bins[10] = 8;
    kBarAnalysis->_y_bins[10] = 8;
    kBarAnalysis->_minX_dim[10] = -113 - gap2;
    kBarAnalysis->_minY_dim[10] = +81;
    kBarAnalysis->_shiftX[10] = ( kBarAnalysis->_mcp_dim[10] - kBarAnalysis->_mcp_det[10] ) / 2;
    kBarAnalysis->_shiftY[10] = ( kBarAnalysis->_mcp_dim[10] - kBarAnalysis->_mcp_det[10] ) / 2;


    kBarAnalysis->_mcpLabel_str[7] = "XP85012 #7 (MCP 7)";
    kBarAnalysis->_mcp_dim[7] = 59;
    kBarAnalysis->_mcp_act[7] = 53;
    kBarAnalysis->_mcp_det[7] = 52;
    kBarAnalysis->_x_bins[7] = 8;
    kBarAnalysis->_y_bins[7] = 8;
    kBarAnalysis->_minX_dim[7] = -54.5 - gap2;
    kBarAnalysis->_minY_dim[7] = -74.5;
    kBarAnalysis->_shiftX[7] = ( kBarAnalysis->_mcp_dim[7] - kBarAnalysis->_mcp_det[7] ) / 2;
    kBarAnalysis->_shiftY[7] = ( kBarAnalysis->_mcp_dim[7] - kBarAnalysis->_mcp_det[7] ) / 2;

    kBarAnalysis->_mcpLabel_str[8] = "Empty";
    kBarAnalysis->_mcp_dim[8] = 59;
    kBarAnalysis->_mcp_act[8] = 53;
    kBarAnalysis->_mcp_det[8] = 52;
    kBarAnalysis->_x_bins[8] = 8;
    kBarAnalysis->_y_bins[8] = 8;
    kBarAnalysis->_minX_dim[8] = -81 - gap2;
    kBarAnalysis->_minY_dim[8] = -149;
    kBarAnalysis->_shiftX[8] = ( kBarAnalysis->_mcp_dim[8] - kBarAnalysis->_mcp_det[8] ) / 2;
    kBarAnalysis->_shiftY[8] = ( kBarAnalysis->_mcp_dim[8] - kBarAnalysis->_mcp_det[8] ) / 2;


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

  for( int i=1; i<50; i++) // default 50 files
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
