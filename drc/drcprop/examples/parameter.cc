
//======
// ROOT
//======
#include <TFile.h>
#include <TString.h>
#include <TStyle.h>
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


  Int_t root_version;
  char slab_material[64], prism_material[64], airBox_material[64], fishtank_material[64];
  char backLens_material[64], frontLens_material[64];
  char lens_material[64]; // obsolete
  Int_t photon_number, particle_number, shoots;
  Int_t refl_limit;
  Double_t lambda_min, lambda_max;
  Double_t frontLens_radius = -666;
  Double_t frontLens_thickness, frontLens_diameter, frontLens_conical;
  Double_t backLens_radius = -666;
  Double_t backLens_thickness, backLens_diameter, backLens_conical;
  Double_t lens_radius = -666;
  Double_t lens_thickness, lens_diameter, lens_conical; // obsolete
  Double_t airgap;
  Double_t slab_width, slab_height, slab_length;
  Double_t prism_length = -666;
  Double_t prism_heightUp1, prism_heightUp2, prism_heightDown1, prism_heightDown2;
  Double_t prism_widthUp1, prism_widthUp2, prism_widthDown1, prism_widthDown2;
  Double_t fishtank_width, fishtank_height, fishtank_length;
  Double_t fishtank_width_offset, fishtank_height_offset;
  Double_t fishtank_thetaX, fishtank_thetaY, fishtank_phi;
  Double_t particle_mass, particle_kinE, particle_beta;
  Double_t parDirX, parDirY, parDirZ;
  Double_t incidence_theta, incidence_phi;
  Double_t hitBarX, hitBarY, hitBarZ;
  Double_t spot_radius, spot_limit;
  Bool_t slab_fresnel, backLens_fresnel, frontLens_fresnel, prism_fresnel, airBox_fresnel, fishtank_fresnel;
  Bool_t lens_fresnel; // obsolete
  Bool_t fishtankBlack_bottom, fishtankBlack_sides, fishtankBlack_top, mirror;
  Double_t gridXstep, gridYstep;
  Double_t cannon_theta, cannon_phi;


  infoTree->SetBranchAddress( "root_version"          , &root_version );
  infoTree->SetBranchAddress( "slab_material"         , &slab_material );
  infoTree->SetBranchAddress( "slab_width"            , &slab_width );
  infoTree->SetBranchAddress( "slab_height"           , &slab_height );
  infoTree->SetBranchAddress( "slab_length"           , &slab_length );
  infoTree->SetBranchAddress( "slab_fresnel"          , &slab_fresnel );
  infoTree->SetBranchAddress( "backLens_material"     , &backLens_material );
  infoTree->SetBranchAddress( "backLens_radius"       , &backLens_radius );
  infoTree->SetBranchAddress( "backLens_thickness"    , &backLens_thickness );
  infoTree->SetBranchAddress( "backLens_diameter"     , &backLens_diameter );
  infoTree->SetBranchAddress( "backLens_conical"      , &backLens_conical );
  infoTree->SetBranchAddress( "backLens_fresnel"      , &backLens_fresnel );
  infoTree->SetBranchAddress( "frontLens_material"     , &frontLens_material );
  infoTree->SetBranchAddress( "frontLens_radius"       , &frontLens_radius );
  infoTree->SetBranchAddress( "frontLens_thickness"    , &frontLens_thickness );
  infoTree->SetBranchAddress( "frontLens_diameter"     , &frontLens_diameter );
  infoTree->SetBranchAddress( "frontLens_conical"      , &frontLens_conical );
  infoTree->SetBranchAddress( "frontLens_fresnel"      , &frontLens_fresnel );
  infoTree->SetBranchAddress( "lens_material"         , &lens_material ); // obsolete
  infoTree->SetBranchAddress( "lens_radius"           , &lens_radius ); // obsolete
  infoTree->SetBranchAddress( "lens_thickness"        , &lens_thickness ); // obsolete
  infoTree->SetBranchAddress( "lens_diameter"         , &lens_diameter ); // obsolete
  infoTree->SetBranchAddress( "lens_conical"          , &lens_conical ); // obsolete
  infoTree->SetBranchAddress( "lens_fresnel"          , &lens_fresnel ); // obsolete
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

  infoTree->GetEntry( 0 );


//==============================================================================
// Print parameters
//==============================================================================
  Bool_t opt_woLens    = false; // obsolete
  Bool_t opt_frontLens = true;
  Bool_t opt_backLens  = true;
  Bool_t opt_prism     = true;
  Bool_t opt_photonCannon = false;

  if( lens_radius == -666 )
    opt_woLens = true;
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

  cout <<   "    slab:     " << slab_material << endl;
  if( !opt_woLens )
    cout << "    lens:     " << lens_material << endl; // obsolete
  if( opt_backLens )
    cout << "    lens (backward): " << backLens_material << endl;
  if( opt_frontLens )
    cout << "    lens (forward):  " << frontLens_material << endl;
  if( opt_prism )
    cout << "    prism:    " << prism_material << endl;
  if( airgap > 0 )
    cout << "    airBox:   " << airBox_material << endl;
  cout <<   "    fishtank: " << fishtank_material << endl;


  cout << "  volume options:" << endl;

  cout <<     "    fishtank bottom blackened: " << fishtankBlack_bottom << endl;
  cout <<     "    fishtank top blackened:    " << fishtankBlack_top << endl;
  cout <<     "    fishtank sides blackened:  " << fishtankBlack_sides << endl;
  cout <<     "    mirror: " << mirror << endl;


  cout << "  fresnel:" << endl;

  cout <<   "    slab:     " << slab_fresnel << endl;
  if( !opt_woLens )
    cout << "    lens:     " << lens_fresnel << endl; // obsolete
  if( opt_backLens )
    cout << "    lens (backward): " << backLens_fresnel << endl;
  if( opt_frontLens )
    cout << "    lens (forward):  " << frontLens_fresnel << endl;
  if( opt_prism )
    cout << "    prism:    " << prism_fresnel << endl;
  if( airgap > 0 )
    cout << "    airBox:   " << airBox_fresnel << endl;
  cout <<   "    fishtank: " << fishtank_fresnel << endl;


  cout << "  dimensions [mm]:" << endl;

  cout << "    slab width:  " << slab_width << endl;
  cout << "    slab heigth: " << slab_height << endl;
  cout << "    slab length: " << slab_length << endl;

  if( !opt_woLens ) // obsolete
  {
    cout << "    lens radius:         " << lens_radius << endl;
    cout << "    lens thickness:      " << lens_thickness << endl;
    cout << "    lens diameter:       " << lens_diameter << endl;
    cout << "    lens conical const.: " << lens_conical << " [dim-less]" << endl;
  }

  if( opt_backLens )
  {
    cout << "    lens (backward) radius:         " << backLens_radius << endl;
    cout << "    lens (backward) thickness:      " << backLens_thickness << endl;
    cout << "    lens (backward) diameter:       " << backLens_diameter << endl;
    cout << "    lens (backward) conical const.: " << backLens_conical << " [dim-less]" << endl;
  }

  if( opt_frontLens )
  {
    cout << "    lens (forward) radius:         " << frontLens_radius << endl;
    cout << "    lens (forward) thickness:      " << frontLens_thickness << endl;
    cout << "    lens (forward) diameter:       " << frontLens_diameter << endl;
    cout << "    lens (forward) conical const.: " << frontLens_conical << " [dim-less]" << endl;
  }

  if( opt_prism )
  {
    cout << "    prism length: "            << prism_length      << endl;
    cout << "    prism height (top 1): "    << prism_heightUp1   << endl;
    cout << "    prism height (top 2): "    << prism_heightUp2   << endl;
    cout << "    prism height (bottom 1): " << prism_heightDown1 << endl;
    cout << "    prism height (bottom 2): " << prism_heightDown2 << endl;
    cout << "    prism width (top 1): "     << prism_widthUp1    << endl;
    cout << "    prism width (top 2): "     << prism_widthUp2    << endl;
    cout << "    prism width (bottom 1): "  << prism_widthDown1  << endl;
    cout << "    prism width (bottom 2): "  << prism_widthDown2  << endl;
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
    if( cannon_theta == 90 )
    {
      cout << "    theta: flat cos(theta)" << endl;
      cout << "    phi:   flat phi" << endl;
    }
    else
    {
      cout << "    theta:    " << cannon_theta << " deg" << endl;
      cout << "    phi:      " << cannon_phi   << " deg" << endl;
    }
    cout <<     "    reflection limit: " << refl_limit << endl;
  }
}
