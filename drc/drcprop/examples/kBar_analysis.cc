
//======
// ROOT
//======
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TStyle.h>
#include <TH2.h>
#include <TH1.h>
#include <TMath.h>
#include <TString.h>
#include <TVector3.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void kBar_analysis( TString inFilename = ""  )
{

    if( inFilename == "" )
    {
        cout << "Usage: analysis( filename )" << endl;
        return;
    }


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
    TFile *inFile   = new TFile( inFilename );
    TTree *infoTree     = (TTree*) inFile->Get("info");
    TTree *photonTree   = (TTree*) inFile->Get("photon");


    Double_t gridXstep, gridYstep;
    Double_t fishtank_width, fishtank_height, fishtank_length;
    Double_t airgap;
    Int_t refl_limit;

    infoTree->SetBranchAddress( "gridXstep"      , &gridXstep );
    infoTree->SetBranchAddress( "gridYstep"      , &gridYstep );
    infoTree->SetBranchAddress( "fishtank_width" , &fishtank_width );
    infoTree->SetBranchAddress( "fishtank_height", &fishtank_height );
    infoTree->SetBranchAddress( "fishtank_length", &fishtank_length );
    infoTree->SetBranchAddress( "airgap"         , &airgap );
    infoTree->SetBranchAddress( "refl_limit"     , &refl_limit );
    infoTree->GetEntry( 0 );


    Double_t kBarX, kBarY, kBarZ, hitPosX, hitPosY, hitPosZ;
    Int_t index_pos;

    int add_pos = 1 + 3*2 + 1; // 1 start + 3 volume transition (+ tiny shifts) + detector
    const int pos_size = refl_limit + 1 + add_pos; // reflection limit + 1 exceed + additional positions
    Double_t posX[ pos_size ], posY[ pos_size ], posZ[ pos_size ];

    TString pos_size_str;
    pos_size_str += pos_size;
    pos_size_str.Remove( TString::kLeading, ' ' );
    TString posX_str = "posX[" + pos_size_str + "]";
    TString posY_str = "posY[" + pos_size_str + "]";
    TString posZ_str = "posZ[" + pos_size_str + "]";


    photonTree->SetBranchAddress( "kBarX"    , &kBarX );
    photonTree->SetBranchAddress( "kBarY"    , &kBarY );
    photonTree->SetBranchAddress( "kBarZ"    , &kBarZ );
    photonTree->SetBranchAddress( "hitPosX"  , &hitPosX );
    photonTree->SetBranchAddress( "hitPosY"  , &hitPosY );
    photonTree->SetBranchAddress( "hitPosZ"  , &hitPosZ );
    photonTree->SetBranchAddress( posX_str   , posX );
    photonTree->SetBranchAddress( posY_str   , posY );
    photonTree->SetBranchAddress( posZ_str   , posZ );
    photonTree->SetBranchAddress( "index_pos", &index_pos );


    Int_t nEntries = photonTree->GetEntries();
    infoTree->GetEntry(0);


    Double_t resolution = 10; // default: 10 ; pixel size

    Double_t x_dimMin = -fishtank_width/2;
    Double_t x_dimMax = fishtank_width/2 + resolution;
    Double_t y_dimMin = -fishtank_height/2;
    Double_t y_dimMax = fishtank_height/2 + resolution;

    const Int_t x_bins = TMath::CeilNint(fishtank_width / resolution);
    const Int_t y_bins = TMath::CeilNint(fishtank_height / resolution);


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

    cout << "*** Initialize a lot of histograms" << endl;

    TH2F *screen = new TH2F( "screen", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX = new TH2F( "kBarX", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY = new TH2F( "kBarY", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ = new TH2F( "kBarZ", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX_direct = new TH2F( "kBarX_direct", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY_direct = new TH2F( "kBarY_direct", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ_direct = new TH2F( "kBarZ_direct", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX_leftRefl = new TH2F( "kBarX_leftRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY_leftRefl = new TH2F( "kBarY_leftRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ_leftRefl = new TH2F( "kBarZ_leftRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX_rightRefl = new TH2F( "kBarX_rightRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY_rightRefl = new TH2F( "kBarY_rightRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ_rightRefl = new TH2F( "kBarZ_rightRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX_upRefl = new TH2F( "kBarX_upRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY_upRefl = new TH2F( "kBarY_upRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ_upRefl = new TH2F( "kBarZ_upRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH2F *kbarX_downRefl = new TH2F( "kBarX_downRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarY_downRefl = new TH2F( "kBarY_downRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );
    TH2F *kbarZ_downRefl = new TH2F( "kBarZ_downRefl", "", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

    TH1F pixelX[x_bins][y_bins];
    TH1F pixelY[x_bins][y_bins];
    TH1F pixelZ[x_bins][y_bins];

    TH1F pixelX_direct[x_bins][y_bins];
    TH1F pixelY_direct[x_bins][y_bins];
    TH1F pixelZ_direct[x_bins][y_bins];

    TH1F pixelX_leftRefl[x_bins][y_bins];
    TH1F pixelY_leftRefl[x_bins][y_bins];
    TH1F pixelZ_leftRefl[x_bins][y_bins];

    TH1F pixelX_rightRefl[x_bins][y_bins];
    TH1F pixelY_rightRefl[x_bins][y_bins];
    TH1F pixelZ_rightRefl[x_bins][y_bins];

    TH1F pixelX_upRefl[x_bins][y_bins];
    TH1F pixelY_upRefl[x_bins][y_bins];
    TH1F pixelZ_upRefl[x_bins][y_bins];

    TH1F pixelX_downRefl[x_bins][y_bins];
    TH1F pixelY_downRefl[x_bins][y_bins];
    TH1F pixelZ_downRefl[x_bins][y_bins];

    for( int i=0; i<x_bins; i++)
    {
        for( int j=0; j<y_bins; j++)
        {
            TString i_str;
            TString j_str;
            i_str += i;
            j_str += j;
            i_str.Remove( TString::kLeading, ' ' );
            j_str.Remove( TString::kLeading, ' ' );

            TString histX_str = "kBarX_" + i_str + "_" + j_str;
            TString histY_str = "kBarY_" + i_str + "_" + j_str;
            TString histZ_str = "kBarZ_" + i_str + "_" + j_str;

            pixelX[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ[i][j] = TH1F( histZ_str, "", 200, -1, 1 );

            histX_str = "kBarX_direct" + i_str + "_" + j_str;
            histY_str = "kBarY_direct" + i_str + "_" + j_str;
            histZ_str = "kBarZ_direct" + i_str + "_" + j_str;

            pixelX_direct[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY_direct[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ_direct[i][j] = TH1F( histZ_str, "", 200, -1, 1 );

            histX_str = "kBarX_leftRefl_" + i_str + "_" + j_str;
            histY_str = "kBarY_leftRefl_" + i_str + "_" + j_str;
            histZ_str = "kBarZ_leftRefl_" + i_str + "_" + j_str;

            pixelX_leftRefl[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY_leftRefl[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ_leftRefl[i][j] = TH1F( histZ_str, "", 200, -1, 1 );

            histX_str = "kBarX_rightRefl_" + i_str + "_" + j_str;
            histY_str = "kBarY_rightRefl_" + i_str + "_" + j_str;
            histZ_str = "kBarZ_rightRefl_" + i_str + "_" + j_str;

            pixelX_rightRefl[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY_rightRefl[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ_rightRefl[i][j] = TH1F( histZ_str, "", 200, -1, 1 );

            histX_str = "kBarX_upRefl_" + i_str + "_" + j_str;
            histY_str = "kBarY_upRefl_" + i_str + "_" + j_str;
            histZ_str = "kBarZ_upRefl_" + i_str + "_" + j_str;

            pixelX_upRefl[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY_upRefl[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ_upRefl[i][j] = TH1F( histZ_str, "", 200, -1, 1 );

            histX_str = "kBarX_downRefl_" + i_str + "_" + j_str;
            histY_str = "kBarY_downRefl_" + i_str + "_" + j_str;
            histZ_str = "kBarZ_downRefl_" + i_str + "_" + j_str;

            pixelX_downRefl[i][j] = TH1F( histX_str, "", 200, -1, 1 );
            pixelY_downRefl[i][j] = TH1F( histY_str, "", 200, -1, 1 );
            pixelZ_downRefl[i][j] = TH1F( histZ_str, "", 200, -1, 1 );
        }
    }


//==============================================================================
// Event loop
//==============================================================================
    cout << "*** Event loop" << endl;

    Int_t n_ph = 0;

    for( int i = 0; i < nEntries; i++ )
    {
        photonTree->GetEntry( i );
        n_ph++;

        screen->Fill(hitPosX, hitPosY);

        kbarX->Fill(hitPosX, hitPosY, kBarX);
        kbarY->Fill(hitPosX, hitPosY, kBarY);
        kbarZ->Fill(hitPosX, hitPosY, kBarZ);


        Int_t pxX = TMath::FloorNint( (hitPosX + fishtank_width/2) / resolution );
        Int_t pxY = TMath::FloorNint( (hitPosY + fishtank_height/2) / resolution );

        pixelX[ pxX ][ pxY ].Fill( kBarX );
        pixelY[ pxX ][ pxY ].Fill( kBarY );
        pixelZ[ pxX ][ pxY ].Fill( kBarZ );


        int nBoxRefl = 0;
        for( int j = 0; j < index_pos; j++ )
        {
            if( j > 0 && posZ[j] > airgap + 0.0001 && posZ[j] < hitPosZ ) // need tiny shift
                //to check the reflections at the fishtank sides
            {
//                 cout << "Reflection at fishtank side for photonID: " << n_ph << " ; posZ: " << posZ[j]
//                         << " ; pixel: (" << pxX << "," << pxY << ")" << endl;

                nBoxRefl++;
                if( nBoxRefl > 2 )
                    cout << "Photon hits more than one time the fishtank side walls" << endl;


                TVector3 dirBefore( posX[j] - posX[j-1], posY[j] - posY[j-1], posZ[j] - posZ[j-1] );
                TVector3 dirAfter(  posX[j+1] - posX[j], posY[j+1] - posY[j], posZ[j+1] - posZ[j] );
                dirBefore = dirBefore.Unit();
                dirAfter = dirAfter.Unit();

//                 cout << "before: " << dirBefore.X() << " " << dirBefore.Y() << " " << dirBefore.Z() <<
//                         "  after: " << dirAfter.X() << " " << dirAfter.Y() << " " << dirAfter.Z() << endl;

                if( posX[j] > 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
                {
//                     cout << "left" << endl;

                    kbarX_leftRefl->Fill(hitPosX, hitPosY, kBarX);
                    kbarY_leftRefl->Fill(hitPosX, hitPosY, kBarY);
                    kbarZ_leftRefl->Fill(hitPosX, hitPosY, kBarZ);

                    pixelX_leftRefl[ pxX ][ pxY ].Fill( kBarX );
                    pixelY_leftRefl[ pxX ][ pxY ].Fill( kBarY );
                    pixelZ_leftRefl[ pxX ][ pxY ].Fill( kBarZ );
                }

                if( posX[j] < 0 && ( (dirBefore.X() > 0 && dirAfter.X() < 0) || (dirBefore.X() < 0 && dirAfter.X() > 0) ) )
                {
//                     cout << "right" << endl;

                    kbarX_rightRefl->Fill(hitPosX, hitPosY, kBarX);
                    kbarY_rightRefl->Fill(hitPosX, hitPosY, kBarY);
                    kbarZ_rightRefl->Fill(hitPosX, hitPosY, kBarZ);

                    pixelX_rightRefl[ pxX ][ pxY ].Fill( kBarX );
                    pixelY_rightRefl[ pxX ][ pxY ].Fill( kBarY );
                    pixelZ_rightRefl[ pxX ][ pxY ].Fill( kBarZ );
                }

                if( posY[j] > 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
                {
//                     cout << "up" << endl;
                    kbarX_upRefl->Fill(hitPosX, hitPosY, kBarX);
                    kbarY_upRefl->Fill(hitPosX, hitPosY, kBarY);
                    kbarZ_upRefl->Fill(hitPosX, hitPosY, kBarZ);

                    pixelX_upRefl[ pxX ][ pxY ].Fill( kBarX );
                    pixelY_upRefl[ pxX ][ pxY ].Fill( kBarY );
                    pixelZ_upRefl[ pxX ][ pxY ].Fill( kBarZ );
                }

                if( posY[j] < 0 && ( (dirBefore.Y() > 0 && dirAfter.Y() < 0) || (dirBefore.Y() < 0 && dirAfter.Y() > 0) ) )
                {
//                     cout << "down" << endl;

                    kbarX_downRefl->Fill(hitPosX, hitPosY, kBarX);
                    kbarY_downRefl->Fill(hitPosX, hitPosY, kBarY);
                    kbarZ_downRefl->Fill(hitPosX, hitPosY, kBarZ);

                    pixelX_downRefl[ pxX ][ pxY ].Fill( kBarX );
                    pixelY_downRefl[ pxX ][ pxY ].Fill( kBarY );
                    pixelZ_downRefl[ pxX ][ pxY ].Fill( kBarZ );
                }
            }
        }

        if( nBoxRefl == 0)
        {
            kbarX_direct->Fill(hitPosX, hitPosY, kBarX);
            kbarY_direct->Fill(hitPosX, hitPosY, kBarY);
            kbarZ_direct->Fill(hitPosX, hitPosY, kBarZ);

            pixelX_direct[ pxX ][ pxY ].Fill( kBarX );
            pixelY_direct[ pxX ][ pxY ].Fill( kBarY );
            pixelZ_direct[ pxX ][ pxY ].Fill( kBarZ );
        }

//             if( pxX == 27 && pxY == 8)
//                 cout << "photonID: " << n_ph << " ; kBarX: " << kBarX << endl;
    }


//==============================================================================
// Plot
//==============================================================================
    TString outFile;
    outFile = "plot_" + inFilename;
    TFile *outroot = new TFile( outFile, "RECREATE" );

    cout << "*** Draw histograms" << endl;

    for( int i=0; i<x_bins; i++)
    {
        for( int j=0; j<y_bins; j++)
        {
            TString i_str;
            TString j_str;
            i_str += i;
            j_str += j;
            i_str.Remove( TString::kLeading, ' ' );
            j_str.Remove( TString::kLeading, ' ' );

            TString histX_str = "kBarX_" + i_str + "_" + j_str;
            TString histY_str = "kBarY_" + i_str + "_" + j_str;
            TString histZ_str = "kBarZ_" + i_str + "_" + j_str;

            pixelX[i][j].Draw();
            canvas->Write(histX_str);
            canvas->Clear();

            pixelY[i][j].Draw();
            canvas->Write(histY_str);
            canvas->Clear();

            pixelZ[i][j].Draw();
            canvas->Write(histZ_str);
            canvas->Clear();


            histX_str = "kBarX_direct" + i_str + "_" + j_str;
            histY_str = "kBarY_direct" + i_str + "_" + j_str;
            histZ_str = "kBarZ_direct" + i_str + "_" + j_str;

            if( pixelX_direct[i][j].GetEntries() != 0 )
            {
                pixelX_direct[i][j].Draw();
                canvas->Write(histX_str);
                canvas->Clear();

                pixelY_direct[i][j].Draw();
                canvas->Write(histY_str);
                canvas->Clear();

                pixelZ_direct[i][j].Draw();
                canvas->Write(histZ_str);
                canvas->Clear();
            }


            histX_str = "kBarX_leftRefl" + i_str + "_" + j_str;
            histY_str = "kBarY_leftRefl" + i_str + "_" + j_str;
            histZ_str = "kBarZ_leftRefl" + i_str + "_" + j_str;

            if( pixelX_leftRefl[i][j].GetEntries() != 0 )
            {
                pixelX_leftRefl[i][j].Draw();
                canvas->Write(histX_str);
                canvas->Clear();

                pixelY_leftRefl[i][j].Draw();
                canvas->Write(histY_str);
                canvas->Clear();

                pixelZ_leftRefl[i][j].Draw();
                canvas->Write(histZ_str);
                canvas->Clear();
            }


            histX_str = "kBarX_rightRefl" + i_str + "_" + j_str;
            histY_str = "kBarY_rightRefl" + i_str + "_" + j_str;
            histZ_str = "kBarZ_rightRefl" + i_str + "_" + j_str;

            if( pixelX_rightRefl[i][j].GetEntries() != 0 )
            {
                pixelX_rightRefl[i][j].Draw();
                canvas->Write(histX_str);
                canvas->Clear();

                pixelY_rightRefl[i][j].Draw();
                canvas->Write(histY_str);
                canvas->Clear();

                pixelZ_rightRefl[i][j].Draw();
                canvas->Write(histZ_str);
                canvas->Clear();
            }


            histX_str = "kBarX_upRefl" + i_str + "_" + j_str;
            histY_str = "kBarY_upRefl" + i_str + "_" + j_str;
            histZ_str = "kBarZ_upRefl" + i_str + "_" + j_str;

            if( pixelX_upRefl[i][j].GetEntries() != 0 )
            {
                pixelX_upRefl[i][j].Draw();
                canvas->Write(histX_str);
                canvas->Clear();

                pixelY_upRefl[i][j].Draw();
                canvas->Write(histY_str);
                canvas->Clear();

                pixelZ_upRefl[i][j].Draw();
                canvas->Write(histZ_str);
                canvas->Clear();
            }


            histX_str = "kBarX_downRefl" + i_str + "_" + j_str;
            histY_str = "kBarY_downRefl" + i_str + "_" + j_str;
            histZ_str = "kBarZ_downRefl" + i_str + "_" + j_str;

            if( pixelX_downRefl[i][j].GetEntries() != 0 )
            {
                pixelX_downRefl[i][j].Draw();
                canvas->Write(histX_str);
                canvas->Clear();

                pixelY_downRefl[i][j].Draw();
                canvas->Write(histY_str);
                canvas->Clear();

                pixelZ_downRefl[i][j].Draw();
                canvas->Write(histZ_str);
                canvas->Clear();
            }
        }
    }


    screen->SetTitle( "screen" );
    screen->SetContour(50);
    screen->SetStats( false );
    screen->Draw("colz");
    canvas->Write("screen");
    canvas->Clear();

    //
    kbarX->SetTitle( "kBarX sum" );
    kbarX->SetContour(50);
    kbarX->SetStats( false );
    kbarX->Draw("colz");
    canvas->Write("kBarX_sum");
    canvas->Clear();

    kbarY->SetTitle( "kBarY sum" );
    kbarY->SetContour(50);
    kbarY->SetStats( false );
    kbarY->Draw("colz");
    canvas->Write("kBarY_sum");
    canvas->Clear();

    kbarZ->SetTitle( "kBarZ sum" );
    kbarZ->SetContour(50);
    kbarZ->SetStats( false );
    kbarZ->Draw("colz");
    canvas->Write("kBarZ_sum");
    canvas->Clear();

    kbarX->SetTitle( "kBarX" );
    kbarX->SetContour(50);
    kbarX->SetStats( false );
    kbarX->Divide( screen );
    kbarX->Draw("colz");
    canvas->Write("kBarX");
    canvas->Clear();

    kbarY->SetTitle( "kBarY" );
    kbarY->SetContour(50);
    kbarY->SetStats( false );
    kbarY->Divide( screen );
    kbarY->Draw("colz");
    canvas->Write("kBarY");
    canvas->Clear();

    kbarZ->SetTitle( "kBarZ" );
    kbarZ->SetContour(50);
    kbarZ->SetStats( false );
    kbarZ->Divide( screen );
    kbarZ->Draw("colz");
    canvas->Write("kBarZ");
    canvas->Clear();

    //
    kbarX_direct->SetTitle( "kBarX_direct sum" );
    kbarX_direct->SetContour(50);
    kbarX_direct->SetStats( false );
    kbarX_direct->Draw("colz");
    canvas->Write("kBarX_direct_sum");
    canvas->Clear();

    kbarY_direct->SetTitle( "kBarY_direct sum" );
    kbarY_direct->SetContour(50);
    kbarY_direct->SetStats( false );
    kbarY_direct->Draw("colz");
    canvas->Write("kBarY_direct_sum");
    canvas->Clear();

    kbarZ_direct->SetTitle( "kBarZ_direct sum" );
    kbarZ_direct->SetContour(50);
    kbarZ_direct->SetStats( false );
    kbarZ_direct->Draw("colz");
    canvas->Write("kBarZ_direct_sum");
    canvas->Clear();

    kbarX_direct->SetTitle( "kBarX_direct" );
    kbarX_direct->SetContour(50);
    kbarX_direct->SetStats( false );
    kbarX_direct->Divide( screen );
    kbarX_direct->Draw("colz");
    canvas->Write("kBarX_direct");
    canvas->Clear();

    kbarY_direct->SetTitle( "kBarY_direct" );
    kbarY_direct->SetContour(50);
    kbarY_direct->SetStats( false );
    kbarY_direct->Divide( screen );
    kbarY_direct->Draw("colz");
    canvas->Write("kBarY_direct");
    canvas->Clear();

    kbarZ_direct->SetTitle( "kBarZ_direct" );
    kbarZ_direct->SetContour(50);
    kbarZ_direct->SetStats( false );
    kbarZ_direct->Divide( screen );
    kbarZ_direct->Draw("colz");
    canvas->Write("kBarZ_direct");
    canvas->Clear();

    //
    kbarX_leftRefl->SetTitle( "kBarX_leftRefl sum" );
    kbarX_leftRefl->SetContour(50);
    kbarX_leftRefl->SetStats( false );
    kbarX_leftRefl->Draw("colz");
    canvas->Write("kBarX_leftRefl_sum");
    canvas->Clear();

    kbarY_leftRefl->SetTitle( "kBarY_leftRefl sum" );
    kbarY_leftRefl->SetContour(50);
    kbarY_leftRefl->SetStats( false );
    kbarY_leftRefl->Draw("colz");
    canvas->Write("kBarY_leftRefl_sum");
    canvas->Clear();

    kbarZ_leftRefl->SetTitle( "kBarZ_leftRefl sum" );
    kbarZ_leftRefl->SetContour(50);
    kbarZ_leftRefl->SetStats( false );
    kbarZ_leftRefl->Draw("colz");
    canvas->Write("kBarZ_leftRefl_sum");
    canvas->Clear();

    kbarX_leftRefl->SetTitle( "kBarX_leftRefl" );
    kbarX_leftRefl->SetContour(50);
    kbarX_leftRefl->SetStats( false );
    kbarX_leftRefl->Divide( screen );
    kbarX_leftRefl->Draw("colz");
    canvas->Write("kBarX_leftRefl");
    canvas->Clear();

    kbarY_leftRefl->SetTitle( "kBarY_leftRefl" );
    kbarY_leftRefl->SetContour(50);
    kbarY_leftRefl->SetStats( false );
    kbarY_leftRefl->Divide( screen );
    kbarY_leftRefl->Draw("colz");
    canvas->Write("kBarY_leftRefl");
    canvas->Clear();

    kbarZ_leftRefl->SetTitle( "kBarZ_leftRefl" );
    kbarZ_leftRefl->SetContour(50);
    kbarZ_leftRefl->SetStats( false );
    kbarZ_leftRefl->Divide( screen );
    kbarZ_leftRefl->Draw("colz");
    canvas->Write("kBarZ_leftRefl");
    canvas->Clear();

    //
    kbarX_rightRefl->SetTitle( "kBarX_rightRefl sum" );
    kbarX_rightRefl->SetContour(50);
    kbarX_rightRefl->SetStats( false );
    kbarX_rightRefl->Draw("colz");
    canvas->Write("kBarX_rightRefl_sum");
    canvas->Clear();

    kbarY_rightRefl->SetTitle( "kBarY_rightRefl sum" );
    kbarY_rightRefl->SetContour(50);
    kbarY_rightRefl->SetStats( false );
    kbarY_rightRefl->Draw("colz");
    canvas->Write("kBarY_rightRefl_sum");
    canvas->Clear();

    kbarZ_rightRefl->SetTitle( "kBarZ_rightRefl sum" );
    kbarZ_rightRefl->SetContour(50);
    kbarZ_rightRefl->SetStats( false );
    kbarZ_rightRefl->Draw("colz");
    canvas->Write("kBarZ_rightRefl_sum");
    canvas->Clear();

    kbarX_rightRefl->SetTitle( "kBarX_rightRefl" );
    kbarX_rightRefl->SetContour(50);
    kbarX_rightRefl->SetStats( false );
    kbarX_rightRefl->Divide( screen );
    kbarX_rightRefl->Draw("colz");
    canvas->Write("kBarX_rightRefl");
    canvas->Clear();

    kbarY_rightRefl->SetTitle( "kBarY_rightRefl" );
    kbarY_rightRefl->SetContour(50);
    kbarY_rightRefl->SetStats( false );
    kbarY_rightRefl->Divide( screen );
    kbarY_rightRefl->Draw("colz");
    canvas->Write("kBarY_rightRefl");
    canvas->Clear();

    kbarZ_rightRefl->SetTitle( "kBarZ_rightRefl" );
    kbarZ_rightRefl->SetContour(50);
    kbarZ_rightRefl->SetStats( false );
    kbarZ_rightRefl->Divide( screen );
    kbarZ_rightRefl->Draw("colz");
    canvas->Write("kBarZ_rightRefl");
    canvas->Clear();

    //
    kbarX_upRefl->SetTitle( "kBarX_upRefl sum" );
    kbarX_upRefl->SetContour(50);
    kbarX_upRefl->SetStats( false );
    kbarX_upRefl->Draw("colz");
    canvas->Write("kBarX_upRefl_sum");
    canvas->Clear();

    kbarY_upRefl->SetTitle( "kBarY_upRefl sum" );
    kbarY_upRefl->SetContour(50);
    kbarY_upRefl->SetStats( false );
    kbarY_upRefl->Draw("colz");
    canvas->Write("kBarY_upRefl_sum");
    canvas->Clear();

    kbarZ_upRefl->SetTitle( "kBarZ_upRefl sum" );
    kbarZ_upRefl->SetContour(50);
    kbarZ_upRefl->SetStats( false );
    kbarZ_upRefl->Draw("colz");
    canvas->Write("kBarZ_upRefl_sum");
    canvas->Clear();

    kbarX_upRefl->SetTitle( "kBarX_upRefl" );
    kbarX_upRefl->SetContour(50);
    kbarX_upRefl->SetStats( false );
    kbarX_upRefl->Divide( screen );
    kbarX_upRefl->Draw("colz");
    canvas->Write("kBarX_upRefl");
    canvas->Clear();

    kbarY_upRefl->SetTitle( "kBarY_upRefl" );
    kbarY_upRefl->SetContour(50);
    kbarY_upRefl->SetStats( false );
    kbarY_upRefl->Divide( screen );
    kbarY_upRefl->Draw("colz");
    canvas->Write("kBarY_upRefl");
    canvas->Clear();

    kbarZ_upRefl->SetTitle( "kBarZ_upRefl" );
    kbarZ_upRefl->SetContour(50);
    kbarZ_upRefl->SetStats( false );
    kbarZ_upRefl->Divide( screen );
    kbarZ_upRefl->Draw("colz");
    canvas->Write("kBarZ_upRefl");
    canvas->Clear();

    //
    kbarX_downRefl->SetTitle( "kBarX_downRefl sum" );
    kbarX_downRefl->SetContour(50);
    kbarX_downRefl->SetStats( false );
    kbarX_downRefl->Draw("colz");
    canvas->Write("kBarX_downRefl_sum");
    canvas->Clear();

    kbarY_downRefl->SetTitle( "kBarY_downRefl sum" );
    kbarY_downRefl->SetContour(50);
    kbarY_downRefl->SetStats( false );
    kbarY_downRefl->Draw("colz");
    canvas->Write("kBarY_downRefl_sum");
    canvas->Clear();

    kbarZ_downRefl->SetTitle( "kBarZ_downRefl sum" );
    kbarZ_downRefl->SetContour(50);
    kbarZ_downRefl->SetStats( false );
    kbarZ_downRefl->Draw("colz");
    canvas->Write("kBarZ_downRefl_sum");
    canvas->Clear();

    kbarX_downRefl->SetTitle( "kBarX_downRefl" );
    kbarX_downRefl->SetContour(50);
    kbarX_downRefl->SetStats( false );
    kbarX_downRefl->Divide( screen );
    kbarX_downRefl->Draw("colz");
    canvas->Write("kBarX_downRefl");
    canvas->Clear();

    kbarY_downRefl->SetTitle( "kBarY_downRefl" );
    kbarY_downRefl->SetContour(50);
    kbarY_downRefl->SetStats( false );
    kbarY_downRefl->Divide( screen );
    kbarY_downRefl->Draw("colz");
    canvas->Write("kBarY_downRefl");
    canvas->Clear();

    kbarZ_downRefl->SetTitle( "kBarZ_downRefl" );
    kbarZ_downRefl->SetContour(50);
    kbarZ_downRefl->SetStats( false );
    kbarZ_downRefl->Divide( screen );
    kbarZ_downRefl->Draw("colz");
    canvas->Write("kBarZ_downRefl");
    canvas->Clear();


    outroot->Write();
    outroot->Close();
    cout << "Root-file " << outFile << " was written" << endl;
}
