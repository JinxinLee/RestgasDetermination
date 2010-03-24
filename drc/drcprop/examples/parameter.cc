
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
	char slab_material[64], lens_material[64], airBox_material[64], fishtank_material[64];
    Int_t photon_number, particle_number, shoots;
    Int_t refl_limit;
    Double_t lambda_min, lambda_max;
    Double_t lens_radius, lens_thickness, lens_diameter, lens_conical;
    Double_t airgap;
    Double_t slab_width, slab_height, slab_length;
    Double_t fishtank_width, fishtank_height, fishtank_length;
    Double_t fishtank_width_offset, fishtank_height_offset;
    Double_t fishtank_thetaX, fishtank_thetaY, fishtank_phi;
    Double_t particle_mass, particle_kinE, particle_beta;
    Double_t parDirX, parDirY, parDirZ;
    Double_t incidence_theta, incidence_phi;
    Double_t hitBarX, hitBarY, hitBarZ;
    Double_t spot_radius, spot_limit;
    Bool_t slab_fresnel, lens_fresnel, airBox_fresnel, fishtank_fresnel;
	Double_t gridXstep, gridYstep;

	infoTree->SetBranchAddress( "root_version"          , &root_version );
    infoTree->SetBranchAddress( "slab_material"         , &slab_material );
	infoTree->SetBranchAddress( "lens_material"         , &lens_material );
	infoTree->SetBranchAddress( "airBox_material"       , &airBox_material );
	infoTree->SetBranchAddress( "fishtank_material"     , &fishtank_material );
    infoTree->SetBranchAddress( "photon_number"         , &photon_number );
    infoTree->SetBranchAddress( "particle_number"       , &particle_number );
    infoTree->SetBranchAddress( "shoots"                , &shoots );
    infoTree->SetBranchAddress( "refl_limit"            , &refl_limit );
    infoTree->SetBranchAddress( "lambda_min"            , &lambda_min );
    infoTree->SetBranchAddress( "lambda_max"            , &lambda_max );
	infoTree->SetBranchAddress( "lens_radius"           , &lens_radius );
	infoTree->SetBranchAddress( "lens_thickness"        , &lens_thickness );
    infoTree->SetBranchAddress( "lens_diameter"         , &lens_diameter );
    infoTree->SetBranchAddress( "lens_conical"          , &lens_conical );
    infoTree->SetBranchAddress( "airgap"                , &airgap );
	infoTree->SetBranchAddress( "slab_width"            , &slab_width );
	infoTree->SetBranchAddress( "slab_height"           , &slab_height );
	infoTree->SetBranchAddress( "slab_length"           , &slab_length );
	infoTree->SetBranchAddress( "fishtank_width"        , &fishtank_width );
	infoTree->SetBranchAddress( "fishtank_height"       , &fishtank_height );
	infoTree->SetBranchAddress( "fishtank_length"       , &fishtank_length );
    infoTree->SetBranchAddress( "fishtank_width_offset" , &fishtank_width_offset );
    infoTree->SetBranchAddress( "fishtank_height_offset", &fishtank_height_offset );
    infoTree->SetBranchAddress( "fishtank_thetaX"       , &fishtank_thetaX );
    infoTree->SetBranchAddress( "fishtank_thetaY"       , &fishtank_thetaY );
    infoTree->SetBranchAddress( "fishtank_phi"          , &fishtank_phi );
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
    infoTree->SetBranchAddress( "slab_fresnel"          , &slab_fresnel );
    infoTree->SetBranchAddress( "lens_fresnel"          , &lens_fresnel );
    infoTree->SetBranchAddress( "airBox_fresnel"        , &airBox_fresnel );
    infoTree->SetBranchAddress( "fishtank_fresnel"      , &fishtank_fresnel );
	infoTree->SetBranchAddress( "gridXstep"             , &gridXstep );
	infoTree->SetBranchAddress( "gridYstep"             , &gridYstep );

    infoTree->GetEntry( 0 );


//==============================================================================
// Print parameters
//==============================================================================
    Bool_t opt_woLens = false;
    Bool_t opt_photonCannon = false;

    if( lens_material == "" )
        opt_woLens = true;
    if( photon_number == -666 )
        opt_photonCannon = true;


    if( opt_photonCannon )
        cout << "*** Photon Cannon ***" << endl;
    else
        cout << "*** Beamtest simulation ***" << endl;


    cout << "  material:" << endl;

    cout <<     "    slab:     " << slab_material << endl;
    if( !opt_woLens )
        cout << "    lens:     " << lens_material << endl;
    if( airgap > 0 )
        cout << "    airBox:   " << airBox_material << endl;
    cout <<     "    fishtank: " << fishtank_material << endl;


    cout << "  fresnel:" << endl;

    cout <<     "    slab:     " << slab_fresnel << endl;
    if( !opt_woLens )
        cout << "    lens:     " << lens_fresnel << endl;
    if( airgap > 0 )
        cout << "    airBox:   " << airBox_fresnel << endl;
    cout <<     "    fishtank: " << fishtank_fresnel << endl;


    cout << "  dimensions [mm]:" << endl;

    cout << "    slab width:  " << slab_width << endl;
    cout << "    slab heigth: " << slab_height << endl;
    cout << "    slab length: " << slab_length << endl;

    if( !opt_woLens )
    {
        cout << "    lens radius:         " << lens_radius << endl;
        cout << "    lens thickness:      " << lens_thickness << endl;
        cout << "    lens diameter:       " << lens_diameter << endl;
        cout << "    lens conical const.: " << lens_conical << " [dim-less]" << endl;
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
            cout << "    photon number:    " << shoots << endl;
        else
        {
            cout << "    photon number:    " << shoots << " per mesh" << endl;
            cout << "    grid const. X:    " << gridXstep << " mm" << endl;
            cout << "    grid const. Y:    " << gridYstep << " mm" << endl;
        }
        cout <<     "    reflection limit: " << refl_limit << endl;
    }
}
