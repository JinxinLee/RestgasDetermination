
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
#include <TStyle.h>
#include <TF1.h>
#include <TGaxis.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void recoCherenkovAngleTrue(Int_t bining = 100)
{
    Double_t todegree = 180./TMath::Pi();
    Double_t indegree = TMath::Pi()/180.;
    Double_t deg2mrad = TMath::Pi()*1000./180.;
    Double_t mrad2deg = 180./(TMath::Pi()*1000.);

    gStyle->SetStatX(0.97);
    gStyle->SetStatY(0.8);
    gStyle->SetOptStat("");
//     gStyle->SetOptStat("mr");
    gStyle->SetOptFit(111);
    gStyle->SetStatFontSize(0.05);


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
//     TString beamtestFilename = "forDiss/beamtest0909_angleAcceptance.root";
    TString beamtestFilename = "beamtest_theta30_1gev.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta27.0_1000par.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_1000par.root";
//     TString beamtestFilename = "/u/rhohler/src/examples/forDiss/beamtest0909_theta45_1000par.root";

    TFile *beamtestFile = new TFile( beamtestFilename );
    TTree *photon = (TTree*) beamtestFile->Get("photon");
    TTree *info = (TTree*) beamtestFile->Get("info");


    Double_t parDirX, parDirY, parDirZ;
    Double_t lambda_min, lambda_max;

    info->SetBranchAddress( "particle_dirX", &parDirX );
    info->SetBranchAddress( "particle_dirY", &parDirY );
    info->SetBranchAddress( "particle_dirZ", &parDirZ );
    info->SetBranchAddress( "lambda_min"   , &lambda_min );
    info->SetBranchAddress( "lambda_max"   , &lambda_max );
    info->GetEntry( 0 );


    Bool_t mcp_effi = false;
    if( lambda_min != lambda_max )
    {
        mcp_effi = true;
        cout << "MCP efficiency was enabled" << endl;
    }


    Double_t kBarX, kBarY, kBarZ;
    Double_t thetaC;
    Bool_t measured;
    Double_t wavelength, time;
    Double_t hitPosX, hitPosY;

    if( parDirX == -666 )
    {
        photon->SetBranchAddress( "particle_dirX", &parDirX );
        photon->SetBranchAddress( "particle_dirY", &parDirY );
        photon->SetBranchAddress( "particle_dirZ", &parDirZ );
    }
    photon->SetBranchAddress( "kBarX"     , &kBarX );
    photon->SetBranchAddress( "kBarY"     , &kBarY );
    photon->SetBranchAddress( "kBarZ"     , &kBarZ );
    photon->SetBranchAddress( "thetaC"    , &thetaC );
    photon->SetBranchAddress( "measured"  , &measured );
    photon->SetBranchAddress( "wavelength", &wavelength );
    photon->SetBranchAddress( "time"      , &time );
    photon->SetBranchAddress( "hitPosX"   , &hitPosX );
    photon->SetBranchAddress( "hitPosY"   , &hitPosY );

    Int_t nEntries = photon->GetEntries();


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
    fEffiArray[61] = 0.0075;
    fEffiArray[62] = 0.005;
    fEffiArray[63] = 0.0025;

    for (int i=0; i<70; i++) fEffiArray[i]=fEffiArray[i]*3; // to avoid the removing of too much photons

    TRandom3 rand;


//==============================================================================
// Canvas & plot settings
//==============================================================================
     // set some global options
//     gStyle->SetCanvasColor( 0 );        // white
//     gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
//     gStyle->SetFrameFillColor( 0 );
//     gStyle->SetFrameBorderMode( 0 );    // no red frame
//     gStyle->SetHistFillColor( 0 );
//     gStyle->SetPadColor( 0 );
//     gStyle->SetPadBorderMode( 0 );      // no yellow frame
//     gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
//     gStyle->SetTitleFontSize( 0.05 );
//     gStyle->SetPalette( 1 );            // better color palette
//     gStyle->SetStatColor( 0 );          // stat. box color


    TCanvas *canvas = new TCanvas( "canvas", "" ,1 );
    canvas->Draw();
    canvas->SetTopMargin( 0.14 ); // for 2 x-axis

    TString title = "Cherenkov angle  (par: #Theta = [0,180[#circ, #phi = [0,360[#circ)";
    Double_t xmin_c = 0. *deg2mrad; // number in degree
    Double_t xmax_c = 60. *deg2mrad;
    TH1F *cherenkov = new TH1F( "cherenkov_angle", "", bining, xmin_c, xmax_c); // 40, 48
//     TH1F *cherenkov = new TH1F( "cherenkov_angle", title, 500, 0, 180);
//     cherenkov->Sumw2(); // error is not sqrt(bin content)
    cherenkov->GetXaxis()->SetTitle( "#Theta_{c} [mrad]" );
    cherenkov->GetXaxis()->CenterTitle();
    cherenkov->GetYaxis()->SetTitle( "Häufigkeit");
    cherenkov->GetYaxis()->CenterTitle();
    cherenkov->GetYaxis()->SetTitleOffset(1.1);
    cherenkov->SetMinimum(0);


//==============================================================================
// Event loop
//==============================================================================
    Int_t cnt = 0;

    for( int i = 0; i < nEntries; i++ )
    {
        photon->GetEntry( i );

        if( i%100000 == 0 )
            cout << i+1 << " / " << nEntries << endl; // " " << parDirX << " " << parDirY << " " << parDirZ << endl;


        if ( measured )
        {
            cnt++;


            TVector3 parDir( parDirX, parDirY, parDirZ);
            TVector3 kBar( kBarX, kBarY, kBarZ);

            Double_t mag = parDir.Mag() * kBar.Mag();

            if( mag == 0 )
                cout << "?" << endl;


            Double_t angleC[8]; // up, down, left, right, forward, backward; total 2*2*2 possibilities in bar


            kBar.SetX( kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( kBarZ );
            angleC[0] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( -kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( kBarZ );
            angleC[1] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( kBarZ );
            angleC[2] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( -kBarZ );
            angleC[3] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[3]=0;

            kBar.SetX( -kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( kBarZ );
            angleC[4] = TMath::ACos( kBar.Dot(parDir) / mag );

            kBar.SetX( -kBarX );
            kBar.SetY( kBarY );
            kBar.SetZ( -kBarZ );
            angleC[5] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[5]=0;

            kBar.SetX( kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( -kBarZ );
            angleC[6] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[6]=0;

            kBar.SetX( -kBarX );
            kBar.SetY( -kBarY );
            kBar.SetZ( -kBarZ );
            angleC[7] = TMath::ACos( kBar.Dot(parDir) / mag );
//             if( time < 10 )
//                 angleC[7]=0;

//             angleC[3] = 0; // no mirrored photons
//             angleC[5] = 0;
//             angleC[6] = 0;
//             angleC[7] = 0;

            if( mcp_effi )
            {
                if( wavelength < 700 && (rand.Uniform() < fEffiArray[(int)(wavelength/10+0.5)]) )
                {
                    for( int k = 0; k < 8; k++ )
                    {
                        if (angleC[k] !=0)
//                             cherenkov->Fill( angleC[k]*todegree );
                            cherenkov->Fill( angleC[k]*1000 );
                    }
                }
                else
                    continue;
            }
            else
            {
                for( int k = 0; k < 8; k++ )
                {
                    if (angleC[k] !=0)
//                     cherenkov->Fill( angleC[k]*todegree );
                    cherenkov->Fill( angleC[k]*1000 );

//                     if( angleC[k]*todegree > 45 && angleC[k]*todegree < 45.5 )
//                       cout << k << " " << angleC[k]*todegree << " " << thetaC*todegree << " " << hitPosX << " " << hitPosY << endl;
                }
            }
        }
    }

    cout << "measured photons: " << cnt << endl;


//==============================================================================
// Plot
//==============================================================================
    cherenkov->Draw();

//     cout << cherenkov->GetRMS()*17.45 << " mrad" << endl;
    cout << cherenkov->GetRMS() << " mrad" << endl;

    // Fit
    // do it interactively
    TF1 f1("f1","gaus(0)+pol0(3)");
//     TF1 f1("f1","gaus(0)");
    f1.SetParNames("A","#mu","#sigma","c");
//     f1.SetParameters(1000,44,0.5,1);
//     f1.SetParLimits(2, 0, 2);
    f1.SetParameters(1000,770,10,1);
    f1.SetParLimits(2, 0, 30);
    f1.SetLineColor(2);
//     cherenkov->Fit("f1","","",42,46);
    cherenkov->Fit("f1","","",42*deg2mrad,46*deg2mrad);// 42, 46
    cherenkov->Draw("e");

//     cout << (f1.GetParameter(2))*17.45 << " +- " << (f1.GetParError(2))*17.45 << " mrad" << endl;
    cout << (f1.GetParameter(2)) << " +- " << (f1.GetParError(2)) << " mrad" << endl;


    TF1 *degreefunc=new TF1("degreefunc","x",xmin_c *mrad2deg, xmax_c *mrad2deg);
//   TF1 *mradfunc=new TF1("mradfunc","TMath::Pi()*1000/180 *x",0,180);
    TGaxis *degreeAxis = new TGaxis(xmin_c,7600,xmax_c,7600,"degreefunc",510,"-");
    degreeAxis->SetLabelFont(132);
    degreeAxis->SetLabelSize(0.05);
    degreeAxis->SetTitle("#Theta_{c} [#circ]");
    degreeAxis->SetTitleFont(132);
    degreeAxis->SetTitleSize(0.06);
    degreeAxis->CenterTitle(true);
    degreeAxis->Draw();
}
