
//======
// ROOT
//======
#include <TCanvas.h>
#include <TFile.h>
#include <TPolyLine.h>
#include <TString.h>
#include <TTree.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void photTrajectory( TString inFilename = "", Double_t photonID = 0 ) // ID 0 means all
{

  if( inFilename == "" )
  {
    cout << "Usage: photTrajectory( filename, photonID )" << endl;
    return;
  }


  //==============================================================================
  // Access to the input ROOT-file & canvas settings
  //==============================================================================
  TFile *inFile_wo = new TFile( "forSetup.root" ); // w/o air box
  TCanvas *setup = (TCanvas*) inFile_wo->Get("Setup");

  TFile *inFile = new TFile( inFilename );

  TTree *infoTree   = (TTree*) inFile->Get("info");
  TTree *photonTree = (TTree*) inFile->Get("photon");


  Int_t refl_limit;
  infoTree->SetBranchAddress( "refl_limit", &refl_limit );
  infoTree->GetEntry( 0 );


  int add_pos = 1 + 3*2 + 1; // 1 start + 3 volume transition (+ tiny shifts) + detector
  const int pos_size = refl_limit + 1 + add_pos; // reflection limit + 1 exceed + additional positions
  Double_t posX[ pos_size ], posY[ pos_size ], posZ[ pos_size ];

  TString pos_size_str;
  pos_size_str += pos_size;
  pos_size_str.Remove( TString::kLeading, ' ' );
  TString posX_str = "posX[" + pos_size_str + "]";
  TString posY_str = "posY[" + pos_size_str + "]";
  TString posZ_str = "posZ[" + pos_size_str + "]";

  Int_t index_pos, nRefl;
  Bool_t measured, absorbed, lost;

  photonTree->SetBranchAddress( posX_str   , posX );
  photonTree->SetBranchAddress( posY_str   , posY );
  photonTree->SetBranchAddress( posZ_str   , posZ );
  photonTree->SetBranchAddress( "index_pos" , &index_pos );
  photonTree->SetBranchAddress( "nRefl"     , &nRefl );
  photonTree->SetBranchAddress( "measured"  , &measured );
  photonTree->SetBranchAddress( "absorbed"  , &absorbed );
  photonTree->SetBranchAddress( "lost"      , &lost );

  Int_t nEntries = photonTree->GetEntries();


  TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
  canvas->Draw();
  setup->DrawClonePad();
  setup->Close(); // w/o -> Error in <RootX11ErrorHandler>


  //==============================================================================
  // Event loop
  //==============================================================================
  Int_t n_ph = 0;
  Int_t n_measured = 0;
  Int_t n_absorbed = 0;
  Int_t n_lost = 0;


  for( int i = 0; i < nEntries; i++ )
  {
    photonTree->GetEntry( i );
    n_ph++;

    if( measured == true )
      n_measured++;
    if( absorbed == true )
      n_absorbed++;
    if( lost == true )
      n_lost++;

    if( n_ph == photonID || photonID == 0 )
    {
      if( photonID != 0 )
      {
        cout << "Reflections: " << nRefl << endl;
        cout << "Fate       : ";
        if( measured == true )
          cout << "measured" << endl;
        if( absorbed == true )
          cout << "absorbed" << endl;
        if( lost == true )
          cout << "lost" << endl;
      }

      if( lost == true )
      {
//         if( n_ph < 100 )
//           cout << n_ph << endl;
      }



      if( n_measured < 200 )
      {
        if( measured == true )//n_lost%100 == 0 )
        {
          for( int j = 0; j < index_pos; j++ )
          {
            if( j > 0 )
            {
              TPolyLine3D *l = new TPolyLine3D(2);

              l->SetPoint(0, posX[j-1], posY[j-1], posZ[j-1]);
              l->SetPoint(1, posX[j], posY[j], posZ[j]);
              l->SetLineColor(3);
              l->Draw();
            }

//             cout << "pos.: (" << posX[j]<< "," << posY[j] << "," << posZ[j] << ")" << endl;
          }
        }
      }
    }
  }

  cout << "measured  photons: " << n_measured << endl;
  cout << "absorbed  photons: " << n_absorbed << endl;
  cout << "lost      photons: " << n_lost     << endl;

  setup->Close(); // w/o -> Error in <RootX11ErrorHandler>
}
