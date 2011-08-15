
//======
// ROOT
//======
#include <TString.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TVector3.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TFrame.h>


//=========
// STD C++
//=========
#include <iostream>

using namespace std;


//==========
// my stuff
//==========
#include "DetEffi.h"
#include "DetEffi.C"



void recoCherenkovAngleTrue( TString inFilename = "", Int_t bining = 100 )
{
  if( inFilename == "" )
  {
    cout << "Usage: recoCherenkovAngleTrue( filename, bining )" << endl;
    return;
  }


  Double_t rad2degree = 180./TMath::Pi();
  Double_t degree2rad = TMath::Pi()/180.;
  Double_t deg2mrad = degree2rad*1000;
  Double_t mrad2deg = rad2degree/1000;

  // plot range
  Double_t xmin_c = 43. *deg2mrad; // number in degree
  Double_t xmax_c = 51. *deg2mrad;



//==============================================================================
// Access to the input ROOT-file
//==============================================================================
  TFile *inFile = new TFile( inFilename );
  TTree *photon = (TTree*) inFile->Get("photon");
  TTree *info = (TTree*) inFile->Get("info");


  Double_t parDirX, parDirY, parDirZ;
  Double_t lambda_min, lambda_max;
  Int_t photon_number;

  info->SetBranchAddress( "particle_dirX", &parDirX );
  info->SetBranchAddress( "particle_dirY", &parDirY );
  info->SetBranchAddress( "particle_dirZ", &parDirZ );
  info->SetBranchAddress( "lambda_min"   , &lambda_min );
  info->SetBranchAddress( "lambda_max"   , &lambda_max );
  info->SetBranchAddress( "photon_number", &photon_number );

  info->GetEntry( 0 );


  if( photon_number == -666 )
  {
    cout << "Photon-gun simualtion is not allowed!" << endl;
    return;
  }


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

  Bool_t opt_angleAcceptance = false;

  if( parDirX == -666 )
  {
    photon->SetBranchAddress( "particle_dirX", &parDirX );
    photon->SetBranchAddress( "particle_dirY", &parDirY );
    photon->SetBranchAddress( "particle_dirZ", &parDirZ );

    opt_angleAcceptance = true;
    cout << "*** included particles with different incidence angles ***" << endl;
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


  if( nEntries > 100000 )
  {
    cout << "*******************************************" << endl;
    cout << "It's recommended to compile this macro.    " << endl;
    cout << "Compile procedure:                         " << endl;
    cout << "root[] gSystem->Load( \"lib_DetEffi.so\" ) " << endl;
    cout << "root[] .L recoCherenkovAngleTrue.cc++      " << endl;
    cout << "*******************************************" << endl;
  }



//==============================================================================
// Event loop
//==============================================================================
  TCanvas *canvas = new TCanvas( "canvas", "", 1 );
  canvas->Draw();
  canvas->SetTopMargin( 0.14 ); // for 2 x-axis

  TH1F *cherenkov = new TH1F( "cherenkov_angle", "", bining, xmin_c, xmax_c);

  cherenkov->GetXaxis()->SetTitle( "#Theta_{c} [mrad]" );
  cherenkov->GetXaxis()->CenterTitle();
  cherenkov->SetMinimum(0);


  Int_t cnt = 0;

  DetEffi *efficiency = new DetEffi(); // detector efficiency


  for( int i = 0; i < nEntries; i++ )
  {
    photon->GetEntry( i );

    if( i%10000 == 0 )
      cout << "Photon: " << i+1 << " / " << nEntries << endl;
//       cout << i+1 << " / " << nEntries << " " << parDirX << " " << parDirY << " " << parDirZ << endl;


    if ( measured )
    {
      cnt++;


      TVector3 parDir( parDirX, parDirY, parDirZ);
      TVector3 kBar( kBarX, kBarY, kBarZ);

      Double_t mag = parDir.Mag() * kBar.Mag();

      if( mag == 0 )
      {
        cout << "Zero vector!" << endl;
        return;
      }


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


//       if( angleC[k]*todegree > 45 && angleC[k]*todegree < 45.5 )
//         cout << k << " " << angleC[k]*rad2degree << " " << thetaC*rad2degree << " " << hitPosX << " " << hitPosY << endl;


      if( mcp_effi )
      {
        if( efficiency->DetEffi::Effi( (int) wavelength ) )
        {
          for( int k = 0; k < 8; k++ )
          {
            if (angleC[k] !=0)
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
            cherenkov->Fill( angleC[k]*1000 );
        }
      }
    }
  }

  cout << "measured photons: " << cnt << endl;


//==============================================================================
// Fit
//==============================================================================
  gStyle->SetStatX(0.97);
  gStyle->SetStatY(0.81);
  gStyle->SetOptStat("mr");
  gStyle->SetOptFit(111);
  gStyle->SetStatFontSize(0.05);

  cherenkov->Draw();

  cout << cherenkov->GetRMS() << " mrad" << endl;


  // Fit
  TF1 *fit;

  if( opt_angleAcceptance )
  {
    fit = new TF1( "fit", "gaus(0)+pol0(3)" );
    fit->SetParNames("A","#mu","#sigma","c");
    fit->SetParameters( 1000, cherenkov->GetMean(), 10, 1 );
  }
  else
  {
    fit = new TF1( "fit", "gaus(0)" );
    fit->SetParNames("A","#mu","#sigma");
    fit->SetParameters( 1000, cherenkov->GetMean(), 10 );
  }

  fit->SetParLimits(2, 0, xmax_c - xmin_c ); // to avoid getting negative sigma
  fit->SetLineColor(2);

  cherenkov->Fit( "fit", "B", "", xmin_c+50, xmax_c-50 );


  cout << (fit->GetParameter(2)) << " +- " << (fit->GetParError(2)) << " mrad" << endl;


  canvas->Update();
  Double_t ymax = canvas->GetFrame()->GetY2();

  TF1 *degreefunc = new TF1( "degreefunc", "x", xmin_c *mrad2deg, xmax_c *mrad2deg);
  TGaxis *degreeAxis = new TGaxis( xmin_c, ymax, xmax_c, ymax, "degreefunc", 510, "-");
  degreeAxis->SetLabelFont(132);
  degreeAxis->SetLabelSize(0.05);
  degreeAxis->SetTitle("#Theta_{c} [#circ]");
  degreeAxis->SetTitleFont(132);
  degreeAxis->SetTitleSize(0.06);
  degreeAxis->CenterTitle(true);
  degreeAxis->Draw();
}
