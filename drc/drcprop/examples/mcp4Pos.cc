
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
#include <TMarker.h>
#include <TMath.h>
#include <TRandom3.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;


void mcp4Pos( Int_t run = 0, Double_t gap1 = 44, Double_t gap2 = 38, Bool_t data = true, Bool_t simu = true )
// void mcp4Pos( TString simi, Int_t run = 1, Double_t gap1 = 58, Double_t gap2 = 54, Bool_t data = false, Bool_t simu = true )
// void mcp4Pos( Int_t run, Bool_t data = false, Bool_t simu = true, Double_t gap = 80 ) // MCP gap (default either 80 or 110mm)
{
  if( !data && !simu )
  {
    cout << "DATA and SIMULATION are disabled => only SIMUALTION is used now" << endl;
    simu = true;
  }


  TString dataFilename[9];
  dataFilename[0] = "/d/panda02/rhohler/data/pa09242172124-191508_cutsFinal.root"; // theta30
  dataFilename[1] = "/d/panda02/rhohler/data/pa09243180103-185844_cutsFinal.root"; // theta30
//   dataFilename[1] = "/d/panda02/rhohler/data/pa09243180103_cutsFinal.root"; // theta30 test
  dataFilename[2] = "/d/panda02/rhohler/data/pa09245173715-193429_cutsFinal.root"; // theta27
  dataFilename[3] = "/d/panda02/rhohler/data/pa09248180821-190257_cutsFinal.root"; // theta24
  dataFilename[4] = "/d/panda02/rhohler/data/pa09249192956-194333_cutsFinal.root"; // theta24, gap 110
  dataFilename[5] = "/d/panda02/rhohler/data/pa09250170450-173422_cutsFinal.root"; // theta21, z?
  dataFilename[6] = "/d/panda02/rhohler/data/pa09250180203-182224_cutsFinal.root"; // theta21, z+18
  dataFilename[7] = "/d/panda02/rhohler/data/pa09250183928_cutsFinal.root"; // theta21, z+9
  dataFilename[8] = "/d/panda02/rhohler/data/pa09251230705-233441_cutsFinal.root"; // theta21, w/o lens


  TString simFilename[9];
//   simFilename[0] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta30_fishTheta0.root";
//   simFilename[1] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta29.2_fishTheta0.root";
//   simFilename[2] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta27_fishTheta0.root";
//   simFilename[3] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta24.2_fishTheta0.root";
//   simFilename[4] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta23.8_fishTheta0.root";
//   simFilename[5] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_fishTheta0.root";
//   simFilename[6] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta22_fishTheta0.root";
//   simFilename[7] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_fishTheta0.root";
//   simFilename[8] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta22_fishTheta0_woLens.root";

  simFilename[0] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta20_fishTankBlackened.root";
//   simFilename[0] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta30_1000par.root";
  simFilename[1] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta29.2_1000par.root";
  simFilename[2] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta27.0_1000par.root";
  simFilename[3] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta24.2_1000par.root";
  simFilename[4] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta23.8_1000par.root";
  simFilename[5] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_1000par.root";
  simFilename[7] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta21.8_1000par.root";
  simFilename[8] = "/u/rhohler/src/examples/forDiss/beamtest0909_theta22.0_1000par_woLens.root";

//   for( int i =0; i < 9; i++)
//     simFilename[i] = simi;


//==============================================================================
// Access to the input ROOT-file
//==============================================================================
  TString outFilename;
  if( data )
  {
    outFilename = dataFilename[run];
    cout << "DATA from       : " << dataFilename[run] << endl;
  }
  else
    outFilename = simFilename[run];

  if( simu )
    cout << "Simulation from : " << simFilename[run] << endl;


  outFilename.Resize( outFilename.Length() - 5 ); // remove file extension .root
//   TString gap_str;
//   gap_str += gap;
//   gap_str.Remove( TString::kLeading, ' ' );
  TString gap1_str;
  gap1_str += gap1;
  TString gap2_str;
  gap2_str += gap2;
  gap1_str.Remove( TString::kLeading, ' ' );
  gap2_str.Remove( TString::kLeading, ' ' );
//   outFilename = outFilename +"_mcpPlots_gap" + gap_str + ".root";
  outFilename = outFilename +"_mcpPlots_gap" + gap1_str + "_" + gap2_str + ".root";


  TFile *dataFile = new TFile( dataFilename[run] );
  TTree *tree     = (TTree*) dataFile->Get("tree");

  const Int_t arr = 210;

  Int_t col[arr]={0};
  Int_t row[arr]={0};
  Int_t mcpD[arr]={0};
  Int_t nPix = -1;

  tree->SetBranchAddress( "nPix", &nPix );
  tree->SetBranchAddress( "col" , col );
  tree->SetBranchAddress( "row" , row );
  tree->SetBranchAddress( "mcp" , mcpD );

  Double_t max_nPix  = tree->GetMaximum( "nPix" );

  Int_t data_nEntries = tree->GetEntries();

  if( max_nPix > arr )
  {
    cout << "PLEASE increase the array length from " << arr << " to " << max_nPix << "!!!" << endl;
    return;
  }


  TFile *simFile = new TFile( simFilename[run] );
  TTree *info     = (TTree*) simFile->Get("info");
  TTree *photon   = (TTree*) simFile->Get("photon");
  TCanvas *screen = (TCanvas*) simFile->Get("Screen");

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
  Double_t thetaC = -666;
  Double_t wavelength;
  Int_t color;
  photon->SetBranchAddress( "hitPosX" , &hitPosX );
  photon->SetBranchAddress( "hitPosY" , &hitPosY );
  photon->SetBranchAddress( "measured", &measured );
  photon->SetBranchAddress( "thetaC"  , &thetaC );
  photon->SetBranchAddress( "color"   , &color );
  photon->SetBranchAddress( "wavelength", &wavelength );

  Int_t sim_nEntries = photon->GetEntries();


  TFile *outFile  = new TFile( outFilename, "RECREATE" );
  TTree *pixelTree = new TTree( "pixel", outFilename ); // for example: MCP1 is pixel 0 - 63; starts at (0,0), (0,1) ...
  TTree *infoTree  = new TTree( "info", outFilename );

  Int_t freq[32][8] = { 0 };
  Int_t px_freq  = 0;
  pixelTree->Branch( "freq", &px_freq, "freq/I" );

  infoTree->Branch( "parDirX", &parDirX, "parDirX/D" );
  infoTree->Branch( "parDirY", &parDirY, "parDirY/D" );
  infoTree->Branch( "parDirZ", &parDirZ, "parDirZ/D" );
  infoTree->Branch( "thetaC" , &thetaC , "thetaC/D" );


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
// MCP positions & plot declarations
//==============================================================================
  Double_t mcp_dim = 75; // MCP case is 71 mm long and wide + 2mm frame due to the holder
  Double_t mcp_active = 51; // MCP active area is 51 mm long and wide

  Double_t minX_dim[4];
  Double_t minY_dim[4];
  minX_dim[1] = -mcp_dim - gap1; // gap: 80 => -115; gap: 110 => -130; old gap/2
  minY_dim[1] = -100; // fishtank bottom (-fishtank_height/2)
  minX_dim[2] = minX_dim[1];
  minY_dim[2] = minY_dim[1] + mcp_dim;
  minX_dim[3] = minX_dim[2];
  minY_dim[3] = minY_dim[2] + mcp_dim;
  minX_dim[0] = gap2; // old gap/2
  minY_dim[0] = minY_dim[2];

//   cout << "MCP gap is : " << gap << "mm" << endl;
  cout << "MCP gap is : " << gap1 << " \& " << gap2 << "mm" << endl;

  Double_t shift = ( mcp_dim - mcp_active ) / 2; // to shift active area in the center of MCP

  Double_t maxX_dim[4];
  Double_t maxY_dim[4];

  Double_t minX_active[4];
  Double_t minY_active[4];
  Double_t maxX_active[4];
  Double_t maxY_active[4];

  TLine line_mcpCase[4][4]; // MCP position case
  TLine line_mcpArea[4][4]; // MCP position avtive area

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


    line_mcpCase[i][0] = TLine( minX_dim[i], minY_dim[i], minX_dim[i], maxY_dim[i] );
    line_mcpCase[i][1] = TLine( minX_dim[i], maxY_dim[i], maxX_dim[i], maxY_dim[i] );
    line_mcpCase[i][2] = TLine( maxX_dim[i], maxY_dim[i], maxX_dim[i], minY_dim[i] );
    line_mcpCase[i][3] = TLine( maxX_dim[i], minY_dim[i], minX_dim[i], minY_dim[i] );

    line_mcpArea[i][0] = TLine( minX_active[i], minY_active[i], minX_active[i], maxY_active[i] );
    line_mcpArea[i][1] = TLine( minX_active[i], maxY_active[i], maxX_active[i], maxY_active[i] );
    line_mcpArea[i][2] = TLine( maxX_active[i], maxY_active[i], maxX_active[i], minY_active[i] );
    line_mcpArea[i][3] = TLine( maxX_active[i], minY_active[i], minX_active[i], minY_active[i] );

    for( int j = 0; j < 4; j++ )
    {
      line_mcpCase[i][j].SetLineWidth(3);
      line_mcpArea[i][j].SetLineWidth(3);
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
//         mcp[i]->SetMaximum(50); // 500


    mcpLabel[i] = new TPaveText( minX_active[i], maxY_active[i], maxX_active[i], maxY_dim[i] );
    mcpLabel[i]->SetFillColor( 0 );
    mcpLabel[i]->AddText( mcpLabel_str[i] );
  }


  Double_t fishtank_heightOffset_dim = 30; // to get space to draw full MCP which exceeds the fishtank
  Double_t oilgap = 10; // difference between oil level and fishtank height
  Double_t offsetX = fishtank_width/2; // coordination origin (default is in the center of the fishtank)
  Double_t offsetY = fishtank_height/2;
  Double_t scaleX = 1/(offsetX*2); // coordinates go from 0 to 1
//   Double_t scaleFishY = 1/(offsetY*2-oilgap); // coordinates with regard to the fishtank
  Double_t scaleY = 1/(offsetY*2 + fishtank_heightOffset_dim); // to draw full MCP which exceeds the fishtank

  Double_t minX_bar = -slab_width/2;
  Double_t minY_bar = -slab_height/2;
  Double_t maxX_bar = +slab_width/2;
  Double_t maxY_bar = +slab_height/2;


  TLine *tankBorder1 = new TLine( -fishtank_width/2, +fishtank_height/2, minX_dim[3], +fishtank_height/2 );
  TLine *tankBorder2 = new TLine( maxX_dim[3], +fishtank_height/2, +fishtank_width/2, +fishtank_height/2 );
  TLine *tankBorder3 = new TLine( +fishtank_width/2, +fishtank_height/2, +fishtank_width/2, +fishtank_height/2-oilgap );
  TLine *tankBorder4 = new TLine( +fishtank_width/2, +fishtank_height/2-oilgap, +fishtank_width/2, -fishtank_height/2 );
  TLine *tankBorder5 = new TLine( +fishtank_width/2, -fishtank_height/2, -fishtank_width/2, -fishtank_height/2 );
  TLine *tankBorder6 = new TLine( -fishtank_width/2, -fishtank_height/2, -fishtank_width/2, +fishtank_height/2-oilgap );
  TLine *tankBorder7 = new TLine( -fishtank_width/2, +fishtank_height/2-oilgap, -fishtank_width/2, +fishtank_height/2 );

  tankBorder1->SetLineWidth(3);
  tankBorder2->SetLineWidth(3);
  tankBorder3->SetLineWidth(3);
  tankBorder4->SetLineWidth(3);
  tankBorder5->SetLineWidth(3);
  tankBorder6->SetLineWidth(3);
  tankBorder7->SetLineWidth(3);

  tankBorder1->SetLineColor(4);
  tankBorder2->SetLineColor(4);
  tankBorder3->SetLineColor(4);
  tankBorder4->SetLineColor(4);
  tankBorder5->SetLineColor(4);
  tankBorder6->SetLineColor(4);
  tankBorder7->SetLineColor(4);


  TLine *barBorder1 = new TLine( +slab_width/2, +slab_height/2, +slab_width/2, -slab_height/2 );
  TLine *barBorder2 = new TLine( +slab_width/2, -slab_height/2, -slab_width/2, -slab_height/2 );
  TLine *barBorder3 = new TLine( -slab_width/2, -slab_height/2, -slab_width/2, +slab_height/2 );
  TLine *barBorder4 = new TLine( -slab_width/2, +slab_height/2, +slab_width/2, +slab_height/2 );

  barBorder1->SetLineWidth(3);
  barBorder2->SetLineWidth(3);
  barBorder3->SetLineWidth(3);
  barBorder4->SetLineWidth(3);


//==============================================================================
// Canvas settings
//==============================================================================
  TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 1050, 810 );
  canvas->Draw();

  TCanvas *canvasScreen = new TCanvas( "canvasScreen", "" ,200, 10, 700, 490 ); // 510
  canvasScreen->Draw();
  TPad *padScreen = new TPad("ps","",0,0,1,1);
  padScreen->Draw();

  TCanvas *canvas4MCP = new TCanvas( "canvas4MCP", "" ,200, 10, 525, 810 );
  canvas4MCP->Draw();

  TCanvas canvasMCP = TCanvas( "canvasMCP", "" ,200, 10, 530, 500 );
  canvasMCP.SetLeftMargin(0.13);
  canvasMCP.SetRightMargin(0.15);
  canvasMCP.Draw();

  TCanvas *canvasTank = new TCanvas( "canvasTank", "" ,200, 10, 1050, 810 );
  canvasTank->Draw();


  canvas->cd();
  TPad *pad[5];
  pad[0] = new TPad( "p1", "", 0.00, 0.28, 0.49, 0.72 );
  pad[1] = new TPad( "p2", "", 0.75, 0.35, 1.00, 0.66 );
  pad[2] = new TPad( "p3", "", 0.50, 0.02, 0.75, 0.33 );
  pad[3] = new TPad( "p4", "", 0.50, 0.35, 0.75, 0.66 );
  pad[4] = new TPad( "p5", "", 0.50, 0.68, 0.75, 0.99 );

  canvas4MCP->cd();
  TPad *pad4MCP[4];
  pad4MCP[0] = new TPad( "pm1", "", 0.50, 0.35, 1.00, 0.66 );
  pad4MCP[1] = new TPad( "pm2", "", 0.00, 0.02, 0.50, 0.33 );
  pad4MCP[2] = new TPad( "pm3", "", 0.00, 0.35, 0.50, 0.66 );
  pad4MCP[3] = new TPad( "pm4", "", 0.00, 0.68, 0.50, 0.99 );

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
      pad[i]->SetLeftMargin(0.15);
      pad[i]->SetRightMargin(0.15);
      pad[i]->SetTopMargin(0.12); // 0.12
      pad[i]->SetBottomMargin(0.12);

      pad4MCP[i-1]->SetLeftMargin(0.15);
      pad4MCP[i-1]->SetRightMargin(0.15);
      pad4MCP[i-1]->SetTopMargin(0.12); // 0.12
      pad4MCP[i-1]->SetBottomMargin(0.12);

      canvas4MCP->cd();
      pad4MCP[i-1]->Draw();
    }

    canvas->cd();

    pad[i]->Draw();
  }


  for( int i = 0; i < 7; i++ )
  {
//         if( i > 1 && i < 6 )
//         if( i > 1 && i < 6 )
//         {
    padTank[i]->SetLeftMargin(0);
    padTank[i]->SetRightMargin(0);
    padTank[i]->SetTopMargin(0);
    padTank[i]->SetBottomMargin(0);
//         }

    if( i == 6 )
      padTank[i]->SetFillStyle(3004);

    canvasTank->cd();
    padTank[i]->Draw();
  }


//==============================================================================
// Event loop
//==============================================================================
  canvasTank->cd();
  padTank[0]->cd();
  TH2F *sim_helper1 = new TH2F("sim_helper1","", 100, -fishtank_width/2, +fishtank_width/2, 100, -fishtank_height/2,
                               (+fishtank_height/2+fishtank_heightOffset_dim));
  sim_helper1->SetStats(false);
  sim_helper1->Draw("AH");

  padTank[1]->cd();
  padTank[1]->SetFrameLineColor(0); // w/o line border
  TH2F *sim_helper2 = new TH2F("sim_helper2","", 100, -fishtank_width/2, +fishtank_width/2 ,100, -fishtank_height/2,
                               (+fishtank_height/2-oilgap));
  sim_helper2->SetStats(false);
  sim_helper2->Draw("AH");

  if( simu )
  {
    cout << "Event loop: Simulation" << endl;

    for( int i = 0; i < sim_nEntries; i++ )
    {
      photon->GetEntry( i );

      if( measured == true)
      {
        Int_t n_test = 0;

        Int_t pxX = -666;
        Int_t pxY = -666;

        if( wavelength < 700 && (rand.Uniform() < fEffiArray[(int)(wavelength/10+0.5)]) )
        {
          TMarker* t = new TMarker( hitPosX, hitPosY, 7);
          t->SetMarkerColor( 2 );
          t->SetMarkerSize(0.5);

//           if( hitPosY > (+fishtank_height/2-oilgap) )
//             padTank[0]->cd();
//           else
            padTank[1]->cd();

          t->Draw("same");

          if( !data && hitPosY < +fishtank_height/2-oilgap )
          {
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
      }
    }
  }


  if( data )
  {
    cout << "Event loop: Data" << endl;

    for( int i = 0; i < data_nEntries; i++ )
    {
      tree->GetEntry( i );

      if( i%500000 == 0)
        cout << "Event: " << i << " / " << data_nEntries << endl;


      for( int j = 0; j < nPix; j++)
      {
        Int_t pxX = -666;
        Int_t pxY = -666;

//       cout << mcpD[j]-1 << col[j] << row[j] << endl;

        Double_t content = mcp[mcpD[j]-1]->GetBinContent(col[j]+1,row[j]+1);
        mcp[mcpD[j]-1]->SetBinContent( col[j]+1, row[j]+1, content+1 );

        pxX = col[j] + 8*(mcpD[j]-1);
        pxY = row[j];

//       cout << pxX << " " << pxY << endl;

        if( pxX == -666 )
          continue;
        else
          freq[ pxX ][ pxY ]++;
      }
    }
  }


//==============================================================================
// Plot histograms
//==============================================================================
  cout << "Plotting" << endl;
  canvas->cd();
  pad[0]->cd();
  screen->DrawClonePad();
//     screen->Close(); // w/o -> Error in <RootX11ErrorHandler>

  canvasScreen->cd();
  padScreen->cd();
//     TCanvas *screen2 = (TCanvas*) inFile->Get("Screen");
  screen->DrawClonePad();
  screen->Close(); // w/o -> Error in <RootX11ErrorHandler>

  for( int i = 0; i < 4; i++ )
  {
    canvas->cd();
    pad[i+1]->cd();
    mcp[i]->Draw( "colz" );

    pad[0]->cd();
    mcpLabel[i]->Draw("same");

    for( int j = 0; j < 4; j++ )
    {
      line_mcpCase[i][j].Draw("");
      line_mcpArea[i][j].Draw("");
    }


    canvas4MCP->cd();
    pad4MCP[i]->cd();
    mcp[i]->Draw( "colz" );

    canvasScreen->cd();
    padScreen->cd();
    mcpLabel[i]->Draw("same");

    for( int j = 0; j < 4; j++ )
    {
      line_mcpCase[i][j].Draw("");
      line_mcpArea[i][j].Draw("");
    }
  }

  canvas->Write( "screenMCP");
  canvas->Close();

  canvasScreen->Write( "screen");
  canvasScreen->Close();

  canvas4MCP->Write( "4MCP");
  canvas4MCP->Close();


  canvasMCP.cd();

//     for( int i = 0; i < 4; i++ )
//     {
//         mcp[i]->Draw( "colz" );
//         canvasMCP.Write( mcpLabel_str[i] );
//         canvasMCP.Clear();
//     }
  //
  canvasMCP.Close();


  canvasTank->cd();

  for( int i = 0; i < 4; i++ )
  {
    padTank[i+2]->cd();
    mcp[i]->SetTitle("");
    mcp[i]->Draw( "col AH" );

    for( int j = 0; j < 4; j++ )
    {
      padTank[1]->cd();
      line_mcpCase[i][j].Draw("");
      line_mcpArea[i][j].Draw("");
    }
  }

  for( int j = 0; j < 4; j++ )
  {
    padTank[0]->cd();
    line_mcpCase[3][j].Draw("");
    line_mcpArea[3][j].Draw("");
  }

  padTank[0]->cd();
  tankBorder1->Draw("");
  tankBorder2->Draw("");
  tankBorder3->Draw("");
  tankBorder7->Draw("");

  padTank[1]->cd();
  tankBorder4->Draw("");
  tankBorder5->Draw("");
  tankBorder6->Draw("");

  barBorder1->Draw("");
  barBorder2->Draw("");
  barBorder3->Draw("");
  barBorder4->Draw("");


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

  TFile::Open( outFilename );
}
