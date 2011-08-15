
//======
// ROOT
//======
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH2.h>
#include <TObject.h>
#include <TPad.h>
#include <TVirtualX.h>
#include <TROOT.h>


//=========
// STD C++
//=========
#include <iostream>

using namespace std;



TH1F **timeArr;
Int_t xbins;
Int_t ybins;


void timeSpec( TString inFilename = "", Double_t resolution = 100, Bool_t effiMode = true ) // resolution in mm,
{

  if( inFilename == "" )
  {
    cout << "Usage: timeSpec( filename, resolution, effiMode )" << endl;
    return;
  }


  gSystem->Load( "lib_DetEffi.so" );



//==============================================================================
// Access to the input ROOT-file
//==============================================================================
  TFile *inFile = new TFile( inFilename );
  TTree *info   = (TTree*) inFile->Get( "info" );
  TTree *photon = (TTree*) inFile->Get( "photon" );


  Double_t fishtank_width, fishtank_height;

  info->SetBranchAddress( "fishtank_width" , &fishtank_width );
  info->SetBranchAddress( "fishtank_height", &fishtank_height );
  info->GetEntry( 0 );


  Double_t hitPosDetX, hitPosDetY;
  Bool_t measured;
  Double_t time, wavelength;

  photon->SetBranchAddress( "hitPosDetX", &hitPosDetX );
  photon->SetBranchAddress( "hitPosDetY", &hitPosDetY );
  photon->SetBranchAddress( "measured"  , &measured );
  photon->SetBranchAddress( "time"      , &time );
  photon->SetBranchAddress( "wavelength", &wavelength );

  Int_t nEntries = photon->GetEntries();



//==============================================================================
// Canvas & Plot settings
//==============================================================================
  TCanvas *canvas = new TCanvas( "canvas", "" ,1 );
  canvas->Draw();

  canvas->SetLeftMargin( 0.13 );
  canvas->SetRightMargin( 0.13 );


  Double_t x_dimMin = -fishtank_width/2;
  Double_t x_dimMax = fishtank_width/2;
  Double_t y_dimMin = -fishtank_height/2;
  Double_t y_dimMax = fishtank_height/2;

  Int_t x_binsH =  TMath::CeilNint(fishtank_width / resolution);
  Int_t y_binsH =  TMath::CeilNint(fishtank_height / resolution);

  const Int_t x_bins = x_binsH;
  const Int_t y_bins = y_binsH;
  xbins = x_bins;
  ybins = y_bins;

  if( (Int_t) (fishtank_width*1000) % (Int_t) (resolution*1000) !=0) // max. 3 digits after comma
    x_dimMax = x_bins*resolution - fishtank_width/2;
  if( (Int_t) (fishtank_height*1000) % (Int_t) (resolution*1000) !=0)
    y_dimMax = y_bins*resolution - fishtank_height/2;


  TH2F *screen = new TH2F( "screen","", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );

  timeArr = new TH1F *[x_bins];

  for( int i; i < x_bins; i++ )
    timeArr[i] = new TH1F [y_bins];

  for( int i = 0; i < x_bins; i++ )
  {
    TString i_str;
    i_str += i;
    i_str.Remove( TString::kLeading, ' ' );

    for( int j = 0; j < y_bins; j++ )
    {
      TString j_str;
      j_str += j;
      j_str.Remove( TString::kLeading, ' ' );

      TString hist_str = "time_" + i_str + "_" + j_str;

      timeArr[i][j] = TH1F( hist_str, hist_str, 500, 0, 50 );
    }
  }



//==============================================================================
// Event loop
//==============================================================================
  DetEffi *efficiency = new DetEffi(); // detector efficiency


  for( int i = 0; i < nEntries; i++ )
  {
    photon->GetEntry( i );

    if( i%100000 == 0 )
      cout << "Photon: " << i+1 << " / " << nEntries << endl;

    if( measured == true )
    {
      Int_t pxX = -666;
      Int_t pxY = -666;


      if( !effiMode || efficiency->DetEffi::Effi( (int) wavelength ) ) // effi or effiOLD
      {
        screen->Fill( hitPosDetX, hitPosDetY );

        pxX = TMath::FloorNint( (hitPosDetX + fishtank_width/2) / resolution );
        pxY = TMath::FloorNint( (hitPosDetY + fishtank_height/2) / resolution );

        timeArr[ pxX ][ pxY ].Fill( time );
      }
    }
  }


  screen->SetStats(false);
  screen->Draw("colz");

  canvas->AddExec("ex", "TimeClicked()");
}



void TimeClicked()
{
  TObject *select = gPad->GetSelected();

  if(!select)
    return;

  if (!select->InheritsFrom(TH2::Class()))
    return;

  gPad->GetCanvas()->FeedbackMode(kTRUE); // to see effect on canvas immediately


  static int pxold1 = 0; // to save old positions
  static int pxold2 = 0;
  static int pyold1 = 0;
  static int pyold2 = 0;

  static int sxold = 0;
  static int syold = 0;

  Int_t px = gPad->GetEventX();
  Int_t py = gPad->GetEventY();

  Double_t xc = gPad->AbsPixeltoX(px); // convert world coord. to histogram coord.
  Double_t yc = gPad->AbsPixeltoY(py);

  Double_t x   = gPad->PadtoX(xc); // additional conversion for log-scale
  Double_t y   = gPad->PadtoY(yc);


  TH2 *map = (TH2*) select;

  Int_t binx = map->GetXaxis()->FindBin(x);
  Int_t biny = map->GetYaxis()->FindBin(y);

  cout << "bin: (" << binx-1 << ", " << biny-1 << ");  entries: " << map->GetBinContent( binx, biny ) << endl;

  Double_t xc1   = gPad->XtoPad(map->GetXaxis()->GetBinLowEdge(binx));
  Double_t xc2   = gPad->XtoPad(map->GetXaxis()->GetBinUpEdge(binx));
  Double_t yc1   = gPad->YtoPad(map->GetYaxis()->GetBinLowEdge(biny));
  Double_t yc2   = gPad->YtoPad(map->GetYaxis()->GetBinUpEdge(biny));


  Int_t px1 = gPad->XtoAbsPixel(xc1);
  Int_t px2 = gPad->XtoAbsPixel(xc2);
  Int_t py1 = gPad->YtoAbsPixel(yc1);
  Int_t py2 = gPad->YtoAbsPixel(yc2);

  Int_t dpx = TMath::Abs( px1 - px2 ); // for big cross
  Int_t dpy = TMath::Abs( py1 - py2 );
  Int_t sx  = TMath::FloorNint( dpx/3. );
  Int_t sy  = TMath::FloorNint( dpy/3. );


  // default Virtual X draw mode is "Invert"
  if( pxold1 || pxold2 || pyold1 || pyold2 ) // draw again negates it
  {
    // box
    //     gVirtualX->DrawBox( pxold1, pyold1, pxold2, pyold2, 1 );

    // cross
    //     gVirtualX->DrawLine( pxold1, pyold1, pxold2, pyold2 );
    //     gVirtualX->DrawLine( pxold1, pyold2, pxold2, pyold1 );

    // big cross
    gVirtualX->DrawBox( pxold1, pyold1 - syold, pxold2, pyold1 - 2*syold, TVirtualX::kOpaque ); // (canvas y coord.: top to bottom)
    gVirtualX->DrawBox( pxold1 + sxold, pyold1, pxold1 + 2*sxold, pyold1 - syold, TVirtualX::kOpaque );
    gVirtualX->DrawBox( pxold1 + sxold, pyold1 - 2*syold, pxold1 + 2*sxold, pyold1 - 3*syold, TVirtualX::kOpaque );
  }

  // box
  //   gVirtualX->DrawBox( px1, py1, px2, py2, 1 );

  // cross
  //     gVirtualX->DrawLine( px1, py1, px2, py2 );
  //     gVirtualX->DrawLine( px1, py2, px2, py1 );

  // big cross
  gVirtualX->DrawBox( px1, py1 - sy, px2, py1 - 2*sy, TVirtualX::kOpaque ); // (canvas y coord.: top to bottom)
  gVirtualX->DrawBox( px1 + sx, py1, px1 + 2*sx, py1 - sy, TVirtualX::kOpaque );
  gVirtualX->DrawBox( px1 + sx, py1 - 2*sy, px1 + 2*sx, py1 - 3*sy, TVirtualX::kOpaque );


  pxold1 = px1; // save old positions
  pxold2 = px2;
  pyold1 = py1;
  pyold2 = py2;

  sxold = sx;
  syold = sy;


  TCanvas *c2 = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("c2");

  if( !c2 )
    c2 = new TCanvas( "c2", "", 300, 300, 700, 500 );

  c2->cd();

  if( binx < 1 || biny < 1 || binx > xbins || biny > ybins ) // TH2 starts with 1,1
    return;

  timeArr[binx-1][biny-1].Draw();

  if( timeArr[binx-1][biny-1].GetEntries() != map->GetBinContent( binx, biny ) )
    cout << "Entries don't match!" << endl;

  c2->Update();
}
