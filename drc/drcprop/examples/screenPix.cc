
//======
// ROOT
//======
#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TH2.h>


//=========
// STD C++
//=========
#include <iostream>

using namespace std;



// resolution in mm, poskBarZ for photons only flying directly to the expansion volume
void screenPix( TString inFilename = "",
                Double_t resolution = 100, Bool_t effiMode = true, Bool_t poskBarZ = false, Int_t hitsPerPixel_par = 200 )
{
  if( inFilename == "" )
  {
    cout << "Usage: screenPix( filename, resolution, effiMode, poskBarZ, hitsPerPixel_par )" << endl;
    return;
  }


  gSystem->Load( "lib_DetEffi.so" );



//==============================================================================
// In- & output
//==============================================================================
  TFile *inFile = new TFile( inFilename );
  TTree *info   = (TTree*) inFile->Get("info");
  TTree *photon = (TTree*) inFile->Get("photon");


  Double_t fishtank_width, fishtank_height;
  Double_t parDirX, parDirY, parDirZ;

  info->SetBranchAddress( "fishtank_width" , &fishtank_width );
  info->SetBranchAddress( "fishtank_height", &fishtank_height );
  info->SetBranchAddress( "particle_dirX", &parDirX );
  info->SetBranchAddress( "particle_dirY", &parDirY );
  info->SetBranchAddress( "particle_dirZ", &parDirZ );
  info->GetEntry( 0 );


  Double_t hitPosDetX, hitPosDetY;
  Double_t kBarZ;
  Bool_t measured;
  Double_t thetaC, wavelength;
  Double_t phot_parDirX, phot_parDirY, phot_parDirZ;

  photon->SetBranchAddress( "hitPosDetX", &hitPosDetX );
  photon->SetBranchAddress( "hitPosDetY", &hitPosDetY );
  photon->SetBranchAddress( "kBarZ"     , &kBarZ );
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

  TString poskBarZ_str = "";
  if( poskBarZ )
    poskBarZ_str = "poskBarZ_";

  TString outFilename = inFilename;
  outFilename.Resize( inFilename.Length() - 5 ); // remove file extension .root
  outFilename = outFilename + "_res" + res_str + "_" + effiMode_str + poskBarZ_str + "screenPix.root";

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


  Int_t freq[x_bins][y_bins];

  for( int i = 0; i < x_bins; i++ )
  {
    for( int j = 0; j < y_bins; j++ )
      freq[i][j] = 0;
  }


  Int_t hitsPerPixel_helper;
  if( parDirX == -666 )
    hitsPerPixel_helper = hitsPerPixel_par; // default: 150 hits per pixel
  else
    hitsPerPixel_helper = 1; // to avoid initiliazing too bog array if is not used

  const Int_t hitsPerPixel = hitsPerPixel_helper;

  Int_t hitsPerPixel_branchHelper; // for Branch because it expected non-const variable
  if( parDirX == -666 )
    hitsPerPixel_branchHelper = hitsPerPixel;
  else
    hitsPerPixel_branchHelper = -666;


  TString hitsPerPixel_str;
  hitsPerPixel_str += hitsPerPixel;
  hitsPerPixel_str.Remove( TString::kLeading, ' ' );
  TString parDirX_str = "parDirX[" + hitsPerPixel_str + "]";
  TString parDirY_str = "parDirY[" + hitsPerPixel_str + "]";
  TString parDirZ_str = "parDirZ[" + hitsPerPixel_str + "]";
  TString parDirX_str2 = parDirX_str + "/D";
  TString parDirY_str2 = parDirY_str + "/D";
  TString parDirZ_str2 = parDirZ_str + "/D";

  Double_t parDirX_px[x_bins][y_bins][hitsPerPixel];
  Double_t parDirY_px[x_bins][y_bins][hitsPerPixel];
  Double_t parDirZ_px[x_bins][y_bins][hitsPerPixel];

  for( int i = 0; i < x_bins; i++ )
  {
    for( int j = 0; j < y_bins; j++ )
    {
      for( int k = 0; k < hitsPerPixel; k++ )
      {
        parDirX_px[i][j][k] = 0;
        parDirY_px[i][j][k] = 0;
        parDirZ_px[i][j][k] = 0;
      }
    }
  }

  Double_t parDirXH[hitsPerPixel];
  Double_t parDirYH[hitsPerPixel];
  Double_t parDirZH[hitsPerPixel];

  for( int i = 0; i < hitsPerPixel; i++ )
  {

    parDirXH[i] = 0;
    parDirYH[i] = 0;
    parDirZH[i] = 0;
  }


  Int_t px_freq  = 0;

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
  infoTree->Branch( "parDir_size", &hitsPerPixel_branchHelper, "parDir_size/I" );
  infoTree->Branch( "thetaC" , &thetaC , "thetaC/D" );
  infoTree->Branch( "x_bins" , &x_binsH, "x_bins/I" );
  infoTree->Branch( "y_bins" , &y_binsH, "y_bins/I" );




//==============================================================================
// Event loop
//==============================================================================
  TCanvas *canvas = new TCanvas( "canvas", "", 1 );
  canvas->Draw();

  canvas->SetLeftMargin( 0.13 );
  canvas->SetRightMargin( 0.13 );

  TH2F *screen = new TH2F( "screen","", x_bins, x_dimMin, x_dimMax, y_bins, y_dimMin, y_dimMax );


  DetEffi *efficiency = new DetEffi(); // detector efficiency


  for( int i = 0; i < nEntries; i++ )
  {
    photon->GetEntry( i );

    if( i%100000 == 0 )
      cout << "Photon: " << i+1 << " / " << nEntries << endl;


    if( measured == true)
    {
      Int_t pxX = -666;
      Int_t pxY = -666;

      if( !effiMode || efficiency->DetEffi::Effi( (int) wavelength ) ) // effi or effiOLD
      {
        if( (poskBarZ && kBarZ > 0) || !poskBarZ )
        {
          screen->Fill( hitPosDetX, hitPosDetY );

          pxX = TMath::FloorNint( (hitPosDetX + fishtank_width/2) / resolution );
          pxY = TMath::FloorNint( (hitPosDetY + fishtank_height/2) / resolution );

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
      }
    }
  }



//==============================================================================
// Save data
//==============================================================================
  infoTree->Fill();


  screen->Draw("colz");
  canvas->Write( "screen");
  canvas->Close();


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


  pixelTree->Write();
  infoTree->Write();
  outFile->Close();

  cout << "Root-file " << outFilename << " was written" << endl;

  TFile::Open( outFilename );
}
