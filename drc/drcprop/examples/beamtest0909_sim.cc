
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
#include "TRandom3.h"
#include "TROOT.h"
#include "TPaveText.h"
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
#include "PndDrcPhoton.h"
#include "PndDrcSurfPolyFlat.h"
#include "PndDrcSurfQuadFlatDiff.h"
#include "PndDrcSurfPolyAsphere.h"
#include "PndDrcOptReflNone.h"
#include "PndDrcOptReflPerfect.h"
#include "PndDrcOptMatAbs.h"
#include "PndDrcOptMatLithotecQ0.h"
#include "PndDrcOptMatBK7.h"
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
    bool opt_beamtest     = false; // beamtest simulation 2009
    bool opt_photonCannon = true; // photon cannon at bar end
    bool opt_singlePhoton = false; // single photon for debugging

    cout << "simulation options:" << endl;

    if( opt_beamtest )
        cout << "  main: \"beamtest\"" << endl;
    if( opt_photonCannon )
        cout << "  main: \"photon cannon\"" << endl;
    if( opt_singlePhoton )
        cout << "  main: \"single photon\" " << endl;


	// sub options
    bool opt_mirror               = false; // default: false ; mirror at slab front end
    bool opt_fishtankBlack_bottom = true; // default: true ; absorbed fishtank side
    bool opt_fishtankBlack_sides  = false; // default: false
    bool opt_fishtankBlack_top    = true; // default: true
    bool opt_Cherenkov_onlyInBar  = false; // Cherenkov photons are only generated in bar (slab)
    bool opt_alongBar             = false; // particles hits the bar at slab front end
    bool opt_woLens               = false; // default: false ; without lens
    bool opt_photonPosList        = true; // write out photon position list ; true takes much longer
    bool opt_noFresnel_slab       = false; // disable Fresnel reflections
    bool opt_noFresnel_lens       = false; //
    bool opt_noFresnel_airBox     = false; //
    bool opt_noFresnel_fishtank   = false; //
    bool opt_debug	              = false; // debug information ; please pipe stdout > log file

    if( opt_photonCannon && !opt_photonPosList )
    {
        opt_photonPosList = true;
        cout << "*** WARN: \"Photon position list\" is now enable because \"photon cannon\" needs it" << endl;
    }

    cout << "  sub:  ";
    if( opt_mirror )
        cout << "\"mirror\" ";
    if( opt_fishtankBlack_bottom )
        cout << "\"black fishtank bottom\" ";
    if( opt_fishtankBlack_sides )
        cout << "\"black fishtank sides\" ";
    if( opt_fishtankBlack_top )
        cout << "\"black fishtank top\" ";
    if( opt_Cherenkov_onlyInBar )
        cout << "\"photon production only in bar\" ";
    if( opt_alongBar )
        cout << "\"along Bar\" ";
    if( opt_woLens )
        cout << "\"w/o lens\" ";
    if( opt_photonPosList )
        cout << "\"photon position list\" ";
    if( opt_noFresnel_slab || opt_noFresnel_lens || opt_noFresnel_airBox || opt_noFresnel_fishtank )
    {
        cout << "\"No Fresnel reflection for";
        if( opt_noFresnel_slab )
            cout << " slab";
        if( opt_noFresnel_lens )
            cout << " lens";
        if( opt_noFresnel_airBox )
            cout << " airBox";
        if( opt_noFresnel_fishtank )
            cout << " fishtank";
        cout << "\"";
    }
    cout << endl;


    int check_opt = opt_beamtest + opt_photonCannon + opt_singlePhoton;

    if( check_opt > 1 || check_opt == 0 )
    {
        cout << "*** ERROR: Select only one main simulation option !" << endl;
        cout << "available main simulation options:" << endl;
        cout << "  beamtest" << endl;
        cout << "  photon cannon" << endl;
        cout << "  own photons" << endl;
        cout << "    add. options:" << endl;
        cout << "      mirror" << endl;
        cout << "      fishtank black (bottom)" << endl;
        cout << "      fishtank black (sides)" << endl;
        cout << "      fishtank black (top)" << endl;
        cout << "      Photon production only in bar" << endl;
        cout << "      along Bar" << endl;
        cout << "      w/o lens" << endl;
        cout << "      photon position list" << endl;

        abort();
    }


//==============================================================================
// Changable parameters & constants
//==============================================================================

	// math constants
    const double pi = Pi();
    const double degree = pi/180.;

	// material
    PndDrcOptMatLithotecQ0  *quartz = new PndDrcOptMatLithotecQ0();
    PndDrcOptMatVacuum      *vacuum = new PndDrcOptMatVacuum();
    PndDrcOptMatBK7         *bk7    = new PndDrcOptMatBK7();

    PndDrcOptMatAbs *mat_slab     = quartz; // default: quartz
    PndDrcOptMatAbs *mat_lens     = bk7; // default: bk7
    PndDrcOptMatAbs *mat_airBox   = vacuum; // default: vacuum
    PndDrcOptMatAbs *mat_fishtank = quartz; // default: quartz


	// dimensions
    double slab_width  = 17; // default: 17 mm
    double slab_height = 35; // default: 35 mm
    double slab_length = 800; // default: 800 mm

    double lens_radius    = 77.52; // f = R/(n-1) ; BK7 Newport f = 150 mm at 589 nm => R = 77.52 mm
    double lens_thickness = 7.5; // measured lens thickness: 7.5 mm ; old: 5 mm
    double lens_diameter  = 40; // default: 40mm ; actually 50.8 mm but currently lens base is not cylindrical
    double lens_conical   = 0; // default: 0 (spherical)

    double airgap = 10; // default: 10 mm ; distance between slab and fishtank ; 0 means no air box

    double fishtank_width  = 300; // default: 300 mm ; old: 400 mm
    double fishtank_height = 200; // default: 200 mm ; old: 400 mm
    double fishtank_length = 200; // default: 200 mm ; old: 220 mm

    double fishtank_width_offset = 0; // default: 0 mm ; 0 means bar is centered
    double fishtank_height_offset = 0; // default: 0 mm

    double fishtank_thetaX = 0; // default: 0 degree (rotation axis X)
    double fishtank_thetaY = 0; // default: 0 degree (rotation axis Y)
    double fishtank_phi = 0; // default: 0 degree (rotation axis Z)


	// particle properties
    const double mass_p  = 0.9383; // proton mass in GeV
    const double mass_K  = 0.4937; // kaon mass
    const double mass_pi = 0.1396; // pion mass
    const double mass_e  = 0.0005; // electron mass
    const double mass_mu = 0.1057; // muon mass

    double mass = mass_p; // default: proton mass
    double kinE = 2.0; // default: 2.0 GeV kinetic energy ; 2.3 GeV
    double beta = Sqrt( 1 - Power( mass / (kinE + mass), 2 ) ); // E = T + E0 = gamma * E0

    double spot_radius = 20; // default: 20 mm 1-sigma beam spot radius (gaus smeared)
    double spot_limit = 50; // default: 50 mm beam spot radius limit

    int particle_number = 300; // default: 300

    double inci_theta = 57; // default: 57 degree
    double inci_phi   = 0; // default: 0 degree

    double hitBarX = slab_width/2; // default: slab_width/2
    double hitBarY = 0; // default: 0 mm
    double hitBarZ = -500; // default: -500 mm


	// photon properties
    int photon_number = 10; // default: 100 (per particle); 0 means realistic number of Cherenkov photons

    double lambda_min = 300; // default: 300 nm ; lowest Cherenkov wavelength
    double lambda_max = 700; // default: 700 nm ; highest Cherenkov wavelength

    int refl_limit = 1000; // default: 1000 reflections


	// photon cannon
    int shoots = 10000; // default: 100000

    double gridXstep = 0; // default: 0 mm ; grid constant in X ; 0 means cannon is always in the center
    double gridYstep = 0; // default: 0 mm

    int refl_limit_2 = 5; // default: 5 ; low limit increases speed


    // single photon
    double singlePosX = 0;
    double singlePosY = 0;
    double singlePosZ = -10;

    double singleDirX = 1;
    double singleDirY = 1;
    double singleDirZ = 1;

    double single_lambda = 500;


	// output filename (w/o file extension)
    TString defaultFilename = "beamtest0909_test"; // default: beamtest0909_sim
    TString outFilename = defaultFilename; // default: defaultFilename



//==============================================================================
// Input parameters
//==============================================================================

	// command line arguments (set variable parameter to avoid permanent compiling)
    for( int i = 1; i < argc; i++)
    {
        if( i == 1)
            outFilename = argv[1];
        if( i == 2)
            fishtank_thetaX = atof( argv[2]);
        if( i == 3 )
            fishtank_thetaY = atof( argv[3]);
        if( i == 4 )
            fishtank_phi = atof( argv[4]);
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
	//       particle    |                              |      particle
	//       trajectory  |                             perp.   trajectory


    double parDirX, parDirY, parDirZ;
    XYZVector parDir;

    if( opt_alongBar )
    {
        parDirX = Tan(inci_theta*degree);
        parDirY = 0;
        parDirZ = 1;

        parDir.SetXYZ( parDirX, parDirY, parDirZ );

        Transform3D rot_phi = Transform3D( RotationZ(inci_phi*degree) );
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


    if( opt_beamtest )
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
                cout << "*** WARN: hit position Z is fixed at " << -slab_length << " mm (slab front end)" << endl << endl;
            }
        }
        else
        {
            if( hitBarX != slab_width/2 )
                cout << "*** WARN: hit position X is fixed at " << slab_width/2 << " mm (1/2 slab width)" << endl << endl;

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

    string mat_slab_str     = mat_slab->Name();
    string mat_lens_str     = mat_lens->Name();
    string mat_airBox_str   = mat_airBox->Name();
    string mat_fishtank_str = mat_fishtank->Name();

    cout <<     "  slab:     " << mat_slab_str << endl;
    if( !opt_woLens )
        cout << "  lens:     " << mat_lens_str << endl;
    if( airgap > 0 )
        cout << "  airBox:   " << mat_airBox_str << endl;
    cout <<     "  fishtank: " << mat_fishtank_str << endl;


	// refractive index check
    if( opt_debug )
    {
        cout << "+++++ DEBUG INFO: refractive index check" << endl;
        double waveLength[4];
        waveLength[0] = 300;
        waveLength[1] = 405;
        waveLength[2] = 532;
        waveLength[3] = 589;
        for( int i=0; i < 4; i++ )
        {
            cout << "     " << mat_slab_str     << " (" << waveLength[i] << " nm) = " << quartz->RefIndex(waveLength[i]) << endl;
            cout << "     " << mat_lens_str     << " (" << waveLength[i] << " nm) = " << quartz->RefIndex(waveLength[i]) << endl;
            cout << "     " << mat_airBox_str   << " (" << waveLength[i] << " nm) = " << quartz->RefIndex(waveLength[i]) << endl;
            cout << "     " << mat_fishtank_str << " (" << waveLength[i] << " nm) = " << quartz->RefIndex(waveLength[i]) << endl;
        }
    }


    cout << "dimensions [mm]:" << endl;

    cout << "  slab width:  " << slab_width << endl;
    cout << "  slab heigth: " << slab_height << endl;
    cout << "  slab length: " << slab_length << endl;

    if( !opt_woLens )
    {
        cout << "  lens radius:         " << lens_radius << endl;
        cout << "  lens thickness:      " << lens_thickness << endl;
        cout << "  lens diameter:       " << lens_diameter << endl;
        cout << "  lens conical const.: " << lens_conical << endl;
    }

    if( lens_thickness > airgap && !opt_woLens )
    {
        cout << "*** ERROR: no air gap possible" << endl;
        abort();
    }

    cout << "  air gap: " << airgap << endl;

    cout << "  fishtank width:  " << fishtank_width << endl;
    cout << "  fishtank heigth: " << fishtank_height << endl;
    cout << "  fishtank length: " << fishtank_length << endl;

    cout << "  fishtank width offset : " << fishtank_width_offset << endl;
    cout << "  fishtank height offset: " << fishtank_height_offset << endl;

    cout << "  fishtank thetaX: " << fishtank_thetaX << endl;
    cout << "  fishtank thetaY: " << fishtank_thetaY << endl;
    cout << "  fishtank phi:    " << fishtank_phi << endl;


    if( opt_beamtest )
    {
        cout << "particle properties:" << endl;

        if( mass == mass_p)
            cout << "  sort:    proton" << endl;
        else if( mass == mass_K )
            cout << "  sort:    kaon" << endl;
        else if( mass == mass_pi )
            cout << "  sort:    pion" << endl;
        else if( mass == mass_e )
            cout << "  sort:    electron" << endl;
        else if( mass == mass_mu )
            cout << "  sort:    muon" << endl;
        else
            cout << "*** WARN: undefined particle sort" << endl;

        cout <<     "  T [GeV]: " << kinE << endl;
        cout <<     "  beta:    " << beta << endl;

        cout <<     "  incidence angle (theta): " << inci_theta << " deg" << endl;
        cout <<     "  incidence angle (phi):   " << inci_phi << " deg" << endl;
        cout <<     "  flight direction:        (" << parDirX << ", " << parDirY << ", " << parDirZ << ")" << endl;
        cout <<     "  centered hit pos on bar: (" << hitBarX << ", " << hitBarY << ", " << hitBarZ << ")" << endl;

        cout <<     "  beam spot radius: " << spot_radius << " mm" << endl;
        cout <<     "  radius limit:     " << spot_limit << " mm" << endl;
        cout <<     "  particle number:  " << particle_number << endl;


        cout << "photon properties:" << endl;

        if( photon_number == 0 )
            cout << "  photon number:      realistic" << endl;
        else
            cout << "  photon number:      " << photon_number << endl;

        cout <<     "  Cherenkov spectrum: [" << lambda_min <<", " << lambda_max << "] nm" << endl;
        cout <<     "  reflection limit:   " << refl_limit << endl;
    }


    if( opt_photonCannon )
    {
        cout << "photon cannon:" << endl;
        if( gridXstep == 0 || gridYstep == 0 )
            cout << "  photon number:    " << shoots << endl;
        else
        {
            cout << "  photon number:    " << shoots << " per mesh" << endl;
            cout << "  grid const. X:    " << gridXstep << " mm" << endl;
            cout << "  grid const. Y:    " << gridYstep << " mm" << endl;
        }
        cout <<     "  reflection limit: " << refl_limit_2 << endl;
    }

    if( opt_singlePhoton )
    {
        cout << "single photon:" << endl;
        cout << "  creation pos.: (" << singlePosX << ", " << singlePosY << ", " << singlePosZ << ")" << endl;
        cout << "  creation dir.: (" << singleDirX << ", " << singleDirY << ", " << singleDirZ << ")" << endl;
        cout << "  lambda:        " << single_lambda << " nm" << endl;
    }



//==============================================================================
// ROOT output file
//==============================================================================

// ROOT file content:
// 	photonList (TTree)
// 	particleList (TTree)
// 	info (TTree) for global parameter like e.g. lens thickness
// 	default plots: Screen, beamspot, Setup geometry

// to analyze this root-file there are two macros:
//	1. mcpPos.cc for fast plotting the effect of different MCP positions
//	2. analyze.cc create a further root-file with a lot of plots (canvases)



// file name
//==============================================================================
    Bool_t defaultFilenameFlag = false;
    if( outFilename == defaultFilename )
        defaultFilenameFlag = true;

    if( defaultFilenameFlag && !opt_beamtest )
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
            cout << "Please select another file name." << endl;
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
    TTree *infoTree  	= new TTree( "info", outFilename );


    // infoTree (parameter list)

//******************************************************************************
// Note:
// Unfortunetly TTree allows only basic c-types (no strings) and no-const types
//******************************************************************************

    const char *slab_mat_helper = mat_slab_str.c_str(); // includes \0 (escape sequence)
    const char *lens_mat_helper = mat_lens_str.c_str();
    const char *airBox_mat_helper = mat_airBox_str.c_str();
    const char *fishtank_mat_helper = mat_fishtank_str.c_str();

    char slab_material[64]     = "";
    char lens_material[64]     = "";
    char airBox_material[64]   = "";
    char fishtank_material[64] = "";

    strcpy(slab_material, slab_mat_helper);
    if( !opt_woLens )
        strcpy(lens_material, lens_mat_helper);
    if( airgap > 0 )
        strcpy(airBox_material, airBox_mat_helper);
    strcpy(fishtank_material, fishtank_mat_helper);

    bool slab_fresnel = true;
    bool lens_fresnel = true;
    bool airBox_fresnel = true;
    bool fishtank_fresnel = true;

    if( opt_noFresnel_slab )
        slab_fresnel = false;
    if( opt_noFresnel_lens )
        lens_fresnel = false;
    if( opt_noFresnel_airBox )
        airBox_fresnel = false;
    if( opt_noFresnel_fishtank )
        fishtank_fresnel = false;

    if( opt_woLens )
    {
        lens_radius    = -666;
        lens_thickness = -666;
        lens_diameter  = -666;
        lens_conical   = -666;
    }

    if( opt_photonCannon )
    {
        photon_number   = -666;
        mass            = -666;
        kinE            = -666;
        beta            = -666;
        inci_theta      = -666;
        inci_phi        = -666;
        hitBarX         = -666;
        hitBarY         = -666;
        hitBarZ         = -666;
        spot_radius     = -666;
        spot_limit      = -666;
        particle_number = -666;

        refl_limit = refl_limit_2;
    }
    else
    {
        shoots    = -666;
        gridXstep = -666;
        gridYstep = -666;
    }


    infoTree->Branch( "root_version"          , &rootVer               , "root_version/I" );
    infoTree->Branch( "slab_material"         , &slab_material         , "slab_material/C" );
    infoTree->Branch( "slab_width"            , &slab_width            , "slab_width/D" );
    infoTree->Branch( "slab_height"           , &slab_height           , "slab_height/D" );
    infoTree->Branch( "slab_length"           , &slab_length           , "slab_length/D" );
    infoTree->Branch( "slab_fresnel"          , &slab_fresnel          , "slab_fresnel/O" );
    infoTree->Branch( "airgap"                , &airgap                , "airgap/D" );
    infoTree->Branch( "lens_material"         , &lens_material         , "lens_material/C" );
    infoTree->Branch( "lens_radius"           , &lens_radius           , "lens_radius/D" );
    infoTree->Branch( "lens_thickness"        , &lens_thickness        , "lens_thickness/D" );
    infoTree->Branch( "lens_diameter"         , &lens_diameter         , "lens_diameter/D" );
    infoTree->Branch( "lens_conical"          , &lens_conical          , "lens_conical/D" );
    infoTree->Branch( "lens_fresnel"          , &lens_fresnel          , "lens_fresnel/O" );
    infoTree->Branch( "airBox_material"       , &airBox_material       , "airBox_material/C" );
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
    infoTree->Branch( "fishtank_fresnel"      , &fishtank_fresnel      , "fiahtank_fresnel/O" );
    infoTree->Branch( "photon_number"         , &photon_number         , "photon_number/I" );
    infoTree->Branch( "lambda_min"            , &lambda_min            , "lambda_min/D" );
    infoTree->Branch( "lambda_max"            , &lambda_max            , "lambda_max/D" );
    infoTree->Branch( "refl_limit"            , &refl_limit            , "refl_limit/I" );
    infoTree->Branch( "particle_mass"         , &mass                  , "particle_mass/D" );
    infoTree->Branch( "particle_kinE"         , &kinE                  , "particle_kinE/D" );
    infoTree->Branch( "particle_beta"         , &beta                  , "particle_beta/D" );
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

    //  vector<double> posX; // needed ROOT >= 5.2
    int add_pos = 1 + 3*2 + 1; // 1 start + 3 volume transition (+ tiny shifts) + detector
    const int pos_size = refl_limit + 1 + add_pos; // reflection limit + 1 exceed + additional positions
    double posX[ pos_size ];
    double posY[ pos_size ];
    double posZ[ pos_size ];
    for( int i=0; i < pos_size; i++)
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
    photonTree->Branch( "hitDirX"   , &hitDirX   , "hitDirX/D" );
    photonTree->Branch( "hitDirY"   , &hitDirY   , "hitDirY/D" );
    photonTree->Branch( "hitDirZ"   , &hitDirZ   , "hitDirY/D" );
    if( opt_photonPosList )
    {
        photonTree->Branch( posX_str    , posX       , posX_str_2 );
        photonTree->Branch( posY_str    , posY       , posY_str_2 );
        photonTree->Branch( posZ_str    , posZ       , posZ_str_2 );
        photonTree->Branch( "index_pos" , &index_pos , "index_pos/I" );
    }
    if( !opt_photonCannon )
    {
        photonTree->Branch( "particleID", &particleID,"particleID/I" );
        photonTree->Branch( "thetaC"    , &thetaC    , "thetaC/D" );
        photonTree->Branch( "phiC"      , &phiC      , "phiC/D" );
    }
    photonTree->Branch( "measured"  , &measured  , "measured/O" ); // O: Bool_t
    photonTree->Branch( "absorbed"  , &absorbed  , "absorbed/O" );
    photonTree->Branch( "lost"      , &lost      , "lost/O" );
    photonTree->Branch( "time"      , &time      , "time/D" );
    photonTree->Branch( "nRefl"     , &nRefl     , "nRefl/I" );


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
    gStyle->SetCanvasColor( 0 );        // white
    gStyle->SetCanvasBorderMode( 0 );   // no yellow frame
    gStyle->SetFrameFillColor( 0 );
    gStyle->SetFrameBorderMode( 0 );    // no red frame
    gStyle->SetHistFillColor( 0 );
    gStyle->SetPadColor( 0 );
    gStyle->SetTitleFillColor( 0 );     // white; not saved in the root file
    gStyle->SetTitleFontSize( 0.05 );
    gStyle->SetPalette( 1 );            // better color palette
    gStyle->SetStatColor( 0 );          // stat. box color

    TCanvas *canvas_beamspot = new TCanvas( "canvas_beamspot", "" ,200, 10, 700, 510 );
    canvas_beamspot->Draw();

    TCanvas *canvas_screen   = new TCanvas( "canvas_screen"  , "" ,200, 10, 700, 510 );
    canvas_screen->Draw();

    TCanvas *canvas_setup    = new TCanvas( "canvas_setip"   , "" ,200, 10, 700, 510 );
    canvas_setup->Draw();


	// beampsot plot
    TString beamspot_title  = "beamspot on bar";
    TString beamspot_titleX;
    TString beamspot_titleY = "y [mm]";
    double center;

    if( opt_alongBar )
    {
        beamspot_titleX = "x [mm]";
        center = hitBarX;
    }
    else
    {
        beamspot_titleX = "z [mm]";
        center = hitBarZ;
    }


    int minX = FloorNint( center - spot_limit/Cos(inci_theta*degree) );
    int maxX = CeilNint( center + spot_limit/Cos(inci_theta*degree) );

    TH1F *beamspot = new TH1F( "beamspot", beamspot_title, 100, minX, maxX );

    beamspot->SetStats( 0 );
    beamspot->SetMinimum(-spot_limit);
    beamspot->SetMaximum(+spot_limit);
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

    if( maxX > 0 && !opt_alongBar)
    {
        left->SetX1(0);
        left->SetY1(-slab_height/2);
        left->SetX2(0);
        left->SetY2(+slab_height/2);

        top->SetX1(minX);
        top->SetY1(-slab_height/2);
        top->SetX2(0);
        top->SetY2(-slab_height/2);

        bottom->SetX1(minX);
        bottom->SetY1(slab_height/2);
        bottom->SetX2(0);
        bottom->SetY2(slab_height/2);
    }
    else if( minX < -slab_length && !opt_alongBar)
    {
        right->SetX1(-slab_length);
        right->SetY1(-slab_height/2);
        right->SetX2(-slab_length);
        right->SetY2(+slab_height/2);

        top->SetX1(-slab_length);
        top->SetY1(-slab_height/2);
        top->SetX2(maxX);
        top->SetY2(-slab_height/2);

        bottom->SetX1(-slab_length);
        bottom->SetY1(slab_height/2);
        bottom->SetX2(maxX);
        bottom->SetY2(slab_height/2);
    }
    else if( !opt_alongBar )
    {
        top->SetX1(minX);
        top->SetY1(-slab_height/2);
        top->SetX2(maxX);
        top->SetY2(-slab_height/2);

        bottom->SetX1(minX);
        bottom->SetY1(slab_height/2);
        bottom->SetX2(maxX);
        bottom->SetY2(slab_height/2);
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
    left  ->Draw( "same" );
    right ->Draw( "same" );
    top   ->Draw( "same" );
    bottom->Draw( "same" );

    beamspot->Draw( "POL" );

	// screen plot
    TString screen_titleX = "x [mm]";
    TString screen_titleY = "y [mm]";

    TH1F *screen = new TH1F( "screen", "", 600, -fishtank_width/2 + fishtank_width_offset, fishtank_width/2  + fishtank_width_offset );

    screen->SetStats( 0 );
    screen->SetMinimum(-fishtank_height/2 + fishtank_height_offset);
    screen->SetMaximum(+fishtank_height/2 + fishtank_height_offset);
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
// Unfortunately Fresnel reflection is hard-coded in PndDrcPhoton.h
// to disable these kind of reflection set in the method "Refract" the fresnelFlag to false
// currently a fresnelFlag for a certain surface is not implemented yet
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


    if( opt_mirror )
        slab_front.SetReflectivity(refl_perfect);


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



// lens
//==============================================================================
    XYZPoint l0(-lens_diameter/2, +lens_diameter/2, 0);
    XYZPoint l1(-lens_diameter/2, -lens_diameter/2, 0);
    XYZPoint l2(+lens_diameter/2, -lens_diameter/2, 0);
    XYZPoint l3(+lens_diameter/2, +lens_diameter/2, 0);


    PndDrcSurfPolyAsphere lens_sphere;
    lens_sphere.AddPoint(l0);
    lens_sphere.AddPoint(l1);
    lens_sphere.AddPoint(l2);
    lens_sphere.AddPoint(l3);
    lens_sphere.SetRadius(lens_radius);
    lens_sphere.SetPrintColor(2);
    lens_sphere.SetConicalConstant(lens_conical);
    lens_sphere.SetName("lens_sphere");

    double dist = lens_sphere.CenterPoint().Z(); // in general the lens radius
    lens_sphere.AddTransform( Transform3D( XYZVector(0,0,-(dist)) ) ); // ( dist |  =>  |(


    double lensMinThickness = Abs( lens_sphere.LimitingPoint(0).Z() );

    if( lensMinThickness > lens_thickness && !opt_woLens)
    {
        cout << "*** ERROR: lens thickness have to be greater than: " << lensMinThickness
                << " (currently " << lens_thickness << " mm)" << endl;
        abort();
    }


    XYZPoint q0 = l0 + XYZVector(0, 0, -lens_thickness);
    XYZPoint q1 = l1 + XYZVector(0, 0, -lens_thickness);
    XYZPoint q2 = l2 + XYZVector(0, 0, -lens_thickness);
    XYZPoint q3 = l3 + XYZVector(0, 0, -lens_thickness);


    PndDrcSurfPolyFlat lens_base;
    lens_base.AddPoint(q0);
    lens_base.AddPoint(q1);
    lens_base.AddPoint(q2);
    lens_base.AddPoint(q3);
    lens_base.SetPrintColor(2);
    lens_base.SetName("lens_base");

    PndDrcSurfQuadFlatDiff lens_right;
    lens_right.AddSurface(lens_base, q0,q1);
    lens_right.AddSurface(lens_sphere, lens_sphere.LimitingPoint(0),lens_sphere.LimitingPoint(1));
    lens_right.SetPrintColor(2);
    lens_right.SetName("lens_right");

    PndDrcSurfQuadFlatDiff lens_bottom;
    lens_bottom.AddSurface(lens_base, q1,q2);
    lens_bottom.AddSurface(lens_sphere, lens_sphere.LimitingPoint(1),lens_sphere.LimitingPoint(2));
    lens_bottom.SetPrintColor(2);
    lens_bottom.SetName("lens_bottom");

    PndDrcSurfQuadFlatDiff lens_left;
    lens_left.AddSurface(lens_base, q2,q3);
    lens_left.AddSurface(lens_sphere, lens_sphere.LimitingPoint(2),lens_sphere.LimitingPoint(3));
    lens_left.SetPrintColor(2);
    lens_left.SetName("lens_left");

    PndDrcSurfQuadFlatDiff lens_top;
    lens_top.AddSurface(lens_base, q3,q0);
    lens_top.AddSurface(lens_sphere, lens_sphere.LimitingPoint(3),lens_sphere.LimitingPoint(0));
    lens_top.SetPrintColor(2);
    lens_top.SetName("lens_top");


    PndDrcOptVol lens;
    lens.SetVerbosity(0);
    lens.AddSurface(lens_base);
    lens.AddSurface(lens_sphere);
    lens.AddSurface(lens_left);
    lens.AddSurface(lens_right);
    lens.AddSurface(lens_bottom);
    lens.AddSurface(lens_top);
    lens.SetOptMaterial( (*mat_lens) );
    lens.SetName("lens");

    if( opt_Cherenkov_onlyInBar )
        lens.SetRadiator(false);

    double lens_shift = lens_thickness;
    lens.AddTransform( Transform3D( XYZVector(0,0,lens_shift) ) ); // |(  =>  (|

    Transform3D rotPhi_fishtank = Transform3D( RotationZ(fishtank_phi*degree) );
    lens.AddTransform( rotPhi_fishtank );


    if( opt_noFresnel_lens )
        lens.SetFresnel(false);


    if( opt_debug )
    {
        cout << "+++++ DEBUG INFO: lens check" << endl;
        lens.SetVerbosity(4);
        lens_base.SetVerbosity(4);
        lens_left.SetVerbosity(4);
        lens_right.SetVerbosity(4);
        lens_bottom.SetVerbosity(4);
        lens_top.SetVerbosity(4);
        lens_sphere.SetVerbosity(4);
// 		lens_left.SetReflectivity(refl_none);
// 		lens_right.SetReflectivity(refl_none);
// 		lens_bottom.SetReflectivity(refl_none);
// 		lens_top.SetReflectivity(refl_none);
// 		lens_sphere.SetReflectivity(refl_none);
// 		lens_left.SetPixel();
// 		lens_right.SetPixel();
// 		lens_bottom.SetPixel();
// 		lens_top.SetPixel();
// 		lens_sphere.SetPixel();
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
	// fishtank thetaX:
	//	positive angle: rotation axis b3-b4
	//	negative angle: rotation axis b1-b2
    //
	// fishtank thetY:
	//	positive angle: rotation axis b2-b3
	//	negative angle: rotation axis b4-b1


    double fishtank_posZ = fishtank_length + airgap; // default: 210 mm


    XYZPoint b1(-fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, airgap);
    XYZPoint b2(-fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, airgap);
    XYZPoint b3(+fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, airgap);
    XYZPoint b4(+fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, airgap);

    XYZPoint b5(-fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, +fishtank_posZ);
    XYZPoint b6(-fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, +fishtank_posZ);
    XYZPoint b7(+fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, +fishtank_posZ);
    XYZPoint b8(+fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, +fishtank_posZ);


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
    Transform3D rotThetaX_fishtank = Transform3D( RotationY(fishtank_thetaX*degree) );

    if( fishtank_thetaX >= 0)
    {
        Transform3D transToRotAxis_fishtank = Transform3D( XYZVector(-b3.X(),0,-b3.Z()) );
        Transform3D transBack_fishtank = Transform3D( XYZVector(b3.X(),0,b3.Z()) );

        fishtank.AddTransform( transToRotAxis_fishtank );
        fishtank.AddTransform( rotThetaX_fishtank );
        fishtank.AddTransform( transBack_fishtank );

        b1 = transToRotAxis_fishtank*b1;
        b1 = rotThetaX_fishtank*b1;
        b1 = transBack_fishtank*b1;

        b2 = transToRotAxis_fishtank*b2;
        b2 = rotThetaX_fishtank*b2;
        b2 = transBack_fishtank*b2;
    }
    else
    {
        Transform3D transToRotAxis_fishtank = Transform3D( XYZVector(-b1.X(),0,-b1.Z()) );
        Transform3D transBack_fishtank = Transform3D( XYZVector(b1.X(),0,b1.Z()) );

        fishtank.AddTransform( transToRotAxis_fishtank );
        fishtank.AddTransform( rotThetaX_fishtank );
        fishtank.AddTransform( transBack_fishtank );

        b3 = transToRotAxis_fishtank*b3;
        b3 = rotThetaX_fishtank*b3;
        b3 = transBack_fishtank*b3;

        b4 = transToRotAxis_fishtank*b4;
        b4 = rotThetaX_fishtank*b4;
        b4 = transBack_fishtank*b4;
    }


    Transform3D rotThetaY_fishtank = Transform3D( RotationX(fishtank_thetaY*degree) );

    if( fishtank_thetaY >= 0)
    {
        Transform3D transToRotAxis_fishtank;
        Transform3D transBack_fishtank;

        if( fishtank_thetaX >= 0 )
        {
            transToRotAxis_fishtank = Transform3D( XYZVector(0,-b3.Y(),-b3.Z()) );
            transBack_fishtank = Transform3D( XYZVector(0,b3.Y(),b3.Z()) );
        }
        else
        {
            transToRotAxis_fishtank = Transform3D( XYZVector(0,-b2.Y(),-b2.Z()) );
            transBack_fishtank = Transform3D( XYZVector(0,b2.Y(),b2.Z()) );
        }

        fishtank.AddTransform( transToRotAxis_fishtank );
        fishtank.AddTransform( rotThetaY_fishtank );
        fishtank.AddTransform( transBack_fishtank );

        b1 = transToRotAxis_fishtank*b1;
        b1 = rotThetaY_fishtank*b1;
        b1 = transBack_fishtank*b1;

        b4 = transToRotAxis_fishtank*b4;
        b4 = rotThetaY_fishtank*b4;
        b4 = transBack_fishtank*b4;

        if( fishtank_thetaX >= 0 )
        {
            b2 = transToRotAxis_fishtank*b2;
            b2 = rotThetaY_fishtank*b2;
            b2 = transBack_fishtank*b2;
        }
        else
        {
            b3 = transToRotAxis_fishtank*b3;
            b3 = rotThetaY_fishtank*b3;
            b3 = transBack_fishtank*b3;
        }
    }
    else
    {
        Transform3D transToRotAxis_fishtank;
        Transform3D transBack_fishtank;

        if( fishtank_thetaX >= 0 )
        {
            transToRotAxis_fishtank = Transform3D( XYZVector(0,-b4.Y(),-b4.Z()) );
            transBack_fishtank = Transform3D( XYZVector(0,b4.Y(),b4.Z()) );
        }
        else
        {
            transToRotAxis_fishtank = Transform3D( XYZVector(0,-b1.Y(),-b1.Z()) );
            transBack_fishtank = Transform3D( XYZVector(0,b1.Y(),b1.Z()) );
        }

        fishtank.AddTransform( transToRotAxis_fishtank );
        fishtank.AddTransform( rotThetaY_fishtank );
        fishtank.AddTransform( transBack_fishtank );

        b2 = transToRotAxis_fishtank*b2;
        b2 = rotThetaY_fishtank*b2;
        b2 = transBack_fishtank*b2;

        b3 = transToRotAxis_fishtank*b3;
        b3 = rotThetaY_fishtank*b3;
        b3 = transBack_fishtank*b3;


        if( fishtank_thetaX >= 0 )
        {
            b1 = transToRotAxis_fishtank*b1;
            b1 = rotThetaY_fishtank*b1;
            b1 = transBack_fishtank*b1;
        }
        else
        {
            b4 = transToRotAxis_fishtank*b4;
            b4 = rotThetaY_fishtank*b4;
            b4 = transBack_fishtank*b4;
        }
    }

    fishtank.AddTransform( rotPhi_fishtank );


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
	//      |  /  _     |  /      _ (0,0,0) origin
    //      | /         | /
	//      a3----------a2


    XYZPoint a1(-fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, 0);
    XYZPoint a2(-fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, 0);
    XYZPoint a3(+fishtank_width/2 + fishtank_width_offset, -fishtank_height/2 + fishtank_height_offset, 0);
    XYZPoint a4(+fishtank_width/2 + fishtank_width_offset, +fishtank_height/2 + fishtank_height_offset, 0);


    PndDrcSurfPolyFlat airBox_front;
    airBox_front.AddPoint(a1);
    airBox_front.AddPoint(a2);
    airBox_front.AddPoint(a3);
    airBox_front.AddPoint(a4);
    airBox_front.SetName("airBox_front");

    PndDrcSurfPolyFlat airBox_bottom;
    airBox_bottom.AddPoint(a2);
    airBox_bottom.AddPoint(a3);
    airBox_bottom.AddPoint(b3);
    airBox_bottom.AddPoint(b2);
    airBox_bottom.SetName("airBox_bottom");

    PndDrcSurfPolyFlat airBox_left;
    airBox_left.AddPoint(a3);
    airBox_left.AddPoint(a4);
    airBox_left.AddPoint(b4);
    airBox_left.AddPoint(b3);
    airBox_left.SetName("airBox_left");

    PndDrcSurfPolyFlat airBox_top;
    airBox_top.AddPoint(a1);
    airBox_top.AddPoint(a4);
    airBox_top.AddPoint(b4);
    airBox_top.AddPoint(b1);
    airBox_top.SetName("airBox_top");

    PndDrcSurfPolyFlat airBox_exit;
    airBox_exit.AddPoint(b1);
    airBox_exit.AddPoint(b2);
    airBox_exit.AddPoint(b3);
    airBox_exit.AddPoint(b4);
    airBox_exit.SetName("airBox_exit");

    PndDrcSurfPolyFlat airBox_right;
    airBox_right.AddPoint(b1);
    airBox_right.AddPoint(b2);
    airBox_right.AddPoint(a2);
    airBox_right.AddPoint(a1);
    airBox_right.SetName("airBox_right");


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

    airBox.AddTransform( rotPhi_fishtank );


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
    //      a4--h4--h1--a1      /
    //      |   |   |   |      /
    //      |   |   |   |     /
    //      |---l3--l0--|    /
	//      |   | H |   |   /       H: lens-hole
    //      |---l2--l1--|  /
    //      |   |   |   | /
    //      |   |   |   |/
	//      a3--h3--h2--a2


    XYZPoint h1(-lens_diameter/2, +fishtank_height/2, 0);
    XYZPoint h2(-lens_diameter/2, -fishtank_height/2, 0);
    XYZPoint h3(+lens_diameter/2, -fishtank_height/2, 0);
    XYZPoint h4(+lens_diameter/2, +fishtank_height/2, 0);


    PndDrcSurfPolyFlat airLens_frontLeft;
    airLens_frontLeft.AddPoint(h4);
    airLens_frontLeft.AddPoint(h3);
    airLens_frontLeft.AddPoint(a3);
    airLens_frontLeft.AddPoint(a4);
//     airLens_frontLeft.SetReflectivity(refl_none);
    airLens_frontLeft.SetName("airLens_frontLeft");

    PndDrcSurfPolyFlat airLens_frontRight;
    airLens_frontRight.AddPoint(a1);
    airLens_frontRight.AddPoint(a2);
    airLens_frontRight.AddPoint(h2);
    airLens_frontRight.AddPoint(h1);
//     airLens_frontRight.SetReflectivity(refl_none);
    airLens_frontRight.SetName("airLens_frontRight");

    PndDrcSurfPolyFlat airLens_frontBottom;
    airLens_frontBottom.AddPoint(l1);
    airLens_frontBottom.AddPoint(h2);
    airLens_frontBottom.AddPoint(h3);
    airLens_frontBottom.AddPoint(l2);
//     airLens_frontBottom.SetReflectivity(refl_none);
    airLens_frontBottom.SetName("airLens_frontBottom");

    PndDrcSurfPolyFlat airLens_frontTop;
    airLens_frontTop.AddPoint(h1);
    airLens_frontTop.AddPoint(l0);
    airLens_frontTop.AddPoint(l3);
    airLens_frontTop.AddPoint(h4);
//     airLens_frontTop.SetReflectivity(refl_none);
    airLens_frontTop.SetName("airLens_frontTop");


    lens_sphere.AddTransform( Transform3D(XYZVector(0, 0, lens_shift)) );
    lens_left.AddTransform( Transform3D(XYZVector(0, 0, lens_shift)) );
    lens_right.AddTransform( Transform3D(XYZVector(0, 0, lens_shift)) );
    lens_bottom.AddTransform( Transform3D(XYZVector(0, 0, lens_shift)) );
    lens_top.AddTransform( Transform3D(XYZVector(0, 0, lens_shift)) );


    PndDrcSurfPolyAsphere  airLens_lens_sphere 	= lens_sphere;
    PndDrcSurfQuadFlatDiff airLens_lens_left 	= lens_left;
    PndDrcSurfQuadFlatDiff airLens_lens_right 	= lens_right;
    PndDrcSurfQuadFlatDiff airLens_lens_bottom 	= lens_bottom;
    PndDrcSurfQuadFlatDiff airLens_lens_top 	= lens_top;

    airLens_lens_sphere.SetName("airLens_lens_sphere");
    airLens_lens_left.SetName("airLens_lens_left");
    airLens_lens_right.SetName("airLens_lens_right");
    airLens_lens_bottom.SetName("airLens_lens_bottom");
    airLens_lens_top.SetName("airLens_lens_top");


    PndDrcSurfPolyFlat airLens_left;
    airLens_left.AddPoint(a3);
    airLens_left.AddPoint(a4);
    airLens_left.AddPoint(b4);
    airLens_left.AddPoint(b3);
//     airLens_left.SetReflectivity(refl_none);
    airLens_left.SetName("airLens_left");

    PndDrcSurfPolyFlat airLens_right;
    airLens_right.AddPoint(a2);
    airLens_right.AddPoint(a1);
    airLens_right.AddPoint(b1);
    airLens_right.AddPoint(b2);
//     airLens_right.SetReflectivity(refl_none);
    airLens_right.SetName("airLens_right");

    PndDrcSurfPolyFlat airLens_bottom;
    airLens_bottom.AddPoint(a2);
    airLens_bottom.AddPoint(a3);
    airLens_bottom.AddPoint(b3);
    airLens_bottom.AddPoint(b2);
//     airLens_bottom.SetReflectivity(refl_none);
    airLens_bottom.SetName("airLens_bottom");

    PndDrcSurfPolyFlat airLens_top;
    airLens_top.AddPoint(a1);
    airLens_top.AddPoint(a4);
    airLens_top.AddPoint(b4);
    airLens_top.AddPoint(b1);
//     airLens_top.SetReflectivity(refl_none);
    airLens_top.SetName("airLens_top");

    PndDrcSurfPolyFlat airLens_exit;
    airLens_exit.AddPoint(b1);
    airLens_exit.AddPoint(b2);
    airLens_exit.AddPoint(b3);
    airLens_exit.AddPoint(b4);
    airLens_exit.SetName("airLens_exit");


    PndDrcOptVol airLens;
    airLens.SetVerbosity(0);
    airLens.AddSurface(airLens_frontLeft);
    airLens.AddSurface(airLens_frontRight);
    airLens.AddSurface(airLens_frontBottom);
    airLens.AddSurface(airLens_frontTop);

    airLens.AddSurface(airLens_lens_sphere);
    airLens.AddSurface(airLens_lens_left);
    airLens.AddSurface(airLens_lens_right);
    airLens.AddSurface(airLens_lens_bottom);
    airLens.AddSurface(airLens_lens_top);

    airLens.AddSurface(airLens_left);
    airLens.AddSurface(airLens_right);
    airLens.AddSurface(airLens_bottom);
    airLens.AddSurface(airLens_top);
    airLens.AddSurface(airLens_exit);
    airLens.SetOptMaterial( (*mat_airBox) );
    airLens.SetName("airLens");

    airLens.AddTransform( rotPhi_fishtank );


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
        airLens_left.SetVerbosity(4);
        airLens_right.SetVerbosity(4);
        airLens_bottom.SetVerbosity(4);
        airLens_top.SetVerbosity(4);
        airLens_exit.SetVerbosity(4);
// 		airLens_exit.SetPixel();
    }



// connect optical devices
//==============================================================================
    PndDrcOptDevSys opt_system;
    opt_system.SetNameCopyNumber("opt_system");
    opt_system.SetVerbosity(0);

    if( opt_woLens )
    {
        opt_system.AddDevice(slab);
        if( airgap > 0 )
            opt_system.AddDevice(airBox);
        opt_system.AddDevice(fishtank);

        if( airgap > 0 )
        {
            opt_system.CoupleDevice("slab","airBox","slab_exit","airBox_front");
            opt_system.CoupleDevice("airBox","fishtank","airBox_exit","fishtank_front");
        }
        else
            opt_system.CoupleDevice("slab","fishtank","slab_exit","fishtank_front");

    }
    else
    {
        opt_system.AddDevice(slab);
        opt_system.AddDevice(lens);
        opt_system.AddDevice(airLens);
        opt_system.AddDevice(fishtank);

        opt_system.CoupleDevice("slab","lens","slab_exit","lens_base");
        opt_system.CoupleDevice("lens","airLens","lens_left","airLens_lens_left");
        opt_system.CoupleDevice("lens","airLens","lens_right","airLens_lens_right");
        opt_system.CoupleDevice("lens","airLens","lens_bottom","airLens_lens_bottom");
        opt_system.CoupleDevice("lens","airLens","lens_top","airLens_lens_top");
        opt_system.CoupleDevice("lens","airLens","lens_sphere","airLens_lens_sphere");
        opt_system.CoupleDevice("airLens","fishtank","airLens_exit","fishtank_front");
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
    // 3. single photon for debugging

    int icnt_measured = 0;
    int icnt_flying   = 0;
    int icnt_lost     = 0;
    int icnt_absorbed = 0;

    int n_iph = 0; // for debugging


// beamtest simulation
//==============================================================================
    if( opt_beamtest )
    {
        double parOriginX = hitBarX - parDirX * spot_limit; // move exterior beam spot particle in bar to bar border
        double parOriginY = hitBarY - parDirY * spot_limit;
        double parOriginZ = hitBarZ - parDirZ * spot_limit;

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
            cout << "particle #" << particleNumber << " of " << particle_number << "  with " << photon_number << " per particle" << endl;


            helper=ortho; // (-1,0,0) is the orthogonal of (0,0,1)
            helper.RotateZ(rand.Rndm()*2*pi);
            gausSmear=rand.Gaus(0,spot_radius);
            if( Abs(gausSmear) > spot_limit ) // beam spot limit
                continue;
            helper2 = TVector3(helper.X()*gausSmear, helper.Y()*gausSmear, helper.Z()*gausSmear);

            if( opt_alongBar )
            {
                helper2.RotateY(inci_theta*degree );
                helper2.RotateZ(inci_phi*degree );
            }
            else
            {
                helper2.RotateY( (-90+inci_theta)*degree ); // rotation in the right direction
                helper2.RotateX(inci_phi*degree );
            }

            spotX = helper2.X()+transX; // translation-(shift)
            spotY = helper2.Y()+transY;
            spotZ = helper2.Z()+transZ;
            spotPos = XYZVector(spotX,spotY,spotZ);


            double stepsToBar;
            if( opt_alongBar )
                stepsToBar = (-spotZ - slab_length) / parDirZ;
            else
                stepsToBar = (spotX - slab_width/2) / Abs(parDirX);

            hitOnBarX = spotX + stepsToBar * parDirX;
            hitOnBarY = spotY + stepsToBar * parDirY;
            hitOnBarZ = spotZ + stepsToBar * parDirZ;


            if( ( !opt_alongBar && spotX < (slab_width / 2) ) || ( opt_alongBar && spotZ > slab_length) ) // should never happen
            {
                cout << spotX << " " << (slab_width / 2) << endl;
                cout << "*** WARN: particle origin production is in the bar (should never happen)" << endl;
                cout << "start pos: (" << spotX << ", " << spotY << ", " << spotZ << ")" << endl;
                cout << "hit pos  : (" << hitOnBarX  << ", " << hitOnBarY  << ", " << hitOnBarZ  << ")" << endl << endl;
                continue;
            }


            double maxZ_fishtank = Sqrt( Power(fishtank_length, 2) + Power(fishtank_width, 2) + Power(fishtank_height, 2) );
            double maxSetupLength = slab_length + airgap + maxZ_fishtank + spot_limit;
            double stepsTo = maxSetupLength / parDirZ  ;

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
                cout << "     range     : " << range << endl << endl;;
            }


            // for beampot plot
            double hitOnBarPlot;
            if( opt_alongBar )
                hitOnBarPlot = hitOnBarX;
            else
                hitOnBarPlot = hitOnBarZ;

            TMarker* t = new TMarker( hitOnBarPlot, hitOnBarY, 20 );

            if( Abs( hitOnBarY ) >= slab_height / 2 )
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

                if( n_posX != n_posY || n_posY != n_posZ )
                {
                    cout << "*** ERROR: photon position list for X,Y and Z has not the same length" << endl;
                    abort();
                }
                else
                    index_pos = n_posX;


                thetaC = (*iph).ThetaC();
                phiC = (*iph).PhiC();

                measured = false;
                absorbed = false;
                lost     = false;

                if ((*iph).Fate()==Drc::kPhotMeasured)
                {
                    icnt_measured++;
                    measured = true;

                    TMarker* t = new TMarker( hitPosX, hitPosY, 7);
                    t->SetMarkerColor( (*iph).ColorNumber((*iph).Wavelength()) );
                    t->SetMarkerSize(0.7);

                    canvas_screen->cd();
                    t->Draw();
                }
                else if( (*iph).Fate()==Drc::kPhotFlying )
                {
                    icnt_flying++; // should never happen.
                    cout << "*** WARN: photon fate is still \"flying\" (should never happen)" << endl;
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

        canvas_beamspot->Write("Beamspot");
        canvas_beamspot->Clear();
    }



// photon cannon
//==============================================================================
    if( opt_photonCannon )
    {
// 		TF1 *f1 = new TF1("f1","1/x",300,700);
        TRandom3 rand;


        if( opt_debug )
            cout << "+++++ DEBUG INFO: photon check" << endl;


        for( double gridX = -slab_width/2; gridX < slab_width/2; gridX += gridXstep)
        {
            if( gridXstep == 0 )
                gridX = 0;

            if( gridX == -slab_width/2 )
                continue;

            for( double gridY = -slab_height/2; gridY < slab_height/2; gridY += gridYstep)
            {
                if( gridYstep == 0 )
                    gridY = 0;

                if( gridY == -slab_height/2 )
                    continue;


                int counter_step = 0;
                int stepFactor = 10000;

                for( int i=0; i<shoots; i++)
                {
                    if( i == stepFactor * counter_step )
                    {
                        counter_step++;
                        cout << "gridX: " << gridX << " gridY: " << gridY << "  photon #" << i+1 << " of " << shoots << " per mesh" << endl;
                    }


// 					double lambda = f1->GetRandom(); // seems to be wrong ; check it later
                    double x1     = 1.0 / lambda_max;
                    double x2     = 1.0 / lambda_min;
                    double x      = rand.Uniform(x1,x2);
                    double lambda = 1.0/x;


                    double costheta = rand.Uniform(0.0, 1.0);
                    double phi = rand.Uniform(0.0, 2*pi);

                    Polar3DVector photDir(1, ACos(costheta), phi); // r, theta, phi
                    photDir = photDir.Unit();
                    XYZVector photDirXYZ( photDir.X(), photDir.Y(), photDir.Z() );

                    double z_offset = -0.01; // to be sure that photon is in bar

                    PndDrcPhoton ph;
                    ph.SetPrintFlag(true);

                    if( opt_debug )
                        ph.SetVerbosity(4);

                    ph.SetReflectionLimit(refl_limit);
                    ph.SetPosition( XYZPoint(gridX, gridY, z_offset));
                    ph.SetDirection(photDirXYZ);
                    ph.SetWavelength(lambda);


                    list<PndDrcPhoton> list_photon;
                    list<PndDrcPhoton>::iterator iph;
                    list_photon.push_back(ph);

                    manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
                    manager->Propagate(); // propagate photons


                    list_photon = manager->PhotonList(); // get list

                    for( iph = list_photon.begin(); iph != list_photon.end(); ++iph )
                    {
                        n_iph++;

                        measured = false;

                        if ((*iph).Fate()==Drc::kPhotMeasured)
                        {
                            icnt_measured++;
                            measured = true;


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

                            if( n_posX != n_posY || n_posY != n_posZ )
                            {
                                cout << "*** ERROR: photon position list for X,Y and Z has not the same length" << endl;
                                abort();
                            }
                            else
                                index_pos = n_posX;


                            TMarker* t = new TMarker( hitPosX, hitPosY, 7);
                            t->SetMarkerColor( (*iph).ColorNumber((*iph).Wavelength()) );
                            t->SetMarkerSize(0.7);

                            canvas_screen->cd();
                            t->Draw();


                            photonTree->Fill();
                        }
                        else if( (*iph).Fate()==Drc::kPhotFlying )
                        {
                            icnt_flying++; // should never happen.
                            cout << "*** WARN: photon fate is still \"flying\" (should never happen)" << endl;
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


// single photon
//==============================================================================
    if( opt_singlePhoton )
    {
        geo.open("geo.tmp",std::ios::out);
        geo << "{" << endl;
        manager->Print(geo);

        PndDrcPhoton ph;
        ph.SetPrintFlag(true);

        if( opt_debug )
            ph.SetVerbosity(4);

        ph.SetReflectionLimit(refl_limit);
        ph.SetPosition( XYZPoint(singlePosX, singlePosY, singlePosZ));
        ph.SetDirection( XYZVector(singleDirX, singleDirY, singleDirZ));
        ph.SetWavelength(single_lambda);

        list<PndDrcPhoton> list_photon;
        list<PndDrcPhoton>::iterator iph;
        list_photon.push_back(ph);

        manager->SetPhotonList(list_photon,"slab","opt_system",0,0);
        manager->Propagate(); // propagate photons


        list_photon.clear();
        list_photon = manager->PhotonList(); // get list

        for( iph = list_photon.begin(); iph != list_photon.end(); ++iph )
        {
            measured = false;
            absorbed = false;
            lost     = false;

            if ((*iph).Fate()==Drc::kPhotMeasured)
            {
                icnt_measured++;
                measured = true;

                TMarker* t = new TMarker( hitPosX, hitPosY, 7);
                t->SetMarkerColor( (*iph).ColorNumber((*iph).Wavelength()) );
                t->SetMarkerSize(0.7);

                canvas_screen->cd();
                t->Draw();
            }
            else if( (*iph).Fate()==Drc::kPhotFlying )
            {
                icnt_flying++; // should never happen.
                cout << "*** WARN: photon fate is still \"flying\" (should never happen)" << endl;
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
        }

        geo << "}" << endl;
        geo.close();
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
