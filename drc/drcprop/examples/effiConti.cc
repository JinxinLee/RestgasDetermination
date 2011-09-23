
//======
// ROOT
//======
#include <TString.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

using namespace TMath;


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



void effiConti( TString inFilename = "" )
{
  if( inFilename == "" )
  {
    cout << "Usage: effiConti( filename )" << endl;
    return;
  }



//==============================================================================
// In- & output
//==============================================================================
  TFile *inFile = new TFile( inFilename );

  TTree *infoTree   = (TTree*) inFile->Get("info");
  TTree *photonTree = (TTree*) inFile->Get("photon");


  Int_t root_version = -666;
  char slab_material[64], prism_material[64], airBox_material[64], fishtank_material[64];
  char backLens_material[64], frontLens_material[64];
  Int_t photon_number = -666, particle_number = -666, shoots = -666, refl_limit = -666;
  Double_t lambda_min = -666, lambda_max = -666;
  Double_t frontLens_radius = -666, frontLens_thickness = -666, frontLens_conical = -666;
  Double_t frontLens_width =- 666, frontLens_height = -666;
  Double_t backLens_radius = -666, backLens_thickness = -666, backLens_conical = -666;
  Double_t backLens_width =- 666, backLens_height = -666;
  Int_t frontLens_cylindrical = -666, backLens_cylindrical = -666;
  Double_t airgap = -666;
  Double_t slab_width = -666, slab_height = -666, slab_length = -666;
  Double_t prism_length = -666;
  Double_t prism_heightUp1 = -666, prism_heightUp2 = -666, prism_heightDown1 = -666, prism_heightDown2 = -666;
  Double_t prism_widthUp1 = -666, prism_widthUp2 = -666, prism_widthDown1 = -666, prism_widthDown2 = -666;
  Double_t fishtank_width = -666, fishtank_height = -666, fishtank_length = -666;
  Double_t fishtank_width_offset = -666, fishtank_height_offset = -666;
  Double_t fishtank_thetaX = -666, fishtank_thetaY = -666, fishtank_phi = -666;
  Double_t particle_mass = -666, particle_kinE = -666, particle_mom = -666, particle_beta = -666;
  Double_t parDirX = -666, parDirY = -666, parDirZ = -666;
  Double_t incidence_theta = -666, incidence_phi = -666;
  Double_t hitBarX = -666, hitBarY = -666, hitBarZ = -666;
  Double_t spot_radius = -666, spot_limit = -666;
  Bool_t slab_fresnel, backLens_fresnel, frontLens_fresnel, prism_fresnel, airBox_fresnel, fishtank_fresnel;
  Bool_t fishtankBlack_bottom, fishtankBlack_sides, fishtankBlack_top, mirror;
  Double_t gridXstep = -666, gridYstep = -666;
  Double_t cannon_theta = -666, cannon_phi = -666, z_offset = -666;
  Int_t pos_size_helper;


  infoTree->SetBranchAddress( "root_version"          , &root_version );
  infoTree->SetBranchAddress( "slab_material"         , &slab_material );
  infoTree->SetBranchAddress( "slab_width"            , &slab_width );
  infoTree->SetBranchAddress( "slab_height"           , &slab_height );
  infoTree->SetBranchAddress( "slab_length"           , &slab_length );
  infoTree->SetBranchAddress( "slab_fresnel"          , &slab_fresnel );
  infoTree->SetBranchAddress( "backLens_material"     , &backLens_material );
  infoTree->SetBranchAddress( "backLens_radius"       , &backLens_radius );
  infoTree->SetBranchAddress( "backLens_thickness"    , &backLens_thickness );
  infoTree->SetBranchAddress( "backLens_width"        , &backLens_width );
  infoTree->SetBranchAddress( "backLens_height"       , &backLens_height );
  infoTree->SetBranchAddress( "backLens_conical"      , &backLens_conical );
  infoTree->SetBranchAddress( "backLens_cylindrical"  , &backLens_cylindrical );
  infoTree->SetBranchAddress( "backLens_fresnel"      , &backLens_fresnel );
  infoTree->SetBranchAddress( "frontLens_material"    , &frontLens_material );
  infoTree->SetBranchAddress( "frontLens_radius"      , &frontLens_radius );
  infoTree->SetBranchAddress( "frontLens_thickness"   , &frontLens_thickness );
  infoTree->SetBranchAddress( "frontLens_width"       , &frontLens_width );
  infoTree->SetBranchAddress( "frontLens_height"      , &frontLens_height );
  infoTree->SetBranchAddress( "frontLens_conical"     , &frontLens_conical );
  infoTree->SetBranchAddress( "frontLens_cylindrical"  , &frontLens_cylindrical );
  infoTree->SetBranchAddress( "frontLens_fresnel"     , &frontLens_fresnel );
  infoTree->SetBranchAddress( "prism_material"        , &prism_material );
  infoTree->SetBranchAddress( "prism_fresnel"         , &prism_fresnel );
  infoTree->SetBranchAddress( "prism_length"          , &prism_length );
  infoTree->SetBranchAddress( "prism_heightUp1"       , &prism_heightUp1 );
  infoTree->SetBranchAddress( "prism_heightUp2"       , &prism_heightUp2 );
  infoTree->SetBranchAddress( "prism_heightDown1"     , &prism_heightDown1 );
  infoTree->SetBranchAddress( "prism_heightDown2"     , &prism_heightDown2 );
  infoTree->SetBranchAddress( "prism_widthUp1"        , &prism_widthUp1 );
  infoTree->SetBranchAddress( "prism_widthUp2"        , &prism_widthUp2 );
  infoTree->SetBranchAddress( "prism_widthDown1"      , &prism_widthDown1 );
  infoTree->SetBranchAddress( "prism_widthDown2"      , &prism_widthDown2 );
  infoTree->SetBranchAddress( "airBox_material"       , &airBox_material );
  infoTree->SetBranchAddress( "airgap"                , &airgap );
  infoTree->SetBranchAddress( "airBox_fresnel"        , &airBox_fresnel );
  infoTree->SetBranchAddress( "fishtank_material"     , &fishtank_material );
  infoTree->SetBranchAddress( "fishtank_width"        , &fishtank_width );
  infoTree->SetBranchAddress( "fishtank_height"       , &fishtank_height );
  infoTree->SetBranchAddress( "fishtank_length"       , &fishtank_length );
  infoTree->SetBranchAddress( "fishtank_width_offset" , &fishtank_width_offset );
  infoTree->SetBranchAddress( "fishtank_height_offset", &fishtank_height_offset );
  infoTree->SetBranchAddress( "fishtank_thetaX"       , &fishtank_thetaX );
  infoTree->SetBranchAddress( "fishtank_thetaY"       , &fishtank_thetaY );
  infoTree->SetBranchAddress( "fishtank_phi"          , &fishtank_phi );
  infoTree->SetBranchAddress( "fishtank_fresnel"      , &fishtank_fresnel );
  infoTree->SetBranchAddress( "fishtankBlack_bottom"  , &fishtankBlack_bottom );
  infoTree->SetBranchAddress( "fishtankBlack_sides"   , &fishtankBlack_sides );
  infoTree->SetBranchAddress( "fishtankBlack_top"     , &fishtankBlack_top );
  infoTree->SetBranchAddress( "mirror"                , &mirror );
  infoTree->SetBranchAddress( "photon_number"         , &photon_number );
  infoTree->SetBranchAddress( "lambda_min"            , &lambda_min );
  infoTree->SetBranchAddress( "lambda_max"            , &lambda_max );
  infoTree->SetBranchAddress( "refl_limit"            , &refl_limit );
  infoTree->SetBranchAddress( "particle_mass"         , &particle_mass );
  infoTree->SetBranchAddress( "particle_kinE"         , &particle_kinE );
  infoTree->SetBranchAddress( "particle_mom"          , &particle_mom );
  infoTree->SetBranchAddress( "particle_beta"         , &particle_beta );
  infoTree->SetBranchAddress( "particle_dirX"         , &parDirX );
  infoTree->SetBranchAddress( "particle_dirY"         , &parDirY );
  infoTree->SetBranchAddress( "particle_dirZ"         , &parDirZ );
  infoTree->SetBranchAddress( "incidence_theta"       , &incidence_theta );
  infoTree->SetBranchAddress( "incidence_phi"         , &incidence_phi );
  infoTree->SetBranchAddress( "hitBarX"               , &hitBarX );
  infoTree->SetBranchAddress( "hitBarY"               , &hitBarY );
  infoTree->SetBranchAddress( "hitBarZ"               , &hitBarZ );
  infoTree->SetBranchAddress( "spot_radius"           , &spot_radius );
  infoTree->SetBranchAddress( "spot_limit"            , &spot_limit );
  infoTree->SetBranchAddress( "particle_number"       , &particle_number );
  infoTree->SetBranchAddress( "shoots"                , &shoots );
  infoTree->SetBranchAddress( "gridXstep"             , &gridXstep );
  infoTree->SetBranchAddress( "gridYstep"             , &gridYstep );
  infoTree->SetBranchAddress( "cannon_theta"          , &cannon_theta );
  infoTree->SetBranchAddress( "cannon_phi"            , &cannon_phi );
  infoTree->SetBranchAddress( "z_offset"              , &z_offset );
  infoTree->SetBranchAddress( "pos_size"              , &pos_size_helper );

  infoTree->GetEntry( 0 );


  const int pos_size = pos_size_helper;

  if( pos_size == -666 )
  {
    cout << "No photon position list included!" << endl;
    return;
  }

  if( incidence_theta == -666 )
  {
    cout << "Please use \"opt_default\" in prototype simulation!" << endl;
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
  TString posX_str_2 = posX_str + "/D";
  TString posY_str_2 = posY_str + "/D";
  TString posZ_str_2 = posZ_str + "/D";

  Double_t wavelength;
  Double_t hitPosX, hitPosY, hitPosZ;
  Double_t hitPosDetX, hitPosDetY, hitPosDetZ;
  Double_t hitDirX, hitDirY, hitDirZ;
  Int_t color, particleID;
  Int_t index_pos, nRefl;
  Double_t thetaC, phiC;
  Double_t kBarX, kBarY, kBarZ;
  Bool_t measured, absorbed, lost;
  Double_t time;


  photonTree->SetBranchAddress( "wavelength", &wavelength );
  photonTree->SetBranchAddress( "color"     , &color );
  photonTree->SetBranchAddress( "kBarX"     , &kBarX );
  photonTree->SetBranchAddress( "kBarY"     , &kBarY );
  photonTree->SetBranchAddress( "kBarZ"     , &kBarZ );
  photonTree->SetBranchAddress( "hitPosX"   , &hitPosX );
  photonTree->SetBranchAddress( "hitPosY"   , &hitPosY );
  photonTree->SetBranchAddress( "hitPosZ"   , &hitPosZ );
  photonTree->SetBranchAddress( "hitPosDetX", &hitPosDetX );
  photonTree->SetBranchAddress( "hitPosDetY", &hitPosDetY );
  photonTree->SetBranchAddress( "hitPosDetZ", &hitPosDetZ );
  photonTree->SetBranchAddress( "hitDirX"   , &hitDirX );
  photonTree->SetBranchAddress( "hitDirY"   , &hitDirY );
  photonTree->SetBranchAddress( "hitDirZ"   , &hitDirZ );
  photonTree->SetBranchAddress( posX_str    , posX );
  photonTree->SetBranchAddress( posY_str    , posY );
  photonTree->SetBranchAddress( posZ_str    , posZ );
  photonTree->SetBranchAddress( "index_pos" , &index_pos );
  photonTree->SetBranchAddress( "particleID", &particleID );
  photonTree->SetBranchAddress( "thetaC"    , &thetaC );
  photonTree->SetBranchAddress( "phiC"      , &phiC );
  photonTree->SetBranchAddress( "measured"  , &measured );
  photonTree->SetBranchAddress( "absorbed"  , &absorbed );
  photonTree->SetBranchAddress( "lost"      , &lost );
  photonTree->SetBranchAddress( "time"      , &time );
  photonTree->SetBranchAddress( "nRefl"     , &nRefl );

  Int_t nEntries = photonTree->GetEntries();



  if( nEntries > 20000 )
  {
    cout << "*******************************************" << endl;
    cout << "It's recommended to compile this macro.    " << endl;
    cout << "Compile procedure:                         " << endl;
    cout << "root[] gSystem->Load( \"lib_DetEffi.so\" ) " << endl;
    cout << "root[] .L effiConti.cc++                   " << endl;
    cout << "*******************************************" << endl;
  }


  TString outFilename = inFilename;
  outFilename.Resize( inFilename.Length() - 5 ); // remove file extension .root
  outFilename = outFilename + "_effiConti.root";

  TFile *outFile = new TFile( outFilename, "RECREATE" );

  TTree *outTree = infoTree->CloneTree(0);
  outTree->SetName( "tree" );

  outTree->Branch( "wavelength", &wavelength, "wavelength/D" );
  outTree->Branch( "color"     , &color     , "color/I" );
  outTree->Branch( "kBarX"     , &kBarX     , "kBarX/D" );
  outTree->Branch( "kBarY"     , &kBarY     , "kBarY/D" );
  outTree->Branch( "kBarZ"     , &kBarZ     , "kBarZ/D" );
  outTree->Branch( "hitPosX"   , &hitPosX   , "hitPosX/D" );
  outTree->Branch( "hitPosY"   , &hitPosY   , "hitPosY/D" );
  outTree->Branch( "hitPosZ"   , &hitPosZ   , "hitPosZ/D" );
  outTree->Branch( "hitPosDetX", &hitPosDetX, "hitPosDetX/D" );
  outTree->Branch( "hitPosDetY", &hitPosDetY, "hitPosDetY/D" );
  outTree->Branch( "hitPosDetZ", &hitPosDetZ, "hitPosDetZ/D" );
  outTree->Branch( "hitDirX"   , &hitDirX   , "hitDirX/D" );
  outTree->Branch( "hitDirY"   , &hitDirY   , "hitDirY/D" );
  outTree->Branch( "hitDirZ"   , &hitDirZ   , "hitDirZ/D" );
  outTree->Branch( posX_str    , posX       , posX_str_2 );
  outTree->Branch( posY_str    , posY       , posY_str_2 );
  outTree->Branch( posZ_str    , posZ       , posZ_str_2 );
  outTree->Branch( "index_pos" , &index_pos , "index_pos/I" );
  outTree->Branch( "particleID", &particleID, "particleID/I" );
  outTree->Branch( "thetaC"    , &thetaC    , "thetaC/D" );
  outTree->Branch( "phiC"      , &phiC      , "phiC/D" );
  outTree->Branch( "measured"  , &measured  , "measured/O" );
  outTree->Branch( "absorbed"  , &absorbed  , "absorbed/O" );
  outTree->Branch( "lost"      , &lost      , "lost/O" );
  outTree->Branch( "time"      , &time      , "time/D" );
  outTree->Branch( "nRefl"     , &nRefl     , "nRefl/I" );


  bool b_effi = false;

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


  outTree->Branch("b_effi"       , &b_effi       , "b_effi/O");
  outTree->Branch("b_lost_refl"  , &b_lost_refl  , "b_lost_refl/O");
  outTree->Branch("b_lost_bar"   , &b_lost_bar   , "b_lost_bar/O");
  outTree->Branch("b_lost_gap"   , &b_lost_gap   , "b_lost_gap/O");
  outTree->Branch("b_lost_lens"  , &b_lost_lens  , "b_lost_lens/O");
  outTree->Branch("b_lost_airBox", &b_lost_airBox, "b_lost_airBox/O");
  outTree->Branch("b_lost_tank"  , &b_lost_tank  , "b_lost_tank/O");
  outTree->Branch("b_abs_limit"  , &b_abs_limit  , "b_abs_limit/O");
  outTree->Branch("b_abs_bar"    , &b_abs_bar    , "b_abs_bar/O");
  outTree->Branch("b_abs_lens"   , &b_abs_lens   , "b_abs_lens/O");
  outTree->Branch("b_abs_tank"   , &b_abs_tank   , "b_abs_tank/O");
  outTree->Branch("b_abs_sides"  , &b_abs_sides  , "b_abs_sides/O");
  outTree->Branch("b_back"       , &b_back       , "b_back/O");
  outTree->Branch("b_back_lens"  , &b_back_lens  , "b_back_lens/O");
  outTree->Branch("b_back_tank"  , &b_back_tank  , "b_back_tank/O");



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


  DetEffi *efficiency = new DetEffi(); // detector efficiency


  for( int i = 0; i < nEntries; i++ )
  {
    photonTree->GetEntry( i );

    if( i%10000 == 0 )
      cout << "Photon: " << i+1 << " / " << nEntries << endl;


    n_ph++;

    if( measured )
      n_measured++;

    if( absorbed )
      n_absorbed++;

    if( lost )
      n_lost++;


    b_effi = false;

    b_lost_refl   = false;
    b_lost_bar    = false;
    b_lost_gap    = false;
    b_lost_lens   = false;
    b_lost_airBox = false;
    b_lost_tank   = false;

    b_abs_limit = false;
    b_abs_bar   = false;
    b_abs_lens  = false;
    b_abs_tank  = false;
    b_abs_sides = false;

    b_back      = false;
    b_back_lens = false;
    b_back_tank = false;


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
               && Abs(hitPosX) <= (frontLens_width/2)+tiny && Abs(hitPosY) <= (frontLens_height/2)+tiny )
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


    if( efficiency->DetEffi::EffiReal( (int) wavelength ) )
      b_effi = true;


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


    if( b_lost_refl )
      n_lost_refl++;

    if( !b_lost_refl && b_lost_bar && !b_back )
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
      n_abs_tank++;

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



//     if( lost && hitPosZ < 0 && !b_back && nRefl>0 ) // debug
//     {
//       cout << "test: " << n_ph << endl;
//       for( int j = 0; j < index_pos; j++ )
//       {
//         if( j != index_pos -1 )
//           cout << posX[j] << " " << posY[j] << " " << posZ[j] << "    "
//               << posX[j+1] << " " << posY[j+1] << " " << posZ[j+1] << " " << nRefl << endl;
//       }
//     }


    outTree->Fill();
  }



//==============================================================================
// Summary
//==============================================================================
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

//   cout << n_outbar << " " << n_tank << endl;

  cout << "total refl:      " << n_totalrefl / n_ph      << endl;
  cout << "(good) abs bar:  " << n_abs_bar / n_totalrefl << endl;
  cout << "out of bar:      " << n_outbar / n_totalrefl << endl;
  cout << "bar to tank:     " << n_tank / n_outbar       << endl;
  cout << "abs tank sides:  " << n_tanknosides / n_tank  << endl;
  cout << "abs tank:        " << (n_rb_abs_tank + n_abs_tank) / n_tanknosides << endl;
  cout << "rb measured:     " << (double) n_rb_measured / n_measured << endl;
  cout << endl;



//==============================================================================
// Save data
//==============================================================================
  outTree->Write();
  outFile->Close();

  cout << "Root-file " << outFilename << " was written" << endl;

  TFile::Open( outFilename );
}
