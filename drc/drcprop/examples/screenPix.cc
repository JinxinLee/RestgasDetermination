
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
#include <TChain.h>
#include <TMath.h>
#include <TRandom3.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void screenPix( TString inFilename = "", Double_t resolution = 6.375, Bool_t effiMode = false ) // resolution in mm
{

    if( inFilename == "" )
    {
        cout << "Usage: screenPix( filename, resolution, effiMode )" << endl;
        return;
    }


//==============================================================================
// Access to the input ROOT-file
//==============================================================================
    TFile *inFile = new TFile( inFilename );
    TTree *info   = (TTree*) inFile->Get("info");
    TTree *photon = (TTree*) inFile->Get("photon");


    Double_t fishtank_width, fishtank_height;
    Double_t slab_width, slab_height;
    Double_t parDirX, parDirY, parDirZ;

    info->SetBranchAddress( "fishtank_width" , &fishtank_width );
    info->SetBranchAddress( "fishtank_height", &fishtank_height );
    info->SetBranchAddress( "particle_dirX", &parDirX );
    info->SetBranchAddress( "particle_dirY", &parDirY );
    info->SetBranchAddress( "particle_dirZ", &parDirZ );
    info->GetEntry( 0 );


    Double_t hitPosX, hitPosY;
    Bool_t measured;
    Double_t thetaC, wavelength;
    Double_t phot_parDirX, phot_parDirY, phot_parDirZ;

    photon->SetBranchAddress( "hitPosX"   , &hitPosX );
    photon->SetBranchAddress( "hitPosY"   , &hitPosY );
    photon->SetBranchAddress( "measured"  , &measured );
    photon->SetBranchAddress( "wavelength", &wavelength );
    photon->SetBranchAddress( "thetaC"    , &thetaC );
    if( parDirX == -666 )
    {
        photon->SetBranchAddress( "particle_dirX", &phot_parDirX );
        photon->SetBranchAddress( "particle_dirY", &phot_parDirY );
        photon->SetBranchAddress( "particle_dirZ", &phot_parDirZ );
    }

    Int_t nEntries = photon->GetEntries();


    TString res_str;
    res_str += resolution;
    res_str.Remove( TString::kLeading, ' ' );
    if( res_str.Length() == 5 )
        res_str.Resize(5); // max. 3 digits after comma

    TString effiMode_str = "";
    if( effiMode )
        effiMode_str = "effi_";

    TString outFilename = inFilename;
    outFilename.Resize( inFilename.Length() - 5 ); // remove file extension .root
    outFilename = outFilename + "_" + res_str + "mm_" + effiMode_str + "screenPix.root";

    TFile *outFile = new TFile( outFilename, "RECREATE" );
    TTree *pixelTree = new TTree( "pixel", outFilename ); // starts at (0,0), (0,1) ...
    TTree *infoTree  = new TTree( "info", outFilename );


    Double_t x_dimMin = -fishtank_width/2;
    Double_t x_dimMax = fishtank_width/2;
    Double_t y_dimMin = -fishtank_height/2;
    Double_t y_dimMax = fishtank_height/2;

    Int_t x_binsH =  TMath::CeilNint(fishtank_width / resolution);
    Int_t y_binsH =  TMath::CeilNint(fishtank_height / resolution);

    const Int_t x_bins = x_binsH;
    const Int_t y_bins = y_binsH;

    if( (Int_t) (fishtank_width*1000) % (Int_t) (resolution*1000) !=0) // max. 3 digits after comma
        x_dimMax = x_bins*resolution - fishtank_width/2;
    if( (Int_t) (fishtank_height*1000) % (Int_t) (resolution*1000) !=0)
        y_dimMax = y_bins*resolution - fishtank_height/2;


    Int_t freq[x_bins][y_bins] = { 0 };
    Int_t px_freq  = 0;

    const Int_t hitsPerPixel = 124; // default: 50 hits per pixel
    Int_t parDir_size = -666;
    if( parDirX == -666 )
        parDir_size = hitsPerPixel;

    TString parDir_size_str;
    parDir_size_str += hitsPerPixel;
    parDir_size_str.Remove( TString::kLeading, ' ' );
    TString parDirX_str = "parDirX[" + parDir_size_str + "]";
    TString parDirY_str = "parDirY[" + parDir_size_str + "]";
    TString parDirZ_str = "parDirZ[" + parDir_size_str + "]";
    TString parDirX_str2 = parDirX_str + "/D";
    TString parDirY_str2 = parDirY_str + "/D";
    TString parDirZ_str2 = parDirZ_str + "/D";

    Double_t parDirX_px[x_bins][y_bins][hitsPerPixel] ={0};
    Double_t parDirY_px[x_bins][y_bins][hitsPerPixel] ={0};
    Double_t parDirZ_px[x_bins][y_bins][hitsPerPixel] ={0};
    Double_t parDirXH[hitsPerPixel]={0};
    Double_t parDirYH[hitsPerPixel]={0};
    Double_t parDirZH[hitsPerPixel]={0};

    pixelTree->Branch( "freq", &px_freq, "freq/I" );
    if( parDirX == -666 )
    {
        pixelTree->Branch( parDirX_str, parDirXH, parDirX_str2 );
        pixelTree->Branch( parDirY_str, parDirYH, parDirY_str2 );
        pixelTree->Branch( parDirZ_str, parDirZH, parDirZ_str2 );
    }
    infoTree->Branch( "parDirX", &parDirX, "parDirX/D" );
    infoTree->Branch( "parDirY", &parDirY, "parDirY/D" );
    infoTree->Branch( "parDirZ", &parDirZ, "parDirZ/D" );
    infoTree->Branch( "thetaC" , &thetaC , "thetaC/D" );
    infoTree->Branch( "fishtank_width" , &fishtank_width , "fishtank_width/D" );
    infoTree->Branch( "fishtank_height", &fishtank_height, "fishtank_height/D" );
    infoTree->Branch( "parDir_size", &parDir_size, "parDir_size/I" );


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


    TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
    canvas->Draw();

    TH2F *screen = new TH2F( "screen","", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );


//==============================================================================
// Photon detector efficiency
//==============================================================================
    Double_t fEffiArray[70];

    for (int i=0; i<70; i++) fEffiArray[i]=0;

  // the argument is the wavelenght (nm) divided by 10.
    fEffiArray[29] = 0.06;
    fEffiArray[30] = 0.10;
    fEffiArray[31] = 0.16;
    fEffiArray[32] = 0.22;
    fEffiArray[33] = 0.27;
    fEffiArray[34] = 0.28;
    fEffiArray[35] = 0.285;
    fEffiArray[36] = 0.31;
    fEffiArray[37] = 0.31;
    fEffiArray[38] = 0.30;
    fEffiArray[39] = 0.29;
    fEffiArray[40] = 0.28;
    fEffiArray[41] = 0.27;
    fEffiArray[42] = 0.26;
    fEffiArray[43] = 0.25;
    fEffiArray[44] = 0.24;
    fEffiArray[45] = 0.23;
    fEffiArray[46] = 0.21;
    fEffiArray[47] = 0.19;
    fEffiArray[48] = 0.18;
    fEffiArray[49] = 0.17;
    fEffiArray[50] = 0.16;
    fEffiArray[51] = 0.14;
    fEffiArray[52] = 0.12;
    fEffiArray[53] = 0.10;
    fEffiArray[54] = 0.07;
    fEffiArray[55] = 0.05;
    fEffiArray[56] = 0.04;
    fEffiArray[57] = 0.03;
    fEffiArray[58] = 0.03;
    fEffiArray[59] = 0.02;
    fEffiArray[60] = 0.01;
    fEffiArray[61] = 0.075;
    fEffiArray[62] = 0.05;
    fEffiArray[63] = 0.025;

    for (int i=0; i<70; i++) fEffiArray[i]=fEffiArray[i]*3; // to avoid the removing of too much photons

    TRandom3 rand;

//==============================================================================
// Event loop
//==============================================================================
    for( int i = 0; i < nEntries; i++ )
    {
        photon->GetEntry( i );

//         if( i%10000 == 0)
//             cout << "Photon: " << i << " / " << nEntries << endl;

        if( measured == true)
        {
            Int_t pxX = -666;
            Int_t pxY = -666;

//             if( TMath::Abs(hitPosY) > 50)
//             {
            if( !effiMode || ( wavelength < 700 && (rand.Uniform() < fEffiArray[(int)(wavelength/10+0.5)]) ) )
            {
                screen->Fill( hitPosX, hitPosY );

                pxX = TMath::FloorNint( (hitPosX + fishtank_width/2) / resolution );
                pxY = TMath::FloorNint( (hitPosY + fishtank_height/2) / resolution );

                freq[ pxX ][ pxY ]++;

                if( parDirX == -666 )
                {
                    Int_t k = 0;
                    while( parDirX_px[pxX][pxY][k] != 0) // was initialized with 0
                    {
                        if( k > hitsPerPixel )
                            break;
                        else
                            k++;
                    }
                    if( k > hitsPerPixel )
                        continue;

                    parDirX_px[pxX][pxY][k] = phot_parDirX;
                    parDirY_px[pxX][pxY][k] = phot_parDirY;
                    parDirZ_px[pxX][pxY][k] = phot_parDirZ;
                }
            }
//             }
        }
    }


//==============================================================================
// Plot histograms
//==============================================================================
    screen->SetStats(false);
    screen->Draw("colz");
    canvas->Write( "screen");
    canvas->Close();


    infoTree->Fill();

    for( int i = 0; i < x_bins; i++ )
    {
        for( int j = 0; j < y_bins; j++ )
        {
            px_freq = freq[i][j];
            if( parDirX == -666 )
            {
                if( hitsPerPixel < freq[i][j] )
                    cout << "More than " << hitsPerPixel << " hits for pixel (" << i << ", " << j << ")  (hits: " << freq[i][j] << ")" << endl;

                for( int k = 0; k < hitsPerPixel; k++)
                {
                    parDirXH[k] = parDirX_px[i][j][k];
                    parDirYH[k] = parDirY_px[i][j][k];
                    parDirZH[k] = parDirZ_px[i][j][k];
                }
            }
            pixelTree->Fill();
        }
    }


    outFile->Write();
    outFile->Close();

    cout << "Root-file " << outFilename << " was written" << endl;
}

