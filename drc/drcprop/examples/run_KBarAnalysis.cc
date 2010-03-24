
//======
// ROOT
//======
#include <TSystem.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>


void run_KBarAnalysis(TString inFileCore = "", TString outFileCore = "", Double_t resolution = 6.375, Double_t gap = 80,
                      Bool_t mcpMode = false, Bool_t effiMode = false )
{
    if( inFileCore == "" || outFileCore == "" )
    {
        cout << "Usage: run_KBarAnalysis( input-core, output-core, resolution, gap, effiMode, mcpMode )" << endl;
        return;
    }


//     gSystem->CompileMacro( "KBarAnalysis.cc" );
    gSystem->Load( "/u/rhohler/src/examples/KBarAnalysis_cc.so" ); // for batch-jobs


     // set some global options
    gStyle->SetCanvasColor( 0 );        // white
    gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
    gStyle->SetFrameFillColor( 0 );
    gStyle->SetFrameBorderMode( 0 );    // no red frame
    gStyle->SetHistFillColor( 0 );
    gStyle->SetPadColor( 0 );
    gStyle->SetPadBorderMode( 0 );      // no yellow frame
    gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
    gStyle->SetTitleFontSize( 0.05 );
    gStyle->SetPalette( 1 );            // better color palette
    gStyle->SetStatColor( 0 );          // stat. box color


    KBarAnalysis *kBarAnalysis = new KBarAnalysis();


    TString inDirectory = "/d/panda02/rhohler/sim/";
//     TString inFileCore = "kBarList_center_2000000";
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


    kBarAnalysis->_outDirectory    = "/s/rhohler/sim/"; // inDirectory (d is full)
//     kBarAnalysis->_outFileCore     = "kBarList_center_2000000_6.375mm"; //inFileCore
    kBarAnalysis->_outFileCore     = outFileCore;
    kBarAnalysis->_fishtank_width  = fishtank_width;
    kBarAnalysis->_fishtank_height = fishtank_height;
    kBarAnalysis->_airgap          = airgap;
    kBarAnalysis->_mcpMode         = mcpMode;
    kBarAnalysis->_effiMode        = effiMode;

    if( kBarAnalysis->_mcpMode )
    {
        Double_t mcp_dim = 75; // MCP case is 71 mm long and wide + 2mm frame due to the holder
        Double_t mcp_active = 51; // MCP active area is 51 mm long and wide

        Double_t minX_dim[4];
        Double_t minY_dim[4];
        minX_dim[1] = -mcp_dim - gap/2; // MCP gap: 80 => -115; gap: 110 => -130
        minY_dim[1] = -100; // fishtank bottom (-fishtank_height/2)
        minX_dim[2] = minX_dim[1];
        minY_dim[2] = minY_dim[1] + mcp_dim;
        minX_dim[3] = minX_dim[2];
        minY_dim[3] = minY_dim[2] + mcp_dim;
        minX_dim[0] = gap/2;
        minY_dim[0] = minY_dim[2];

        cout << "MCP gap is : " << gap << "mm" << endl;

        TString gap_str;
        gap_str += gap;
        gap_str.Remove( TString::kLeading, ' ' );
        kBarAnalysis->_gap_str = gap_str;

        kBarAnalysis->_mcp_dim = mcp_dim;
        kBarAnalysis->_mcp_active = mcp_active;

        for( int i = 0; i < 4; i++ )
        {
            kBarAnalysis->_minX_dim[i] = minX_dim[i];
            kBarAnalysis->_minY_dim[i] = minY_dim[i];
        }
    }
    else
        kBarAnalysis->_resolution = resolution; // in mm


    TChain * chain = new TChain( "photon" );

    for( int i=1; i<51; i++)
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
