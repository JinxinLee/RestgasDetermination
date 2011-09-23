
//======
// ROOT
//======
#include <TString.h>
#include <TFile.h>
#include <TTree.h>


//=========
// STD C++
//=========
#include <iostream>

using namespace std;



void parameter( TString inFilename = ""  )
{

  if( inFilename == "" )
  {
    cout << "Usage: parameter( filename )" << endl;
    return;
  }



//==============================================================================
// Access to the input ROOT-file
//==============================================================================
  TFile *inFile   = new TFile( inFilename );
  TTree *infoTree = (TTree*) inFile->Get("info");


  Int_t root_version = -666;
  char slab_material[64] = "-666", prism_material[64] = "-666", airBox_material[64] = "-666", fishtank_material[64] = "-666";
  char backLens_material[64] = "-666", frontLens_material[64]= "-666";
  Int_t photon_number = -666, particle_number = -666, shoots = -666, refl_limit = -666;
  Double_t lambda_min = -666, lambda_max = -666;
  Double_t frontLens_radius = -666, frontLens_thickness = -666, frontLens_conical = -666;
  Double_t frontLens_width =- 666, frontLens_height = -666;
  Double_t backLens_radius = -666, backLens_thickness = -666, backLens_conical = -666;
  Double_t backLens_width =- 666, backLens_height = -666;
  Int_t frontLens_cylindrical = -666, backLens_cylindrical = -666;
  Double_t airgap = -666;
  Double_t slab_width = -666, slab_height = -666, slab_length = -666;
  Double_t prism_length = -666, prism_height = -666, prism_width = -666;
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
  Double_t cannon_theta = -666, cannon_phi = -666, canno_rotX = -666, canno_rotY = -666;
  Double_t cannon_posX = -666, cannon_posY = -666, cannon_posZ = -666;


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
  infoTree->SetBranchAddress( "frontLens_cylindrical" , &frontLens_cylindrical );
  infoTree->SetBranchAddress( "frontLens_fresnel"     , &frontLens_fresnel );
  infoTree->SetBranchAddress( "prism_material"        , &prism_material );
  infoTree->SetBranchAddress( "prism_fresnel"         , &prism_fresnel );
  infoTree->SetBranchAddress( "prism_length"          , &prism_length );
  infoTree->SetBranchAddress( "prism_height"          , &prism_height );
  infoTree->SetBranchAddress( "prism_heightUp1"       , &prism_heightUp1 );
  infoTree->SetBranchAddress( "prism_heightUp2"       , &prism_heightUp2 );
  infoTree->SetBranchAddress( "prism_heightDown1"     , &prism_heightDown1 );
  infoTree->SetBranchAddress( "prism_heightDown2"     , &prism_heightDown2 );
  infoTree->SetBranchAddress( "prism_width"           , &prism_width );
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
  infoTree->SetBranchAddress( "cannon_rotX"           , &cannon_rotX );
  infoTree->SetBranchAddress( "cannon_rotY"           , &cannon_rotY );
  infoTree->SetBranchAddress( "cannon_posX"           , &cannon_posX );
  infoTree->SetBranchAddress( "cannon_posY"           , &cannon_posY );
  infoTree->SetBranchAddress( "cannon_posZ"           , &cannon_posZ );

  infoTree->GetEntry( 0 );


//==============================================================================
// Print parameters
//==============================================================================
  Bool_t opt_frontLens = true;
  Bool_t opt_backLens  = true;
  Bool_t opt_prism     = true;
  Bool_t opt_photonCannon = false;

  if( frontLens_radius == -666 )
    opt_frontLens = false;
  if( backLens_radius == -666 )
    opt_backLens = false;
  if( prism_length == -666 )
    opt_prism = false;
  if( photon_number == -666 )
    opt_photonCannon = true;


  if( opt_photonCannon )
    cout << "*** Photon Cannon ***" << endl;
  else
    cout << "*** Beamtest simulation ***" << endl;


  cout << "  material:" << endl;

  cout <<   "    slab:            " << slab_material << endl;
  if( opt_backLens )
    cout << "    lens (backward): " << backLens_material << endl;
  if( opt_frontLens )
    cout << "    lens (forward):  " << frontLens_material << endl;
  if( opt_prism )
    cout << "    prism:           " << prism_material << endl;
  if( airgap > 0 )
    cout << "    airBox:          " << airBox_material << endl;
  cout <<   "    fishtank:        " << fishtank_material << endl;


  cout << "  volume options:" << endl;

  cout <<     "    fishtank bottom blackened: " << fishtankBlack_bottom << endl;
  cout <<     "    fishtank top blackened:    " << fishtankBlack_top << endl;
  cout <<     "    fishtank sides blackened:  " << fishtankBlack_sides << endl;
  cout <<     "    mirror: " << mirror << endl;


  cout << "  fresnel:" << endl;

  cout <<   "    slab:            " << slab_fresnel << endl;
  if( opt_backLens )
    cout << "    lens (backward): " << backLens_fresnel << endl;
  if( opt_frontLens )
    cout << "    lens (forward):  " << frontLens_fresnel << endl;
  if( opt_prism )
    cout << "    prism:           " << prism_fresnel << endl;
  if( airgap > 0 )
    cout << "    airBox:          " << airBox_fresnel << endl;
  cout <<   "    fishtank:        " << fishtank_fresnel << endl;


  cout << "  dimensions [mm]:" << endl;

  cout << "    slab width:  " << slab_width << endl;
  cout << "    slab heigth: " << slab_height << endl;
  cout << "    slab length: " << slab_length << endl;

  if( opt_backLens )
  {
    cout << "    lens (backward) radius:         " << backLens_radius << endl;
    cout << "    lens (backward) thickness:      " << backLens_thickness << endl;
    cout << "    lens (backward) width:          " << backLens_width << endl;
    cout << "    lens (backward) height:         " << backLens_height << endl;
    cout << "    lens (backward) conical const.: " << backLens_conical << " [dim-less]" << endl;

    if( backLens_cylindrical == 1 )
      cout << "  lens (backward) cylindrical:    in x" << endl;
    if( backLens_cylindrical == 2 )
      cout << "  lens (backward) cylindrical:    in y" << endl;
  }

  if( opt_frontLens )
  {
    cout << "    lens (forward) radius:         " << frontLens_radius << endl;
    cout << "    lens (forward) thickness:      " << frontLens_thickness << endl;
    cout << "    lens (frontward) width:        " << frontLens_width << endl;
    cout << "    lens (frontward) height:       " << frontLens_height << endl;
    cout << "    lens (forward) conical const.: " << frontLens_conical << " [dim-less]" << endl;

    if( frontLens_cylindrical == 1 )
      cout << "  lens (forward) cylindrical:    in x" << endl;
    if( frontLens_cylindrical == 2 )
      cout << "  lens (forward) cylindrical:    in y" << endl;
  }

  if( opt_prism )
  {
    cout << "    prism length:            " << prism_length      << endl;
    cout << "    prism height:            " << prism_height      << endl;
    cout << "    prism height (top 1):    " << prism_heightUp1   << endl;
    cout << "    prism height (top 2):    " << prism_heightUp2   << endl;
    cout << "    prism height (bottom 1): " << prism_heightDown1 << endl;
    cout << "    prism height (bottom 2): " << prism_heightDown2 << endl;
    cout << "    prism width:             " << prism_width       << endl;
    cout << "    prism width (top 1):     " << prism_widthUp1    << endl;
    cout << "    prism width (top 2):     " << prism_widthUp2    << endl;
    cout << "    prism width (bottom 1):  " << prism_widthDown1  << endl;
    cout << "    prism width (bottom 2):  " << prism_widthDown2  << endl;
  }

  cout << "    air gap: " << airgap << endl;

  cout << "    fishtank width:  " << fishtank_width << endl;
  cout << "    fishtank heigth: " << fishtank_height << endl;
  cout << "    fishtank length: " << fishtank_length << endl;

  cout << "    fishtank width offset : " << fishtank_width_offset << endl;
  cout << "    fishtank height offset: " << fishtank_height_offset << endl;

  cout << "    fishtank thetaX: " << fishtank_thetaX << " deg" << endl;
  cout << "    fishtank thetaY: " << fishtank_thetaY << " deg" << endl;
  cout << "    fishtank phi:    " << fishtank_phi << " deg" << endl;


  if( !opt_photonCannon )
  {
    cout << "  particle properties:" << endl;
    cout << "    particle mass [GeV]: " << particle_mass << endl;
    cout << "    T [GeV]:             " << particle_kinE << endl;
    cout << "    p [GeV]:             " << particle_mom  << endl;
    cout << "    beta:                " << particle_beta << endl;

    cout << "    incidence angle (theta): " << incidence_theta << " deg" << endl;
    cout << "    incidence angle (phi):   " << incidence_phi << " deg" << endl;
    cout << "    flight direction:        (" << parDirX << ", " << parDirY << ", " << parDirZ << ")" << endl;
    cout << "    centered hit pos on bar: (" << hitBarX << ", " << hitBarY << ", " << hitBarZ << ")" << endl;

    cout << "    beam spot radius: " << spot_radius << " mm" << endl;
    cout << "    radius limit:     " << spot_limit << " mm" << endl;
    cout << "    particle number:  " << particle_number << endl;


    cout << "  photon properties:" << endl;

    if( photon_number == 0 )
      cout << "    photon number:      realistic" << endl;
    else
      cout << "    photon number:      " << photon_number << endl;

    cout <<     "    Cherenkov spectrum: [" << lambda_min <<", " << lambda_max << "] nm" << endl;
    cout <<     "    reflection limit:   " << refl_limit << endl;
  }
  else
  {
    cout << "  photon properties:" << endl;
    if( gridXstep == 0 || gridYstep == 0 )
      cout << "    photon number: " << shoots << endl;
    else
    {
      cout << "    photon number: " << shoots << " per mesh" << endl;
      cout << "    grid const. X: " << gridXstep << " mm" << endl;
      cout << "    grid const. Y: " << gridYstep << " mm" << endl;
    }

    if( cannon_theta == 180 )
      cout << "    theta: flat cos(theta)" << endl;
    else
      cout << "    theta:      " << cannon_theta << " deg" << endl;

    if( cannon_phi == 360 )
      cout << "    phi:   flat phi" << endl;
    else
      cout << "    phi:        " << cannon_phi   << " deg" << endl;

    cout <<   "    rotation Y: " << cannon_rotY  << " deg" << endl;
    cout <<   "    rotation X: " << cannon_rotX  << " deg" << endl;
    cout <<   "    pos X: " << cannon_posX << " mm" << endl;
    cout <<   "    pos Y: " << cannon_posY << " mm" << endl;
    cout <<   "    pos Z: " << cannon_posZ << " mm" << endl;
    cout <<   "    reflection limit: " << refl_limit << endl;
    cout <<   "    lambda: [" << lambda_min << ", " << lambda_max << "] nm" << endl;
  }
}
