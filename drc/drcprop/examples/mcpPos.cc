
//======
// ROOT
//======
#include <TH2.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TMath.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void mcpPos( TString inFilename = "", Double_t minX = 0, Double_t minY = 0  )
{

    if( inFilename == "" )
    {
        cout << "Usage: mcpPos( filename, left_bottom_mcp_pos_X, left_bottom_mcp_pos_Y)" << endl;
        return;
    }


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
    TFile *inFile   = new TFile( inFilename );
    TTree *inTree   = (TTree*) inFile->Get("ntuple");
    TCanvas *screen = (TCanvas*) inFile->Get("Screen"); // fishtank screen; PROBLEM to draw it again in a canvas


    Double_t wavelength, originDirX, originDirY, originDirZ, posX, posY;
    Bool_t measured, absorbed, lost;

    inTree->SetBranchAddress( "wavelength" , &wavelength );
    inTree->SetBranchAddress( "originDirX" , &originDirX );
    inTree->SetBranchAddress( "originDirY" , &originDirY );
    inTree->SetBranchAddress( "originDirZ" , &originDirZ );
    inTree->SetBranchAddress( "posX"       , &posX       );
    inTree->SetBranchAddress( "posY"       , &posY       );
    inTree->SetBranchAddress( "measured"   , &measured   );
    inTree->SetBranchAddress( "absorbed"   , &absorbed   );
    inTree->SetBranchAddress( "lost"       , &lost       );


    Int_t nEntries = inTree->GetEntries();


    // set some global options
    gStyle->SetCanvasColor( 0 );        // white
    gStyle->SetPadColor( 0 );
    gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
    gStyle->SetPadBorderMode( 0 );
    gStyle->SetFrameBorderMode( 0 );    // no red frame
    gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
    gStyle->SetTitleFontSize( 0.05 );
    gStyle->SetPalette( 1 );            // better color palette
    gStyle->SetStatColor( 0 );          // stat. box color

    TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 1200, 500 );
    canvas->Draw();
    canvas->cd();
    TPad *pad1 = new TPad( "p1", "", 0, 0, 0.58, 1 );
    TPad *pad2 = new TPad( "p2", "", 0.6, 0, 1, 1 );
    pad1->Draw();
    pad2->Draw();


//==============================================================================
// Plot declarations & MCP position
//==============================================================================
    Double_t mcp_dim = 51; // MCP is 51 mm long and wide
    Double_t maxX = minX + mcp_dim;
    Double_t maxY = minY + mcp_dim;

    // MCP position
    TLine *l1 = new TLine( minX, minY, minX, maxY );
    TLine *l2 = new TLine( minX, maxY, maxX, maxY );
    TLine *l3 = new TLine( maxX, maxY, maxX, minY );
    TLine *l4 = new TLine( maxX, minY, minX, minY );

    l1->SetLineWidth(3);
    l2->SetLineWidth(3);
    l3->SetLineWidth(3);
    l4->SetLineWidth(3);

    // mcp plot
    TH2F *mcp = new TH2F( "mcp", "MCP", 10, minX, maxX, 10, minY, maxY );
    mcp->SetStats(0);
    mcp->GetXaxis()->SetTitle( "x [mm]" );
    mcp->GetXaxis()->CenterTitle();
    mcp->GetYaxis()->SetTitle( "y [mm]" );
    mcp->GetYaxis()->CenterTitle();
    mcp->GetYaxis()->SetTitleOffset( 1.2 );


//==============================================================================
// Event loop
//==============================================================================
    for( int i = 0; i < nEntries; i++ )
    {
        inTree->GetEntry( i );

        if( measured == true)
            mcp->Fill( posX, posY );
    }


//==============================================================================
// Plot histograms
//==============================================================================
    pad1->cd();
    screen->DrawClonePad();
    screen->Close(); // w/o -> Error in <RootX11ErrorHandler>
    l1->Draw("same");
    l2->Draw("same");
    l3->Draw("same");
    l4->Draw("same");

    pad2->cd();
    mcp->Draw( "colz" );
}
