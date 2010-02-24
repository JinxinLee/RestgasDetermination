
//======
// ROOT
//======
#include <TCanvas.h>
#include <TFile.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TString.h>
#include <TStyle.h>
#include <TTree.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void mcp4Pos( TString inFilename = "", Double_t gap = 80 ) // MCP gap (default either 80 or 110mm)
{

    if( inFilename == "" )
    {
        cout << "Usage: mcp4Pos( filename, gap )" << endl;
        return;
    }


//==============================================================================
// Access to the input ROOT-file
//==============================================================================
    TString outFilename = inFilename;
    outFilename.Resize( inFilename.Length() - 5 ); // remove file extension .root
    TString gap_str;
    gap_str += gap;
    gap_str.Remove( TString::kLeading, ' ' );
    outFilename = outFilename +"_mcpPlots_gap" + gap_str + ".root";

    TFile *inFile   = new TFile( inFilename );
    TFile *outFile  = new TFile( outFilename, "RECREATE" );
    TTree *info     = (TTree*) inFile->Get("info");
    TTree *photon   = (TTree*) inFile->Get("photon");
    TCanvas *screen = (TCanvas*) inFile->Get("Screen");

    TTree *pixelTree = new TTree( "pixel", outFilename ); // for example: MCP1 is pixel 0 - 63; starts at (0,0), (0,1) ...
    TTree *infoTree  = new TTree( "info", outFilename );


    Double_t fishtank_width, fishtank_height;
    Double_t slab_width, slab_height;
    Double_t parDirX, parDirY, parDirZ;
    info->SetBranchAddress( "fishtank_width" , &fishtank_width );
    info->SetBranchAddress( "fishtank_height", &fishtank_height );
    info->SetBranchAddress( "slab_width"     , &slab_width );
    info->SetBranchAddress( "slab_height"    , &slab_height );
    info->SetBranchAddress( "particle_dirX"  , &parDirX );
    info->SetBranchAddress( "particle_dirY"  , &parDirY );
    info->SetBranchAddress( "particle_dirZ"  , &parDirZ );
    info->GetEntry( 0 );

    Double_t hitPosX, hitPosY;
    Bool_t measured;
    Double_t thetaC;
    photon->SetBranchAddress( "hitPosX" , &hitPosX );
    photon->SetBranchAddress( "hitPosY" , &hitPosY );
    photon->SetBranchAddress( "measured", &measured );
    photon->SetBranchAddress( "thetaC"  , &thetaC );

    Int_t nEntries = photon->GetEntries();


    Int_t freq[32][8] = { 0 };
    Int_t px_freq  = 0;
    pixelTree->Branch( "freq", &px_freq, "freq/I" );

    infoTree->Branch( "parDirX", &parDirX, "parDirX/D" );
    infoTree->Branch( "parDirY", &parDirY, "parDirY/D" );
    infoTree->Branch( "parDirZ", &parDirZ, "parDirZ/D" );
    infoTree->Branch( "thetaC" , &thetaC , "thetaC/D" );


//==============================================================================
// MCP positions & plot declarations
//==============================================================================
    Double_t mcp_dim = 75; // MCP case is 71 mm long and wide + 2mm frame due to the holder
    Double_t mcp_active = 51; // MCP active area is 51 mm long and wide

    Double_t minX_dim[4];
    Double_t minY_dim[4];
    minX_dim[1] = -mcp_dim - gap/2; // gap: 80 => -115; gap: 110 => -130
    minY_dim[1] = -100; // fishtank bottom (-fishtank_height/2)
    minX_dim[2] = minX_dim[1];
    minY_dim[2] = minY_dim[1] + mcp_dim;
    minX_dim[3] = minX_dim[2];
    minY_dim[3] = minY_dim[2] + mcp_dim;
    minX_dim[0] = gap/2;
    minY_dim[0] = minY_dim[2];

    cout << "MCP gap is : " << gap << "mm" << endl;

    Double_t shift = ( mcp_dim - mcp_active ) / 2; // to shift active area in the center of MCP

    Double_t maxX_dim[4];
    Double_t maxY_dim[4];

    Double_t minX_active[4];
    Double_t minY_active[4];
    Double_t maxX_active[4];
    Double_t maxY_active[4];

    TLine *line_mcpCase[4][4]; // MCP position case
    TLine *line_mcpArea[4][4]; // MCP position avtive area

    TH2F *mcp[4];
    TPaveText *mcpLabel[4];
    TString mcpLabel_str[4];

    for( int i = 0; i < 4; i++ )
    {
        maxX_dim[i] = minX_dim[i] + mcp_dim;
        maxY_dim[i] = minY_dim[i] + mcp_dim;

        minX_active[i] = minX_dim[i] + shift;
        minY_active[i] = minY_dim[i] + shift;
        maxX_active[i] = minX_dim[i] + shift + mcp_active;
        maxY_active[i] = minY_dim[i] + shift + mcp_active;


        line_mcpCase[i][0] = new TLine( minX_dim[i], minY_dim[i], minX_dim[i], maxY_dim[i] );
        line_mcpCase[i][1] = new TLine( minX_dim[i], maxY_dim[i], maxX_dim[i], maxY_dim[i] );
        line_mcpCase[i][2] = new TLine( maxX_dim[i], maxY_dim[i], maxX_dim[i], minY_dim[i] );
        line_mcpCase[i][3] = new TLine( maxX_dim[i], minY_dim[i], minX_dim[i], minY_dim[i] );

        line_mcpArea[i][0] = new TLine( minX_active[i], minY_active[i], minX_active[i], maxY_active[i] );
        line_mcpArea[i][1] = new TLine( minX_active[i], maxY_active[i], maxX_active[i], maxY_active[i] );
        line_mcpArea[i][2] = new TLine( maxX_active[i], maxY_active[i], maxX_active[i], minY_active[i] );
        line_mcpArea[i][3] = new TLine( maxX_active[i], minY_active[i], minX_active[i], minY_active[i] );

        for( int j = 0; j < 4; j++ )
        {
            line_mcpCase[i][j]->SetLineWidth(3);
            line_mcpArea[i][j]->SetLineWidth(3);
        }


        TString i_str;
        i_str += (i + 1);
        i_str.Remove( TString::kLeading, ' ' );
        mcpLabel_str[i] = "MCP" +i_str;

        mcp[i] = new TH2F( mcpLabel_str[i], mcpLabel_str[i], 8, minX_active[i], maxX_active[i], 8, minY_active[i], maxY_active[i] );
        mcp[i]->SetStats(0);
        mcp[i]->GetXaxis()->SetTitle( "x [mm]" );
        mcp[i]->GetXaxis()->CenterTitle();
        mcp[i]->GetYaxis()->SetTitle( "y [mm]" );
        mcp[i]->GetYaxis()->CenterTitle();
        mcp[i]->GetYaxis()->SetTitleOffset( 1.2 );
        mcp[i]->SetMaximum(500);


        mcpLabel[i] = new TPaveText( minX_active[i], maxY_active[i], maxX_active[i], maxY_dim[i] );
        mcpLabel[i]->SetFillColor( 0 );
        mcpLabel[i]->AddText( mcpLabel_str[i] );
    }


    Double_t fishtank_heightOffset_dim = 30; // to get space to draw full MCP which exceeds the fishtank
    Double_t oilgap = 10; // difference between oil level and fishtank height
    Double_t offsetX = fishtank_width/2; // coordination origin (default is in the center of the fishtank)
    Double_t offsetY = fishtank_height/2;
    Double_t scaleX = 1/(offsetX*2); // coordinates go from 0 to 1
    Double_t scaleFishY = 1/(offsetY*2-oilgap); // coordinates with regard to the fishtank
    Double_t scaleY = 1/(offsetY*2 + fishtank_heightOffset_dim); // to draw full MCP which exceeds the fishtank

    Double_t minX_bar = -slab_width/2;
    Double_t minY_bar = -slab_height/2;
    Double_t maxX_bar = +slab_height/2;
    Double_t maxY_bar = +slab_height/2;


    TLine *line_mcpCase_tank[5][4]; // MCP position case for fishtank plot (one more MCP to draw full MCP which exceeds the fishtank)
    TLine *line_mcpArea_tank[5][4]; // MCP position avtive area for fishtank plot (")

    for( int i = 0; i < 5; i++ )
    {
        if( i < 4 )
        {
            line_mcpCase_tank[i][0] = new TLine( (minX_dim[i]+offsetX)*scaleX, (minY_dim[i]+offsetY)*scaleFishY, (minX_dim[i]+offsetX)*scaleX,
                    (maxY_dim[i]+offsetY)*scaleFishY );
            line_mcpCase_tank[i][1] = new TLine( (minX_dim[i]+offsetX)*scaleX, (maxY_dim[i]+offsetY)*scaleFishY, (maxX_dim[i]+offsetX)*scaleX,
                    (maxY_dim[i]+offsetY)*scaleFishY );
            line_mcpCase_tank[i][2] = new TLine( (maxX_dim[i]+offsetX)*scaleX, (maxY_dim[i]+offsetY)*scaleFishY, (maxX_dim[i]+offsetX)*scaleX,
                    (minY_dim[i]+offsetY)*scaleFishY );
            line_mcpCase_tank[i][3] = new TLine( (maxX_dim[i]+offsetX)*scaleX, (minY_dim[i]+offsetY)*scaleFishY, (minX_dim[i]+offsetX)*scaleX,
                    (minY_dim[i]+offsetY)*scaleFishY );

            line_mcpArea_tank[i][0] = new TLine( (minX_active[i]+offsetX)*scaleX, (minY_active[i]+offsetY)*scaleFishY,
                    (minX_active[i]+offsetX)*scaleX, (maxY_active[i]+offsetY)*scaleFishY );
            line_mcpArea_tank[i][1] = new TLine( (minX_active[i]+offsetX)*scaleX, (maxY_active[i]+offsetY)*scaleFishY,
                    (maxX_active[i]+offsetX)*scaleX, (maxY_active[i]+offsetY)*scaleFishY );
            line_mcpArea_tank[i][2] = new TLine( (maxX_active[i]+offsetX)*scaleX, (maxY_active[i]+offsetY)*scaleFishY,
                    (maxX_active[i]+offsetX)*scaleX, (minY_active[i]+offsetY)*scaleFishY );
            line_mcpArea_tank[i][3] = new TLine( (maxX_active[i]+offsetX)*scaleX, (minY_active[i]+offsetY)*scaleFishY,
                    (minX_active[i]+offsetX)*scaleX,(minY_active[i]+offsetY)*scaleFishY );
        }
        else
        {
            line_mcpCase_tank[4][0] = new TLine( (minX_dim[3]+offsetX)*scaleX, (minY_dim[3]+offsetY)*scaleY, (minX_dim[3]+offsetX)*scaleX,
                    (maxY_dim[3]+offsetY)*scaleY );
            line_mcpCase_tank[4][1] = new TLine( (minX_dim[3]+offsetX)*scaleX, (maxY_dim[3]+offsetY)*scaleY, (maxX_dim[3]+offsetX)*scaleX,
                    (maxY_dim[3]+offsetY)*scaleY );
            line_mcpCase_tank[4][2] = new TLine( (maxX_dim[3]+offsetX)*scaleX, (maxY_dim[3]+offsetY)*scaleY, (maxX_dim[3]+offsetX)*scaleX,
                    (minY_dim[3]+offsetY)*scaleY );
            line_mcpCase_tank[4][3] = new TLine( (maxX_dim[3]+offsetX)*scaleX, (minY_dim[3]+offsetY)*scaleY, (minX_dim[3]+offsetX)*scaleX,
                    (minY_dim[3]+offsetY)*scaleY );

            line_mcpArea_tank[4][0] = new TLine( (minX_active[3]+offsetX)*scaleX, (minY_active[3]+offsetY)*scaleY, (minX_active[3]+offsetX)*scaleX,
                    (maxY_active[3]+offsetY)*scaleY );
            line_mcpArea_tank[4][1] = new TLine( (minX_active[3]+offsetX)*scaleX, (maxY_active[3]+offsetY)*scaleY, (maxX_active[3]+offsetX)*scaleX,
                    (maxY_active[3]+offsetY)*scaleY );
            line_mcpArea_tank[4][2] = new TLine( (maxX_active[3]+offsetX)*scaleX, (maxY_active[3]+offsetY)*scaleY, (maxX_active[3]+offsetX)*scaleX,
                    (minY_active[3]+offsetY)*scaleY );
            line_mcpArea_tank[4][3] = new TLine( (maxX_active[3]+offsetX)*scaleX, (minY_active[3]+offsetY)*scaleY, (minX_active[3]+offsetX)*scaleX,
                    (minY_active[3]+offsetY)*scaleY );
        }

        for( int j = 0; j < 4; j++ )
        {
            line_mcpCase_tank[i][j]->SetLineWidth(3);
            line_mcpArea_tank[i][j]->SetLineWidth(3);
        }
    }


    TLine *tankBorder1 = new TLine( (-fishtank_width/2+offsetX)*scaleX, (+fishtank_height/2+offsetY)*scaleY, (minX_dim[3]+offsetX)*scaleX,
                                      (+fishtank_height/2+offsetY)*scaleY );
    TLine *tankBorder2 = new TLine( (maxX_dim[3]+offsetX)*scaleX, (+fishtank_height/2+offsetY)*scaleY, (+fishtank_width/2+offsetX)*scaleX,
                                     (+fishtank_height/2+offsetY)*scaleY );

    tankBorder1->SetLineWidth(3);
    tankBorder2->SetLineWidth(3);
//     tankBorder1->SetLineColor(15);
//     tankBorder2->SetLineColor(15);


    TLine *line_bar[4];
    line_bar[0] = new TLine( (minX_bar+offsetX)*scaleX, (minY_bar+offsetY)*scaleFishY, (minX_bar+offsetX)*scaleX, (maxY_bar+offsetY)*scaleFishY );
    line_bar[1] = new TLine( (minX_bar+offsetX)*scaleX, (maxY_bar+offsetY)*scaleFishY, (maxX_bar+offsetX)*scaleX, (maxY_bar+offsetY)*scaleFishY );
    line_bar[2] = new TLine( (maxX_bar+offsetX)*scaleX, (maxY_bar+offsetY)*scaleFishY, (maxX_bar+offsetX)*scaleX, (minY_bar+offsetY)*scaleFishY );
    line_bar[3] = new TLine( (maxX_bar+offsetX)*scaleX, (minY_bar+offsetY)*scaleFishY, (minX_bar+offsetX)*scaleX, (minY_bar+offsetY)*scaleFishY );

    for( int i = 0; i < 4; i++ )
        line_bar[i]->SetLineWidth(3);


//==============================================================================
// Canvas settings
//==============================================================================
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


    TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 1050, 810 );
    canvas->Draw();

    TCanvas *canvasMCP = new TCanvas( "canvasMCP", "" ,200, 10, 530, 500 );
    canvasMCP->SetLeftMargin(0.13);
    canvasMCP->SetRightMargin(0.15);
    canvasMCP->Draw();

    TCanvas *canvasTank = new TCanvas( "canvasTank", "" ,200, 10, 1050, 810 );
    canvasTank->Draw();


    canvas->cd();
    TPad *pad[5];
    TPad *pad[0] = new TPad( "p1", "", 0.00, 0.28, 0.49, 0.72 );
    TPad *pad[1] = new TPad( "p2", "", 0.75, 0.35, 1.00, 0.66 );
    TPad *pad[2] = new TPad( "p3", "", 0.50, 0.02, 0.75, 0.33 );
    TPad *pad[3] = new TPad( "p4", "", 0.50, 0.35, 0.75, 0.66 );
    TPad *pad[4] = new TPad( "p5", "", 0.50, 0.68, 0.75, 0.99 );

    canvasTank->cd();
    TPad *padTank[7];
    padTank[0] = new TPad( "pp1", "", 0.00, 0.00, 1.00, 1.00, 0 );
    padTank[1] = new TPad( "pp2", "", 0.00, 0.00, 1.00, (fishtank_height - oilgap)*scaleY, 11 );
    padTank[2] = new TPad( "pp3", "", (minX_active[0] + offsetX)*scaleX, (minY_active[0] + offsetY)*scaleY,
                           (maxX_active[0] + offsetX)*scaleX, (maxY_active[0] + offsetY)*scaleY );
    padTank[3] = new TPad( "pp4", "", (minX_active[1] + offsetX)*scaleX, (minY_active[1] + offsetY)*scaleY,
                           (maxX_active[1] + offsetX)*scaleX, (maxY_active[1] + offsetY)*scaleY );
    padTank[4] = new TPad( "pp5", "", (minX_active[2] + offsetX)*scaleX, (minY_active[2] + offsetY)*scaleY,
                           (maxX_active[2] + offsetX)*scaleX, (maxY_active[2] + offsetY)*scaleY );
    padTank[5] = new TPad( "pp6", "", (minX_active[3] + offsetX)*scaleX, (minY_active[3] + offsetY)*scaleY,
                           (maxX_active[3] + offsetX)*scaleX, (maxY_active[3] + offsetY)*scaleY );
    padTank[6] = new TPad( "pp7", "", (minX_bar + offsetX)*scaleX, (minY_bar + offsetY)*scaleY,
                           (maxX_bar + offsetX)*scaleX, (maxY_bar + offsetY)*scaleY, 1 );


    for( int i = 0; i < 5; i++ )
    {
        if( i > 0 )
        {
            pad[i]->SetLeftMargin(0.10);
            pad[i]->SetRightMargin(0.20);
            pad[i]->SetTopMargin(0.12);
            pad[i]->SetBottomMargin(0.12);
        }

        canvas->cd();
        pad[i]->Draw();
    }


    for( int i = 0; i < 7; i++ )
    {
        if( i > 1 && i < 6 )
        {
            padTank[i]->SetLeftMargin(0);
            padTank[i]->SetRightMargin(0);
            padTank[i]->SetTopMargin(0);
            padTank[i]->SetBottomMargin(0);
        }

        if( i == 6 )
            padTank[i]->SetFillStyle(3004);

        canvasTank->cd();
        padTank[i]->Draw();
    }


//==============================================================================
// Event loop
//==============================================================================
    for( int i = 0; i < nEntries; i++ )
    {
        photon->GetEntry( i );

        if( measured == true)
        {
            Int_t n_test = 0;

            Int_t pxX = -666;
            Int_t pxY = -666;

            for( int j = 0; j < 4; j++)
            {
                mcp[j]->Fill( hitPosX, hitPosY );

                if( hitPosX >= minX_active[j] && hitPosX < maxX_active[j] && hitPosY >= minY_active[j] && hitPosY < maxY_active[j] )
                {
                    pxX = TMath::FloorNint( (hitPosX - minX_active[j])*8 / mcp_active ) + 8*j;
                    pxY = TMath::FloorNint( (hitPosY - minY_active[j])*8 / mcp_active );

                    n_test++;
                }
            }

            if( n_test > 1 )
            {
                cout << "Something is wrong (hit for different MCPs)." << endl;
                abort();
            }

            if( pxX == -666 )
                continue;
            else
                freq[ pxX ][ pxY ]++;
        }
    }


//==============================================================================
// Plot histograms
//==============================================================================
    canvas->cd();
    pad[0]->cd();
    screen->DrawClonePad();
    screen->Close(); // w/o -> Error in <RootX11ErrorHandler>

    for( int i = 0; i < 4; i++ )
    {
        pad[i+1]->cd();
        mcp[i]->Draw( "colz" );

        pad[0]->cd();
        mcpLabel[i]->Draw("same");

        for( int j = 0; j < 4; j++ )
        {
            line_mcpCase[i][j]->Draw("same");
            line_mcpArea[i][j]->Draw("same");
        }
    }

    canvas->Write( "screenMCP");
    canvas->Close();


    canvasMCP->cd();

    for( int i = 0; i < 4; i++ )
    {
        mcp[i]->Draw( "colz" );
        canvasMCP->Write( mcpLabel_str[i] );
        canvasMCP->Clear();
    }

    canvasMCP->Close();


    canvasTank->cd();

    for( int i = 0; i < 4; i++ )
    {
        padTank[i+2]->cd();
        mcp[i]->SetTitle("");
        mcp[i]->Draw( "col AH" );

        for( int j = 0; j < 4; j++ )
        {
            padTank[1]->cd();
            line_mcpCase_tank[i][j]->Draw("same");
            line_mcpArea_tank[i][j]->Draw("same");
        }
    }

    for( int j = 0; j < 4; j++ )
    {
        padTank[0]->cd();
        line_mcpCase_tank[4][j]->Draw("same");
        line_mcpArea_tank[4][j]->Draw("same");
    }

    tankBorder1->Draw("same");
    tankBorder2->Draw("same");

    canvasTank->Write( "fishtank");
    canvasTank->Close();


    infoTree->Fill();

    for( int i = 0; i < 32; i++ )
    {
        for( int j = 0; j < 8; j++ )
        {
            px_freq = freq[i][j];
            pixelTree->Fill();
        }
    }


    outFile->Write();
    outFile->Close();

    cout << "Root-file " << outFilename << " was written" << endl;
}
