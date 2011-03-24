
//======
// ROOT
//======
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TMath.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TF1.h>
#include <TGaxis.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void recoCherenkovAngle( Int_t bining = 100, Double_t meanAngle = 44.12, Bool_t data = true )
{
  if( data)
    cout << "***** DATA *****" << endl;
  else
    cout << "***** SIMULATION *****" << endl;


  Double_t todegree = 180./TMath::Pi();
  Double_t indegree = TMath::Pi()/180.;
  Double_t deg2mrad = TMath::Pi()*1000./180.;
  Double_t mrad2deg = 180./(TMath::Pi()*1000.);

  gStyle->SetStatX(0.88);
  gStyle->SetStatY(0.8);
//   gStyle->SetStatH(0.2); // doesn't work
//   gStyle->SetStatX(0.95);
//   gStyle->SetStatY(0.95);
//   gStyle->SetStatX(0.97);
//   gStyle->SetStatY(0.8);
//   gStyle->SetOptStat("");
  gStyle->SetOptStat("mr");
//   gStyle->SetOptFit(111);
//   gStyle->SetOptStat(1);
  gStyle->SetStatFontSize(0.06);

//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
//   TString beamtestFilename = "forDiss/beamtest0909_angleAcceptance_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_angleAcceptance_1GeV_6.375mm_effi_screenPix.root";

//   TString beamtestFilename = "forDiss/beamtest0909_theta29.2_fishTheta0_mcpPlots_gap44_38.root";

//   TString beamtestFilename = "forDiss/beamtest0909_theta29.2_1000par_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta27.0_1000par_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta24.2_1000par_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta23.8_1000par_mcpPlots_gap58_54.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta21.8_1000par_mcpPlots_gap58_54.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta22.0_1000par_woLens_mcpPlots_gap58_54.root";

  Double_t angleData = 22.0;
  cout << "Angle DATA is: " << angleData << endl;

//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09243180103-185844_cutsFinal_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09245173715-193429_cutsFinal_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09248180821-190257_cutsFinal_mcpPlots_gap44_38.root";
//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09249192956-194333_cutsFinal_mcpPlots_gap58_54.root";
//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09250170450-173422_cutsFinal_mcpPlots_gap58_54.root";
//   TString beamtestFilename = "/d/panda02/rhohler/data/pa09250183928_cutsFinal_mcpPlots_gap58_54.root";
  TString beamtestFilename = "/d/panda02/rhohler/data/pa09251230705-233441_cutsFinal_mcpPlots_gap58_54.root";


//   TString kBarFilename     = "/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_mcpPlots_gap44_38.root";
//   TString kBarFilename     = "/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_mcpPlots_gap58_54.root";
  TString kBarFilename     = "/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_woLens_mcpPlots_gap58_54.root";

//   TString beamtestFilename = "forDiss/beamtest0909_theta20_phi20_1000par_6.375mm_effi_poskBarZ_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta20_1000par_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta30_1000par_6.375mm_effi_poskBarZ_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta30_1000par_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta45_1000par_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta30_phi5_1000par_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta30_phi10_1000par_6.375mm_effi_screenPix.root";
//   TString beamtestFilename = "forDiss/beamtest0909_theta30_phi20_1000par_6.375mm_effi_screenPix.root";
//   TString kBarFilename     = "/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_screenPlots.root";


//   TString beamtestFilename = "forDiss/beamtest0909_theta20_1000par_1mm_effi_screenPix.root";


//   TChain *pixel_kBar = new TChain( "pixel" );
//   pixel_kBar->Add("/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_res1_screenPlots.root");
//   pixel_kBar->Add("/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_res1_screenPlots_1.root");
//   pixel_kBar->Add("/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_screenPlots.root");
//   pixel_kBar->Add("/d/panda02/rhohler/sim/kBarList_center_400nm_2000000_screenPlots.root");



  TFile *beamtestFile = new TFile( beamtestFilename );
  TTree *pixel_beamtest = (TTree*) beamtestFile->Get("pixel");
  TTree *info = (TTree*) beamtestFile->Get("info");


  Double_t parDirX, parDirY, parDirZ;
  Double_t thetaC;
  Int_t parDir_size = 1;

  info->SetBranchAddress( "parDirX", &parDirX );
  info->SetBranchAddress( "parDirY", &parDirY );
  info->SetBranchAddress( "parDirZ", &parDirZ );
  info->SetBranchAddress( "thetaC" , &thetaC );
  info->SetBranchAddress( "parDir_size", &parDir_size );
  info->GetEntry( 0 );


  Int_t hitsPerPixel_helper = 1;
  if( !data )
    hitsPerPixel_helper = parDir_size; // default: 50 hits per pixel

  if( parDir_size == -666 )
    hitsPerPixel_helper = 1;

  const Int_t hitsPerPixel = hitsPerPixel_helper;


  TString parDir_size_str;
  parDir_size_str += hitsPerPixel;
  parDir_size_str.Remove( TString::kLeading, ' ' );
  TString parDirX_str = "parDirX[" + parDir_size_str + "]";
  TString parDirY_str = "parDirY[" + parDir_size_str + "]";
  TString parDirZ_str = "parDirZ[" + parDir_size_str + "]";


  Int_t freq_beamtest;
  Double_t phot_parDirX[hitsPerPixel]; // ={0}; doesn't work as compiled version with the interpreter
  Double_t phot_parDirY[hitsPerPixel]; // ={0};
  Double_t phot_parDirZ[hitsPerPixel]; // ={0};

  for( int i=0; i<hitsPerPixel; i++)
  {
    phot_parDirX[i] = 0;
    phot_parDirY[i] = 0;
    phot_parDirZ[i] = 0;
  }

  pixel_beamtest->SetBranchAddress( "freq", &freq_beamtest );
  if( parDirX == -666 && !data )
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
//   gStyle->SetCanvasColor( 0 );        // white
//   gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
//   gStyle->SetFrameFillColor( 0 );
//   gStyle->SetFrameBorderMode( 0 );    // no red frame
//   gStyle->SetHistFillColor( 0 );
//   gStyle->SetPadColor( 0 );
//   gStyle->SetPadBorderMode( 0 );      // no yellow frame
//   gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
//   gStyle->SetTitleFontSize( 0.05 );
//   gStyle->SetPalette( 1 );            // better color palette
//   gStyle->SetStatColor( 0 );          // stat. box color


  TCanvas *canvas = new TCanvas( "canvas", "" ,1 );
  canvas->Draw();
  canvas->SetTopMargin( 0.14 ); // for 2 x-axis


//   TH1F *kkkhist = new TH1F( "kkkhist", "#Theta = 30#circ, #phi = 20#circ, #sigma = 12.2mrad", 8, -0.4, 0.4);
//   kkkhist->GetXaxis()->SetTitle( "k_{y}" );
//   kkkhist->GetXaxis()->CenterTitle();
//   kkkhist->GetYaxis()->SetTitle( "#sigma [mrad]" );
//   kkkhist->GetYaxis()->CenterTitle();
//   kkkhist->SetMarkerSize(1.5);

//   for( int kkk = 1; kkk < 9; kkk++)
//   {


//   TString title = "Cherenkov angle (simulation, par: #Theta = [0,180[#circ, #phi = [0,360[#circ)";
  TString title = "Cherenkov angle (simulation, par: #Theta = 30#circ, #phi = 0#circ)";

//   TH1F *cherenkov = new TH1F( "cherenkov_angle", title, bining, 35, 55);

  Double_t xmin_c = 42. *deg2mrad; // number in degree
  Double_t xmax_c = 46. *deg2mrad;
  TH1F *cherenkov = new TH1F( "cherenkov_angle", "", bining, xmin_c, xmax_c);
//     TH1F *cherenkov = new TH1F( "cherenkov_angle", "", bining, 29, 49);
//     cherenkov->Sumw2(); // error is not sqrt(bin content)
  cherenkov->GetXaxis()->SetTitle( "#Theta_{c} [mrad]" );
  cherenkov->GetXaxis()->CenterTitle();
  cherenkov->GetYaxis()->SetTitle( "Häufigkeit");
//   cherenkov->GetYaxis()->SetTitle( "Number of solutions");
  cherenkov->GetYaxis()->CenterTitle();
  cherenkov->GetYaxis()->SetTitleOffset( 1.1 ); // 1.1
  cherenkov->SetMinimum(0);


//   TH2F *mcp = new TH2F( "","", 8,0,8,8,0,8);
//   mcp->GetXaxis()->SetTitle( "MCP-Spalte" );
//   mcp->GetXaxis()->CenterTitle();
//   mcp->GetYaxis()->SetTitle( "MCP-Reihe");
//   mcp->GetYaxis()->CenterTitle();
//   mcp->GetYaxis()->SetTitleOffset( 0.8 );
//   canvas->SetLeftMargin(0.13);
//   canvas->SetRightMargin(0.15);\
//   Int_t freq_all = 0;



  TH1F *ambiguities = new TH1F("number_of_ambiguities per pixel","", 16, 1, 17);

  TH1F *cherenkovBetter = new TH1F("cherenkov_angle_better","", bining, 42, 46);
  cherenkovBetter->GetXaxis()->SetTitle( "#Theta_{c} [#circ]" );
  cherenkovBetter->GetXaxis()->CenterTitle();
  cherenkovBetter->GetYaxis()->SetTitle( "Number of solutions");
  cherenkovBetter->GetYaxis()->CenterTitle();
  cherenkovBetter->GetYaxis()->SetTitleOffset( 1.1 ); // 1.1


//==============================================================================
// Event loop
//==============================================================================
  if( nPixel_beamtest != nPixel_kBar )
  {
    cout << "Different number pf pixel for beamtest data and kBar tree! (" << nPixel_beamtest << ", " << nPixel_kBar << ")" << endl;
    return;
  }


  Int_t pxY = 0; // row
  Int_t pxX = 0; // col

  Int_t x_bins = 8; // mcp
  Int_t y_bins = 8;

//   Int_t x_bins = 48; // screen 6.375
//   Int_t y_bins = 32;

//   Int_t x_bins = 48; // screen 1mm
//   Int_t y_bins = 32;


  for( int i = 0; i < nPixel_kBar; i++ )
  {
    pixel_kBar->GetEntry( i );
    pixel_beamtest->GetEntry( i );


    if( i%y_bins == 0 && i != 0 )
    {
      pxX++;
      pxY = 0;
    }


    Int_t cut = 0;
    Int_t cut1 = 4000;
    Int_t cut2 = 0;
    Int_t cut3 = 2000;
    Int_t cut4 = 0;

    if( pxX < 8 ) // mcp1
      cut = cut1;

    if( pxX > 7 && pxX < 16 ) // mcp2
      cut = cut2;

    if( pxX > 15 && pxX < 24 ) // mcp3
      cut = cut3;

    if( pxX > 23 ) // mcp4
      cut = cut4;



//     if( pxX < 8 ) // mcp1
//       continue;

    if( pxX > 7 && pxX < 16 ) // mcp2
      continue;

//     if( pxX > 15 && pxX < 24 ) // mcp3
//       continue;

    if( pxX > 23 ) // mcp4
      continue;

//     cout << pxX << " " << pxY << " " << nPixel_kBar << endl;
    //
//     cout << "PIXEL: " << i+1 << " / " << nPixel_kBar << endl;


//     if( freq_beamtest - cut > 0 ) // cut 10000
//     {
//       mcp->Fill(pxX%8,pxY,freq_beamtest-cut);
//       freq_all+= (freq_beamtest - cut);
//     }


    TVector3 kBar[6];

//         Int_t pxX = TMath::FloorNint(i/8) + 1;
//         Int_t pxY = i%8 + 1;
//         cout << pxX << " " << pxY << endl;

//         if( freq_beamtest !=0 )
//             cout << i << endl;

//         timediff->SetBinContent(pxX,pxY,kBarY[2]-kBarY[1]);


    Int_t loopForDiffParctiles;

    if( data || parDirX != -666  ) // have to check w/o -666 (simu stage)
      loopForDiffParctiles = 1;
    else
      loopForDiffParctiles = freq_beamtest;


    for( int l =0; l < loopForDiffParctiles; l++) // only necessary if hits per pixel comes from different particles
    {
      if( freq_beamtest > hitsPerPixel && parDirX == -666 )
        break;

      TVector3 parDir;

      if( parDirX == -666 && !data )
      {
        parDir.SetX(phot_parDirX[l]);
        parDir.SetY(phot_parDirY[l]);
        parDir.SetZ(phot_parDirZ[l]);
      }
      else
      {
        if( data )
        {
          parDir.SetX(-1);
          parDir.SetY(0);
          parDir.SetZ(TMath::Tan(angleData*indegree));

          parDir = parDir.Unit();
        }
        else
        {
          parDir.SetX(parDirX);
          parDir.SetY(parDirY);
          parDir.SetZ(parDirZ);
        }
      }


//       cout << parDir.X() << " " << parDir.Y() << " " << parDir.Z() << " " << endl;


      Int_t n_ambi = 0; // number of ambiguities per pixel
      Double_t bestAngleSolution = -666; // take only 1 solution into account
      Int_t freq_bestSolution = 0;
      Double_t angleDiff = 180;


      // wo reflexion in expansion volume => j<2
      for( int j = 1; j < 6; j++ ) // 0: all, 1: direct, 2: left, 3: right, 4: up, 5: down
      {
        Double_t angleC[8]; // up, down, left, right, forward, backward; total 2*2*2 possibilities in bar


        kBar[j].SetX( kBarX[j] );
        kBar[j].SetY( kBarY[j] );
        kBar[j].SetZ( kBarZ[j] );

        Double_t mag = parDir.Mag() * kBar[j].Mag();

        if( mag == 0 )
          cout << "?" << parDir.Mag() << " " << kBar[j].Mag() << endl;


//         if( kBarY[j] > (kkk*0.1-0.4) || kBarY[j] <= (kkk*0.1-0.5) )
//           continue;


        angleC[0] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( -kBarX[j] );
        kBar[j].SetY( kBarY[j] );
        kBar[j].SetZ( kBarZ[j] );
        angleC[1] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( kBarX[j] );
        kBar[j].SetY( -kBarY[j] );
        kBar[j].SetZ( kBarZ[j] );
        angleC[2] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( kBarX[j] );
        kBar[j].SetY( kBarY[j] );
        kBar[j].SetZ( -kBarZ[j] );
        angleC[3] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( -kBarX[j] );
        kBar[j].SetY( -kBarY[j] );
        kBar[j].SetZ( kBarZ[j] );
        angleC[4] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( -kBarX[j] );
        kBar[j].SetY( kBarY[j] );
        kBar[j].SetZ( -kBarZ[j] );
        angleC[5] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( kBarX[j] );
        kBar[j].SetY( -kBarY[j] );
        kBar[j].SetZ( -kBarZ[j] );
        angleC[6] = TMath::ACos( kBar[j].Dot(parDir) / mag );

        kBar[j].SetX( -kBarX[j] );
        kBar[j].SetY( -kBarY[j] );
        kBar[j].SetZ( -kBarZ[j] );
        angleC[7] = TMath::ACos( kBar[j].Dot(parDir) / mag );


//         angleC[2] = 0; // phi is zero; symmetry case
//         angleC[4] = 0;
//         angleC[6] = 0;
//         angleC[7] = 0;
        //
//         angleC[3] = 0; // no mirrored photons
//         angleC[5] = 0;
//         angleC[6] = 0;
//         angleC[7] = 0;


        for( int k = 0; k < 8; k++ )
        {
          if( TMath::Abs( kBar[j].X() ) == 666 || TMath::Abs( kBar[j].Y() ) == 666 || TMath::Abs( kBar[j].Z() ) == 666 )
            angleC[k] = 0;

          if( angleC[k]*todegree < 46 && angleC[k]*todegree > 42 && (freq_beamtest - cut) > 0 )
          {
            n_ambi++;

            if( TMath::Abs( angleC[k]*todegree - meanAngle ) < angleDiff )
            {
              angleDiff = TMath::Abs( angleC[k]*todegree - meanAngle );
              bestAngleSolution = angleC[k]*todegree;
              freq_bestSolution = (freq_beamtest - cut);

//               cout << angleDiff << " " << angleC[k]*todegree << endl;
            }
          }
        }


        for( int k = 0; k < 8; k++ )
        {
//                 if( angleC[k]*todegree > 90 )
//                     angleC[k] = 180/todegree - angleC[k];


//           if( j > 0 && angleC[k] != 0 && ( (pxX<8 && freq_beamtest > 10000 ) || (pxX > 15 && pxX < 24 && freq_beamtest > 5000 ) ) )
//           if( j > 0 && angleC[k] != 0 && freq_beamtest > cut && j<4 )
          if( j > 0 && angleC[k] != 0 && (freq_beamtest - cut) > 0 && j < 6 )
          {
//             cout << freq_beamtest -cut << endl;

            if( loopForDiffParctiles > 1 )
//               cherenkov->Fill( angleC[k]*todegree, 1 );
              cherenkov->Fill( angleC[k]*1000, 1 );
            else
            {
//               cherenkov->Fill( angleC[k]*todegree, freq_beamtest - cut );
              cherenkov->Fill( angleC[k]*1000, freq_beamtest - cut );
//               cout << "frequi" << endl;
            }
          }

//           if( angleC[k]*todegree > 45 && angleC[k]*todegree < 55 && freq_beamtest != 0 )
//           if( angleC[k]*todegree < 48 && angleC[k]*todegree > 40 && freq_beamtest != 0  )
//             cout << pxX << " " << pxY << " " << k << " " << angleC[k]*todegree << endl;


//           if( k == 1)
//             cout << i << " kBar: (" << kBarX[j] << ", " << kBarY[j] << ", " << kBarZ[j] << " )  "
//                 << angleC[k]*todegree << " " << freq_beamtest << endl;
        }

        ambiguities->Fill(n_ambi);

        if( bestAngleSolution != -666 )
        {
          if( loopForDiffParctiles > 1 )
            cherenkovBetter->Fill( bestAngleSolution, 1 );
          else
            cherenkovBetter->Fill( bestAngleSolution, freq_bestSolution );
        }
      }
    }

    pxY++;
  }


//==============================================================================
// Plot
//==============================================================================
//   cherenkov->Draw("e");
//   cherenkov->Smooth(3);

//   TF1 *divi = new TF1("divi","1",xmin_c,xmax_c);
//   cherenkov->Divide(divi, 1000);
  cherenkov->Draw();

//   mcp->Draw("colz");
//   cout << freq_all << endl;
//   ambiguities->Draw();
//   cherenkovBetter->Draw();


//   cout << kkk*0.1-0.4 << " "; // kkk

//   cout << "RMS: " << cherenkov->GetRMS() << " deg  " << cherenkov->GetRMS()*17.45 << " mrad" << endl;
  cout << "RMS: " << cherenkov->GetRMS() << " mrad  " << cherenkov->GetRMS()*mrad2deg << " degree" << endl;
  cout << "Mean: " << cherenkov->GetMean() << " mrad  " << cherenkov->GetMean()*mrad2deg << " degree" << endl;

//   cout << "RMS: " << cherenkovBetter->GetRMS() << " deg  " << cherenkovBetter->GetRMS()*17.45 << " mrad" << endl;

//   kkkhist->Fill(kkk*0.1-0.4-0.05,cherenkov->GetRMS()*17.45);
//   kkkhist->Draw("P");

//     TF1 f1("f1","gaus(0)+pol2(3)");
// //     TF1 f1("f1","pol2(0)");
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



//   TF1 f1("f1","gaus(0)+pol0(3)");
// //   TF1 f1("f1","gaus(0)+pol3(4)");
// //   TF1 f1("f1","gaus(0)");
// //   f1.SetParameters(1000,44,0.5,1000);
//   f1.SetParameters(1000,770,10,1000);
//   f1.SetParNames("A","#mu","#sigma","c");
// //   f1.SetParLimits(0, 300, 5000);
// //   f1.SetParLimits(1, 43.8, 44.2);
// //   f1.SetParLimits(2, 0, 2);
//   f1.SetParLimits(2, 0, 30);
//   f1.SetParLimits(3, 0, 2000);
  //   //
//   f1.SetLineColor(2);
// //   cherenkov->Fit("f1","m","",41,47);
// //   cherenkov->Fit("f1","w","",41,47);
  //
// //   cherenkov->Fit("f1","","",41,47);
//   cherenkov->Fit("f1","","",41*deg2mrad,47*deg2mrad);
  //
// //   cherenkov->Fit("f1","","",36,42);
//   cherenkov->Draw("e");
  //
// //   cout << (f1.GetParameter(2))*17.45 << " +- " << (f1.GetParError(2))*17.45 << " mrad" << endl;
//   cout << (f1.GetParameter(2)) << " +- " << (f1.GetParError(2)) << " mrad" << endl;

//     }// kkk


//   cherenkov->Draw("X+"); // X+ : x-axis at top
//   TF1 *mradfunc=new TF1("mradfunc","x",TMath::Pi()*1000/180 *xmin_c,TMath::Pi()*1000/180 *xmax_c);
// //   TF1 *mradfunc=new TF1("mradfunc","TMath::Pi()*1000/180 *x",0,180);
//   TGaxis *mradAxis = new TGaxis(xmin_c,0,xmax_c,0,"mradfunc");
//   mradAxis->SetLabelFont(132);
//   mradAxis->SetLabelSize(0.05);
//   mradAxis->SetTitle("[mrad]");
//   mradAxis->SetTitleFont(132);
//   mradAxis->SetTitleSize(0.05);
//   mradAxis->CenterTitle(true);
//   mradAxis->Draw();


//   TF1 *degreefunc=new TF1("degreefunc","x",xmin_c *mrad2deg, xmax_c *mrad2deg);
// //   TF1 *mradfunc=new TF1("mradfunc","TMath::Pi()*1000/180 *x",0,180);
//   TGaxis *degreeAxis = new TGaxis(xmin_c,6100,xmax_c,6100,"degreefunc",510,"-");
//   degreeAxis->SetLabelFont(132);
//   degreeAxis->SetLabelSize(0.05);
//   degreeAxis->SetTitle("#Theta_{c} [#circ]");
//   degreeAxis->SetTitleFont(132);
//   degreeAxis->SetTitleSize(0.06);
//   degreeAxis->CenterTitle(true);
//   degreeAxis->Draw();
}
