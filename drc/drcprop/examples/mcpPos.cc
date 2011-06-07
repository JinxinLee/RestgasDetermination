
//======
// ROOT
//======
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPad.h>
#include <TLine.h>
#include <TH2.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void mcpPos( TString inFilename = "", Double_t minX_dim = 0, Double_t minY_dim = 0  )
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
	TTree *inTree   = (TTree*) inFile->Get("photon");
	TCanvas *screen = (TCanvas*) inFile->Get("Screen");


	Double_t hitPosX, hitPosY;
	Bool_t measured;

	inTree->SetBranchAddress( "hitPosX"   , &hitPosX );
	inTree->SetBranchAddress( "hitPosY"   , &hitPosY );
	inTree->SetBranchAddress( "measured"  , &measured );

	Int_t nEntries = inTree->GetEntries();


    // set some global options
// 	gStyle->SetCanvasColor( 0 );        // white
// 	gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
// 	gStyle->SetFrameFillColor( 0 );
// 	gStyle->SetFrameBorderMode( 0 );    // no red frame
// 	gStyle->SetHistFillColor( 0 );
// 	gStyle->SetPadColor( 0 );
// 	gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
// 	gStyle->SetTitleFontSize( 0.05 );
// 	gStyle->SetPalette( 1 );            // better color palette
// 	gStyle->SetStatColor( 1 );          // stat. box color

	TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 1250, 500 );
	canvas->Draw();
	canvas->cd();

	TPad *pad1 = new TPad( "p1", "", 0, 0, 0.56, 1 );
	TPad *pad2 = new TPad( "p2", "", 0.56, 0, 1, 1 );
    pad2->SetLeftMargin( 0.14 );
    pad2->SetRightMargin( 0.18 );
	pad1->Draw();
	pad2->Draw();


//==============================================================================
// Plot declarations & MCP position
//==============================================================================
	Double_t mcp_dim = 71; // MCP case is 71 mm long and wide
	Double_t mcp_active = 51; // MCP active area is 51 mm long and wide
	Double_t shift = ( mcp_dim -mcp_active ) / 2; // to shift active area in the center of MCP

	Double_t maxX_dim = minX_dim + mcp_dim;
	Double_t maxY_dim = minY_dim + mcp_dim;

	Double_t minX_active = minX_dim + shift;
	Double_t minY_active = minY_dim + shift;
	Double_t maxX_active = minX_dim + shift + mcp_active;
	Double_t maxY_active = minY_dim + shift + mcp_active;


    // MCP position case
	TLine *l1 = new TLine( minX_dim, minY_dim, minX_dim, maxY_dim );
	TLine *l2 = new TLine( minX_dim, maxY_dim, maxX_dim, maxY_dim );
	TLine *l3 = new TLine( maxX_dim, maxY_dim, maxX_dim, minY_dim );
	TLine *l4 = new TLine( maxX_dim, minY_dim, minX_dim, minY_dim );

	// MCP position avtive area
	TLine *l5 = new TLine( minX_active, minY_active, minX_active, maxY_active );
	TLine *l6 = new TLine( minX_active, maxY_active, maxX_active, maxY_active );
	TLine *l7 = new TLine( maxX_active, maxY_active, maxX_active, minY_active );
	TLine *l8 = new TLine( maxX_active, minY_active, minX_active, minY_active );

	l1->SetLineWidth(3);
	l2->SetLineWidth(3);
	l3->SetLineWidth(3);
	l4->SetLineWidth(3);
	l5->SetLineWidth(3);
	l6->SetLineWidth(3);
	l7->SetLineWidth(3);
	l8->SetLineWidth(3);

    // mcp plot
	TH2F *mcp = new TH2F( "mcp", "MCP", 8, minX_active, maxX_active, 8, minY_active, maxY_active );
	mcp->SetStats(1);
	mcp->GetXaxis()->SetTitle( "x [mm]" );
	mcp->GetXaxis()->CenterTitle();
	mcp->GetYaxis()->SetTitle( "y [mm]" );
	mcp->GetYaxis()->CenterTitle();
	mcp->GetYaxis()->SetTitleOffset( 1.5 );


//==============================================================================
// Event loop
//==============================================================================
	for( int i = 0; i < nEntries; i++ )
	{
		inTree->GetEntry( i );

		if( measured == true )
			mcp->Fill( hitPosX, hitPosY );
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
	l5->Draw("same");
	l6->Draw("same");
	l7->Draw("same");
	l8->Draw("same");

	pad2->cd();
	mcp->Draw( "colz" );
}

