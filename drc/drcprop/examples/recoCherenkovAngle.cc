
//======
// ROOT
//======
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TMath.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TRandom3.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void recoCherenkovAngle( Double_t bining = 100 )
{
    Double_t degree = 180./TMath::Pi();

//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
    TString beamtestFilename = "beamtest0909_simu/beamtest0909_angleAcceptance_6.375mm_effi_screenPix.root";
    TString kBarFilename     = "/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_6.375mm_screenPlots.root";


    TFile *beamtestFile = new TFile( beamtestFilename );
    TTree *pixel_beamtest = (TTree*) beamtestFile->Get("pixel");
    TTree *info = (TTree*) beamtestFile->Get("info");


    Double_t parDirX, parDirY, parDirZ;
    Double_t thetaC;
    Double_t fishtank_width, fishtank_height;
    Int_t parDir_size;

    info->SetBranchAddress( "parDirX", &parDirX );
    info->SetBranchAddress( "parDirY", &parDirY );
    info->SetBranchAddress( "parDirZ", &parDirZ );
    info->SetBranchAddress( "thetaC" , &thetaC );
    info->SetBranchAddress( "fishtank_width" , &fishtank_width );
    info->SetBranchAddress( "fishtank_height", &fishtank_height );
    info->SetBranchAddress( "parDir_size", &parDir_size );
    info->GetEntry( 0 );


    const Int_t hitsPerPixel = parDir_size; // default: 50 hits per pixel
    TString parDir_size_str;
    parDir_size_str += hitsPerPixel;
    parDir_size_str.Remove( TString::kLeading, ' ' );
    TString parDirX_str = "parDirX[" + parDir_size_str + "]";
    TString parDirY_str = "parDirY[" + parDir_size_str + "]";
    TString parDirZ_str = "parDirZ[" + parDir_size_str + "]";


    Int_t freq_beamtest;
    Double_t phot_parDirX[hitsPerPixel] ={0};
    Double_t phot_parDirY[hitsPerPixel] ={0};
    Double_t phot_parDirZ[hitsPerPixel] ={0};

    pixel_beamtest->SetBranchAddress( "freq", &freq_beamtest );
    if( parDirX == -666 )
    {
        pixel_beamtest->SetBranchAddress( parDirX_str, phot_parDirX );
        pixel_beamtest->SetBranchAddress( parDirY_str, phot_parDirY );
        pixel_beamtest->SetBranchAddress( parDirZ_str, phot_parDirZ );
    }

    Int_t nPixel_beamtest = pixel_beamtest->GetEntries();
    pixel_beamtest->GetEntry(0);


    TFile *kBarFile = new TFile( kBarFilename );
    TTree *pixel_kBar = (TTree*) kBarFile->Get("pixel");

    Double_t kBarX[6], kBarY[6], kBarZ[6];
    Double_t kBarXerr[6], kBarYerr[6], kBarZerr[6];
    Int_t freq[6];

    pixel_kBar->SetBranchAddress( "kBarX[6]"   , kBarX );
    pixel_kBar->SetBranchAddress( "kBarY[6]"   , kBarY );
    pixel_kBar->SetBranchAddress( "kBarZ[6]"   , kBarZ );
    pixel_kBar->SetBranchAddress( "kBarXerr[6]", kBarXerr );
    pixel_kBar->SetBranchAddress( "kBarYerr[6]", kBarYerr );
    pixel_kBar->SetBranchAddress( "kBarZerr[6]", kBarZerr );
    pixel_kBar->SetBranchAddress( "freq[6]"    , freq );

    Int_t nPixel_kBar = pixel_kBar->GetEntries();


//==============================================================================
// Canvas & plot settings
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


    TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 500 );
    canvas->Draw();

    TString title = "Cherenkov angle  (par: #Theta = [0,180[#circ, #phi = [0,360[#circ)";
    TH1F *cherenkov = new TH1F( "cherenkov_angle", title, bining, 35, 55);
//     TH1F *cherenkov = new TH1F( "cherenkov_angle", title, 200, 0, 180);


//==============================================================================
// Event loop
//==============================================================================
    if( nPixel_beamtest != nPixel_kBar )
    {
        cout << "Different number pf pixel for beamtest data and kBar tree! (" << nPixel_beamtest << ", " << nPixel_kBar << ")" << endl;
    }


    for( int i = 0; i < nPixel_kBar; i++ )
    {
        pixel_kBar->GetEntry( i );
        pixel_beamtest->GetEntry( i );

        TVector3 kBar[6];

//         Int_t pxX = TMath::FloorNint(i/8) + 1;
//         Int_t pxY = i%8 + 1;
//         cout << pxX << " " << pxY << endl;

//         if( freq_beamtest !=0 )
//             cout << i << endl;

//         timediff->SetBinContent(pxX,pxY,kBarY[2]-kBarY[1]);


        for( int l =0; l < freq_beamtest; l++)
        {
            if( freq_beamtest > hitsPerPixel && parDirX == -666 )
                break;

            TVector3 parDir;

            if( parDirX == -666 )
            {
                parDir.SetX(phot_parDirX[l]);
                parDir.SetY(phot_parDirY[l]);
                parDir.SetZ(phot_parDirZ[l]);
            }
            else
            {
                parDir.SetX(parDirX[l]);
                parDir.SetY(parDirY[l]);
                parDir.SetZ(parDirZ[l]);
            }

//             cout << parDir.X() << " " << parDir.Y() << " " << parDir.Z() << " " << endl;


            for( int j = 1; j < 6; j++ ) // 0: all, 1: direct, 2: left, 3: right, 4: up, 5: down
            {
                Double_t angleC[8]; // up, down, left, right, forward, backward; total 2*2*2 possibilities in bar


                kBar[j].SetX( kBarX[j] );
                kBar[j].SetY( kBarY[j] );
                kBar[j].SetZ( kBarZ[j] );

                Double_t mag = parDir.Mag() * kBar[j].Mag();

                if( mag == 0 )
                    cout << "?" << parDir.Mag() << " " << kBar[j].Mag() << endl;


                Double_t angleC[0] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( -kBarX[j] );
                kBar[j].SetY( kBarY[j] );
                kBar[j].SetZ( kBarZ[j] );
                Double_t angleC[1] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( kBarX[j] );
                kBar[j].SetY( -kBarY[j] );
                kBar[j].SetZ( kBarZ[j] );
                Double_t angleC[2] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( kBarX[j] );
                kBar[j].SetY( kBarY[j] );
                kBar[j].SetZ( -kBarZ[j] );
                Double_t angleC[3] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( -kBarX[j] );
                kBar[j].SetY( -kBarY[j] );
                kBar[j].SetZ( kBarZ[j] );
                Double_t angleC[4] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( -kBarX[j] );
                kBar[j].SetY( kBarY[j] );
                kBar[j].SetZ( -kBarZ[j] );
                Double_t angleC[5] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( kBarX[j] );
                kBar[j].SetY( -kBarY[j] );
                kBar[j].SetZ( -kBarZ[j] );
                Double_t angleC[6] = TMath::ACos( kBar[j].Dot(parDir) / mag );

                kBar[j].SetX( -kBarX[j] );
                kBar[j].SetY( -kBarY[j] );
                kBar[j].SetZ( -kBarZ[j] );
                Double_t angleC[7] = TMath::ACos( kBar[j].Dot(parDir) / mag );


                for( int k = 0; k < 8; k++ )
                {
                    if( TMath::Abs( kBar[j].X() ) == 666 || TMath::Abs( kBar[j].Y() ) == 666 || TMath::Abs( kBar[j].Z() ) == 666 )
                        angleC[k] = 0;

//                 if( angleC[k]*degree > 90 )
//                     angleC[k] = 180/degree - angleC[k];


                    if( j > 0 && angleC[k] != 0 )
                        cherenkov->Fill( angleC[k]*degree, 1 );

//                     if( k == 1)
//                         cout << i << " kBar: (" << kBarX[j] << ", " << kBarY[j] << ", " << kBarZ[j] << " )  "
//                                 << angleC[k]*degree << " " << freq_beamtest << endl;
                }
            }
        }
    }


//==============================================================================
// Plot
//==============================================================================
    cherenkov->Draw();

//     TF1 f1("f1","gaus(0)+pol2(3)");
//     TF1 f1("f1","pol2(0)");
//     f1.SetParameters(700,44,1,1,100,1);
//     f1.SetParameters(1,100,1);
//     f1.SetParLimits(0,100,1000);
//     f1.SetParLimits(1,43,45);
//     f1.FixParameter(3,-18950);
//     f1.FixParameter(4,870);
//     f1.FixParameter(5,-9.9);
//     f1.SetParLimits(2,-3.2,-4);

//     TF1 f1("f1","gaus(0)");
//     f1.SetParameters(500,44,1);

    TF1 f1("f1","gaus(0)+pol0(3)");
    f1.SetParameters(500,44,1,1000);

    f1.SetLineColor(2);
    cherenkov->Fit("f1","m","",41,47);
    gStyle->SetOptFit(111);
    cout << (f1.GetParameter(2))*17.45 << " +- " << (f1.GetParError(2))*17.45 << " mrad" << endl;
}
