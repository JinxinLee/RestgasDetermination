
//======
// ROOT
//======
#include <TCanvas.h>
#include <TFile.h>
#include <TPolyLine.h>
#include <TString.h>
#include <TTree.h>
#include <TMath.h>
using namespace TMath;

//=========
// STD C++
//=========
#include <iostream>

using namespace std;



void effiConti( TString inFilename = "", Double_t lambdaCut = 0 )
{

  if( inFilename == "" )
  {
    cout << "Usage: photTrajectory( filename, photonID )" << endl;
    return;
  }


  //==============================================================================
  // Access to the input ROOT-file & canvas settings
  //==============================================================================
  TFile *inFile = new TFile( inFilename );

  TTree *infoTree   = (TTree*) inFile->Get("info");
  TTree *photonTree = (TTree*) inFile->Get("photon");


  const int pos_size;
  Double_t frontLens_thickness = -666, frontLens_diameter = -666, airgap = -666;
  Double_t slab_width= -666, slab_height = -666, fishtank_width = -666, fishtank_height = -666;
  Int_t refl_limit = -666;

  infoTree->SetBranchAddress( "pos_size"            , &pos_size );
  infoTree->SetBranchAddress( "frontLens_thickness" , &frontLens_thickness );
  infoTree->SetBranchAddress( "frontLens_diameter"  , &frontLens_diameter );
  infoTree->SetBranchAddress( "airgap"              , &airgap );
  infoTree->SetBranchAddress( "slab_width"          , &slab_width );
  infoTree->SetBranchAddress( "slab_height"         , &slab_height );
  infoTree->SetBranchAddress( "fishtank_width"      , &fishtank_width );
  infoTree->SetBranchAddress( "fishtank_height"     , &fishtank_height );
  infoTree->SetBranchAddress( "refl_limit"          , &refl_limit );
  infoTree->GetEntry( 0 );

  if( pos_size == -666 )
  {
    cout << "No photon position list included!" << endl;
    return;
  }


  bool b_frontLens = true;
  bool b_airgap    = true;

  if( frontLens_thickness == -666 )
    b_frontLens = false;

  if( airgap == -666 )
    b_airgap = false;


  Double_t posX[ pos_size ], posY[ pos_size ], posZ[ pos_size ];

  TString pos_size_str;
  pos_size_str += pos_size;
  pos_size_str.Remove( TString::kLeading, ' ' );
  TString posX_str = "posX[" + pos_size_str + "]";
  TString posY_str = "posY[" + pos_size_str + "]";
  TString posZ_str = "posZ[" + pos_size_str + "]";

  Int_t index_pos, nRefl;
  Bool_t measured, absorbed, lost;
  Double_t hitPosX, hitPosY, hitPosZ, wavelength;

  photonTree->SetBranchAddress( posX_str   , posX );
  photonTree->SetBranchAddress( posY_str   , posY );
  photonTree->SetBranchAddress( posZ_str   , posZ );
  photonTree->SetBranchAddress( "index_pos" , &index_pos );
  photonTree->SetBranchAddress( "nRefl"     , &nRefl );
  photonTree->SetBranchAddress( "measured"  , &measured );
  photonTree->SetBranchAddress( "absorbed"  , &absorbed );
  photonTree->SetBranchAddress( "lost"      , &lost );
  photonTree->SetBranchAddress( "hitPosX"   , &hitPosX );
  photonTree->SetBranchAddress( "hitPosY"   , &hitPosY );
  photonTree->SetBranchAddress( "hitPosZ"   , &hitPosZ );
  photonTree->SetBranchAddress( "wavelength", &wavelength );

  Int_t nEntries = photonTree->GetEntries();


//   TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
//   canvas->Draw();
//   setup->DrawClonePad();
//   setup->Close(); // w/o -> Error in <RootX11ErrorHandler>


  //==============================================================================
  // Event loop
  //==============================================================================

  int n_ph       = 0;
  int n_measured = 0;
  int n_absorbed = 0;
  int n_lost     = 0;

  int n_lost_refl      = 0;
  int n_lost_bar       = 0;
  int n_lost_gap       = 0;
  int n_lost_lens_back = 0;
  int n_lost_lens_curv = 0;
  int n_lost_airBox    = 0;
  int n_lost_tank      = 0;

  int n_abs_limit = 0;
  int n_abs_bar   = 0;
  int n_abs_lens  = 0;
  int n_abs_tank  = 0;
  int n_abs_sides = 0;

  int n_rb_measured  = 0;
  int n_rb_absorbed  = 0;
  int n_rb_lost      = 0;

  int n_rb_abs_limit = 0;
  int n_rb_abs_bar   = 0;
  int n_rb_abs_lens  = 0;
  int n_rb_abs_tank  = 0;
  int n_rb_abs_sides = 0;

  int n_rb_lost_bar       = 0;
  int n_rb_lost_lens_back = 0;
  int n_rb_lost_lens_curv = 0;
  int n_rb_lost_airBox    = 0;
  int n_rb_lost_tank      = 0;


  for( int i = 0; i < nEntries; i++ )
  {
    photonTree->GetEntry( i );

    // lambda cut
    if( wavelength < lambdaCut )
      continue;

    n_ph++;

    if( measured )
      n_measured++;

    if( absorbed )
      n_absorbed++;

    if( lost )
      n_lost++;


    bool b_lost_refl   = false;
    bool b_lost_bar    = false;
    bool b_lost_gap    = false;
    bool b_lost_lens   = false;
    bool b_lost_airBox = false;
    bool b_lost_tank   = false;

    bool b_abs_limit = false;
    bool b_abs_bar   = false;
    bool b_abs_lens  = false;
    bool b_abs_tank  = false;
    bool b_abs_sides = false;

    bool b_back      = false;
    bool b_back_lens = false;
    bool b_back_tank = false;

    const int last = index_pos-1;

    const double tiny = 0.00001; // unclear when a tiny shift is used


    for( int j = 0; j < index_pos; j++ )
    {
      if( b_frontLens && posZ[j] > 0 && posZ[j] <= frontLens_thickness && posZ[j+1] < posZ[j] && j != last)
        b_back_lens = true;

      if( b_airgap && posZ[j] == airgap  && posZ[j+1] < airgap && j != last )
        b_back_tank = true;
    }


    if( hitPosZ != posZ[index_pos-1] )
      cout << "Something is wrong!" << endl;


    if( lost )
    {
      if( (Abs(posX[1]) == slab_width/2 || Abs(posY[1]) == slab_height/2) && nRefl == 0 && posZ[1] < 0 )
        b_lost_refl = true;

      if( hitPosZ < 0 && Abs(hitPosX) < (slab_width/2)+tiny && Abs(hitPosY) < (slab_height/2)+tiny )
        b_lost_bar = true;
      else if( hitPosZ < frontLens_thickness && b_frontLens
               && Abs(hitPosX) <= (frontLens_diameter/2)+tiny && Abs(hitPosY) <= (frontLens_diameter/2)+tiny )
        b_lost_lens = true;
      else if( hitPosZ < airgap && b_airgap )
        b_lost_airBox = true;
      else
        b_lost_tank = true;
    }


    if( absorbed )
    {

      if( hitPosZ < 0 || (hitPosZ == 0 && Abs(hitPosX) <= slab_width/2 && Abs(hitPosY) <= slab_height/2) )
        b_abs_bar = true;
      else if( b_frontLens && hitPosZ <= frontLens_thickness )
        b_abs_lens = true;
      else if( !( Abs(hitPosX) == fishtank_width/2 || Abs(hitPosY) == fishtank_height/2 ) )
        b_abs_tank = true;
      else
        b_abs_sides = true;
    }


    if( absorbed && nRefl == (refl_limit + 1)  )
      b_abs_limit = true;

    if( b_back_lens || b_back_tank )
      b_back = true;

    if( b_lost_lens || b_lost_airBox )
      b_lost_gap = true;


    if( measured && b_back )
      n_rb_measured++;

    if( absorbed && b_back )
      n_rb_absorbed++;

    if( lost && b_back )
    {
      n_rb_lost++;

      if( b_lost_bar )
        n_rb_lost_bar++;
      else if( b_lost_lens )
      {
        if( Abs(hitPosZ) < tiny )
          n_rb_lost_lens_back++;
        else
          n_rb_lost_lens_curv++;
      }
      else if( b_lost_airBox )
        n_rb_lost_airBox++;
      else
        n_rb_lost_tank++;
    }


    if( b_lost_refl && !b_back )
      n_lost_refl++;

    if( b_lost_bar && !b_lost_refl && !b_back )
      n_lost_bar++;

    if( b_lost_gap && !b_back )
    {
      n_lost_gap++;

      if( b_lost_lens )
      {
        if( hitPosZ < tiny )
          n_lost_lens_back++;
        else
          n_lost_lens_curv++;
      }
      else
        n_lost_airBox++;
    }

    if( b_lost_tank && !b_back )
      n_lost_tank++;


    if( b_abs_limit )
    {
      if( !b_back )
        n_abs_limit++;
      else
        n_rb_abs_limit++;
    }

    if( b_abs_bar && !b_abs_limit && !b_back )
      n_abs_bar++;

    if( b_abs_lens && !b_abs_limit && !b_back )
      n_abs_lens++;

    if( b_abs_tank && !b_abs_limit && !b_back )
    {
      n_abs_tank++;
            cout << n_ph << " " << hitPosZ << " " << posZ[index_pos-2] << endl;
    }

    if( b_abs_sides && !b_abs_limit && !b_back )
      n_abs_sides++;


    if( b_abs_bar && !b_abs_limit && b_back )
      n_rb_abs_bar++;

    if( b_abs_lens && !b_abs_limit && b_back )
      n_rb_abs_lens++;

    if( b_abs_tank && !b_abs_limit && b_back )
      n_rb_abs_tank++;

    if( b_abs_sides && !b_abs_limit && b_back )
      n_rb_abs_sides++;



//     if( lost && hitPosZ < 0 && !b_back && nRefl>0 )// debug
//     {
//       cout << "test: " << n_ph << endl;
//       for( int j = 0; j < index_pos; j++ )
//       {
//         if( j != index_pos -1 )
//           cout << posX[j] << " " << posY[j] << " " << posZ[j] << "    "
//               << posX[j+1] << " " << posY[j+1] << " " << posZ[j+1] << " " << nRefl << endl;
//       }
//     }

  }


  cout << endl;
  cout << "generated photons: " << n_ph       << endl;
  cout << "measured  photons: " << n_measured << endl;
  cout << "  rb_measured:       " << n_rb_measured << endl;
  cout << "lost photons:      " << n_lost     << endl;
  cout << "  rb_lost:           " << n_rb_lost << endl;
  cout << "    rb_lost_bar:       " << n_rb_lost_bar << endl;
  cout << "    rb_lost_lens_back: " << n_rb_lost_lens_back << endl;
  cout << "    rb_lost_lens_curv: " << n_rb_lost_lens_curv << endl;
  cout << "    rb_lost_airBox:    " << n_rb_lost_airBox << endl;
  cout << "    rb_lost_tank:      " << n_rb_lost_tank << endl;
  cout << "  lost_refl:         " << n_lost_refl << endl;
  cout << "  lost_bar:          " << n_lost_bar  << endl;
  cout << "  lost_gap:          " << n_lost_gap << endl;
  cout << "    lost_lens_back:    " << n_lost_lens_back << endl;
  cout << "    lost_lens_curv:    " << n_lost_lens_curv << endl;
  cout << "    lost_airBox:       " << n_lost_airBox << endl;
  cout << "  lost_tank:         " << n_lost_tank << endl;
  cout << "absorbed photons:  " << n_absorbed << endl;
  cout << "  rb_absorbed:       " << n_rb_absorbed << endl;
  cout << "    rb_abs_limit:      " << n_rb_abs_limit << endl;
  cout << "    rb_abs_bar:        " << n_rb_abs_bar << endl;
  cout << "    rb_abs_lens:       " << n_rb_abs_lens << endl;
  cout << "    rb_abs_tank:       " << n_rb_abs_tank << endl;
  cout << "    rb_abs_sides:      " << n_rb_abs_sides << endl;
  cout << "  abs_limit:         " << n_abs_limit << endl;
  cout << "  abs_bar:           " << n_abs_bar   << endl;
  cout << "  abs_lens:          " << n_abs_lens  << endl;
  cout << "  abs_tank:          " << n_abs_tank  << endl;
  cout << "  abs_sides:         " << n_abs_sides << endl;
  cout << endl;


  double n_totalrefl   = n_ph - n_lost_refl - n_lost_bar;
  double n_outbar      = n_totalrefl - n_abs_bar - n_abs_limit;
  double n_tank        = n_outbar - n_abs_lens - n_lost_gap - n_rb_lost + n_rb_lost_tank - n_rb_absorbed + n_rb_abs_tank + n_rb_abs_sides;
  double n_tanknosides = n_tank -  n_abs_sides - n_rb_abs_sides;
  cout << n_outbar << " " << n_tank << endl;

  cout << "total refl (no edge):  " << n_totalrefl / n_ph      << endl;
  cout << "(good) abs bar:        " << n_abs_bar / n_totalrefl << endl;
  cout << "out of bar:            " << n_outbar / n_totalrefl << endl;
  cout << "bar to tank:           " << n_tank / n_outbar       << endl;
  cout << "abs tank sides:        " << n_tanknosides / n_tank  << endl;
  cout << "abs tank:              " << (n_rb_abs_tank + n_abs_tank) / n_tanknosides << endl;
  cout << "rb measured:           " << (double) n_rb_measured / n_measured << endl;
  cout << endl;
}
