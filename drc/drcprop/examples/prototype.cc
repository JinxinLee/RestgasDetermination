
//==========
// STD C/C++
//==========
#include <cstdlib>
using std::abort;
#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using std::fstream;
#include <list>
using std::list;
#include <string>
using std::string;


//======
// ROOT
//======
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TLine.h"
#include "TMarker.h"
#include "TMath.h"
using TMath::Abs;
using TMath::CeilNint;
using TMath::ACos;
using TMath::Cos;
using TMath::FloorNint;
using TMath::Pi;
using TMath::Power;
using TMath::Sin;
using TMath::Sqrt;
using TMath::Tan;
#include "TPaveText.h"
#include "TRandom3.h"
#include "TROOT.h"
#include "TRotation.h"
#include "TString.h"
#include "TStyle.h"
#include "TTree.h"
#include "TVector3.h"


//==========
// ROOT Math
//==========
#include "Math/Point3D.h"
using ROOT::Math::XYZPoint;
#include "Math/Transform3D.h"
using ROOT::Math::Transform3D;
#include "Math/Vector3D.h"
using ROOT::Math::XYZVector;
using ROOT::Math::Polar3DVector;
#include "Math/RotationX.h"
using ROOT::Math::RotationX;
#include "Math/RotationY.h"
using ROOT::Math::RotationY;
#include "Math/RotationZ.h"
using ROOT::Math::RotationZ;


//========
// drcprop
//========
#include "PndDrcSurfAbs.h"
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcSurfPolyAsphere.h"
#include "PndDrcSurfPolyCyl.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatAbs.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatBK7.h"
#include "PndDrcOptMatMarcol7.h"
#include "PndDrcOptMatVacuum.h"
#include "PndDrcOptDevSys.h"
#include "PndDrcOptVol.h"
#include "PndDrcOptDevManager.h"



int main(int argc, char *argv[])
{
  cout << "Parameter usage:" << endl;
  cout << "  1. Par.: output filename (default: beamtest0909_test)" << endl;
  cout << "  Further parameter can be set by yourself in section \"Input parameters\" to avoid permament compiling" << endl;
  cout << endl;



//==============================================================================
// Simulation options
//==============================================================================

  // main options
  bool opt_default         = true;  // default simulation
  bool opt_angleAcceptance = false; // beamtest simulation with diff. theta and phi
  bool opt_photonCannon    = false; // photon cannon

  cout << "simulation options:" << endl;

  if( opt_default )
    cout << "  main: \"default\"" << endl;
  if( opt_photonCannon )
    cout << "  main: \"photon cannon\"" << endl;
  if( opt_angleAcceptance )
    cout << "  main: \"angle acceptance\" " << endl;


  // sub options
  bool opt_fishtankBlack_bottom = true; // absorbed fishtank side
  bool opt_fishtankBlack_sides  = true; //
  bool opt_fishtankBlack_top    = true; //
  bool opt_slabBlack_front      = false; //
  bool opt_frontLens            = true; // forward lens
  bool opt_prism                = false; // prism (forward)
  bool opt_prismIsTank          = false; // prism is fishtank (expansion volume)
  bool opt_backLens             = false; // backward lens
  bool opt_mirror               = false; // mirror (backward)
  bool opt_noFresnel_backLens   = false; // disable Fresnel reflections
  bool opt_noFresnel_slab       = false; //
  bool opt_noFresnel_frontLens  = false; //
  bool opt_noFresnel_airBox     = false; //
  bool opt_noFresnel_prism      = false; //
  bool opt_noFresnel_fishtank   = false; //
  bool opt_Cherenkov_onlyInBar  = true; // Cherenkov photons are only generated in bar (slab)
  bool opt_alongBar             = false; // particles hits the bar at slab front end
  bool opt_fromBelow            = false; // particles hits the bar from below
  bool opt_photonPosList        = false; // write out photon position list ; true takes much longer
  bool opt_fullCannonList       = false; // default: false, with true also not measured photons are tagged
  bool opt_debug                = false; // debug information ; please pipe stdout > log file

  if( opt_prismIsTank && !opt_prism )
  {
    opt_prism = true;
    opt_frontLens = false;
    cout << "*** WARNING: No front lens because fishtank has a prism shape" << endl;
  }

  if( opt_backLens && !opt_mirror )
  {
    opt_mirror = true;
    cout << "*** WARNING: Lens in backward direction w/o mirrored end is useless" << endl;
  }

  if( opt_slabBlack_front && opt_mirror )
  {
    cout << "*** ERROR: Full reflectivity (mirror) and full absorption at bar end doesn't work" << endl;
    abort();
  }

  if( opt_photonCannon && !opt_photonPosList )
  {
    opt_photonPosList = true;
    cout << "*** WARNING: \"Photon position list\" is now enable because \"photon cannon\" needs it" << endl;
  }

  if( opt_alongBar && opt_fromBelow )
  {
    cout << "*** ERROR: Either one of both (\"along Bar\", \"from below\") or none" << endl;
    abort();
  }


  cout << "  sub:  ";
  if( opt_fishtankBlack_bottom )
    cout << "\"black fishtank bottom\" ";
  if( opt_fishtankBlack_sides )
    cout << "\"black fishtank sides\" ";
  if( opt_fishtankBlack_top )
    cout << "\"black fishtank top\" ";
  if( opt_slabBlack_front )
    cout << "\"black slab front\" ";
  if( opt_frontLens )
    cout << "\"front lens\" ";
  if( opt_prism )
    cout << "\"prism\" ";
  if( opt_prismIsTank )
    cout << "\"prism is fishtank\" ";
  if( opt_backLens )
    cout << "\"back lens\" ";
  if( opt_mirror )
    cout << "\"mirror\" ";
  if( opt_Cherenkov_onlyInBar )
    cout << "\"photon production only in bar\" ";
  if( opt_alongBar )
    cout << "\"along bar\" ";
  if( opt_fromBelow )
    cout << "\"from below\" ";
  if( opt_photonPosList )
    cout << "\"photon position list\" ";
  if( opt_photonCannon && opt_photonPosList && opt_fullCannonList  )
    cout << "\"full list\" ";
  if( opt_noFresnel_backLens || opt_noFresnel_slab || opt_noFresnel_frontLens || opt_noFresnel_airBox || opt_noFresnel_fishtank )
  {
    cout << "\"No Fresnel reflection for";
    if( opt_noFresnel_backLens )
      cout << " lens (backward)";
    if( opt_noFresnel_slab )
      cout << " slab";
    if( opt_noFresnel_frontLens )
      cout << " lens (forward)";
    if( opt_noFresnel_airBox )
      cout << " airBox";
    if( opt_noFresnel_prism )
      cout << " prism";
    if( opt_noFresnel_fishtank )
      cout << " fishtank";
    cout << "\"";
  }
  cout << endl;


  int check_opt = opt_default + opt_photonCannon + opt_angleAcceptance;

  if( check_opt > 1 || check_opt == 0 )
  {
    cout << "*** ERROR: Select only one main simulation option !" << endl;
    cout << "available main simulation options:" << endl;
    cout << "  default" << endl;
    cout << "  photon cannon" << endl;
    cout << "  angle acceptance" << endl;

    abort();
  }



//==============================================================================
// Changable parameters & constants
//==============================================================================
//
// dimensions in mm
// angle in degree
//

  // math constants
  const double pi = Pi();
  const double degree = pi/180.;

  // material
  PndDrcOptMatLithotecQ0  *quartz  = new PndDrcOptMatLithotecQ0();
  PndDrcOptMatVacuum      *vacuum  = new PndDrcOptMatVacuum();
  PndDrcOptMatBK7         *bk7     = new PndDrcOptMatBK7();
  PndDrcOptMatMarcol7     *marcol7 = new PndDrcOptMatMarcol7();

  PndDrcOptMatAbs *mat_backLens  = quartz;
  PndDrcOptMatAbs *mat_slab      = quartz;
  PndDrcOptMatAbs *mat_frontLens = quartz;
  PndDrcOptMatAbs *mat_airBox    = vacuum;
  PndDrcOptMatAbs *mat_prism     = quartz;
  PndDrcOptMatAbs *mat_fishtank  = marcol7;


  // dimensions
  double slab_width  = 17.; // default: 17 mm
  double slab_height = 35.; // default: 35 mm
  double slab_length = 800.; // default: 800 mm

  double backLens_radius    = 520.; // f = R/(n-1) (from manufacturer in general at 589 nm)
  double backLens_thickness = 10.;
  double backLens_width     = slab_width; // rectangular lens base shape (cylindrical not implemented yet)
  double backLens_height    = slab_height;
  double backLens_conical   = 0.; // default: 0 (spherical)
  int backLens_cylindrical  = 0; // default: 0 (use conical), 1: only curved in x, 2: in y

  double frontLens_radius    = 117.4; // 115.0 or 117.4
  double frontLens_thickness = 9.0; // 6.0 or 9.0
  double frontLens_width     = slab_width;
  double frontLens_height    = slab_height;
  double frontLens_conical   = 0.;
  int frontLens_cylindrical  = 0;


//                      /|
//                     / |
//                    /  |
//                   /   |<-heightUp2
//             ____ /____|____
//      heightUp1->|     |
//   ______________|     |____
//                 |     |
//        slab     |prism|<-height
//   ______________|     |____
//    heightDown1->|     |
//             ____|_____|____
//                  \    |
//                   \   |<-heightDown2
//                    \  |
//                     \ |
//                      \|
//
//                 |--L--|
//
// same for the width

  double prism_length      = 300.;
  double prism_height      = slab_height;
  double prism_heightUp1   = 15.;
  double prism_heightUp2   = Tan(30.*degree)*prism_length; // or Tan(x*degree)*prism_length
  double prism_heightDown1 = 15.;
  double prism_heightDown2 = 0.; // or Tan(x*degree)*prism_length
  double prism_width       = 760.;
  double prism_widthUp1    = 0.;
  double prism_widthUp2    = 0.; // or Tan(x*degree)*prism_length
  double prism_widthDown1  = 0.;
  double prism_widthDown2  = 0.; // or Tan(x*degree)*prism_length


  double airgap = 20.; // distance between slab or prism and fishtank ; 0 means no air box

  double fishtank_width  = 760.;
  double fishtank_height = 760.;
  double fishtank_length = 300.;

  double fishtank_width_offset  = 0.; // default: 0 mm ; 0 means bar is centered
  double fishtank_height_offset = 0.; // default: 0 mm

  double fishtank_thetaX = 0.; // default: 0 degree (rotation axis X in fishtank)
  double fishtank_thetaY = 0.;  // default: 0 degree (rotation axis Y in fishtank)
  double fishtank_phi    = 0.;  // default: 0 degree (rotation axis Z in fishtank)


  // particle properties
  const double mass_p  = 0.9383; // proton mass in GeV
  const double mass_K  = 0.4937; // kaon mass
  const double mass_pi = 0.1396; // pion mass
  const double mass_e  = 0.0005; // electron mass
  const double mass_mu = 0.1057; // muon mass

  double mass = mass_pi;

  double kinE = -666; // in GeV; T = sqrt( m^2 + p^2 ) - m; -666 means unset, then mom has to be set
  double mom  = 1.7; // in GeV; p = sqrt( (T + m)^2 - m^2 ); -666 means unset, then kinE has to be set
  double beta = -666; // just for initialization; will be overwritten later

  double spot_radius = 20.; // default: 20 mm 1-sigma beam spot radius (gaus smeared)
  double spot_limit  = 50.; // default: 50 mm beam spot radius limit

  int particle_number = 10; // default: 300

  double inci_theta = 30.; // default: 30 degree
  double inci_phi   = 0.;  // default: 0 degree

  double hitBarX = slab_width/2.; // default: slab_width/2
  double hitBarY = 0.; // default: 0 mm
  double hitBarZ = -300.; // default: -500 mm


  // photon properties
  int photon_number = 0; // default: 100 (per particle); 0 means realistic number of Cherenkov photons

  double lambda_min = 400.; // default: 300 nm ; lowest Cherenkov wavelength
  double lambda_max = 400.; // default: 700 nm ; highest Cherenkov wavelength

  int refl_limit = 1000; // default: 1000 reflections


  // photon cannon
  int shoots = 20000; // default: 2000000 (for center) ; for grid this number is per gridpoint

  double gridXstep = 0.; // default: 0 mm ; grid constant in X ; 0 means cannon is always in the center
  double gridYstep = 0.; // default: 0 mm

  double cannon_theta = 180.; // default: 180 degree; 180 degree means flat cos(angle) distribution otherwise fixed angle
  double cannon_phi   = 360.; // default: 360 degree; 360 degree means flat angle distribution otherwise fixed angle
  double cannon_rotY  = 0.; // default: 0 degree; rotate cannon axis around y-axis
  double cannon_rotX  = 0.; // default: 0 degree; rotate cannon axis around x-axis (or new axis rotated by the previous one)

  int refl_limit_2 = 5; // default: 5 ; low limit increases speed

  int seed = 4357; // default TRandom3 value

  double cannon_posX = 0.; // offset in X, also valid for gridXstep
  double cannon_posY = 0.;
  double cannon_posZ = -0.01; // default: -0.01 mm to be sure that photon is in bar


  // output filename (w/o file extension)
  TString defaultFilename = "prototype_test"; // default: prototype_test
  TString outFilename     = defaultFilename;  // default: defaultFilename



//==============================================================================
// Input parameters
//==============================================================================

  // command line arguments (set variable parameter to avoid permanent compiling)
  for( int i = 1; i < argc; i++)
  {
    if( i == 1 )
      outFilename = argv[i];

    if( i == 2 )
      seed = atoi( argv[i]);
//     if( i == 3 )
//       shoots = atoi( argv[i]);
//     if( i == 4 )
//       cannon_posZ = atof( argv[i]);

//     if( i == 2 )
//       inci_theta = atof( argv[i]);
//     if( i == 3 )
//       inci_phi = atof( argv[i]);
//
//     if( i == 4 )
//       hitBarZ = atof( argv[i]);

//     if( i == 2 )
//       fishtank_thetaX = atof( argv[i]);
//     if( i == 3 )
//       fishtank_thetaY = atof( argv[i]);
//     if( i == 4 )
//       fishtank_phi = atof( argv[i]);

//     if( i == 2 )
//       opt_frontLens = atoi( argv[i]);
//     if( i == 3 )
//       frontLens_cylindrical = atoi( argv[i]);
//     if( i == 4 )
//       airgap = atof( argv[i]);

//     if( i == 2 )
//       cannon_posZ = atof( argv[i]);
//     if( i == 3 )
//       cannon_rotY = atof( argv[i]);
  }



//==============================================================================
// Incidence angle & hit position on bar
//==============================================================================

//                    detector              alongBar option:
//                       ^ z
//                       |                       detector
//                       |                          ^
//                 x<----|                          |
//                   exit end                       |
//                   --------                       |
//                   |                    |                  |
//                   |                    |                  |
//                   |                    |        bar       |
//                   |                    |                  |
//                   |  bar               -------------------- front end
//                   |                              |\.
//                   |                              | \.
// perp. ------------|                              |  \.
//           \ theta/|                              |   \.
//            \    / |                              |    \.
//             \  /  |                              |     \.
//              \/   |                              | theta\.
//              /    |                              |       \.
//       particle    |  test.root                            |      particle
//       trajectory  |                             perp.   trajectory


  double parDirX, parDirY, parDirZ;
  XYZVector parDir;

  if( opt_alongBar )
  {
    parDirX = Tan(inci_theta*degree);
    parDirY = 0;
    parDirZ = 1;

    parDir.SetXYZ( parDirX, parDirY, parDirZ );

    Transform3D rot_phi = Transform3D( RotationZ(-inci_phi*degree) );
    parDir = rot_phi*parDir;
  }
  else if( opt_fromBelow )
  {
    parDirX = Tan(inci_theta*degree);
    parDirY = 1;
    parDirZ = 0;

    parDir.SetXYZ( parDirX, parDirY, parDirZ );

    Transform3D rot_phi = Transform3D( RotationY(-inci_phi*degree) );
    parDir = rot_phi*parDir;
  }
  else
  {
    parDirX = -1;
    parDirY = 0;
    parDirZ = Tan(inci_theta*degree);

    parDir.SetXYZ( parDirX, parDirY, parDirZ );

    Transform3D rot_phi = Transform3D( RotationX(inci_phi*degree) );
    parDir = rot_phi*parDir;
  }

  parDir = parDir.Unit();
  parDirX = parDir.X();
  parDirY = parDir.Y();
  parDirZ = parDir.Z();


// hit position on bar (is independent of the incidence angle)

//              Y     exit
//              ^    z
//              |   ^
//              |  /
//              | /
//              |/
//   x <--------x

// origin x is centered at the slab exit (back end)

//                    exit
//               s8----------s5
//              /|     _    /|      _ (0,0,0) origin
//             / |         / |
//            /  s7-------/--s6
//           /  /        /  /
//          /  /        /  /
//         /  /        /  /
//        /  /        /  /
//      s4----------s1  /
//      |  /        |  /
//      | / front   | /
//      s3----------s2


  if( opt_default || opt_angleAcceptance )
  {
    if( opt_alongBar )
    {
      if( hitBarX < -slab_width/2 || hitBarX > slab_width/2 )
      {
        cout << "*** ERROR: hit position X is not on bar surface ! " << endl;
        abort();
      }

      if( hitBarY < -slab_height/2 || hitBarY > slab_height/2 )
      {
        cout << "*** ERROR: hit position Y is not on bar surface ! " << endl;
        abort();
      }

      if( hitBarZ != -slab_length )
      {
        hitBarZ = -slab_length; // is fixed at slab front end
        cout << "*** WARNING: hit position Z is fixed at " << -slab_length << " mm (slab front end)" << endl << endl;
      }
    }
    else if( opt_fromBelow )
    {
      if( hitBarX < -slab_width/2 || hitBarX > slab_width/2 )
      {
        cout << "*** ERROR: hit position X is not on bar surface ! " << endl;
        abort();
      }

      if( hitBarY != -slab_height/2 )
      {
        hitBarY = -slab_height/2;
        cout << "*** WARNING: hit position Y is fixed at " << -slab_height/2 << " mm (1/2 slab height)" << endl << endl;
      }

      if( hitBarZ < -slab_length || hitBarZ > 0 )
      {
        cout << "*** ERROR: hit position Z is not on bar surface ! " << endl;
        abort();
      }
    }
    else
    {
      if( hitBarX != slab_width/2 )
      {
        hitBarX = slab_width/2;
        cout << "*** WARNING: hit position X is fixed at " << slab_width/2 << " mm (1/2 slab width)" << endl << endl;
      }

      if( hitBarY < -slab_height/2 || hitBarY > slab_height/2 )
      {
        cout << "*** ERROR: hit position Y is not on bar surface ! " << endl;
        abort();
      }

      if( hitBarZ < -slab_length || hitBarZ > 0 )
      {
        cout << "*** ERROR: hit position Z is not on bar surface ! " << endl;
        abort();
      }
    }
  }

  XYZPoint pos(hitBarX, hitBarY, hitBarZ);



//==============================================================================
// Parameter output
//==============================================================================
  cout << "material:" << endl;

  string mat_backLens_str  = mat_backLens->Name();
  string mat_slab_str      = mat_slab->Name();
  string mat_frontLens_str = mat_frontLens->Name();
  string mat_airBox_str    = mat_airBox->Name();
  string mat_prism_str     = mat_prism->Name();
  string mat_fishtank_str  = mat_fishtank->Name();

  cout   << "  slab:            " << mat_slab_str << endl;
  if( opt_backLens )
    cout << "  lens (backward): " << mat_backLens_str << endl;
  if( opt_frontLens )
    cout << "  lens (forward):  " << mat_frontLens_str << endl;
  if( opt_prism )
    cout << "  prism:           " << mat_prism_str << endl;
  if( airgap > 0 )
    cout << "  airBox:          " << mat_airBox_str << endl;
  if( !opt_prismIsTank )
    cout << "  fishtank:        " << mat_fishtank_str << endl;


  // refractive index check
  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: refractive index check" << endl;
    double waveLength[4];
    waveLength[0] = 300.;
    waveLength[1] = 405.;
    waveLength[2] = 532.;
    waveLength[3] = 589.;
    for( int i = 0; i < 4; i++ )
    {
      cout << "     " << mat_slab_str      << " (" << waveLength[i] << " nm) = " << mat_slab->RefIndex(waveLength[i]) << endl;
      cout << "     " << mat_backLens_str  << " (" << waveLength[i] << " nm) = " << mat_backLens->RefIndex(waveLength[i]) << endl;
      cout << "     " << mat_frontLens_str << " (" << waveLength[i] << " nm) = " << mat_frontLens->RefIndex(waveLength[i]) << endl;
      cout << "     " << mat_airBox_str    << " (" << waveLength[i] << " nm) = " << mat_airBox->RefIndex(waveLength[i]) << endl;
      cout << "     " << mat_prism_str     << " (" << waveLength[i] << " nm) = " << mat_prism->RefIndex(waveLength[i]) << endl;
      cout << "     " << mat_fishtank_str  << " (" << waveLength[i] << " nm) = " << mat_fishtank->RefIndex(waveLength[i]) << endl;
    }
  }


  cout << "dimensions [mm]:" << endl;

  cout << "  slab width:  " << slab_width  << endl;
  cout << "  slab heigth: " << slab_height << endl;
  cout << "  slab length: " << slab_length << endl;

  if( opt_backLens )
  {
    cout << "  lens (backward) radius:         " << backLens_radius << endl;
    cout << "  lens (backward) thickness:      " << backLens_thickness << endl;
    cout << "  lens (backward) width:          " << backLens_width << endl;
    cout << "  lens (backward) height:         " << backLens_height << endl;
    cout << "  lens (backward) conical const.: " << backLens_conical << endl;

    if( backLens_cylindrical == 1 )
      cout << "  lens (backward) cylindrical:    in x" << endl;
    if( backLens_cylindrical == 2 )
      cout << "  lens (backward) cylindrical:    in y" << endl;
  }

  if( opt_frontLens )
  {
    cout << "  lens (forward) radius:         " << frontLens_radius << endl;
    cout << "  lens (forward) thickness:      " << frontLens_thickness << endl;
    cout << "  lens (forward) height:         " << frontLens_height << endl;
    cout << "  lens (forward) height:         " << frontLens_height << endl;
    cout << "  lens (forward) conical const.: " << frontLens_conical << endl;

    if( frontLens_cylindrical == 1 )
      cout << "  lens (forward) cylindrical:    in x" << endl;
    if( frontLens_cylindrical == 2 )
      cout << "  lens (forward) cylindrical:    in y" << endl;
  }

  if( frontLens_thickness > airgap && opt_frontLens )
  {
    cout << "*** ERROR: no air gap possible" << endl;
    abort();
  }

  if( opt_prism )
  {
    cout << "  prism length:            " << prism_length      << endl;
    cout << "  prism height:            " << prism_height      << endl;
    cout << "  prism height (top 1):    " << prism_heightUp1   << endl;
    cout << "  prism height (top 2):    " << prism_heightUp2   << endl;
    cout << "  prism height (bottom 1): " << prism_heightDown1 << endl;
    cout << "  prism height (bottom 2): " << prism_heightDown2 << endl;
    cout << "  prism width:             " << prism_width       << endl;
    cout << "  prism width (top 1):     " << prism_widthUp1    << endl;
    cout << "  prism width (top 2):     " << prism_widthUp2    << endl;
    cout << "  prism width (bottom 1):  " << prism_widthDown1  << endl;
    cout << "  prism width (bottom 2):  " << prism_widthDown2  << endl;
  }

  cout << "  air gap: " << airgap << endl;

  if( opt_prismIsTank && airgap != 0 )
  {
    cout << "*** ERROR: no air gap with fishtank as prism possible" << endl;
    abort();
  }

  if( !opt_prismIsTank )
  {
    cout << "  fishtank width:  " << fishtank_width  << endl;
    cout << "  fishtank heigth: " << fishtank_height << endl;
    cout << "  fishtank length: " << fishtank_length << endl;

    cout << "  fishtank width offset : " << fishtank_width_offset  << endl;
    cout << "  fishtank height offset: " << fishtank_height_offset << endl;

    cout << "  fishtank thetaX: " << fishtank_thetaX << endl;
    cout << "  fishtank thetaY: " << fishtank_thetaY << endl;
    cout << "  fishtank phi:    " << fishtank_phi    << endl;
  }


  if( opt_default || opt_angleAcceptance )
  {
    cout << "particle properties:" << endl;

    if( mass == mass_p)
      cout << "  sort:    proton"   << endl;
    else if( mass == mass_K )
      cout << "  sort:    kaon"     << endl;
    else if( mass == mass_pi )
      cout << "  sort:    pion"     << endl;
    else if( mass == mass_e )
      cout << "  sort:    electron" << endl;
    else if( mass == mass_mu )
      cout << "  sort:    muon"     << endl;
    else
      cout << "*** WARNING: undefined particle sort" << endl;

    if( mom == -666 )
    {
      mom  = Sqrt( Power( kinE + mass, 2 ) - mass*mass );
      beta = Sqrt( 1 - Power( mass / (kinE + mass), 2 ) ); // E = T + m = gamma * m; gamma^2 = 1 / (1 - beta^2)
    }
    else if ( kinE == -666 )
    {
      kinE = Sqrt( mass*mass + mom*mom ) - mass;
      beta = mom / Sqrt( mass*mass + mom*mom ); // E^2 = m^2 + p^2
    }
    else
    {
      if( mom != Sqrt( Power( kinE + mass, 2 ) - mass*mass ) )
      {
        cout << "*** ERROR: particle momentum and kinetic energy are not consistent; use -666 to unset one" << endl;
        abort();
      }

      beta = mom / Sqrt( mass*mass + mom*mom );
    }

    cout <<     "  T [GeV]: " << kinE << endl;
    cout <<     "  p [GeV]: " << mom  << endl;
    cout <<     "  beta:    " << beta << endl;

    cout <<     "  incidence angle (theta): " << inci_theta << " deg" << endl;
    cout <<     "  incidence angle (phi):   " << inci_phi   << " deg" << endl;
    cout <<     "  flight direction:        (" << parDirX << ", " << parDirY << ", " << parDirZ << ")" << endl;
    cout <<     "  centered hit pos on bar: (" << hitBarX << ", " << hitBarY << ", " << hitBarZ << ")" << endl;

    cout <<     "  beam spot radius: " << spot_radius << " mm" << endl;
    cout <<     "  radius limit:     " << spot_limit  << " mm" << endl;
    cout <<     "  particle number:  " << particle_number << endl;


    cout << "photon properties:" << endl;

    if( photon_number == 0 )
      cout << "  photon number:      realistic" << endl;
    else
      cout << "  photon number:      " << photon_number << endl;

    cout <<     "  Cherenkov spectrum: [" << lambda_min << ", " << lambda_max << "] nm" << endl;
    cout <<     "  reflection limit:   " << refl_limit << endl;
  }


  if( opt_photonCannon )
  {
    cout << "photon cannon:" << endl;
    if( gridXstep == 0 || gridYstep == 0 )
      cout << "  photon number: " << shoots << endl;
    else
    {
      cout << "  photon number: " << shoots << " per mesh" << endl;
      cout << "  grid const. X: " << gridXstep << " mm" << endl;
      cout << "  grid const. Y: " << gridYstep << " mm" << endl;
    }

    if( cannon_theta == 180 )
      cout << "  theta: flat cos(theta)" << endl;
    else
      cout << "  theta:      " << cannon_theta << " deg" << endl;

    if( cannon_phi == 360 )
      cout << "  phi:   flat phi" << endl;
    else
      cout << "  phi:        " << cannon_phi   << " deg" << endl;

    cout <<   "  rotation Y: " << cannon_rotY  << " deg" << endl;
    cout <<   "  rotation X: " << cannon_rotX  << " deg" << endl;
    cout <<   "  pos X: " << cannon_posX << " mm" << endl;
    cout <<   "  pos Y: " << cannon_posY << " mm" << endl;
    cout <<   "  pos Z: " << cannon_posZ << " mm" << endl;
    cout <<   "  reflection limit: " << refl_limit_2 << endl;
    cout <<   "  lambda: [" << lambda_min << ", " << lambda_max << "] nm" << endl;
  }



//==============================================================================
// ROOT output file
//==============================================================================

// ROOT file content:
//  photon (TTree)
//  particle (TTree)
//  info (TTree) for global parameter like e.g. lens thickness
//  default plots: screen, beamspot, setup geometry
//
// debug tools
// 1. paramater.cc to check the used paramters for a certain ROOT-file
// 2. parTrajectory.cc to visualize the particle trajectories
// 3. photTrajectory.cc to visualize the photon trajectories
//
// to analyze this root-file there:
// 1. kBarAnalysis.cc produces a lot of histograms to study the kBar-vectors
// 2. mcpPos.cc for fast plotting the effect of different MCP positions
// 3. mcp4Pos.cc to create a further root-file for Cherenkov angle reconstruction



// file name
//==============================================================================
  Bool_t defaultFilenameFlag = false;
  if( outFilename == defaultFilename )
    defaultFilenameFlag = true;

  if( defaultFilenameFlag && !(opt_default || opt_angleAcceptance) )
  {
    if( opt_photonCannon )
      outFilename = "kBarList_test";
    else
      outFilename = "test";
  }

  TString originalFilename = outFilename;
  outFilename = outFilename + ".root";

  TFile *checkFile = new TFile( outFilename, "CREATE" );

  Bool_t fileIsOpen = true;
  if( !checkFile->IsOpen() ) // doesn't work with checkFile->Open(...) before
    fileIsOpen = false;

  checkFile->Delete();

  Int_t fileCounter = 0;

  while( !fileIsOpen && !defaultFilenameFlag )
  {
    fileCounter++;
    if( fileCounter > 10 )
    {
      cout << "*** ERROR: Please select another file name." << endl;
      abort();
    }
    TString fileCounter_str;
    fileCounter_str += fileCounter;
    fileCounter_str.Remove( TString::kLeading, ' ' );

    outFilename = originalFilename + "_" + fileCounter_str;
    outFilename = outFilename + ".root";

    TFile testFile( outFilename, "CREATE" );
    if( testFile.IsOpen() )
      fileIsOpen = true;
  }

  TFile *outFile = new TFile( outFilename, "RECREATE" ); // with CREATE ROOT says "not opened in write mode" (?)

  cout << "ROOT output file: " << outFilename << endl;

  int rootVer = gROOT->GetVersionInt(); // global pointer from ROOT
  cout << "ROOT version: "<< rootVer << endl << endl;



// trees
//==============================================================================
  TTree *photonTree  	= new TTree( "photon", outFilename );
  TTree *particleTree = new TTree( "particle", outFilename );
  TTree *infoTree  	  = new TTree( "info", outFilename );


  // infoTree (parameter list)

//******************************************************************************
// Note:
// Unfortunetly TTree allows only basic c-types (no strings) and no-const types
//******************************************************************************

  const char *slab_mat_helper      = mat_slab_str.c_str(); // includes \0 (escape sequence)
  const char *backLens_mat_helper  = mat_backLens_str.c_str();
  const char *frontLens_mat_helper = mat_frontLens_str.c_str();
  const char *airBox_mat_helper    = mat_airBox_str.c_str();
  const char *prism_mat_helper     = mat_prism_str.c_str();
  const char *fishtank_mat_helper  = mat_fishtank_str.c_str();

  char slab_material[64]      = "";
  char backLens_material[64]  = "";
  char frontLens_material[64] = "";
  char airBox_material[64]    = "";
  char prism_material[64]     = "";
  char fishtank_material[64]  = "";

  strcpy(slab_material, slab_mat_helper);
  if( opt_backLens )
    strcpy(backLens_material, backLens_mat_helper);
  if( opt_frontLens )
    strcpy(frontLens_material, frontLens_mat_helper);
  if( airgap > 0 )
    strcpy(airBox_material, airBox_mat_helper);
  if( opt_prism )
    strcpy(prism_material, prism_mat_helper);
  strcpy(fishtank_material, fishtank_mat_helper);

  bool slab_fresnel      = !opt_noFresnel_slab;
  bool backLens_fresnel  = !opt_noFresnel_backLens;
  bool frontLens_fresnel = !opt_noFresnel_frontLens;
  bool airBox_fresnel    = !opt_noFresnel_airBox;
  bool prism_fresnel     = !opt_noFresnel_prism;
  bool fishtank_fresnel  = !opt_noFresnel_fishtank;

  bool fishtankBlack_bottom = opt_fishtankBlack_bottom;
  bool fishtankBlack_sides  = opt_fishtankBlack_sides;
  bool fishtankBlack_top    = opt_fishtankBlack_top;

  bool slabBlack_front = opt_slabBlack_front;

  bool mirror = opt_mirror;

  if( opt_prismIsTank )
  {
    fishtank_width         = prism_width + prism_widthUp2 + prism_widthDown2;
    fishtank_height        = prism_height + prism_heightUp2 + prism_heightDown2;
    fishtank_length        = -666;
    fishtank_width_offset  = -666;
    fishtank_height_offset = -666;
    fishtank_thetaX        = -666;
    fishtank_thetaY        = -666;
    fishtank_phi           = -666;
  }

  if( !opt_backLens )
  {
    backLens_radius      = -666;
    backLens_thickness   = -666;
    backLens_width       = -666;
    backLens_height      = -666;
    backLens_conical     = -666;
    backLens_cylindrical = -666;
  }

  if( !opt_frontLens )
  {
    frontLens_radius      = -666;
    frontLens_thickness   = -666;
    frontLens_width       = -666;
    frontLens_height      = -666;
    frontLens_conical     = -666;
    frontLens_cylindrical = -666;
  }

  if( !opt_prism )
  {
    prism_length      = -666;
    prism_height      = -666;
    prism_heightUp1   = -666;
    prism_heightUp2   = -666;
    prism_heightDown1 = -666;
    prism_heightDown2 = -666;
    prism_width       = -666;
    prism_widthUp1    = -666;
    prism_widthUp2    = -666;
    prism_widthDown1  = -666;
    prism_widthDown2  = -666;;
  }

  if( opt_photonCannon || opt_angleAcceptance )
  {
    parDirX         = -666;
    parDirY         = -666;
    parDirZ         = -666;
    inci_theta      = -666;
    inci_phi        = -666;
  }

  if( opt_photonCannon )
  {
    photon_number   = -666;
    mass            = -666;
    kinE            = -666;
    beta            = -666;
    hitBarX         = -666;
    hitBarY         = -666;
    hitBarZ         = -666;
    spot_radius     = -666;
    spot_limit      = -666;
    particle_number = -666;

    refl_limit = refl_limit_2;
  }

  if( !opt_photonCannon )
  {
    shoots       = -666;
    gridXstep    = -666;
    gridYstep    = -666;
    cannon_theta = -666;
    cannon_phi   = -666;
    cannon_phi   = -666;
    cannon_rotX  = -666;
    cannon_rotY  = -666;
    cannon_posX  = -666;
    cannon_posY  = -666;
    cannon_posZ  = -666;
  }

  const int pos_size = refl_limit + 100; // due to tiny shifts at volume transitions, start and detection position)
  int pos_size_info = pos_size;

  if( !opt_photonPosList )
    pos_size_info = -666;


  infoTree->Branch( "root_version"          , &rootVer               , "root_version/I" );
  infoTree->Branch( "slab_material"         , &slab_material         , "slab_material/C" );
  infoTree->Branch( "slab_width"            , &slab_width            , "slab_width/D" );
  infoTree->Branch( "slab_height"           , &slab_height           , "slab_height/D" );
  infoTree->Branch( "slab_length"           , &slab_length           , "slab_length/D" );
  infoTree->Branch( "slab_fresnel"          , &slab_fresnel          , "slab_fresnel/O" );
  infoTree->Branch( "backLens_material"     , &backLens_material     , "backLens_material/C" );
  infoTree->Branch( "backLens_radius"       , &backLens_radius       , "backLens_radius/D" );
  infoTree->Branch( "backLens_thickness"    , &backLens_thickness    , "backLens_thickness/D" );
  infoTree->Branch( "backLens_width"        , &backLens_width        , "backLens_width/D" );
  infoTree->Branch( "backLens_height"       , &backLens_height       , "backLens_height/D" );
  infoTree->Branch( "backLens_conical"      , &backLens_conical      , "backLens_conical/D" );
  infoTree->Branch( "backLens_cylindrical"  , &backLens_cylindrical  , "backLens_cylindrical/I" );
  infoTree->Branch( "backLens_fresnel"      , &backLens_fresnel      , "backLens_fresnel/O" );
  infoTree->Branch( "frontLens_material"    , &frontLens_material    , "frontLens_material/C" );
  infoTree->Branch( "frontLens_radius"      , &frontLens_radius      , "frontLens_radius/D" );
  infoTree->Branch( "frontLens_thickness"   , &frontLens_thickness   , "frontLens_thickness/D" );
  infoTree->Branch( "frontLens_width"       , &frontLens_width       , "frontLens_width/D" );
  infoTree->Branch( "frontLens_height"      , &frontLens_height      , "frontLens_height/D" );
  infoTree->Branch( "frontLens_conical"     , &frontLens_conical     , "frontLens_conical/D" );
  infoTree->Branch( "frontLens_cylindrical" , &frontLens_cylindrical , "frontLens_cylindrical/I" );
  infoTree->Branch( "frontLens_fresnel"     , &frontLens_fresnel     , "frontLens_fresnel/O" );
  infoTree->Branch( "prism_material"        , &prism_material        , "prism_material/C" );
  infoTree->Branch( "prism_fresnel"         , &prism_fresnel         , "prism_fresnel/O" );
  infoTree->Branch( "prism_length"          , &prism_length          , "prism_length/D" );
  infoTree->Branch( "prism_height"          , &prism_height          , "prism_height/D" );
  infoTree->Branch( "prism_heightUp1"       , &prism_heightUp1       , "prism_heightUp1/D" );
  infoTree->Branch( "prism_heightUp2"       , &prism_heightUp2       , "prism_heightUp2/D" );
  infoTree->Branch( "prism_heightDown1"     , &prism_heightDown1     , "prism_heightDown1/D" );
  infoTree->Branch( "prism_heightDown2"     , &prism_heightDown2     , "prism_heightDown2/D" );
  infoTree->Branch( "prism_width"           , &prism_width           , "prism_width/D" );
  infoTree->Branch( "prism_widthUp1"        , &prism_widthUp1        , "prism_widthUp1/D" );
  infoTree->Branch( "prism_widthUp2"        , &prism_widthUp2        , "prism_widthUp2/D" );
  infoTree->Branch( "prism_widthDown1"      , &prism_widthDown1      , "prism_widthDown1/D" );
  infoTree->Branch( "prism_widthDown2"      , &prism_widthDown2      , "prism_widthDown2/D" );
  infoTree->Branch( "airBox_material"       , &airBox_material       , "airBox_material/C" );
  infoTree->Branch( "airgap"                , &airgap                , "airgap/D" );
  infoTree->Branch( "airBox_fresnel"        , &airBox_fresnel        , "airBox_fresnel/O" );
  infoTree->Branch( "fishtank_material"     , &fishtank_material     , "fishtank_material/C" );
  infoTree->Branch( "fishtank_width"        , &fishtank_width        , "fishtank_width/D" );
  infoTree->Branch( "fishtank_height"       , &fishtank_height       , "fishtank_height/D" );
  infoTree->Branch( "fishtank_length"       , &fishtank_length       , "fishtank_length/D" );
  infoTree->Branch( "fishtank_width_offset" , &fishtank_width_offset , "fishtank_width_offset/D" );
  infoTree->Branch( "fishtank_height_offset", &fishtank_height_offset, "fishtank_height_offset/D" );
  infoTree->Branch( "fishtank_thetaX"       , &fishtank_thetaX       , "fishtank_thetaX/D" );
  infoTree->Branch( "fishtank_thetaY"       , &fishtank_thetaY       , "fishtank_thetaY/D" );
  infoTree->Branch( "fishtank_phi"          , &fishtank_phi          , "fishtank_phi/D" );
  infoTree->Branch( "fishtank_fresnel"      , &fishtank_fresnel      , "fishtank_fresnel/O" );
  infoTree->Branch( "fishtankBlack_bottom"  , &fishtankBlack_bottom  , "fishtankBlack_bottom/O" );
  infoTree->Branch( "fishtankBlack_sides"   , &fishtankBlack_sides   , "fishtankBlack_sides/O" );
  infoTree->Branch( "fishtankBlack_top"     , &fishtankBlack_top     , "fishtankBlack_top/O" );
  infoTree->Branch( "slabBlack_front"       , &slabBlack_front       , "slabBlack_front/O" );
  infoTree->Branch( "mirror"                , &mirror                , "mirror/O" );
  infoTree->Branch( "photon_number"         , &photon_number         , "photon_number/I" );
  infoTree->Branch( "lambda_min"            , &lambda_min            , "lambda_min/D" );
  infoTree->Branch( "lambda_max"            , &lambda_max            , "lambda_max/D" );
  infoTree->Branch( "refl_limit"            , &refl_limit            , "refl_limit/I" );
  infoTree->Branch( "particle_mass"         , &mass                  , "particle_mass/D" );
  infoTree->Branch( "particle_kinE"         , &kinE                  , "particle_kinE/D" );
  infoTree->Branch( "particle_mom"          , &mom                   , "particle_mom/D" );
  infoTree->Branch( "particle_beta"         , &beta                  , "particle_beta/D" );
  infoTree->Branch( "particle_dirX"         , &parDirX               , "particle_dirX/D" );
  infoTree->Branch( "particle_dirY"         , &parDirY               , "particle_dirY/D" );
  infoTree->Branch( "particle_dirZ"         , &parDirZ               , "particle_dirZ/D" );
  infoTree->Branch( "incidence_theta"       , &inci_theta            , "incidence_theta/D" );
  infoTree->Branch( "incidence_phi"         , &inci_phi              , "incidence_phi/D" );
  infoTree->Branch( "hitBarX"               , &hitBarX               , "hitBarX/D" );
  infoTree->Branch( "hitBarY"               , &hitBarY               , "hitBarY/D" );
  infoTree->Branch( "hitBarZ"               , &hitBarZ               , "hitBarZ/D" );
  infoTree->Branch( "spot_radius"           , &spot_radius           , "spot_radius/D" );
  infoTree->Branch( "spot_limit"            , &spot_limit            , "spot_limit/D" );
  infoTree->Branch( "particle_number"       , &particle_number       , "particle_number/I" );
  infoTree->Branch( "shoots"                , &shoots                , "shoots/I");
  infoTree->Branch( "gridXstep"             , &gridXstep             , "gridXstep/D");
  infoTree->Branch( "gridYstep"             , &gridYstep             , "gridYstep/D");
  infoTree->Branch( "cannon_theta"          , &cannon_theta          , "cannon_theta/D");
  infoTree->Branch( "cannon_phi"            , &cannon_phi            , "cannon_phi/D");
  infoTree->Branch( "cannon_rotX"           , &cannon_rotX           , "cannon_rotX/D");
  infoTree->Branch( "cannon_rotY"           , &cannon_rotY           , "cannon_rotY/D");
  infoTree->Branch( "cannon_posX"           , &cannon_posX           , "cannon_posX/D");
  infoTree->Branch( "cannon_posY"           , &cannon_posY           , "cannon_posY/D");
  infoTree->Branch( "cannon_posZ"           , &cannon_posZ           , "cannon_posZ/D");
  infoTree->Branch( "pos_size"              , &pos_size_info         , "pos_size/I");

  infoTree->Fill();


  // photonTree
  double wavelength = -666;
  int color = -666;
  double kBarX = -666;
  double kBarY = -666;
  double kBarZ = -666;
  double hitPosX = -666;
  double hitPosY = -666;
  double hitPosZ = -666;
  double hitPosDetX = -666; // fishtank is rotated => hitPos != hitPosDet
  double hitPosDetY = -666;
  double hitPosDetZ = -666;
  double hitDirX = -666;
  double hitDirY = -666;
  double hitDirZ = -666;
  int index_pos = -666;
  int particleID = -666;
  double thetaC = -666;
  double phiC = -666;
  double time = -666;
  int nRefl = -666;
  bool measured = false;
  bool absorbed = false;
  bool lost = false;

  double phot_parDirX    = -666;
  double phot_parDirY    = -666;
  double phot_parDirZ    = -666;
  double phot_inci_theta = -666;
  double phot_inci_phi   = -666;

  //  vector<double> posX; // needed ROOT >= 5.2
  double posX[ pos_size ];
  double posY[ pos_size ];
  double posZ[ pos_size ];
  for( int i = 0; i < pos_size; i++)
  {
    posX[i] = -666;
    posY[i] = -666;
    posZ[i] = -666;
  }

  TString pos_size_str;
  pos_size_str += pos_size;
  pos_size_str.Remove( TString::kLeading, ' ' );
  TString posX_str = "posX[" + pos_size_str + "]";
  TString posY_str = "posY[" + pos_size_str + "]";
  TString posZ_str = "posZ[" + pos_size_str + "]";
  TString posX_str_2 = posX_str + "/D";
  TString posY_str_2 = posY_str + "/D";
  TString posZ_str_2 = posZ_str + "/D";

  photonTree->Branch( "wavelength", &wavelength, "wavelength/D" ); // D: Double_t
  photonTree->Branch( "color"     , &color     , "color/I" ); // I: Int_t ;  definition: see PndDrcPhoton.cxx::ColorNumber
  photonTree->Branch( "kBarX"     , &kBarX     , "kBarX/D" );
  photonTree->Branch( "kBarY"     , &kBarY     , "kBarY/D" );
  photonTree->Branch( "kBarZ"     , &kBarZ     , "kBarZ/D" );
  photonTree->Branch( "hitPosX"   , &hitPosX   , "hitPosX/D" );
  photonTree->Branch( "hitPosY"   , &hitPosY   , "hitPosY/D" );
  photonTree->Branch( "hitPosZ"   , &hitPosZ   , "hitPosZ/D" );
  photonTree->Branch( "hitPosDetX", &hitPosDetX, "hitPosDetX/D" );
  photonTree->Branch( "hitPosDetY", &hitPosDetY, "hitPosDetY/D" );
  photonTree->Branch( "hitPosDetZ", &hitPosDetZ, "hitPosDetZ/D" );
  photonTree->Branch( "hitDirX"   , &hitDirX   , "hitDirX/D" );
  photonTree->Branch( "hitDirY"   , &hitDirY   , "hitDirY/D" );
  photonTree->Branch( "hitDirZ"   , &hitDirZ   , "hitDirZ/D" );
  if( opt_photonPosList )
  {
    photonTree->Branch( posX_str    , posX       , posX_str_2 );
    photonTree->Branch( posY_str    , posY       , posY_str_2 );
    photonTree->Branch( posZ_str    , posZ       , posZ_str_2 );
    photonTree->Branch( "index_pos" , &index_pos , "index_pos/I" );
  }
  if( !opt_photonCannon )
  {
    photonTree->Branch( "particleID", &particleID, "particleID/I" );
    photonTree->Branch( "thetaC"    , &thetaC    , "thetaC/D" );
    photonTree->Branch( "phiC"      , &phiC      , "phiC/D" );
  }
  photonTree->Branch( "measured"  , &measured  , "measured/O" ); // O: Bool_t
  photonTree->Branch( "absorbed"  , &absorbed  , "absorbed/O" );
  photonTree->Branch( "lost"      , &lost      , "lost/O" );
  photonTree->Branch( "time"      , &time      , "time/D" );
  photonTree->Branch( "nRefl"     , &nRefl     , "nRefl/I" );
  if( opt_angleAcceptance )
  {
    photonTree->Branch( "particle_dirX"  , &phot_parDirX   , "particle_dirX/D" );
    photonTree->Branch( "particle_dirY"  , &phot_parDirY   , "particle_dirY/D" );
    photonTree->Branch( "particle_dirZ"  , &phot_parDirZ   , "particle_dirZ/D" );
    photonTree->Branch( "incidence_theta", &phot_inci_theta, "incidence_theta/D" );
    photonTree->Branch( "incidence_phi"  , &phot_inci_phi  , "incidence_phi/D" );
  }


  // particleTree
  double spotX = -666;
  double spotY = -666;
  double spotZ = -666;
  double hitOnBarX = -666;
  double hitOnBarY = -666;
  double hitOnBarZ = -666;
  double spotEndX = -666;
  double spotEndY = -666;
  double spotEndZ = -666;
  double range = -666;

  if( !opt_photonCannon)
  {
    particleTree->Branch( "spotX"     , &spotX    , "spotX/D" );
    particleTree->Branch( "spotY"     , &spotY    , "spotY/D" );
    particleTree->Branch( "spotZ"     , &spotZ    , "spotZ/D" );
    particleTree->Branch( "hitOnBarX" , &hitOnBarX, "hitOnBarX/D" );
    particleTree->Branch( "hitOnBarY" , &hitOnBarY, "hitOnBarY/D" );
    particleTree->Branch( "hitOnBarZ" , &hitOnBarZ, "hitOnBarZ/D" );
    particleTree->Branch( "spotEndX"  , &spotEndX , "spotEndX/D" );
    particleTree->Branch( "spotEndY"  , &spotEndY , "spotEndY/D" );
    particleTree->Branch( "spotEndZ"  , &spotEndZ , "spotEndZ/D" );
    particleTree->Branch( "pathlength", &range    , "pathlength/D" );
  }


// plot declarations
//==============================================================================

  // set some global options
//   gStyle->SetCanvasColor( 0 );        // white
//   gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
//   gStyle->SetFrameFillColor( 0 );
//   gStyle->SetFrameBorderMode( 0 );    // no red frame
//   gStyle->SetHistFillColor( 0 );
//   gStyle->SetPadColor( 0 );
//   gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
//   gStyle->SetTitleFontSize( 0.05 );
//   gStyle->SetPalette( 1 );            // better color palette
//   gStyle->SetStatColor( 0 );          // stat. box color
  gROOT->SetStyle("Plain");

  // use Times-Roman fonts
  gStyle->SetTextFont(132);
  gStyle->SetTextSize(0.07);
  gStyle->SetTitleFont(132,"xyz");
  gStyle->SetTitleSize(0.05,"xyz");
  gStyle->SetLabelFont(132,"xyz");
  gStyle->SetLabelSize(0.04,"xyz");
  gStyle->SetStatFont(132);

  // use bold lines and markers
  gStyle->SetMarkerStyle(20); // full circle
  gStyle->SetMarkerSize(0.4);

  // paletts (2,3D plots)
  gStyle->SetPalette(1,0);
  gStyle->SetNumberContours(50);

  // canvas or pad margins
  gStyle->SetPadLeftMargin(0.11);
  gStyle->SetPadRightMargin(0.11);
  gStyle->SetPadTopMargin(0.11);
  gStyle->SetPadBottomMargin(0.11);

  // stat. options
  gStyle->SetOptStat(1);

  // canvas size in batch mode is strange and fixed
  TCanvas *canvas_beamspot = new TCanvas( "canvas_beamspot", "" , 1 );
  canvas_beamspot->Draw();

  TCanvas *canvas_screen   = new TCanvas( "canvas_screen"  , "" , 1 );
  canvas_screen->Draw();

  TCanvas *canvas_setup    = new TCanvas( "canvas_setup"   , "" , 1 );
  canvas_setup->Draw();


  // beampsot plot
  TString beamspot_title  = "beamspot on bar";
  TString beamspot_titleX;
  TString beamspot_titleY;
  double center;

  if( opt_alongBar )
  {
    beamspot_titleX = "x [mm]";
    beamspot_titleY = "y [mm]";
    center = hitBarX;
  }
  else if( opt_fromBelow )
  {
    beamspot_titleX = "z [mm]";
    beamspot_titleY = "x [mm]";
    center = hitBarZ;
  }
  else
  {
    beamspot_titleX = "z [mm]";
    beamspot_titleY = "y [mm]";
    center = hitBarZ;
  }


  int minX = FloorNint( center - spot_limit/Cos(inci_theta*degree) );
  int maxX = CeilNint( center + spot_limit/Cos(inci_theta*degree) );
  double spot_var = spot_limit;

  if( minX == maxX )
  {
    minX -= 1;
    maxX += 1;
    spot_var = slab_width + slab_height;
  }

  TH1F *beamspot = new TH1F( "beamspot", beamspot_title, 100, minX, maxX );

  beamspot->SetStats( 0 );
  beamspot->SetMinimum(-spot_var);
  beamspot->SetMaximum(+spot_var);
  beamspot->GetXaxis()->SetTitle( beamspot_titleX );
  beamspot->GetXaxis()->CenterTitle();
  beamspot->GetYaxis()->SetTitle( beamspot_titleY );
  beamspot->GetYaxis()->CenterTitle();


  TLine *left   = new TLine(-666,-666,-666,-666);
  TLine *right  = new TLine(-666,-666,-666,-666);
  TLine *top    = new TLine(-666,-666,-666,-666);
  TLine *bottom = new TLine(-666,-666,-666,-666);
  left  ->SetLineWidth( 3 );
  right ->SetLineWidth( 3 );
  top   ->SetLineWidth( 3 );
  bottom->SetLineWidth( 3 );

  double slab_var;
  if( opt_fromBelow )
    slab_var = slab_width;
  else
    slab_var = slab_height;

  if( maxX > 0 && !opt_alongBar)
  {
    left->SetX1(0);
    left->SetY1(-slab_var/2);
    left->SetX2(0);
    left->SetY2(+slab_var/2);

    top->SetX1(minX);
    top->SetY1(-slab_var/2);
    top->SetX2(0);
    top->SetY2(-slab_var/2);

    bottom->SetX1(minX);
    bottom->SetY1(slab_var/2);
    bottom->SetX2(0);
    bottom->SetY2(slab_var/2);
  }
  else if( minX < -slab_length && !opt_alongBar)
  {
    right->SetX1(-slab_length);
    right->SetY1(-slab_var/2);
    right->SetX2(-slab_length);
    right->SetY2(+slab_var/2);

    top->SetX1(-slab_length);
    top->SetY1(-slab_var/2);
    top->SetX2(maxX);
    top->SetY2(-slab_var/2);

    bottom->SetX1(-slab_length);
    bottom->SetY1(slab_var/2);
    bottom->SetX2(maxX);
    bottom->SetY2(slab_var/2);
  }
  else if( !opt_alongBar )
  {
    top->SetX1(minX);
    top->SetY1(-slab_var/2);
    top->SetX2(maxX);
    top->SetY2(-slab_var/2);

    bottom->SetX1(minX);
    bottom->SetY1(slab_var/2);
    bottom->SetX2(maxX);
    bottom->SetY2(slab_var/2);
  }
  if( opt_alongBar )
  {
    left->SetX1(-slab_width/2);
    left->SetY1(-slab_height/2);
    left->SetX2(-slab_width/2);
    left->SetY2(+slab_height/2);

    right->SetX1(+slab_width/2);
    right->SetY1(-slab_height/2);
    right->SetX2(+slab_width/2);
    right->SetY2(+slab_height/2);

    top->SetX1(-slab_width/2);
    top->SetY1(+slab_height/2);
    top->SetX2(+slab_width/2);
    top->SetY2(+slab_height/2);

    bottom->SetX1(-slab_width/2);
    bottom->SetY1(-slab_height/2);
    bottom->SetX2(+slab_width/2);
    bottom->SetY2(-slab_height/2);
  }


  canvas_beamspot->cd();
  beamspot->Draw( "POL" );
  left  ->Draw();
  right ->Draw();
  top   ->Draw();
  bottom->Draw();

  // screen plot
  TString screen_titleX = "x [mm]";
  TString screen_titleY = "y [mm]";

  double screen_width1;
  double screen_width2;

  double screen_height1;
  double screen_height2;

  if( opt_prismIsTank )
  {
    screen_width1 = -prism_width/2 - prism_widthDown2 - prism_widthDown1;
    screen_width2 = +prism_width/2 + prism_widthUp2 + prism_widthUp1;

    screen_height1 = -prism_height/2 - prism_heightDown2 - prism_heightDown1;
    screen_height2 = +prism_height/2 + prism_heightUp2 + prism_heightUp1;
  }
  else
  {
    screen_width1 = -fishtank_width/2 + fishtank_width_offset;
    screen_width2 = +fishtank_width/2  + fishtank_width_offset;

    screen_height1 = -fishtank_height/2 + fishtank_height_offset;
    screen_height2 = +fishtank_height/2  + fishtank_height_offset;
  }

  TH1F *screen = new TH1F( "screen", "", 600, screen_width1, screen_width2 );

  screen->SetStats( 0 );
  screen->SetMinimum( screen_height1 );
  screen->SetMaximum( screen_height2 );
  screen->GetXaxis()->SetTitle( screen_titleX );
  screen->GetXaxis()->CenterTitle();
  screen->GetYaxis()->SetTitle( screen_titleY );
  screen->GetYaxis()->CenterTitle();

  canvas_screen->cd();
  screen->Draw("POL");



//==============================================================================
// Optical device declarations
//==============================================================================

// reflectivity

//******************************************************************************
// Note:
// Fresnel reflection is enable by default (hard-coded)
// currently a fresnelFlag for a certain surface is not implemented yet (only for volumes)
// for non-specular reflection (diffuse) change "diffuseProb" in the method "Refract" (PndDerPhoton.h)
//******************************************************************************

  PndDrcOptReflPerfect refl_perfect;  // reflected (mirror)
  PndDrcOptReflNone    refl_none;     // absorbed (black)

// coordination system for device sketches

//             top
//
//              Y     exit
//              ^    z
//              |   ^
//              |  /
//     left     | /    right
//              |/
//   x <--------x
//
//           bottom

// origin x is centered at the slab end


// quartz bar (slab)
//==============================================================================

//                    exit
//               s8----------s5
//              /|     _    /|      _ (0,0,0) origin
//             / |         / |
//            /  s7-------/--s6
//           /  /        /  /
//          /  /        /  /
//         /  /        /  /
//        /  /        /  /
//      s4----------s1  /
//      |  /        |  /
//      | / front   | /
//      s3----------s2


  XYZPoint s1(-slab_width/2, +slab_height/2, -slab_length);
  XYZPoint s2(-slab_width/2, -slab_height/2, -slab_length);
  XYZPoint s3(+slab_width/2, -slab_height/2, -slab_length);
  XYZPoint s4(+slab_width/2, +slab_height/2, -slab_length);

  XYZPoint s5(-slab_width/2, +slab_height/2, 0);
  XYZPoint s6(-slab_width/2, -slab_height/2, 0);
  XYZPoint s7(+slab_width/2, -slab_height/2, 0);
  XYZPoint s8(+slab_width/2, +slab_height/2, 0);


  // surface creation by AddPoint function only works for closed curves which don't cross itself

  PndDrcSurfPolyFlat slab_exit; // slab end
  slab_exit.AddPoint(s5);
  slab_exit.AddPoint(s6);
  slab_exit.AddPoint(s7);
  slab_exit.AddPoint(s8);
  slab_exit.SetName("slab_exit");

  PndDrcSurfPolyFlat slab_left;
  slab_left.AddPoint(s8);
  slab_left.AddPoint(s7);
  slab_left.AddPoint(s3);
  slab_left.AddPoint(s4);
  slab_left.SetName("slab_left");

  PndDrcSurfPolyFlat slab_right;
  slab_right.AddPoint(s5);
  slab_right.AddPoint(s6);
  slab_right.AddPoint(s2);
  slab_right.AddPoint(s1);
  slab_right.SetName("slab_right");

  PndDrcSurfPolyFlat slab_bottom;
  slab_bottom.AddPoint(s6);
  slab_bottom.AddPoint(s2);
  slab_bottom.AddPoint(s3);
  slab_bottom.AddPoint(s7);
  slab_bottom.SetName("slab_bottom");

  PndDrcSurfPolyFlat slab_top;
  slab_top.AddPoint(s5);
  slab_top.AddPoint(s1);
  slab_top.AddPoint(s4);
  slab_top.AddPoint(s8);
  slab_top.SetName("slab_top");

  PndDrcSurfPolyFlat slab_front;
  slab_front.AddPoint(s1);
  slab_front.AddPoint(s2);
  slab_front.AddPoint(s3);
  slab_front.AddPoint(s4);
  slab_front.SetName("slab_front");


  if( opt_mirror && !opt_backLens )
    slab_front.SetReflectivity(refl_perfect);

  if( opt_slabBlack_front )
    slab_front.SetReflectivity(refl_none);


  PndDrcOptVol slab;
  slab.AddSurface(slab_exit);
  slab.AddSurface(slab_left);
  slab.AddSurface(slab_right);
  slab.AddSurface(slab_bottom);
  slab.AddSurface(slab_top);
  slab.AddSurface(slab_front);
  slab.SetOptMaterial( (*mat_slab) );
  slab.SetName("slab");

  if( opt_noFresnel_slab )
    slab.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: slab check" << endl;
    slab.SetVerbosity(4);
    slab_front.SetVerbosity(4);
    slab_left.SetVerbosity(4);
    slab_right.SetVerbosity(4);
    slab_bottom.SetVerbosity(4);
    slab_top.SetVerbosity(4);
    slab_exit.SetVerbosity(4);
    // 	slab_exit.SetPixel();
  }



// lens (forward)
//==============================================================================
  XYZPoint l0(-frontLens_width/2, +frontLens_height/2, 0);
  XYZPoint l1(-frontLens_width/2, -frontLens_height/2, 0);
  XYZPoint l2(+frontLens_width/2, -frontLens_height/2, 0);
  XYZPoint l3(+frontLens_width/2, +frontLens_height/2, 0);


  PndDrcSurfPolyAsphere frontLens_sphere;
  frontLens_sphere.AddPoint(l0);
  frontLens_sphere.AddPoint(l1);
  frontLens_sphere.AddPoint(l2);
  frontLens_sphere.AddPoint(l3);
  frontLens_sphere.SetRadius(frontLens_radius);
  frontLens_sphere.SetPrintColor(2);
  frontLens_sphere.SetConicalConstant(frontLens_conical);
  frontLens_sphere.SetName("frontLens_sphere");

  double frontLens_dist = frontLens_sphere.CenterPoint().Z(); // in general the frontLens radius
  frontLens_sphere.AddTransform( Transform3D( XYZVector(0,0,-(frontLens_dist)) ) ); // ( dist |  =>  |(


  double frontLensMinThickness = Abs( frontLens_sphere.LimitingPoint(0).Z() );

  if( frontLensMinThickness > frontLens_thickness && opt_frontLens && frontLens_cylindrical == 0 )
  {
    cout << "*** ERROR: frontLens thickness have to be greater than: " << frontLensMinThickness
        << " (currently " << frontLens_thickness << " mm)" << endl;
    abort();
  }

  if( ( frontLens_width < slab_width || frontLens_height < slab_height ) && !opt_prism && opt_frontLens )
  {
    cout << "*** ERROR: frontLens is smaller than bar cross-section" << endl;
    abort();
  }

  double prism_end_width  = prism_width + prism_widthUp1 + prism_widthUp2 + prism_widthDown1 + prism_widthDown2;
  double prism_end_height = prism_height + prism_heightUp1 + prism_heightUp2 + prism_heightDown1 + prism_heightDown2;

  if( ( frontLens_width < prism_end_width || frontLens_height < prism_end_height ) && opt_prism && opt_frontLens )
  {
    cout << "*** ERROR: frontLens is smaller than prism end cross-section" << endl;
    abort();
  }


  XYZPoint q0 = l0 + XYZVector(0, 0, -frontLens_thickness);
  XYZPoint q1 = l1 + XYZVector(0, 0, -frontLens_thickness);
  XYZPoint q2 = l2 + XYZVector(0, 0, -frontLens_thickness);
  XYZPoint q3 = l3 + XYZVector(0, 0, -frontLens_thickness);


  PndDrcSurfPolyFlat frontLens_base;
  frontLens_base.AddPoint(q0);
  frontLens_base.AddPoint(q1);
  frontLens_base.AddPoint(q2);
  frontLens_base.AddPoint(q3);
  frontLens_base.SetPrintColor(2);
  frontLens_base.SetName("frontLens_base");

  PndDrcSurfQuadFlatDiff frontLens_right;
  frontLens_right.AddSurface(frontLens_base, q0,q1);
  frontLens_right.AddSurface(frontLens_sphere, frontLens_sphere.LimitingPoint(0),frontLens_sphere.LimitingPoint(1));
  frontLens_right.SetPrintColor(2);
  frontLens_right.SetName("frontLens_right");

  PndDrcSurfQuadFlatDiff frontLens_bottom;
  frontLens_bottom.AddSurface(frontLens_base, q1,q2);
  frontLens_bottom.AddSurface(frontLens_sphere, frontLens_sphere.LimitingPoint(1),frontLens_sphere.LimitingPoint(2));
  frontLens_bottom.SetPrintColor(2);
  frontLens_bottom.SetName("frontLens_bottom");

  PndDrcSurfQuadFlatDiff frontLens_left;
  frontLens_left.AddSurface(frontLens_base, q2,q3);
  frontLens_left.AddSurface(frontLens_sphere, frontLens_sphere.LimitingPoint(2),frontLens_sphere.LimitingPoint(3));
  frontLens_left.SetPrintColor(2);
  frontLens_left.SetName("frontLens_left");

  PndDrcSurfQuadFlatDiff frontLens_top;
  frontLens_top.AddSurface(frontLens_base, q3,q0);
  frontLens_top.AddSurface(frontLens_sphere, frontLens_sphere.LimitingPoint(3),frontLens_sphere.LimitingPoint(0));
  frontLens_top.SetPrintColor(2);
  frontLens_top.SetName("frontLens_top");


  double frontLens_shift = frontLens_thickness;

  PndDrcOptVol frontLens;
  if( frontLens_cylindrical == 0 )
  {
    frontLens.SetVerbosity(0);
    frontLens.AddSurface(frontLens_base);
    frontLens.AddSurface(frontLens_sphere);
    frontLens.AddSurface(frontLens_left);
    frontLens.AddSurface(frontLens_right);
    frontLens.AddSurface(frontLens_bottom);
    frontLens.AddSurface(frontLens_top);
    frontLens.SetOptMaterial( (*mat_frontLens) );
    frontLens.SetName("frontLens");

    if( opt_prism )
      frontLens_shift = frontLens_thickness + prism_length;

    frontLens.AddTransform( Transform3D( XYZVector(0,0,frontLens_shift) ) ); // |(  =>  (|
  }

  if( opt_Cherenkov_onlyInBar )
    frontLens.SetRadiator(false);

  if( opt_noFresnel_frontLens )
    frontLens.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: frontLens check" << endl;
    frontLens.SetVerbosity(4);
    frontLens_base.SetVerbosity(4);
    frontLens_left.SetVerbosity(4);
    frontLens_right.SetVerbosity(4);
    frontLens_bottom.SetVerbosity(4);
    frontLens_top.SetVerbosity(4);
    frontLens_sphere.SetVerbosity(4);
    //    frontLens_left.SetReflectivity(refl_none);
    //    frontLens_right.SetReflectivity(refl_none);
    //    frontLens_bottom.SetReflectivity(refl_none);
    //    frontLens_top.SetReflectivity(refl_none);
    //    frontLens_sphere.SetReflectivity(refl_none);
    //    frontLens_left.SetPixel();
    //    frontLens_right.SetPixel();
    //    frontLens_bottom.SetPixel();
    //    frontLens_top.SetPixel();
    //    frontLens_sphere.SetPixel();
  }



// lens (forward, cylindrical)
//==============================================================================
  Transform3D rot_XtoY = Transform3D( RotationZ(90*degree) );

  XYZPoint c0 = l0;
  XYZPoint c1 = l1;
  XYZPoint c2 = l2;
  XYZPoint c3 = l3;

  if( frontLens_cylindrical == 2 )
  {
    c0 = rot_XtoY*c0;
    c1 = rot_XtoY*c1;
    c2 = rot_XtoY*c2;
    c3 = rot_XtoY*c3;
  }


  PndDrcSurfPolyCyl frontLensCyl_sphere;
  frontLensCyl_sphere.AddPoint(c0);
  frontLensCyl_sphere.AddPoint(c1);
  frontLensCyl_sphere.AddPoint(c2);
  frontLensCyl_sphere.AddPoint(c3);
  frontLensCyl_sphere.SetRadius(frontLens_radius);
  frontLensCyl_sphere.SetPrintColor(2);
  frontLensCyl_sphere.SetName("frontLens_sphere");


  frontLens_dist = frontLensCyl_sphere.CenterPoint().Z(); // in general the frontLens radius
  frontLensCyl_sphere.AddTransform( Transform3D( XYZVector(0,0,-(frontLens_dist)) ) ); // ( dist |  =>  |(


  frontLensMinThickness = Abs( frontLensCyl_sphere.LimitingPoint(0).Z() );

  if( frontLensMinThickness >= frontLens_thickness && opt_frontLens && frontLens_cylindrical != 0 )
  {
    cout << "*** ERROR: frontLens thickness have to be greater than: " << frontLensMinThickness
        << " (currently " << frontLens_thickness << " mm)" << endl;
    abort();
  }


  q0 = c0 + XYZVector(0, 0, -frontLens_thickness);
  q1 = c1 + XYZVector(0, 0, -frontLens_thickness);
  q2 = c2 + XYZVector(0, 0, -frontLens_thickness);
  q3 = c3 + XYZVector(0, 0, -frontLens_thickness);

  XYZPoint r0 = c0 + XYZVector(0, 0, -frontLensMinThickness);
  XYZPoint r1 = c1 + XYZVector(0, 0, -frontLensMinThickness);
  XYZPoint r2 = c2 + XYZVector(0, 0, -frontLensMinThickness);
  XYZPoint r3 = c3 + XYZVector(0, 0, -frontLensMinThickness);


  PndDrcSurfPolyFlat frontLensCyl_base;
  frontLensCyl_base.AddPoint(q0);
  frontLensCyl_base.AddPoint(q1);
  frontLensCyl_base.AddPoint(q2);
  frontLensCyl_base.AddPoint(q3);
  frontLensCyl_base.SetPrintColor(2);
  frontLensCyl_base.SetName("frontLens_base");

  PndDrcSurfPolyFlat frontLensCyl_right;
  frontLensCyl_right.AddPoint(q0);
  frontLensCyl_right.AddPoint(q1);
  frontLensCyl_right.AddPoint(r1);
  frontLensCyl_right.AddPoint(r0);
  frontLensCyl_right.SetPrintColor(2);
  if( frontLens_cylindrical == 2 )
    frontLensCyl_right.SetName("frontLens_bottom");
  else
    frontLensCyl_right.SetName("frontLens_right");

  PndDrcSurfQuadFlatDiff frontLensCyl_bottom;
  frontLensCyl_bottom.AddSurface(frontLensCyl_base, q1,q2);
  frontLensCyl_bottom.AddSurface(frontLensCyl_sphere,
                                 frontLensCyl_sphere.LimitingPoint(1),frontLensCyl_sphere.LimitingPoint(2));
  frontLensCyl_bottom.SetPrintColor(2);
  if( frontLens_cylindrical == 2 )
    frontLensCyl_bottom.SetName("frontLens_left");
  else
    frontLensCyl_bottom.SetName("frontLens_bottom");

  PndDrcSurfPolyFlat frontLensCyl_left;
  frontLensCyl_left.AddPoint(q2);
  frontLensCyl_left.AddPoint(q3);
  frontLensCyl_left.AddPoint(r3);
  frontLensCyl_left.AddPoint(r2);
  frontLensCyl_left.SetPrintColor(2);
  if( frontLens_cylindrical == 2 )
    frontLensCyl_left.SetName("frontLens_top");
  else
    frontLensCyl_left.SetName("frontLens_left");

  PndDrcSurfQuadFlatDiff frontLensCyl_top;
  frontLensCyl_top.AddSurface(frontLensCyl_base, q3,q0);
  frontLensCyl_top.AddSurface(frontLensCyl_sphere, frontLensCyl_sphere.LimitingPoint(3),frontLensCyl_sphere.LimitingPoint(0));
  frontLensCyl_top.SetPrintColor(2);
  if( frontLens_cylindrical == 2 )
    frontLensCyl_top.SetName("frontLens_right");
  else
    frontLensCyl_top.SetName("frontLens_top");


  if( frontLens_cylindrical != 0 )
  {
    frontLens.SetVerbosity(0);
    frontLens.AddSurface(frontLensCyl_base);
    frontLens.AddSurface(frontLensCyl_sphere);
    frontLens.AddSurface(frontLensCyl_left);
    frontLens.AddSurface(frontLensCyl_right);
    frontLens.AddSurface(frontLensCyl_bottom);
    frontLens.AddSurface(frontLensCyl_top);
    frontLens.SetOptMaterial( (*mat_frontLens) );
    frontLens.SetName("frontLens");

    if( opt_prism )
      frontLens_shift = frontLens_thickness + prism_length;

    frontLens.AddTransform( Transform3D( XYZVector(0,0,frontLens_shift) ) ); // |(  =>  (|
  }

  if( frontLens_cylindrical == 2 )
    frontLens.AddTransform( rot_XtoY.Inverse() );


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: frontLensCyl check" << endl;
    frontLens.SetVerbosity(4);
    frontLensCyl_base.SetVerbosity(4);
    frontLensCyl_left.SetVerbosity(4);
    frontLensCyl_right.SetVerbosity(4);
    frontLensCyl_bottom.SetVerbosity(4);
    frontLensCyl_top.SetVerbosity(4);
    frontLensCyl_sphere.SetVerbosity(4);
    //    frontLensCyl_left.SetReflectivity(refl_none);
    //    frontLensCyl_right.SetReflectivity(refl_none);
    //    frontLensCyl_bottom.SetReflectivity(refl_none);
    //    frontLensCyl_top.SetReflectivity(refl_none);
    //    frontLensCyl_sphere.SetReflectivity(refl_none);
    //    frontLensCyl_left.SetPixel();
    //    frontLensCyl_right.SetPixel();
    //    frontLensCyl_bottom.SetPixel();
    //    frontLensCyl_top.SetPixel();
    //    frontLensCyl_sphere.SetPixel();
  }



// lens (backward)
//==============================================================================
  XYZPoint l4(-backLens_width/2, +backLens_height/2, 0);
  XYZPoint l5(-backLens_width/2, -backLens_height/2, 0);
  XYZPoint l6(+backLens_width/2, -backLens_height/2, 0);
  XYZPoint l7(+backLens_width/2, +backLens_height/2, 0);


  PndDrcSurfPolyAsphere backLens_sphere;
  backLens_sphere.AddPoint(l4);
  backLens_sphere.AddPoint(l5);
  backLens_sphere.AddPoint(l6);
  backLens_sphere.AddPoint(l7);
  backLens_sphere.SetRadius(backLens_radius);
  backLens_sphere.SetPrintColor(2);
  backLens_sphere.SetConicalConstant(backLens_conical);
  backLens_sphere.SetName("backLens_sphere");
  backLens_sphere.SetReflectivity(refl_perfect); // as focused mirror

  double backLens_dist = backLens_sphere.CenterPoint().Z(); // in general the backLens radius
  backLens_sphere.AddTransform( Transform3D( XYZVector(0,0,-(backLens_dist)) ) ); // ( dist |  =>  |(


  double backLensMinThickness = Abs( backLens_sphere.LimitingPoint(0).Z() );

  if( backLensMinThickness > backLens_thickness && opt_backLens )
  {
    cout << "*** ERROR: backLens thickness have to be greater than: " << backLensMinThickness
        << " (currently " << backLens_thickness << " mm)" << endl;
    abort();
  }

  if( ( backLens_width < slab_width || backLens_height < slab_height ) && opt_backLens )
  {
    cout << "*** ERROR: backLens is smaller than bar cross-section" << endl;
    abort();
  }


  XYZPoint q4 = l4 + XYZVector(0, 0, -backLens_thickness);
  XYZPoint q5 = l5 + XYZVector(0, 0, -backLens_thickness);
  XYZPoint q6 = l6 + XYZVector(0, 0, -backLens_thickness);
  XYZPoint q7 = l7 + XYZVector(0, 0, -backLens_thickness);


  PndDrcSurfPolyFlat backLens_base;
  backLens_base.AddPoint(q4);
  backLens_base.AddPoint(q5);
  backLens_base.AddPoint(q6);
  backLens_base.AddPoint(q7);
  backLens_base.SetPrintColor(2);
  backLens_base.SetName("backLens_base");

  PndDrcSurfQuadFlatDiff backLens_right;
  backLens_right.AddSurface(backLens_base, q4,q5);
  backLens_right.AddSurface(backLens_sphere, backLens_sphere.LimitingPoint(0),backLens_sphere.LimitingPoint(1));
  backLens_right.SetPrintColor(2);
  backLens_right.SetName("backLens_right");

  PndDrcSurfQuadFlatDiff backLens_bottom;
  backLens_bottom.AddSurface(backLens_base, q5,q6);
  backLens_bottom.AddSurface(backLens_sphere, backLens_sphere.LimitingPoint(1),backLens_sphere.LimitingPoint(2));
  backLens_bottom.SetPrintColor(2);
  backLens_bottom.SetName("backLens_bottom");

  PndDrcSurfQuadFlatDiff backLens_left;
  backLens_left.AddSurface(backLens_base, q6,q7);
  backLens_left.AddSurface(backLens_sphere, backLens_sphere.LimitingPoint(2),backLens_sphere.LimitingPoint(3));
  backLens_left.SetPrintColor(2);
  backLens_left.SetName("backLens_left");

  PndDrcSurfQuadFlatDiff backLens_top;
  backLens_top.AddSurface(backLens_base, q7,q4);
  backLens_top.AddSurface(backLens_sphere, backLens_sphere.LimitingPoint(3),backLens_sphere.LimitingPoint(0));
  backLens_top.SetPrintColor(2);
  backLens_top.SetName("backLens_top");


  double backLens_shift = backLens_thickness + slab_length;

  PndDrcOptVol backLens;
  if( backLens_cylindrical == 0 )
  {
    backLens.SetVerbosity(0);
    backLens.AddSurface(backLens_base);
    backLens.AddSurface(backLens_sphere);
    backLens.AddSurface(backLens_left);
    backLens.AddSurface(backLens_right);
    backLens.AddSurface(backLens_bottom);
    backLens.AddSurface(backLens_top);
    backLens.SetOptMaterial( (*mat_backLens) );
    backLens.SetName("backLens");

    backLens.AddTransform( Transform3D( RotationY(180.*degree) ) ); // |(  =>  )|

    backLens.AddTransform( Transform3D( XYZVector(0,0,-backLens_shift) ) ); // )|   |  =>  |   |)
  }

  if( opt_Cherenkov_onlyInBar )
    backLens.SetRadiator(false);

  if( opt_noFresnel_backLens )
    backLens.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: backLens check" << endl;
    backLens.SetVerbosity(4);
    backLens_base.SetVerbosity(4);
    backLens_left.SetVerbosity(4);
    backLens_right.SetVerbosity(4);
    backLens_bottom.SetVerbosity(4);
    backLens_top.SetVerbosity(4);
    backLens_sphere.SetVerbosity(4);
    //    backLens_left.SetReflectivity(refl_none);
    //    backLens_right.SetReflectivity(refl_none);
    //    backLens_bottom.SetReflectivity(refl_none);
    //    backLens_top.SetReflectivity(refl_none);
    //    backLens_sphere.SetReflectivity(refl_none);
    //    backLens_left.SetPixel();
    //    backLens_right.SetPixel();
    //    backLens_bottom.SetPixel();
    //    backLens_top.SetPixel();
    //    backLens_sphere.SetPixel();
  }



// lens (backward, cylindrical)
//==============================================================================
  XYZPoint c4 = l4;
  XYZPoint c5 = l5;
  XYZPoint c6 = l6;
  XYZPoint c7 = l7;

  if( frontLens_cylindrical == 2 )
  {
    c4 = rot_XtoY*c4;
    c5 = rot_XtoY*c5;
    c6 = rot_XtoY*c6;
    c7 = rot_XtoY*c7;
  }


  PndDrcSurfPolyCyl backLensCyl_sphere;
  backLensCyl_sphere.AddPoint(c4);
  backLensCyl_sphere.AddPoint(c5);
  backLensCyl_sphere.AddPoint(c6);
  backLensCyl_sphere.AddPoint(c7);
  backLensCyl_sphere.SetRadius(backLens_radius);
  backLensCyl_sphere.SetPrintColor(2);
  backLensCyl_sphere.SetName("backLens_sphere");
  backLensCyl_sphere.SetReflectivity(refl_perfect); // as focused mirror


  backLens_dist = backLensCyl_sphere.CenterPoint().Z(); // in general the backLens radius
  backLensCyl_sphere.AddTransform( Transform3D( XYZVector(0,0,-(backLens_dist)) ) ); // ( dist |  =>  |(


  backLensMinThickness = Abs( backLensCyl_sphere.LimitingPoint(0).Z() );

  if( backLensMinThickness > backLens_thickness && opt_backLens && backLens_cylindrical != 0 )
  {
    cout << "*** ERROR: backLens thickness have to be greater than: " << backLensMinThickness
        << " (currently " << backLens_thickness << " mm)" << endl;
    abort();
  }


  q4 = c4 + XYZVector(0, 0, -backLens_thickness);
  q5 = c5 + XYZVector(0, 0, -backLens_thickness);
  q6 = c6 + XYZVector(0, 0, -backLens_thickness);
  q7 = c7 + XYZVector(0, 0, -backLens_thickness);

  XYZPoint r4 = c4 + XYZVector(0, 0, -backLensMinThickness);
  XYZPoint r5 = c5 + XYZVector(0, 0, -backLensMinThickness);
  XYZPoint r6 = c6 + XYZVector(0, 0, -backLensMinThickness);
  XYZPoint r7 = c7 + XYZVector(0, 0, -backLensMinThickness);


  PndDrcSurfPolyFlat backLensCyl_base;
  backLensCyl_base.AddPoint(q4);
  backLensCyl_base.AddPoint(q5);
  backLensCyl_base.AddPoint(q6);
  backLensCyl_base.AddPoint(q7);
  backLensCyl_base.SetPrintColor(2);
  backLensCyl_base.SetName("backLens_base");

  PndDrcSurfPolyFlat backLensCyl_right;
  backLensCyl_right.AddPoint(q4);
  backLensCyl_right.AddPoint(q5);
  backLensCyl_right.AddPoint(r5);
  backLensCyl_right.AddPoint(r4);
  backLensCyl_right.SetPrintColor(2);
  if( backLens_cylindrical == 2 )
    backLensCyl_right.SetName("backLens_bottom");
  else
    backLensCyl_right.SetName("backLens_right");

  PndDrcSurfQuadFlatDiff backLensCyl_bottom;
  backLensCyl_bottom.AddSurface(backLensCyl_base, q5,q6);
  backLensCyl_bottom.AddSurface(backLensCyl_sphere,
                                 backLensCyl_sphere.LimitingPoint(1),backLensCyl_sphere.LimitingPoint(2));
  backLensCyl_bottom.SetPrintColor(2);
  if( backLens_cylindrical == 2 )
    backLensCyl_bottom.SetName("backLens_left");
  else
    backLensCyl_bottom.SetName("backLens_bottom");

  PndDrcSurfPolyFlat backLensCyl_left;
  backLensCyl_left.AddPoint(q6);
  backLensCyl_left.AddPoint(q7);
  backLensCyl_left.AddPoint(r7);
  backLensCyl_left.AddPoint(r6);
  backLensCyl_left.SetPrintColor(2);
  if( backLens_cylindrical == 2 )
    backLensCyl_left.SetName("backLens_top");
  else
    backLensCyl_left.SetName("backLens_left");

  PndDrcSurfQuadFlatDiff backLensCyl_top;
  backLensCyl_top.AddSurface(backLensCyl_base, q7,q4);
  backLensCyl_top.AddSurface(backLensCyl_sphere, backLensCyl_sphere.LimitingPoint(3),backLensCyl_sphere.LimitingPoint(0));
  backLensCyl_top.SetPrintColor(2);
  if( backLens_cylindrical == 2 )
    backLensCyl_top.SetName("backLens_right");
  else
    backLensCyl_top.SetName("backLens_top");


  if( backLens_cylindrical != 0 )
  {
    backLens.SetVerbosity(0);
    backLens.AddSurface(backLensCyl_base);
    backLens.AddSurface(backLensCyl_sphere);
    backLens.AddSurface(backLensCyl_left);
    backLens.AddSurface(backLensCyl_right);
    backLens.AddSurface(backLensCyl_bottom);
    backLens.AddSurface(backLensCyl_top);
    backLens.SetOptMaterial( (*mat_backLens) );
    backLens.SetName("backLens");

    if( backLens_cylindrical == 2 )
      backLens.AddTransform( rot_XtoY.Inverse() );

    backLens.AddTransform( Transform3D( RotationY(180.*degree) ) ); // |(  =>  )|

    backLens.AddTransform( Transform3D( XYZVector(0,0,-backLens_shift) ) ); // )|   |  =>  |   |)
  }


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: backLensCyl check" << endl;
    backLens.SetVerbosity(4);
    backLensCyl_base.SetVerbosity(4);
    backLensCyl_left.SetVerbosity(4);
    backLensCyl_right.SetVerbosity(4);
    backLensCyl_bottom.SetVerbosity(4);
    backLensCyl_top.SetVerbosity(4);
    backLensCyl_sphere.SetVerbosity(4);
    //    backLensCyl_left.SetReflectivity(refl_none);
    //    backLensCyl_right.SetReflectivity(refl_none);
    //    backLensCyl_bottom.SetReflectivity(refl_none);
    //    backLensCyl_top.SetReflectivity(refl_none);
    //    backLensCyl_sphere.SetReflectivity(refl_none);
    //    backLensCyl_left.SetPixel();
    //    backLensCyl_right.SetPixel();
    //    backLensCyl_bottom.SetPixel();
    //    backLensCyl_top.SetPixel();
    //    backLensCyl_sphere.SetPixel();
  }



// fishtank with oil
//==============================================================================

//                   screen
//               b8----------b5
//              /|          /|
//             / |         / |
//            /  b7-------/--b6
//           /  /        /  /
//          /  /        /  /
//         /  /        /  /
//        /  /        /  /
//      b4----------b1  /
//      |  /        |  /
//      | /         | /
//      b3----------b2
//
//
// fishtank thetX:
//	positive angle: rotation axis b2-b3
//	negative angle: rotation axis b4-b1
//
// fishtank thetaY:
//  positive angle: rotation axis b3-b4
//  negative angle: rotation axis b1-b2


  double fishtank_posZ_front = airgap;
  double fishtank_posZ_exit  = fishtank_length + airgap;

  if( opt_prism )
  {
    fishtank_posZ_front = airgap + prism_length;
    fishtank_posZ_exit  = fishtank_length + airgap + prism_length;
  }


  XYZPoint b1(-fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, fishtank_posZ_front);
  XYZPoint b2(-fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, fishtank_posZ_front);
  XYZPoint b3(+fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, fishtank_posZ_front);
  XYZPoint b4(+fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, fishtank_posZ_front);

  XYZPoint b5(-fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, fishtank_posZ_exit);
  XYZPoint b6(-fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, fishtank_posZ_exit);
  XYZPoint b7(+fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, fishtank_posZ_exit);
  XYZPoint b8(+fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, fishtank_posZ_exit);


  PndDrcSurfPolyFlat fishtank_front;
  fishtank_front.AddPoint(b1);
  fishtank_front.AddPoint(b2);
  fishtank_front.AddPoint(b3);
  fishtank_front.AddPoint(b4);
  fishtank_front.SetName("fishtank_front");

  PndDrcSurfPolyFlat fishtank_left;
  fishtank_left.AddPoint(b3);
  fishtank_left.AddPoint(b4);
  fishtank_left.AddPoint(b8);
  fishtank_left.AddPoint(b7);
  fishtank_left.SetName("fishtank_left");

  PndDrcSurfPolyFlat fishtank_right;
  fishtank_right.AddPoint(b5);
  fishtank_right.AddPoint(b6);
  fishtank_right.AddPoint(b2);
  fishtank_right.AddPoint(b1);
  fishtank_right.SetName("fishtank_right");

  PndDrcSurfPolyFlat fishtank_bottom;
  fishtank_bottom.AddPoint(b2);
  fishtank_bottom.AddPoint(b3);
  fishtank_bottom.AddPoint(b7);
  fishtank_bottom.AddPoint(b6);
  fishtank_bottom.SetName("fishtank_bottom");

  PndDrcSurfPolyFlat fishtank_top;
  fishtank_top.AddPoint(b1);
  fishtank_top.AddPoint(b4);
  fishtank_top.AddPoint(b8);
  fishtank_top.AddPoint(b5);
  fishtank_top.SetName("fishtank_top");

  PndDrcSurfPolyFlat fishtank_screen;
  fishtank_screen.AddPoint(b5);
  fishtank_screen.AddPoint(b6);
  fishtank_screen.AddPoint(b7);
  fishtank_screen.AddPoint(b8);
  fishtank_screen.SetName("fishtank_screen");


  if( opt_fishtankBlack_bottom )
    fishtank_bottom.SetReflectivity(refl_none);

  if ( opt_fishtankBlack_sides )
  {
    fishtank_left.SetReflectivity(refl_none);
    fishtank_right.SetReflectivity(refl_none);
  }

  if( opt_fishtankBlack_top )
    fishtank_top.SetReflectivity(refl_none);

  fishtank_screen.SetPixel(); // detector plane


  PndDrcOptVol fishtank;
  fishtank.SetVerbosity(0);
  fishtank.AddSurface(fishtank_front);
  fishtank.AddSurface(fishtank_left);
  fishtank.AddSurface(fishtank_right);
  fishtank.AddSurface(fishtank_bottom);
  fishtank.AddSurface(fishtank_top);
  fishtank.AddSurface(fishtank_screen);
  fishtank.SetOptMaterial( (*mat_fishtank) );
  fishtank.SetName("fishtank");

  if( opt_Cherenkov_onlyInBar )
    fishtank.SetRadiator(false);



// fishtank rotation
//==============================================================================

// for example both rotation angles theta are positive:
// 1. translation b3
// 2. rotation about y-axis
// 3. rotation about (new) b2
// 4. back translation b3
// 5. translation (new) origin_fishtank_front
// 6. rotation about (new) origin_fishtank_back
// 7. back translation (new) origin_fishtank_front


  XYZPoint origin_fishtank_front(0,0, fishtank_posZ_front);
  XYZPoint origin_fishtank_back(0,0, fishtank_posZ_exit);


  Transform3D transToRotAxisY_fishtank;

  if( fishtank_thetaY >= 0 && fishtank_thetaX >= 0 )
    transToRotAxisY_fishtank = Transform3D( XYZVector(-b3.X(),-b3.Y(),-b3.Z()) );

  if( fishtank_thetaY >= 0 && fishtank_thetaX < 0 )
    transToRotAxisY_fishtank = Transform3D( XYZVector(-b4.X(),-b4.Y(),-b4.Z()) );

  if( fishtank_thetaY < 0 && fishtank_thetaX >= 0 )
    transToRotAxisY_fishtank = Transform3D( XYZVector(-b2.X(),-b2.Y(),-b2.Z()) );

  if( fishtank_thetaY < 0 && fishtank_thetaX < 0 )
    transToRotAxisY_fishtank = Transform3D( XYZVector(-b1.X(),-b1.Y(),-b1.Z()) );


  Transform3D rotThetaY_fishtank = Transform3D( RotationY(fishtank_thetaY*degree) );

  fishtank.AddTransform( transToRotAxisY_fishtank );
  fishtank.AddTransform( rotThetaY_fishtank );

  b1 = transToRotAxisY_fishtank*b1;
  b1 = rotThetaY_fishtank*b1;

  b2 = transToRotAxisY_fishtank*b2;
  b2 = rotThetaY_fishtank*b2;

  b3 = transToRotAxisY_fishtank*b3;
  b3 = rotThetaY_fishtank*b3;

  b4 = transToRotAxisY_fishtank*b4;
  b4 = rotThetaY_fishtank*b4;

  origin_fishtank_front = transToRotAxisY_fishtank*origin_fishtank_front;
  origin_fishtank_front = rotThetaY_fishtank*origin_fishtank_front;

  origin_fishtank_back = transToRotAxisY_fishtank*origin_fishtank_back;
  origin_fishtank_back = rotThetaY_fishtank*origin_fishtank_back;


  XYZVector newX;

  if( fishtank_thetaY >= 0 && fishtank_thetaX >= 0 )
    newX.SetXYZ( -b2.X(), -b2.Y(), -b2.Z() );

  if( fishtank_thetaY >= 0 && fishtank_thetaX < 0 )
    newX.SetXYZ( -b1.X(), -b1.Y(), -b1.Z() );

  if( fishtank_thetaY < 0 && fishtank_thetaX >= 0 )
    newX.SetXYZ( b3.X(), b3.Y(), b3.Z() );

  if( fishtank_thetaY < 0 && fishtank_thetaX < 0 )
    newX.SetXYZ( b4.X(), b4.Y(), b4.Z() );

  newX = newX.Unit();

  TVector3 helper_rotThetaX_fishtank(newX.X(),newX.Y(),newX.Z());
  TRotation r_thetaX;
  r_thetaX.Rotate(fishtank_thetaX*degree, helper_rotThetaX_fishtank);


  Transform3D rotThetaX_fishtank = Transform3D(
      r_thetaX.XX(), r_thetaX.XY(), r_thetaX.XZ(), 0,
  r_thetaX.YX(), r_thetaX.YY(), r_thetaX.YZ(), 0,
  r_thetaX.ZX(), r_thetaX.ZY(), r_thetaX.ZZ(), 0 );

  fishtank.AddTransform( rotThetaX_fishtank );
  fishtank.AddTransform( transToRotAxisY_fishtank.Inverse() );

  b1 = rotThetaX_fishtank*b1;
  b1 = transToRotAxisY_fishtank.Inverse()*b1;

  b2 = rotThetaX_fishtank*b2;
  b2 = transToRotAxisY_fishtank.Inverse()*b2;

  b3 = rotThetaX_fishtank*b3;
  b3 = transToRotAxisY_fishtank.Inverse()*b3;

  b4 = rotThetaX_fishtank*b4;
  b4 = transToRotAxisY_fishtank.Inverse()*b4;

  origin_fishtank_front = rotThetaX_fishtank*origin_fishtank_front;
  origin_fishtank_front = transToRotAxisY_fishtank.Inverse()*origin_fishtank_front;

  origin_fishtank_back = rotThetaX_fishtank*origin_fishtank_back;
  origin_fishtank_back = transToRotAxisY_fishtank.Inverse()*origin_fishtank_back;


  Transform3D transTo_origin_fishtank_front =  Transform3D(
      XYZVector(-origin_fishtank_front.X(),-origin_fishtank_front.Y(),-origin_fishtank_front.Z()) );

  fishtank.AddTransform( transTo_origin_fishtank_front );
  b1 = transTo_origin_fishtank_front*b1;
  b2 = transTo_origin_fishtank_front*b2;
  b3 = transTo_origin_fishtank_front*b3;
  b4 = transTo_origin_fishtank_front*b4;
  origin_fishtank_front = transTo_origin_fishtank_front*origin_fishtank_front;
  origin_fishtank_back = transTo_origin_fishtank_front*origin_fishtank_back;


  XYZVector newZ(origin_fishtank_back.X(), origin_fishtank_back.Y(), origin_fishtank_back.Z() );

  newZ = newZ.Unit();

  TVector3 helper_rotPhi_fishtank(newZ.X(),newZ.Y(),newZ.Z());
  TRotation r_phi;
  r_phi.Rotate(fishtank_phi*degree, helper_rotPhi_fishtank);

  Transform3D rotPhi_fishtank = Transform3D(
      r_phi.XX(), r_phi.XY(), r_phi.XZ(), 0,
  r_phi.YX(), r_phi.YY(), r_phi.YZ(), 0,
  r_phi.ZX(), r_phi.ZY(), r_phi.ZZ(), 0 );

  fishtank.AddTransform( rotPhi_fishtank );
  fishtank.AddTransform( transTo_origin_fishtank_front.Inverse() );

  b1 = rotPhi_fishtank*b1;
  b1 = transTo_origin_fishtank_front.Inverse()*b1;

  b2 = rotPhi_fishtank*b2;
  b2 = transTo_origin_fishtank_front.Inverse()*b2;

  b3 = rotPhi_fishtank*b3;
  b3 = transTo_origin_fishtank_front.Inverse()*b3;

  b4 = rotPhi_fishtank*b4;
  b4 = transTo_origin_fishtank_front.Inverse()*b4;

  origin_fishtank_front = rotPhi_fishtank*origin_fishtank_front;
  origin_fishtank_front = transTo_origin_fishtank_front.Inverse()*origin_fishtank_front;

  origin_fishtank_back = rotPhi_fishtank*origin_fishtank_back;
  origin_fishtank_back = transTo_origin_fishtank_front.Inverse()*origin_fishtank_back;


  if( opt_noFresnel_fishtank )
    fishtank.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: fishtank check" << endl;
    fishtank.SetVerbosity(4);
    fishtank_front.SetVerbosity(4);
    fishtank_left.SetVerbosity(4);
    fishtank_right.SetVerbosity(4);
    fishtank_bottom.SetVerbosity(4);
    fishtank_top.SetVerbosity(4);
    fishtank_screen.SetVerbosity(4);
    // 		fishtank_front.SetPixel();
  }



// air box
//==============================================================================

//               b4----------b1
//              /|          /|
//             / |         / |
//            /  b3-------/--b2
//           /  /        /  /
//          /  /        /  /
//         /  /        /  /
//        /  /        /  /
//      a4----------a1  /
//      |  /  _     |  /      _ (0,0,0) origin (w/o prism)
//      | /         | /
//      a3----------a2


  double aZ = 0;
  if( opt_prism )
    aZ = prism_length;

  XYZPoint a1(b1.X(), b1.Y(), aZ);
  XYZPoint a2(b2.X(), b2.Y(), aZ);
  XYZPoint a3(b3.X(), b3.Y(), aZ);
  XYZPoint a4(b4.X(), b4.Y(), aZ);


  if( ( fishtank_thetaX != 0 || fishtank_thetaY != 0 ) && !opt_frontLens && airgap == 0 && !opt_prismIsTank  )
  {
    cout << "*** ERROR: w/o forward lens only fishtank rotation in z-direction is allowed" << endl;
    abort();
  }


  PndDrcSurfPolyFlat airBox_front;
  airBox_front.AddPoint(a1);
  airBox_front.AddPoint(a2);
  airBox_front.AddPoint(a3);
  airBox_front.AddPoint(a4);
  airBox_front.SetName("airBox_front");
  airBox_front.SetPrintColor(0);

  PndDrcSurfPolyFlat airBox_bottom;
  airBox_bottom.AddPoint(a2);
  airBox_bottom.AddPoint(a3);
  airBox_bottom.AddPoint(b3);
  airBox_bottom.AddPoint(b2);
  airBox_bottom.SetName("airBox_bottom");
  airBox_bottom.SetPrintColor(0);

  PndDrcSurfPolyFlat airBox_left;
  airBox_left.AddPoint(a3);
  airBox_left.AddPoint(a4);
  airBox_left.AddPoint(b4);
  airBox_left.AddPoint(b3);
  airBox_left.SetName("airBox_left");
  airBox_left.SetPrintColor(0);

  PndDrcSurfPolyFlat airBox_top;
  airBox_top.AddPoint(a1);
  airBox_top.AddPoint(a4);
  airBox_top.AddPoint(b4);
  airBox_top.AddPoint(b1);
  airBox_top.SetName("airBox_top");
  airBox_top.SetPrintColor(0);

  PndDrcSurfPolyFlat airBox_exit;
  airBox_exit.AddPoint(b1);
  airBox_exit.AddPoint(b2);
  airBox_exit.AddPoint(b3);
  airBox_exit.AddPoint(b4);
  airBox_exit.SetName("airBox_exit");
  airBox_exit.SetPrintColor(0);

  PndDrcSurfPolyFlat airBox_right;
  airBox_right.AddPoint(b1);
  airBox_right.AddPoint(b2);
  airBox_right.AddPoint(a2);
  airBox_right.AddPoint(a1);
  airBox_right.SetName("airBox_right");
  airBox_right.SetPrintColor(0);


  //     airBox_left.SetReflectivity(refl_none);
  //     airBox_right.SetReflectivity(refl_none);
  //     airBox_bottom.SetReflectivity(refl_none);
  //     airBox_top.SetReflectivity(refl_none);


  PndDrcOptVol airBox;
  airBox.SetVerbosity(0);
  airBox.AddSurface(airBox_front);
  airBox.AddSurface(airBox_bottom);
  airBox.AddSurface(airBox_left);
  airBox.AddSurface(airBox_top);
  airBox.AddSurface(airBox_exit);
  airBox.AddSurface(airBox_right);
  airBox.SetOptMaterial( (*mat_airBox) );
  airBox.SetName("airBox");


  if( opt_noFresnel_airBox )
    airBox.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: airBox check" << endl;
    airBox.SetVerbosity(4);
    airBox_front.SetVerbosity(4);
    airBox_left.SetVerbosity(4);
    airBox_right.SetVerbosity(4);
    airBox_bottom.SetVerbosity(4);
    airBox_top.SetVerbosity(4);
    airBox_exit.SetVerbosity(4);
    // 		airBox_exit.SetPixel();
  }



// air box with hole for air lens
//==============================================================================

//              b4----------b1
//             /|          / |
//            / |         /  |
//           /  |        /   |
//          /   |       /    |
//         /    |      /     |
//        /     b3----/------b2
//       /     /     /       /
//      a4----------a1      /
//      | \        /|      /
//      |  \      / |     /
//      |   l3--l0  |    /
//      |   | H |   |   /       H: lens-hole
//      |   l2--l1  |  /
//      |  /      \ | /
//      | /        \|/
//      a3----------a2


  double maxXY;
  if( frontLens_height > frontLens_width )
    maxXY= frontLens_height/2;
  else
    maxXY= frontLens_width/2;

  if( (maxXY > Abs(b1.X()) || maxXY > Abs(b1.Y()) || b1.Z() < 0 || maxXY > Abs(b2.X()) || maxXY > Abs(b2.Y()) || b2.Z() < 0 ||
       maxXY > Abs(b3.X()) || maxXY > Abs(b3.Y()) || b3.Z() < 0 || maxXY > Abs(b4.X()) || maxXY > Abs(b2.Y()) || b4.Z() < 0 )
       && opt_frontLens  )
  {
    cout << "*** ERROR: fishtank rotation angle is too big (due to air box/lens construction)" << endl;
    abort();
  }

  if( opt_prism )
  {
    l0 = l0 + XYZVector(0, 0, prism_length);
    l1 = l1 + XYZVector(0, 0, prism_length);
    l2 = l2 + XYZVector(0, 0, prism_length);
    l3 = l3 + XYZVector(0, 0, prism_length);
  }


  PndDrcSurfPolyFlat airLens_frontLeft;
  airLens_frontLeft.AddPoint(l3);
  airLens_frontLeft.AddPoint(l2);
  airLens_frontLeft.AddPoint(a3);
  airLens_frontLeft.AddPoint(a4);
  //     airLens_frontLeft.SetReflectivity(refl_none);
  airLens_frontLeft.SetName("airLens_frontLeft");
  airLens_frontLeft.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_frontRight;
  airLens_frontRight.AddPoint(a1);
  airLens_frontRight.AddPoint(a2);
  airLens_frontRight.AddPoint(l1);
  airLens_frontRight.AddPoint(l0);
  //     airLens_frontRight.SetReflectivity(refl_none);
  airLens_frontRight.SetName("airLens_frontRight");
  airLens_frontRight.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_frontBottom;
  airLens_frontBottom.AddPoint(l2);
  airLens_frontBottom.AddPoint(l1);
  airLens_frontBottom.AddPoint(a2);
  airLens_frontBottom.AddPoint(a3);
  //     airLens_frontBottom.SetReflectivity(refl_none);
  airLens_frontBottom.SetName("airLens_frontBottom");
  airLens_frontBottom.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_frontTop;
  airLens_frontTop.AddPoint(l0);
  airLens_frontTop.AddPoint(l3);
  airLens_frontTop.AddPoint(a4);
  airLens_frontTop.AddPoint(a1);
  //     airLens_frontTop.SetReflectivity(refl_none);
  airLens_frontTop.SetName("airLens_frontTop");
  airLens_frontTop.SetPrintColor(0);


  frontLens_sphere.AddTransform( Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLens_left.AddTransform(   Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLens_right.AddTransform(  Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLens_bottom.AddTransform( Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLens_top.AddTransform(    Transform3D(XYZVector(0, 0, frontLens_shift)) );

  PndDrcSurfPolyAsphere  airLens_lens_sphere 	= frontLens_sphere;
  PndDrcSurfQuadFlatDiff airLens_lens_left 	  = frontLens_left;
  PndDrcSurfQuadFlatDiff airLens_lens_right 	= frontLens_right;
  PndDrcSurfQuadFlatDiff airLens_lens_bottom 	= frontLens_bottom;
  PndDrcSurfQuadFlatDiff airLens_lens_top 	  = frontLens_top;

  airLens_lens_sphere.SetName("airLens_lens_sphere");
  airLens_lens_left.SetName(  "airLens_lens_left"  );
  airLens_lens_right.SetName( "airLens_lens_right" );
  airLens_lens_bottom.SetName("airLens_lens_bottom");
  airLens_lens_top.SetName(   "airLens_lens_top"   );


  frontLensCyl_sphere.AddTransform( Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLensCyl_left.AddTransform(   Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLensCyl_right.AddTransform(  Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLensCyl_bottom.AddTransform( Transform3D(XYZVector(0, 0, frontLens_shift)) );
  frontLensCyl_top.AddTransform(    Transform3D(XYZVector(0, 0, frontLens_shift)) );

  PndDrcSurfPolyCyl      airLens_lensCyl_sphere  = frontLensCyl_sphere;
  PndDrcSurfPolyFlat     airLens_lensCyl_left    = frontLensCyl_left;
  PndDrcSurfPolyFlat     airLens_lensCyl_right   = frontLensCyl_right;
  PndDrcSurfQuadFlatDiff airLens_lensCyl_bottom  = frontLensCyl_bottom;
  PndDrcSurfQuadFlatDiff airLens_lensCyl_top     = frontLensCyl_top;

  if( frontLens_cylindrical == 2 )
  {
    airLens_lensCyl_sphere.AddTransform( rot_XtoY.Inverse() );
    airLens_lensCyl_left.AddTransform( rot_XtoY.Inverse() );
    airLens_lensCyl_right.AddTransform( rot_XtoY.Inverse() );
    airLens_lensCyl_bottom.AddTransform( rot_XtoY.Inverse() );
    airLens_lensCyl_top.AddTransform( rot_XtoY.Inverse() );
  }

  airLens_lensCyl_sphere.SetName("airLens_lens_sphere");
  airLens_lensCyl_left.SetName(  "airLens_lens_left"  );
  airLens_lensCyl_right.SetName( "airLens_lens_right" );
  airLens_lensCyl_bottom.SetName("airLens_lens_bottom");
  airLens_lensCyl_top.SetName(   "airLens_lens_top"   );

  if( frontLens_cylindrical == 2 )
  {
    airLens_lensCyl_sphere.SetName("airLens_lens_sphere");
    airLens_lensCyl_left.SetName(  "airLens_lens_top"  );
    airLens_lensCyl_right.SetName( "airLens_lens_bottom" );
    airLens_lensCyl_bottom.SetName("airLens_lens_left");
    airLens_lensCyl_top.SetName(   "airLens_lens_right"   );
  }


  PndDrcSurfPolyFlat airLens_left;
  airLens_left.AddPoint(a3);
  airLens_left.AddPoint(a4);
  airLens_left.AddPoint(b4);
  airLens_left.AddPoint(b3);
  //     airLens_left.SetReflectivity(refl_none);
  airLens_left.SetName("airLens_left");
  airLens_left.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_right;
  airLens_right.AddPoint(a2);
  airLens_right.AddPoint(a1);
  airLens_right.AddPoint(b1);
  airLens_right.AddPoint(b2);
  //     airLens_right.SetReflectivity(refl_none);
  airLens_right.SetName("airLens_right");
  airLens_right.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_bottom;
  airLens_bottom.AddPoint(a2);
  airLens_bottom.AddPoint(a3);
  airLens_bottom.AddPoint(b3);
  airLens_bottom.AddPoint(b2);
  //     airLens_bottom.SetReflectivity(refl_none);
  airLens_bottom.SetName("airLens_bottom");
  airLens_bottom.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_top;
  airLens_top.AddPoint(a1);
  airLens_top.AddPoint(a4);
  airLens_top.AddPoint(b4);
  airLens_top.AddPoint(b1);
  //     airLens_top.SetReflectivity(refl_none);
  airLens_top.SetName("airLens_top");
  airLens_top.SetPrintColor(0);

  PndDrcSurfPolyFlat airLens_exit;
  airLens_exit.AddPoint(b1);
  airLens_exit.AddPoint(b2);
  airLens_exit.AddPoint(b3);
  airLens_exit.AddPoint(b4);
  airLens_exit.SetName("airLens_exit");
  airLens_exit.SetPrintColor(0);


  PndDrcOptVol airLens;
  airLens.SetVerbosity(0);
  airLens.AddSurface(airLens_frontLeft);
  airLens.AddSurface(airLens_frontRight);
  airLens.AddSurface(airLens_frontBottom);
  airLens.AddSurface(airLens_frontTop);

  if( frontLens_cylindrical == 0 )
  {
    airLens.AddSurface(airLens_lens_sphere);
    airLens.AddSurface(airLens_lens_left);
    airLens.AddSurface(airLens_lens_right);
    airLens.AddSurface(airLens_lens_bottom);
    airLens.AddSurface(airLens_lens_top);
  }
  else
  {
    airLens.AddSurface(airLens_lensCyl_sphere);
    airLens.AddSurface(airLens_lensCyl_left);
    airLens.AddSurface(airLens_lensCyl_right);
    airLens.AddSurface(airLens_lensCyl_bottom);
    airLens.AddSurface(airLens_lensCyl_top);
  }

  airLens.AddSurface(airLens_left);
  airLens.AddSurface(airLens_right);
  airLens.AddSurface(airLens_bottom);
  airLens.AddSurface(airLens_top);
  airLens.AddSurface(airLens_exit);
  airLens.SetOptMaterial( (*mat_airBox) );
  airLens.SetName("airLens");


  if( opt_noFresnel_airBox )
    airLens.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: airLens check" << endl;
    airLens.SetVerbosity(4);
    airLens_frontLeft.SetVerbosity(4);
    airLens_frontRight.SetVerbosity(4);
    airLens_frontBottom.SetVerbosity(4);
    airLens_frontTop.SetVerbosity(4);
    airLens_lens_sphere.SetVerbosity(4);
    airLens_lens_left.SetVerbosity(4);
    airLens_lens_right.SetVerbosity(4);
    airLens_lens_bottom.SetVerbosity(4);
    airLens_lens_top.SetVerbosity(4);
    airLens_lensCyl_sphere.SetVerbosity(4);
    airLens_lensCyl_left.SetVerbosity(4);
    airLens_lensCyl_right.SetVerbosity(4);
    airLens_lensCyl_bottom.SetVerbosity(4);
    airLens_lensCyl_top.SetVerbosity(4);
    airLens_left.SetVerbosity(4);
    airLens_right.SetVerbosity(4);
    airLens_bottom.SetVerbosity(4);
    airLens_top.SetVerbosity(4);
    airLens_exit.SetVerbosity(4);
    // 		airLens_exit.SetPixel();
  }



// prism
//==============================================================================
//
//                      /|
//                     / |
//                    /  |
//                   /   |<-heightUp2
//             ____ /____|____
//      heightUp1->|     |
//   ______________|     |____
//                 |     |
//        slab     |prism|<-height
//   ______________|     |____
//    heightDown1->|     |
//             ____|_____|____
//                  \    |
//                   \   |<-heightDown2
//                    \  |
//                     \ |
//                      \|
//
//                 |--L--|
//
// same for the width
//
//
//               p8----------p5
//              /|          /|
//             / |         / |
//            /  p7-------/--p6
//           /  /        /  /
//          /  /        /  /
//         /  /        /  /
//        /  /        /  /
//      p4----------p1  /
//      |  /  _     |  /    _ (0,0,0) origin
//      | /         | /
//      p3----------p2


  XYZPoint p1(-prism_width/2 - prism_widthDown1, +prism_height/2 + prism_heightUp1,   0);
  XYZPoint p2(-prism_width/2 - prism_widthDown1, -prism_height/2 - prism_heightDown1, 0);
  XYZPoint p3(+prism_width/2 + prism_widthUp1  , -prism_height/2 - prism_heightDown1, 0);
  XYZPoint p4(+prism_width/2 + prism_widthUp1  , +prism_height/2 + prism_heightUp1,   0);

  double prism_end_heightUp   = prism_height/2 + prism_heightUp1   + prism_heightUp2;
  double prism_end_heightDown = prism_height/2 + prism_heightDown1 + prism_heightDown2;
  double prism_end_widthUp    = prism_width/2  + prism_widthUp1    + prism_widthUp2;
  double prism_end_widthDown  = prism_width/2  + prism_widthDown1  + prism_widthDown2;

  XYZPoint p5(-prism_end_widthDown, +prism_end_heightUp  , prism_length);
  XYZPoint p6(-prism_end_widthDown, -prism_end_heightDown, prism_length);
  XYZPoint p7(+prism_end_widthUp  , -prism_end_heightDown, prism_length);
  XYZPoint p8(+prism_end_widthUp  , +prism_end_heightUp  , prism_length);


  PndDrcSurfPolyFlat prism_exit; // prism end
  prism_exit.AddPoint(p5);
  prism_exit.AddPoint(p6);
  prism_exit.AddPoint(p7);
  prism_exit.AddPoint(p8);
  prism_exit.SetName("prism_exit");

  PndDrcSurfPolyFlat prism_left;
  prism_left.AddPoint(p8);
  prism_left.AddPoint(p7);
  prism_left.AddPoint(p3);
  prism_left.AddPoint(p4);
  prism_left.SetName("prism_left");

  PndDrcSurfPolyFlat prism_right;
  prism_right.AddPoint(p5);
  prism_right.AddPoint(p6);
  prism_right.AddPoint(p2);
  prism_right.AddPoint(p1);
  prism_right.SetName("prism_right");

  PndDrcSurfPolyFlat prism_bottom;
  prism_bottom.AddPoint(p6);
  prism_bottom.AddPoint(p2);
  prism_bottom.AddPoint(p3);
  prism_bottom.AddPoint(p7);
  prism_bottom.SetName("prism_bottom");

  PndDrcSurfPolyFlat prism_top;
  prism_top.AddPoint(p5);
  prism_top.AddPoint(p1);
  prism_top.AddPoint(p4);
  prism_top.AddPoint(p8);
  prism_top.SetName("prism_top");

  PndDrcSurfPolyFlat prism_front;
  prism_front.AddPoint(p1);
  prism_front.AddPoint(p2);
  prism_front.AddPoint(p3);
  prism_front.AddPoint(p4);
  prism_front.SetName("prism_front");


  if( opt_fishtankBlack_bottom && opt_prismIsTank )
    prism_bottom.SetReflectivity(refl_none);

  if ( opt_fishtankBlack_sides && opt_prismIsTank )
  {
    prism_left.SetReflectivity(refl_none);
    prism_right.SetReflectivity(refl_none);
  }

  if( opt_fishtankBlack_top && opt_prismIsTank )
    prism_top.SetReflectivity(refl_none);

  if( opt_prismIsTank )
    prism_exit.SetPixel(); // detector plane


  PndDrcOptVol prism;
  prism.AddSurface(prism_exit);
  prism.AddSurface(prism_left);
  prism.AddSurface(prism_right);
  prism.AddSurface(prism_bottom);
  prism.AddSurface(prism_top);
  prism.AddSurface(prism_front);
  prism.SetOptMaterial( (*mat_prism) );
  prism.SetName("prism");

  if( opt_noFresnel_prism )
    prism.SetFresnel(false);


  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: prism check" << endl;
    prism.SetVerbosity(4);
    prism_front.SetVerbosity(4);
    prism_left.SetVerbosity(4);
    prism_right.SetVerbosity(4);
    prism_bottom.SetVerbosity(4);
    prism_top.SetVerbosity(4);
    prism_exit.SetVerbosity(4);
    //  prism_exit.SetPixel();
  }



// connect optical devices
//==============================================================================
  PndDrcOptDevSys opt_system;
  opt_system.SetNameCopyNumber("opt_system");
  opt_system.SetVerbosity(0);

  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: PndDrcOptDevSys" << endl;
    opt_system.SetVerbosity(4);
  }

  opt_system.AddDevice(slab);

  if( !opt_prismIsTank )
    opt_system.AddDevice(fishtank);

  if( opt_backLens )
  {
    opt_system.AddDevice(backLens);
    opt_system.CoupleDevice("backLens","slab","backLens_base","slab_front");
  }

  if( opt_prism )
  {
    opt_system.AddDevice(prism);
    opt_system.CoupleDevice("slab","prism","slab_exit","prism_front");
  }

  if( opt_frontLens )
  {
    opt_system.AddDevice(frontLens);

    if( opt_prism )
      opt_system.CoupleDevice("prism","frontLens","prism_exit","frontLens_base");
    else
      opt_system.CoupleDevice("slab","frontLens","slab_exit","frontLens_base");
  }

  if( airgap > 0 )
  {
    if( opt_frontLens )
    {
      opt_system.AddDevice(airLens);
      opt_system.CoupleDevice("frontLens","airLens","frontLens_left"  ,"airLens_lens_left");
      opt_system.CoupleDevice("frontLens","airLens","frontLens_right" ,"airLens_lens_right");
      opt_system.CoupleDevice("frontLens","airLens","frontLens_bottom","airLens_lens_bottom");
      opt_system.CoupleDevice("frontLens","airLens","frontLens_top"   ,"airLens_lens_top");
      opt_system.CoupleDevice("frontLens","airLens","frontLens_sphere","airLens_lens_sphere");
      opt_system.CoupleDevice("airLens","fishtank","airLens_exit","fishtank_front");
    }
    else
    {
      opt_system.AddDevice(airBox);
      if( opt_prism )
        opt_system.CoupleDevice("prism","airBox","prism_exit","airBox_front");
      else
        opt_system.CoupleDevice("slab","airBox","slab_exit","airBox_front");

      opt_system.CoupleDevice("airBox","fishtank","airBox_exit","fishtank_front");
    }
  }
  else
  {
    if( !opt_prismIsTank )
    {
      if( opt_prism )
        opt_system.CoupleDevice("prism","fishtank","prism_exit","fishtank_front");
      else
        opt_system.CoupleDevice("slab","fishtank","slab_exit","fishtank_front");
    }
  }


  PndDrcOptDevManager* manager = new PndDrcOptDevManager();
  manager->AddDeviceSystem(opt_system);

  if( opt_debug )
  {
    cout << "+++++ DEBUG INFO: PndDrcManager" << endl;
    manager->SetVerbosity(4);
  }


// setup plot
//==============================================================================
  canvas_setup->cd();
  fstream geo;
  geo.open("geo.tmp",std::ios::out);
  manager->Print(geo); //draw setup also in canvas
  geo.close();
  canvas_setup->Write("Setup");
  canvas_setup->Clear();


//==============================================================================
// Photon propagation
//==============================================================================

// 1. simulation for beamtest_2009 with or w/o lens and beamtest_2008
// 2. photon cannon

  int icnt_measured = 0;
  int icnt_flying   = 0;
  int icnt_lost     = 0;
  int icnt_absorbed = 0;

  int n_iph = 0; // for debugging


// beamtest simulation
//==============================================================================
  if( opt_default || opt_angleAcceptance )
  {
    TRandom3 randAngles;
    double numberAngles = 1;

    if( opt_angleAcceptance )
      numberAngles = 1000;


    for( int k = 0; k < numberAngles; k++ )
    {
      double costheta = randAngles.Uniform(0.0, 1.0);
      double phi = randAngles.Uniform(0.0, 2*pi);

      if( opt_angleAcceptance )
      {
        if( k%10 == 0 )
          cout << "angle #" << k+1 << " of " << numberAngles << endl;

        if( opt_alongBar )
        {
          parDirX = Tan(ACos(costheta));
          parDirY = 0;
          parDirZ = 1;

          parDir.SetXYZ( parDirX, parDirY, parDirZ );

          Transform3D rot_phi = Transform3D( RotationZ(-phi) );
          parDir = rot_phi*parDir;
        }
        else if( opt_fromBelow )
        {
          parDirX = Tan(ACos(costheta));
          parDirY = 1;
          parDirZ = 0;

          parDir.SetXYZ( parDirX, parDirY, parDirZ );

          Transform3D rot_phi = Transform3D( RotationY(-phi) );
          parDir = rot_phi*parDir;
        }
        else
        {
          parDirX = -1;
          parDirY = 0;
          parDirZ = Tan(ACos(costheta));

          parDir.SetXYZ( parDirX, parDirY, parDirZ );

          Transform3D rot_phi = Transform3D( RotationX(phi) );
          parDir = rot_phi*parDir;
        }

        parDir = parDir.Unit();
        parDirX = parDir.X();
        parDirY = parDir.Y();
        parDirZ = parDir.Z();
      }


      double parOriginX;
      double parOriginY;
      double parOriginZ;

      if( opt_alongBar )
      {
        parOriginX = hitBarX - parDirX/Abs(parDirZ) * spot_limit; // move exterior beam spot particle in bar to outside
        parOriginY = hitBarY - parDirY/Abs(parDirZ) * spot_limit;
        parOriginZ = hitBarZ - parDirZ/Abs(parDirZ) * spot_limit;
      }
      else if( opt_fromBelow )
      {
        parOriginX = hitBarX - parDirX/Abs(parDirY) * spot_limit; // move exterior beam spot particle in bar to outside
        parOriginY = hitBarY - parDirY/Abs(parDirY) * spot_limit;
        parOriginZ = hitBarZ - parDirZ/Abs(parDirY) * spot_limit;
      }
      else
      {
        parOriginX = hitBarX - parDirX/Abs(parDirX) * spot_limit; // move exterior beam spot particle in bar to outside
        parOriginY = hitBarY - parDirY/Abs(parDirX) * spot_limit;
        parOriginZ = hitBarZ - parDirZ/Abs(parDirX) * spot_limit;
      }

      TVector3 z = TVector3(0,0,1);
      TVector3 ortho;
      ortho = z.Orthogonal();
      TRandom3 rand;
      TVector3 helper;
      TVector3 helper2;
      double gausSmear; // beam spot is gaus smeared
      double transX = parOriginX; // for the translation-(shift) (see above)
      double transY = parOriginY;
      double transZ = parOriginZ;
      XYZPoint spotPos;


      if( opt_debug )
        cout << "+++++ DEBUG INFO: photon check" << endl;


      for(int i=0; i < particle_number; i++)
      {
        int particleNumber = i+1;
        if( i%10 == 0 && !opt_angleAcceptance )
          cout << "particle #" << particleNumber << " of " << particle_number << "  with " << photon_number << " per particle" << endl;


        helper=ortho; // (-1,0,0) is the orthogonal of (0,0,1)
        helper.RotateZ(rand.Rndm()*2*pi);
        gausSmear=rand.Gaus(0,spot_radius);
        if( Abs(gausSmear) > spot_limit ) // beam spot limit
          continue;
        helper2 = TVector3(helper.X()*gausSmear, helper.Y()*gausSmear, helper.Z()*gausSmear);

        if( opt_alongBar )
        {
          helper2.RotateY( inci_theta*degree );
          helper2.RotateZ( -inci_phi*degree );
        }
        else if( opt_fromBelow )
        {
          helper2.RotateX( 90*degree );
          helper2.RotateZ( -inci_theta*degree );
          helper2.RotateY( -inci_phi*degree );
        }
        else
        {
          helper2.RotateY( (-90+inci_theta)*degree );
          helper2.RotateX( inci_phi*degree );
        }

        spotX = helper2.X()+transX; // translation-(shift)
        spotY = helper2.Y()+transY;
        spotZ = helper2.Z()+transZ;
        spotPos = XYZVector(spotX,spotY,spotZ);


        double stepsToBar;
        if( opt_alongBar )
          stepsToBar = (-spotZ - slab_length) / parDirZ; // spotZ is negative
        else if( opt_fromBelow )
          stepsToBar = (-spotY - slab_height/2) / parDirY; // spotY is negative
        else
          stepsToBar = (spotX - slab_width/2) / Abs(parDirX);

        hitOnBarX = spotX + stepsToBar * parDirX;
        hitOnBarY = spotY + stepsToBar * parDirY;
        hitOnBarZ = spotZ + stepsToBar * parDirZ;


        if( ( !opt_alongBar && !opt_fromBelow && spotX < (slab_width / 2) )
               || ( opt_fromBelow && spotY > (-slab_height / 2) )
               || ( opt_alongBar && spotZ > -slab_length) ) // should never happen
        {
          cout << "*** WARNING: particle origin production is in the bar (should never happen)" << endl;
          cout << "start pos: (" << spotX << ", " << spotY << ", " << spotZ << ")" << endl;
          cout << "hit pos  : (" << hitOnBarX  << ", " << hitOnBarY  << ", " << hitOnBarZ  << ")" << endl << endl;
          continue;
        }


        double maxSetupLength;
        if( opt_prism && !opt_prismIsTank )
          maxSetupLength = Sqrt( Power(slab_length + airgap + prism_length + fishtank_length + spot_limit, 2)
              + Power( fishtank_width + fishtank_height, 2) );
        else if ( opt_prismIsTank )
          maxSetupLength = Sqrt( Power(slab_length + airgap + prism_length + spot_limit, 2)
              + Power( prism_height + prism_heightUp1 + prism_heightUp2 + prism_heightDown1 + prism_heightDown2
                  + prism_width + prism_widthUp1 + prism_widthUp2 + prism_widthDown1 + prism_widthDown2, 2) );
        else
          maxSetupLength = Sqrt( Power(slab_length + airgap + fishtank_length + spot_limit, 2)
              + Power( fishtank_width + fishtank_height, 2) );
        double stepsTo = maxSetupLength / Abs(parDirZ);

        if( stepsTo > maxSetupLength )
          stepsTo = maxSetupLength;

        spotEndX = spotX + stepsTo * parDirX;
        spotEndY = spotY + stepsTo * parDirY;
        spotEndZ = spotZ + stepsTo * parDirZ;
        TVector3 spotEnd(spotEndX-spotX, spotEndY-spotY, spotEndZ-spotZ);

        range = spotEnd.Mag();


        if( opt_debug )
        {
          cout << "+++++ DEBUG INFO: particle's origin and hit position" << endl;
          cout << "     start pos: (" << spotX << ", " << spotY << ", " << spotZ << ")" << endl;
          cout << "     hit pos  : (" << hitOnBarX  << ", " << hitOnBarY  << ", " << hitOnBarZ  << ")" << endl;
          cout << "     end pos  : (" << spotEndX << ", " << spotEndY << ", " << spotEndZ << ")" << endl;
          cout << "     range    : " << range << endl << endl;;
        }


        // for beampot plot
        double hitOnBarPlotX;
        double hitOnBarPlotY;
        if( opt_alongBar )
        {
          hitOnBarPlotX = hitOnBarX;
          hitOnBarPlotY = hitOnBarY;
        }
        else if( opt_fromBelow )
        {
          hitOnBarPlotX = hitOnBarZ;
          hitOnBarPlotY = hitOnBarX;
        }
        else
        {
          hitOnBarPlotX = hitOnBarZ;
          hitOnBarPlotY = hitOnBarY;
        }

        TMarker* t = new TMarker( hitOnBarPlotX, hitOnBarPlotY, 20 );

        if( (!opt_alongBar && !opt_fromBelow && (Abs( hitOnBarY ) >= slab_height / 2 || hitOnBarZ >= 0))
              || (opt_alongBar && (Abs( hitOnBarY ) >= slab_height / 2 || Abs( hitOnBarX ) >= slab_width / 2))
              || (opt_fromBelow && (Abs( hitOnBarX ) >= slab_width / 2 || hitOnBarZ >= 0)) )
          t->SetMarkerColor( 4 );
        else
          t->SetMarkerColor( 2 );

        t->SetMarkerStyle(20);
        t->SetMarkerSize(1);

        canvas_beamspot->cd();
        t->Draw();

        particleTree->Fill();


        manager->Cerenkov(spotPos, parDir, beta, photon_number, range, lambda_min, lambda_max, refl_limit, particleNumber);


        list<PndDrcPhoton> list_photon;
        list<PndDrcPhoton>::iterator iph;
        list_photon = manager->PhotonList(); // get list

        for( iph = list_photon.begin(); iph != list_photon.end(); ++iph)
        {
          if( opt_photonPosList )
            (*iph).SetPrintFlag(true); // write out photon position
          else
            (*iph).SetPrintFlag(false);

          if( opt_debug )
            (*iph).SetVerbosity(4);
        }

        manager->SetPhotonList(list_photon);
        manager->Propagate(); // propagate photons


        list_photon = manager->PhotonList(); // get list

        for( iph = list_photon.begin(); iph != list_photon.end(); ++iph )
        {
          n_iph++;

          phot_parDirX = parDirX;
          phot_parDirY = parDirY;
          phot_parDirZ = parDirZ;
          phot_inci_theta = ACos(costheta)*degree;
          phot_inci_phi = phi*degree;

          wavelength  = (*iph).Wavelength();
          color       = (*iph).ColorNumber(wavelength);
          time        = (*iph).Time();
          nRefl       = (*iph).Reflections();
          particleID  = (*iph).ParticleIDnumber();

          XYZVector kBar = (*iph).OriginDirection();
          kBarX = kBar.X();
          kBarY = kBar.Y();
          kBarZ = kBar.Z();

          XYZVector hitDir = (*iph).Direction();
          hitDirX = hitDir.X();
          hitDirY = hitDir.Y();
          hitDirZ = hitDir.Z();

          XYZPoint hitPos = (*iph).Position();
          hitPosX = hitPos.X();
          hitPosY = hitPos.Y();
          hitPosZ = hitPos.Z();


          XYZPoint hitPosDet( hitPosX, hitPosY, hitPosZ );

          if( !opt_prismIsTank )
          {
            hitPosDet = transTo_origin_fishtank_front*hitPosDet;
            hitPosDet = rotPhi_fishtank.Inverse()*hitPosDet;
            hitPosDet = transTo_origin_fishtank_front.Inverse()*hitPosDet;
            hitPosDet = transToRotAxisY_fishtank*hitPosDet;
            hitPosDet = rotThetaX_fishtank.Inverse()*hitPosDet;
            hitPosDet = rotThetaY_fishtank.Inverse()*hitPosDet;
            hitPosDet = transToRotAxisY_fishtank.Inverse()*hitPosDet;
          }

          hitPosDetX = hitPosDet.X();
          hitPosDetY = hitPosDet.Y();
          hitPosDetZ = hitPosDet.Z();

          // rounding issue (round to 10 digits)
          hitPosDetZ = floor(hitPosDetZ * Power( 10, 10) + 0.5) * Power(10, -10);


          int posX_size = (*iph).PositionXlist().size();
          int posY_size = (*iph).PositionYlist().size();
          int posZ_size = (*iph).PositionZlist().size();

          if( posX_size != posY_size || posY_size != posZ_size ) // should not happen
          {
            cout << "*** ERROR: photon position list for X,Y and Z has not the same length" << endl;
            abort();
          }
          else
            index_pos = posX_size;

          if( posX_size > pos_size )
          {
            cout << "*** ERROR: increase position list size \"pos_size\" to" << posX_size << endl;
            abort();
          }


          int n_posX = 0;
          int n_posY = 0;
          int n_posZ = 0;
          list<double>::iterator ipos;

          list<double> list_positionX = (*iph).PositionXlist();
          for( ipos = list_positionX.begin(); ipos != list_positionX.end(); ++ipos )
          {
            posX[n_posX] = (*ipos);
            n_posX++;
          }

          list<double> list_positionY = (*iph).PositionYlist();
          for( ipos = list_positionY.begin(); ipos != list_positionY.end(); ++ipos )
          {
            posY[n_posY] = (*ipos);
            n_posY++;
          }

          list<double> list_positionZ = (*iph).PositionZlist();
          for( ipos = list_positionZ.begin(); ipos != list_positionZ.end(); ++ipos )
          {
            posZ[n_posZ] = (*ipos);
            n_posZ++;
          }


          thetaC = (*iph).ThetaC();
          phiC = (*iph).PhiC();

          measured = false;
          absorbed = false;
          lost     = false;

          if ((*iph).Fate()==Drc::kPhotMeasured)
          {
            icnt_measured++;
            measured = true;

            TMarker* t = new TMarker( hitPosDetX, hitPosDetY, 7);
            t->SetMarkerColor( (*iph).ColorNumber((*iph).Wavelength()) );
            t->SetMarkerSize(0.7);

            canvas_screen->cd();
            t->Draw();
          }
          else if( (*iph).Fate()==Drc::kPhotFlying )
          {
            icnt_flying++; // should never happen.
            cout << "*** WARNING: photon fate is still \"flying\" (should never happen)" << endl;
          }
          else if( (*iph).Fate()==Drc::kPhotAbsorbed )
          {
            icnt_absorbed++;
            absorbed = true;
          }
          else
          {
            icnt_lost++;
            lost = true;
          }

          photonTree->Fill();
        }

        manager->ClearPhotonList();
      }
    }

    canvas_beamspot->Write("Beamspot");
    canvas_beamspot->Clear();
  }



// photon cannon
//==============================================================================
  if( opt_photonCannon )
  {
    // 		TF1 *f1 = new TF1("f1","1/x",300,700);
    TRandom3 rand;

    if( seed == 0 )
    {
      seed = 4357;
      cout << "Don't use seed=0 because it's slow. Seed is now 4357." << endl;
    }

    rand.SetSeed( seed );


    if( opt_debug )
      cout << "+++++ DEBUG INFO: photon check" << endl;


    Transform3D rotY_cannon = Transform3D( RotationY(cannon_rotY*degree) );
    XYZVector axisX(1,0,0);
    XYZVector newRotX = rotY_cannon*axisX;
    newRotX = newRotX.Unit();

    TVector3 helper_rotX_cannon(newRotX.X(),newRotX.Y(),newRotX.Z());
    TRotation r_X;
    r_X.Rotate(cannon_rotX*degree, helper_rotX_cannon);

    Transform3D rotX_cannon = Transform3D(
        r_X.XX(), r_X.XY(), r_X.XZ(), 0,
    r_X.YX(), r_X.YY(), r_X.YZ(), 0,
    r_X.ZX(), r_X.ZY(), r_X.ZZ(), 0 );


    for( double gridX = -slab_width/2; gridX < slab_width/2; gridX += gridXstep)
    {
      if( gridXstep == 0 )
        gridX = 0;

      if( gridX == -slab_width/2 || slab_width/2 < gridX + 0.001 ) // no photon production at the bar edges
        continue;

      for( double gridY = -slab_height/2; gridY < slab_height/2; gridY += gridYstep)
      {
        if( gridYstep == 0 )
          gridY = 0;

        if( gridY == -slab_height/2 || slab_height/2 < gridY + 0.001 )
          continue;

        for( int i=0; i<shoots; i++)
        {
          if( i%10000 == 0 )
            cout << "gridX: " << gridX << " gridY: " << gridY << "  photon #" << i+1 << " of " << shoots << " per mesh" << endl;


          // 					double lambda = f1->GetRandom(); // seems to be wrong ; check it later
          double x1     = 1.0 / lambda_max;
          double x2     = 1.0 / lambda_min;
          double x      = rand.Uniform(x1,x2);
          double lambda = 1.0/x;


          double costheta;
          double phi;

          if( cannon_theta == 180 )
            costheta = rand.Uniform(0.0, 1.0);
          else
            costheta = Cos(cannon_theta*degree);

          if( cannon_phi == 360 )
            phi = rand.Uniform(0.0, 2*pi);
          else
            phi = cannon_phi*degree;


          Polar3DVector photDir(1, ACos(costheta), phi); // r, theta, phi
          photDir = photDir.Unit();
          XYZVector photDirXYZ( photDir.X(), photDir.Y(), photDir.Z() );
          photDirXYZ = rotY_cannon*photDirXYZ;
          photDirXYZ = rotX_cannon*photDirXYZ;

          PndDrcPhoton ph;
          ph.SetPrintFlag(true);

          if( opt_debug )
            ph.SetVerbosity(4);

          double gridPosX = gridX + cannon_posX;
          double gridPosY = gridY + cannon_posY;

          ph.SetReflectionLimit(refl_limit);
          ph.SetPosition( XYZPoint(gridPosX, gridPosY, cannon_posZ));
          ph.SetDirection(photDirXYZ);
          ph.SetWavelength(lambda);


          list<PndDrcPhoton> list_photon;
          list<PndDrcPhoton>::iterator iph;
          list_photon.push_back(ph);

          string start_vol;

          if( cannon_posZ < 0 && cannon_posZ > -slab_length )
            start_vol = "slab";
          else if( cannon_posZ > 0 && cannon_posZ < frontLens_thickness && opt_frontLens )
            start_vol = "frontLens";
          else
          {
            cout << "*** ERROR: photon cannon position (in z) is not valid ! " << endl;
            abort();
          }


          manager->SetPhotonList(list_photon,start_vol,"opt_system",0,0);
          manager->Propagate(); // propagate photons


          list_photon = manager->PhotonList(); // get list

          for( iph = list_photon.begin(); iph != list_photon.end(); ++iph )
          {
            n_iph++;

            measured = false;
            absorbed = false;
            lost     = false;

            if( (*iph).Fate()==Drc::kPhotMeasured || opt_fullCannonList )
            {
              if( (*iph).Fate()==Drc::kPhotMeasured )
              {
                icnt_measured++;
                measured = true;
              }
              else if( (*iph).Fate()==Drc::kPhotFlying )
              {
                icnt_flying++; // should never happen.
                cout << "*** WARNING: photon fate is still \"flying\" (should never happen)" << endl;
              }
              else if( (*iph).Fate()==Drc::kPhotAbsorbed )
              {
                icnt_absorbed++;
                absorbed = true;
              }
              else
              {
                icnt_lost++;
                lost = true;
              }


              wavelength  = (*iph).Wavelength();
              color       = (*iph).ColorNumber(wavelength);
              time        = (*iph).Time();
              nRefl       = (*iph).Reflections();

              XYZVector kBar = (*iph).OriginDirection();
              kBarX = kBar.X();
              kBarY = kBar.Y();
              kBarZ = kBar.Z();

              XYZVector hitDir = (*iph).Direction();
              hitDirX = hitDir.X();
              hitDirY = hitDir.Y();
              hitDirZ = hitDir.Z();

              XYZPoint hitPos = (*iph).Position();
              hitPosX = hitPos.X();
              hitPosY = hitPos.Y();
              hitPosZ = hitPos.Z();


              XYZPoint hitPosDet( hitPosX, hitPosY, hitPosZ );

              if( !opt_prismIsTank )
              {
                hitPosDet = transTo_origin_fishtank_front*hitPosDet;
                hitPosDet = rotPhi_fishtank.Inverse()*hitPosDet;
                hitPosDet = transTo_origin_fishtank_front.Inverse()*hitPosDet;
                hitPosDet = transToRotAxisY_fishtank*hitPosDet;
                hitPosDet = rotThetaX_fishtank.Inverse()*hitPosDet;
                hitPosDet = rotThetaY_fishtank.Inverse()*hitPosDet;
                hitPosDet = transToRotAxisY_fishtank.Inverse()*hitPosDet;
              }

              hitPosDetX = hitPosDet.X();
              hitPosDetY = hitPosDet.Y();
              hitPosDetZ = hitPosDet.Z();

              // rounding issue (round to 10 digits)
              hitPosDetZ = floor(hitPosDetZ * Power( 10, 10) + 0.5) * Power(10, -10);


              int posX_size = (*iph).PositionXlist().size();
              int posY_size = (*iph).PositionYlist().size();
              int posZ_size = (*iph).PositionZlist().size();

              if( posX_size != posY_size || posY_size != posZ_size ) // should not happen
              {
                cout << "*** ERROR: photon position list for X,Y and Z has not the same length" << endl;
                abort();
              }
              else
                index_pos = posX_size;

              if( posX_size > pos_size )
              {
                cout << "*** ERROR: increase position list size \"pos_size\" to" << posX_size << endl;
                abort();
              }


              int n_posX = 0;
              int n_posY = 0;
              int n_posZ = 0;
              list<double>::iterator ipos;

              list<double> list_positionX = (*iph).PositionXlist();
              for( ipos = list_positionX.begin(); ipos != list_positionX.end(); ++ipos )
              {
                posX[n_posX] = (*ipos);
                n_posX++;
              }

              list<double> list_positionY = (*iph).PositionYlist();
              for( ipos = list_positionY.begin(); ipos != list_positionY.end(); ++ipos )
              {
                posY[n_posY] = (*ipos);
                n_posY++;
              }

              list<double> list_positionZ = (*iph).PositionZlist();
              for( ipos = list_positionZ.begin(); ipos != list_positionZ.end(); ++ipos )
              {
                posZ[n_posZ] = (*ipos);
                n_posZ++;
              }


              TMarker* t = new TMarker( hitPosDetX, hitPosDetY, 7);
              t->SetMarkerColor( (*iph).ColorNumber((*iph).Wavelength()) );
              t->SetMarkerSize(0.7);

              canvas_screen->cd();
              t->Draw();


              photonTree->Fill();
            }
            else if( (*iph).Fate()==Drc::kPhotFlying )
            {
              icnt_flying++; // should never happen.
              cout << "*** WARNING: photon fate is still \"flying\" (should never happen)" << endl;
            }
            else if( (*iph).Fate()==Drc::kPhotAbsorbed )
              icnt_absorbed++;
            else
              icnt_lost++;
          }

          manager->ClearPhotonList();
        }

        if( gridYstep == 0 )
          break;
      }

      if( gridXstep == 0 )
        break;
    }
  }



// Photon summary
//==============================================================================
  int icnt = icnt_measured + icnt_flying + icnt_lost + icnt_absorbed;

  cout << endl << endl;
  cout << " generated photons: " <<icnt          << endl;
  cout << " measured  photons: " <<icnt_measured << endl;
  cout << " absorbed  photons: " <<icnt_absorbed << endl;
  cout << " lost      photons: " <<icnt_lost     << endl << endl;


  TString str_icnt;
  str_icnt += icnt;
  str_icnt.Remove(TString::kLeading,' ');
  TString str_icnt_det;
  int det = icnt - icnt_lost - icnt_absorbed;
  str_icnt_det += det;
  str_icnt_det.Remove(TString::kLeading,' ');


  TPaveText *stat = new TPaveText(0.8,0.88,0.98,0.98,"brNDC");

  stat->SetFillColor(0);
  stat->AddText( "gen.: " + str_icnt    );
  stat->AddText( "det.: " + str_icnt_det);

  canvas_screen->cd();
  stat->Draw();

  canvas_screen->Write("Screen");
  canvas_screen->Clear();
  canvas_screen->Close();


  outFile->Write();
  outFile->Close();
  cout << "Root-file " << outFilename << " was written" << endl;


  // 	delete manager; // segmentation violation with opt_photonList

  return EXIT_SUCCESS;
}
