/*
 * Pndcpp
 *
 * this class gives you methods to retrieve parameters of the geometry
 *
 *  Created on: Oct 5, 2012
 *      Author: promme
 */

#include <PndLmdDim.h>
#include<TGeoMatrix.h>
#include <stdlib.h>
// //work with DB
// #include<PndLmdContFact.h>
// #include<TList.h>
// #include<PndLmdAlignPar.h>
PndLmdDim* PndLmdDim::pinstance = 0;


#include <TROOT.h>
PndLmdDim::PndLmdDim()
{
	double test_mult_fact = 1.; //100.; // should be 1 when not debuggin code
	// pi
	pi = 3.141592654;
	// number of detector planes
	n_planes = 4;
	// number of modules per plane half
	nmodules = 5;
	// position of planes where the first plane defines the origin
	plane_pos_z = new double[4];
	plane_pos_z[0] = 0.0; plane_pos_z[1] = 20.0; plane_pos_z[2] = 30.0; plane_pos_z[3] = 40.0;
	half_offset_x = 0.005*test_mult_fact; // 50 mum
	half_offset_y = 0.005*test_mult_fact;
	half_offset_z = 0.005*test_mult_fact;
	half_tilt_phi = 0.; // negligible
	half_tilt_theta = 0.;
	half_tilt_psi = 0.;
	plane_half_offset_x = 0.005*test_mult_fact;
	plane_half_offset_y = 0.005*test_mult_fact;
	plane_half_offset_z = 0.005*test_mult_fact;
	plane_half_tilt_phi = 0.0; // negligible
	plane_half_tilt_theta = 0.0;
	plane_half_tilt_psi = 0.0;
	cvd_offset_x = 0.005*test_mult_fact;
	cvd_offset_y = 0.005*test_mult_fact;
	cvd_offset_z = 0.0;
	cvd_tilt_phi = 0.001*test_mult_fact;
	cvd_tilt_theta = 0.;
	cvd_tilt_psi = 0.;
	side_offset_x = 0.005*test_mult_fact;
	side_offset_y = 0.005*test_mult_fact;
	side_offset_z = 0.; // do not use! -> clashing volumes
	side_tilt_phi = 0.; // do not use! -> clashing volumes
	side_tilt_theta = 0.;
	side_tilt_psi = 0.; // do not use! -> clashing volumes
	// cvd_diamond is cut out of 79.5 mm discs of 200 micron thickness
	// inner min. radius due to beam pipe + a safety margin
	inner_rad = 3.7;
	// not used yet but should be the outer acceptance
	outer_rad = 12.;
	// number of CVD diamond discs per plane
	n_cvd_discs = 10;
	// radius of a CVD diamond disc
	cvd_disc_rad = 7.95/2.;
	// the half of the diamond thickness
	cvd_disc_thick_half = 0.01;
	// even and odd discs in a plane will be shifted in z in order to prevent
	// mechanical damage during assembly
	cvd_disc_even_odd_offset = 0.25;
	// angle from the division of a circle into n_cvd_discs
	delta_phi = 2.*pi / ((double) (n_cvd_discs));
	// a polygon of n_cvd_discs sides fitting a radius of inner_rad
	// has a side length pol_side_lg of
	pol_side_lg_half = inner_rad * sin(delta_phi / 2.);
	// the minimum distance to the center of the polygone is given by
	pol_side_dist_min = inner_rad * cos(delta_phi / 2.);
	// the cvd disc has to be placed such that the disc crosses
	// the inner ring at an angle of 0 and delta_phi
	// this defines the distance to the center according to pythagoras
	cvd_disc_dist = pol_side_dist_min + sqrt(
		cvd_disc_rad * cvd_disc_rad - pol_side_lg_half * pol_side_lg_half);

	kapton_disc_thick_half = 0.0025; // by now 50 mu kapton

	maps_n_col = 3;
	maps_n_row = 2;
	// enabled [row][col]
	enabled = new bool*[maps_n_row];
	enabled[0] = new bool[maps_n_col];
	enabled[1] = new bool[maps_n_col];
	enabled[0][0] = true;
	enabled[0][1] = true;
	enabled[0][2] = true;
	enabled[1][0] = false;
	enabled[1][1] = true;
	enabled[1][2] = true;
	n_sensors = 5;
	// NOTE: MOST of the following VARIABLES are HALF of it
	// due to geometry construction in GEANT
	maps_thickness      = 0.005/ 2.;
	maps_passive_top    = 0.01 / 2.;//*12000.;
	maps_passive_bottom = 0.05  / 2.;
	maps_passive_left   = 0.01 / 2.;//*6000.;
	maps_passive_right  = 0.01 / 2.;//*3000.;
	maps_active_width   = 2.0  / 2. - maps_passive_left - maps_passive_right;
	maps_active_height  = 2.0  / 2. - maps_passive_top  - maps_passive_bottom;

	maps_width  = maps_passive_left + maps_active_width  + maps_passive_right ;
	maps_height = maps_passive_top  + maps_active_height + maps_passive_bottom;

	maps_active_offset_x   = (maps_passive_left - maps_passive_right )/2.;
	maps_active_offset_y   = (maps_passive_top  - maps_passive_bottom)/2.;


	die_gap = 0.1; // (cm)

	maps_die_width  = maps_width  * maps_n_col;
	maps_die_height = maps_height * maps_n_row + die_gap * maps_n_row - 1;

	die_offset_x = 0.;
	die_offset_y = 0.;
	die_offset_z = 0.;
	die_tilt_phi = 0.;
	die_tilt_theta = 0.;
	die_tilt_psi = 0.;
	//*********************************** lumi box parameters ***********************************
	// see CAD files for details
	// https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
	// width
	box_size_x = 36./2.;
	// height
	box_size_y = 60.6/2.;
	// length
	box_size_z = 95.6/2.;
	// thickness of the V2A steel plates
	box_thickness = 0.5;
	// position of the inner rib
	pos_rib = 36.4;
	// beam pipe radius at entrance
	rad_entrance = 9. + 1.;
	// beam pipe radius at exit
	rad_exit = 4.5;
	// beam pipe separating non interacting paricles
	rad_pipe = 3.5;
	// beam pipe thickness;
	pipe_thickness = 0.1;
	// cone height of the transition region
	length_transision = 36.4;
	// length of the inner pipe
	length_pipe = 50.;
	//*********************************** global parameters *************************************
	// where bending starts with
	end_seg_upstream = 361;
	// the bending radius
	r_bend = 5700.;
	// and the angle of the circle path
	phi_bend = 40.0e-3;;
	// the point where both tangents of the straight beam pipe tubes meet is
	pos_rot_z = end_seg_upstream + tan(phi_bend/2.)*r_bend;
	// z position of the lmd box
	pos_z = 1050.; //(cm)
	// position of the first detector plane
	pos_plane_0 = 74.1;
	end_seg_bend = end_seg_upstream+sin(phi_bend)*r_bend;
	// x position of the lmd
	pos_x =
			( pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);
	// y position of the lmd
	pos_y = 0.;
	rot_x = 0.;
	rot_y = phi_bend;
	rot_z = 0.;

	// later transformations will require a relative
	// navigation structure based on strings
	// the path to individual volumes is stored here

	// luminosity detector top box
	nav_paths.push_back("lmd_vol_vac");
	// luminosity detector reference system
	nav_paths.push_back("lmd_vol_ref_sys");
	// luminosity detector halfs with the
	// number 0 for top and 1 for bottom
	nav_paths.push_back("lmd_vol_half_");
	// luminosity detector plane 0 to 3
	nav_paths.push_back("lmd_vol_plane_");
	// luminosity detector module 0 to 5
	// clockwise around direction upstream (z)
	nav_paths.push_back("lmd_vol_module_");
	// luminosity detector front side (upstream = 0)
	// and backside (downstream = 1)
	nav_paths.push_back("lmd_vol_side_");
	// luminosity detector die
	// ( 0 for the 1x3 sensors and 1 for the 1x2 sensors )
	nav_paths.push_back("lmd_vol_die_");
	// luminosity detector sensor
	//nav_paths.push_back("lmd_vol_sensor_"); misalignment of individual sensors is not supportet yet
	// luminosity detector active sensor
	// 0 is the most inner sensor
	nav_paths.push_back("LumActivePixelRect_");

	fgGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	if (!fgGeoMan) cout << "Error: could not find the geometry manager!" << endl;
}



PndLmdDim::PndLmdDim(const PndLmdDim & instance)
{
	if (!instance.box_thickness) cout << " fix for pedantic compiler will never appear on cout " << endl;
}



PndLmdDim::~PndLmdDim()
{
	cout << endl;
	cout << " Cleaning up PndLmdDim " << endl;
	cout << " If you see that message several times please check the performance of your code! " << endl;
	Cleanup();
	delete pinstance;
}



PndLmdDim & PndLmdDim::Get_instance()
{
	if (!pinstance){
		pinstance = new PndLmdDim();
	}
	return (*pinstance);
}

PndLmdDim* PndLmdDim::Instance(){
	if (!pinstance){
		pinstance = new PndLmdDim();
	}
	return (pinstance);
}


/*
void PndLmdDim::transform_sensor_local_to_lmd_local(const int sensor_id, double & x, double & y, double & z, bool misaligned)
{
}



void PndLmdDim::transform_local_lmd_to_global(const int sensor_id, double & x, double & y, double & z, bool misaligned)
{
}



void PndLmdDim::transform_local_sensor()
{
}*/

#include<FairGeoLoader.h>
#include<FairGeoInterface.h>
#include<FairGeoMedia.h>
#include<FairGeoBuilder.h>
#include<TGeoTorus.h>
#include<TGeoSphere.h>
#include<TGeoTube.h>
#include<TGeoCompositeShape.h>
#include<TGeoCone.h>
#include<TGeoBBox.h>

void PndLmdDim::Generate_rootgeom(TGeoVolume& mothervol, bool misaligned){
	Cleanup();
	FairGeoLoader* geoLoad =  FairGeoLoader::Instance();
	if (!geoLoad){
		geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
		cout << " creating FairGeoLoader instance " << endl;
	}
	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	string dir = getenv("VMCWORKDIR");
	geoFace->setMediaFile((dir+"/geometry/media_pnd.geo").c_str());//("${VMCWORKDIR}/geometry/media_pnd.geo");
	geoFace->readMedia();
	//geoFace->print();

	FairGeoMedia *Media = geoFace->getMedia();
	FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

	// retrieve available media
	FairGeoMedium* FairMediumAir = Media->getMedium("air");
	FairGeoMedium* FairMediumSteel = Media->getMedium("steel");
	FairGeoMedium* FairMediumAl = Media->getMedium("Aluminum");
	FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");
	FairGeoMedium *FairMediumDiamond = Media->getMedium("HYPdiamond");
	FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum");
	FairGeoMedium *FairMediumKapton = Media->getMedium("kapton");

	if (!FairMediumAir || !FairMediumSteel || !FairMediumAl || !FairMediumKapton || !FairMediumSilicon || !FairMediumVacuum) {
		std::cout << " Error: not all media found " << std::endl;
		return;
	}

	int nmed;
	nmed = geobuild->createMedium(FairMediumAir);
	nmed = geobuild->createMedium(FairMediumSteel);
	nmed = geobuild->createMedium(FairMediumAl);
	nmed = geobuild->createMedium(FairMediumKapton);
	nmed = geobuild->createMedium(FairMediumSilicon);
	nmed = geobuild->createMedium(FairMediumDiamond);
	nmed = geobuild->createMedium(FairMediumVacuum);
	if (!nmed) cout << " fix for pedantic compiler line " << endl;

	// no translation nor rotation
	TGeoRotation* rot_no = new TGeoRotation("rot_no", 0., 0., 0.); // no rotation
	TGeoCombiTrans* rottrans_no = new TGeoCombiTrans("rottrans_no", 0., 0.,
				0., rot_no);
	rottrans_no->RegisterYourself();

	// ************ create the luminosity monitor box ***********
	// create the bounding box
	double tube_upstream_length = 33.3/2.;
	double tube_upstream_rad_out = 9. + 1.;
	double tube_upstream_rad_in = tube_upstream_rad_out - 0.2;

	// create a vacuum around the luminosity detector
	double lmd_total_length = box_size_z + tube_upstream_length;

	double origin[3] = {0.,0., lmd_total_length};
	TGeoBBox* lmd_box_vac =
			new TGeoBBox("lmd_box_vac",
					box_size_x,
					box_size_y,
					lmd_total_length, origin);
	TGeoVolume *lmd_vol_vac = new TGeoVolume(nav_paths[0].c_str(), lmd_box_vac,
					fgGeoMan->GetMedium("vacuum"));
	//vol_lmd_vac->SetTransparency(20);
	lmd_vol_vac->SetLineColor(3);
	double x, y, z, rottheta, rotphi, rotpsi;
	Get_pos_lmd_global(x, y, z, rottheta, rotphi, rotpsi);
	TGeoRotation* lmd_rot = new TGeoRotation("lmd_rot");
	lmd_rot->RotateX(rottheta/pi*180.);
	lmd_rot->RotateY(rotphi/pi*180.);
	lmd_rot->RotateZ(rotpsi/pi*180.);
	TGeoCombiTrans* lmd_transrot = new TGeoCombiTrans(x, y, z, lmd_rot);
	lmd_transrot->SetName("lmd_transrot");
	lmd_transrot->RegisterYourself();
	// pumping station upstream
	TGeoTube* pipe_upstream =
			new TGeoTube("pipe_upstream", tube_upstream_rad_in, tube_upstream_rad_out, tube_upstream_length);
	TGeoCombiTrans* comb_trans_pipe_upstream = new TGeoCombiTrans("comb_trans_pipe_upstream",
			0., 0., tube_upstream_length, rot_no);
	//comb_trans_pipe_upstream->RegisterYourself();
	TGeoVolume* lmd_vol_pipe_up =
			new TGeoVolume("lmd_vol_pipe_up", pipe_upstream, fgGeoMan->GetMedium("steel"));
	lmd_vol_pipe_up->SetLineColor(11);
	lmd_vol_vac->AddNode(lmd_vol_pipe_up, 0, comb_trans_pipe_upstream);
	// the lmd box
	TGeoBBox* lmd_box_outer
		= new TGeoBBox("lmd_box_outer",
				box_size_x , box_size_y  , box_size_z );
	TGeoBBox* lmd_box_inner
		= new TGeoBBox("lmd_box_inner",
				box_size_x - box_thickness,
				box_size_y - box_thickness,
				box_size_z - box_thickness);
	TGeoBBox* lmd_box_rib
		= new TGeoBBox("lmd_box_rib",
			box_size_x - box_thickness,
			box_size_y - box_thickness,
			box_thickness / 2.);
	TGeoTube* box_hole_upstream =
			new TGeoTube("box_hole_upstream", 0.0, rad_entrance, box_thickness);
	// move the cut pipe upstream
	TGeoCombiTrans* comb_trans_cut_pipe_upstream = new TGeoCombiTrans("comb_trans_cut_pipe_upstream",
			0., 0., -box_size_z+box_thickness/2., rot_no);
	comb_trans_cut_pipe_upstream->RegisterYourself();
	TGeoTube* box_hole_downstream =
				new TGeoTube("box_hole_downstream", 0.0, rad_exit, box_thickness);
	// move the cut pipe downstream
	TGeoCombiTrans* comb_trans_cut_pipe_downstream = new TGeoCombiTrans("comb_trans_cut_pipe_downstream",
				0., 0., +box_size_z-box_thickness/2., rot_no);
	comb_trans_cut_pipe_downstream->RegisterYourself();
	// move rib upstream
	TGeoCombiTrans* comb_trans_rib = new TGeoCombiTrans("comb_trans_rib",
				0., 0., -box_size_z+pos_rib, rot_no);
	comb_trans_rib->RegisterYourself();
	// compose all the parts into one luminosity vacuum box
	TGeoCompositeShape *shape_lmd_box = new TGeoCompositeShape("shape_lmd_box",
			"(lmd_box_outer-lmd_box_inner + ((lmd_box_rib-box_hole_upstream):comb_trans_rib))-box_hole_upstream:comb_trans_cut_pipe_upstream-box_hole_downstream:comb_trans_cut_pipe_downstream");
	TGeoVolume *lmd_vol_box = new TGeoVolume("lmd_vol_box", shape_lmd_box,
				fgGeoMan->GetMedium("steel"));
	lmd_vol_box->SetLineColor(11);
	//lmd_vol_box->SetVisibility(false);//TEST
	TGeoCombiTrans* comb_trans_lmd_box = new TGeoCombiTrans("comb_trans_lmd_box",
			0., 0., 2*tube_upstream_length+box_size_z, rot_no);
	//comb_trans_pipe_upstream->RegisterYourself();
	lmd_vol_vac->AddNode(lmd_vol_box, 0, comb_trans_lmd_box);

	//	TGeoTube* lmd_flange_upstr = new TGeoTube(
	//			"lmd_flange_upstr", 9.2, 25.3/2., 1.2);
	//	TGeoCombiTrans* lmd_trans_fl_up = new TGeoCombiTrans("lmd_trans_fl_up", 0., 0., 1.2+delta, r1);
	//	lmd_trans_fl_up->RegisterYourself();
		// upstream flange holding the kapton cone
	//	TGeoTube* lmd_cone_flange_upstr = new TGeoTube(
	//			"lmd_cone_flange_upstr", 9.2, 25.3/2., 1.5);
	//	TGeoCombiTrans* lmd_trans_co_fl_up = new TGeoCombiTrans("lmd_trans_co_fl_up", 0., 0., -1.5+50.-delta, r1);
	//	lmd_trans_co_fl_up->RegisterYourself();

	// 20 mu thick kapton foil cone
	double cone_height = 32./2.;
	double cone_r_in_upstream = 24.4/2.;
	double cone_r_in_downstream = 7./2.;
	double cone_thickness = 0.002;
	TGeoCone* lmd_capton_cone = new TGeoCone("lmd_capton_cone",
			cone_height, cone_r_in_upstream,
			cone_r_in_upstream+cone_thickness/2.,
			cone_r_in_downstream, cone_r_in_downstream+cone_thickness/2.);
	TGeoCombiTrans* lmd_trans_cap_co = new TGeoCombiTrans("lmd_trans_cap_co", 0., 0., 2*tube_upstream_length+box_thickness + cone_height, rot_no);
	lmd_trans_cap_co->RegisterYourself();
	TGeoVolume *vlum_CaptonCone = new TGeoVolume("vlum_CaptonCone", lmd_capton_cone,
			fgGeoMan->GetMedium("kapton"));
	vlum_CaptonCone->SetLineColor(kRed);//39);
	lmd_vol_vac->AddNode(vlum_CaptonCone, 0, lmd_trans_cap_co);//TEST without cone!!!
	// 10 mu thick kapton foil aluminum coating
	cone_r_in_upstream = cone_r_in_upstream+cone_thickness;
	cone_r_in_downstream = cone_r_in_downstream+cone_thickness;
	cone_thickness = 0.001;
	TGeoCone* lmd_al_cone = new TGeoCone("lmd_al_cone",
			cone_height, cone_r_in_upstream,
			cone_r_in_upstream+cone_thickness/2.,
			cone_r_in_downstream, cone_r_in_downstream+cone_thickness/2.);
	//TGeoCombiTrans* lmd_trans_cap_co = new TGeoCombiTrans("lmd_trans_cap_co", 0., 0., 2*tube_upstream_length+box_thickness + cone_height, rot_no);
	//lmd_trans_cap_co->RegisterYourself();
	TGeoVolume *vlum_AlCone = new TGeoVolume("vlum_AlCone", lmd_al_cone,
			fgGeoMan->GetMedium("Aluminum"));
	vlum_AlCone->SetLineColor(kGray);//39);
	lmd_vol_vac->AddNode(vlum_AlCone, 0, lmd_trans_cap_co);
	// beam pipe to shield the sensors
	double pipe_inner_r_in = 7./2.;
	double pipe_inner_length = 50./2.;
	//double pipe_thickness = 0.1;
	TGeoTube* lmd_beam_pipe = new TGeoTube("lmd_beam_pipe", pipe_inner_r_in, pipe_inner_r_in + pipe_thickness, pipe_inner_length);
	TGeoCombiTrans* lmd_trans_p = new TGeoCombiTrans("lmd_trans_p", 0., 0., 2*tube_upstream_length+box_thickness + 2*cone_height + pipe_inner_length, rot_no);
	lmd_trans_p->RegisterYourself();
	TGeoVolume *vlum_trans_p = new TGeoVolume("vlum_trans_p", lmd_beam_pipe,
				fgGeoMan->GetMedium("steel"));
	lmd_vol_vac->AddNode(vlum_trans_p, 0, lmd_trans_p);
	// beam pipe cone downstream
	double cone_p_height = 10./2.;
	double cone_p_r_in_upstream = pipe_inner_r_in;
	double cone_p_r_in_downstream = 9./2.;
	double cone_p_thickness = 0.2;
	TGeoCone* lmd_cone_downstr = new TGeoCone("lmd_cone_downstr", cone_p_height, cone_p_r_in_upstream,
			cone_p_r_in_upstream+cone_p_thickness/2.,
			cone_p_r_in_downstream, cone_p_r_in_downstream+cone_p_thickness/2.);
	TGeoCombiTrans* lmd_trans_co_do = new TGeoCombiTrans("lmd_trans_co_do", 0., 0.,
			2*tube_upstream_length+box_thickness + 2*cone_height + 2*pipe_inner_length + cone_p_height, rot_no);
	lmd_trans_co_do->RegisterYourself();
	TGeoVolume *vlum_pipe_inner_cone = new TGeoVolume("vlum_pipe_inner_cone", lmd_cone_downstr,
				fgGeoMan->GetMedium("steel"));
	lmd_vol_vac->AddNode(vlum_pipe_inner_cone, 0, lmd_trans_co_do);
		// flange holding the kapton cone downstream
	//	TGeoCone* lmd_cone_flange_downstr = new TGeoCone("lmd_cone_flange_downstr", 3.12/2., 3.1, 8.6/2., 3.1, 3.2);
	//	TGeoCombiTrans* lmd_trans_co_fl_do = new TGeoCombiTrans("lmd_trans_co_fl_do", 0., 0., 50.+23.386+3.12/2., r1);
	//	lmd_trans_co_fl_do->RegisterYourself();
		// beam pipe to shield the sensors
	//	TGeoTube* lmd_beam_pipe = new TGeoTube("lmd_beam_pipe", 3.5, 3.6, 50./2.);
	//	TGeoCombiTrans* lmd_trans_p = new TGeoCombiTrans("lmd_trans_p", 0., 0., 50.+23.386+50./2., r1);
	//	lmd_trans_p->RegisterYourself();
		// beam pipe cone downstream
	//	TGeoCone* lmd_cone_downstr = new TGeoCone("lmd_cone_downstr", 20./2., 3.5, 3.7, 9./2., 9.2/2.);
	//	TGeoCombiTrans* lmd_trans_co_do = new TGeoCombiTrans("lmd_trans_co_do", 0., 0., 50.+23.386+50.+20./2., r1);
	//	lmd_trans_co_do->RegisterYourself();
		// beam pipe downstream
	//	TGeoTube* lmd_beam_pipe_downstream = new TGeoTube("lmd_beam_pipe_downstream", 9./2., 9.2/2., 56./2.);
	//	TGeoCombiTrans* lmd_trans_p_down = new TGeoCombiTrans("lmd_trans_p_down", 0., 0., 50.+23.386+50.+20.+56./2., r1);
	//	lmd_trans_p_down->RegisterYourself();*/

	// ****************************** luminosity detector reference system ************************
	// definition of the luminosity detector local system
	TGeoCombiTrans* rottrans_lmd_in_box = new TGeoCombiTrans("rottrans_lmd_in_box", 0., 0.,
			pos_plane_0, rot_no);
	TGeoVolumeAssembly* lmd_vol_ref_sys = new TGeoVolumeAssembly(nav_paths[1].c_str());
	lmd_vol_vac->AddNode(lmd_vol_ref_sys, 0, rottrans_lmd_in_box);
	// definition of the retractable luminosity detector halves

	// ****************************** cvd cooling support discs ************************
	// the cvd disc shape
	TGeoTube* shape_cvd_disc = new TGeoTube("shape_cvd_disc", 0.,
			cvd_disc_rad, cvd_disc_thick_half);
	// The inner beam pipe defines the inner acceptance region for the cvd cut_out
	TGeoTube* shape_cvd_cutout_inner = new TGeoTube("shape_cvd_cutout_inner",
			0., inner_rad, 1.);
	// finally cvd discs will be cut at the left and right down to 36 degree in phi
	// for that we subtract tube segments
	TGeoTubeSeg* shape_cvd_disc_cut_side = new TGeoTubeSeg(
			"shape_cvd_disc_cut_side", 0., outer_rad, 1.,
			+delta_phi / 2. / pi * 180.,
			-delta_phi / 2. / pi * 180.);
	// before: cvd disc was moved to the displaced position around the z axis
	// now: segments for the cut are moved off centered and cvd disc remains in the center
	TGeoRotation* cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
	TGeoTranslation* cvd_translation = new TGeoTranslation("cvd_translation",
			-cvd_disc_dist, 0, 0);
	TGeoCombiTrans* cvd_combtrans = new TGeoCombiTrans(*cvd_translation,
			*cvd_rotation);
	cvd_combtrans->SetName("cvd_combtrans");
	cvd_combtrans->RegisterYourself();
	TGeoCompositeShape
			*shape_cvd_support =
					new TGeoCompositeShape(
							"shape_cvd_support",
							"(shape_cvd_disc-shape_cvd_cutout_inner:cvd_combtrans-shape_cvd_disc_cut_side:cvd_combtrans)");

	TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
							shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
	lmd_vol_cvd_disc->SetLineColor(9);
	// ****************************** kapton flexible circuits to the sensors ************************
	// the cvd disc shape
	TGeoTube* shape_kapton_disc = new TGeoTube("shape_kapton_disc", 0.,
			cvd_disc_rad, kapton_disc_thick_half);
	// The inner beam pipe defines the inner acceptance region for the kapton cut_out
	TGeoTube* shape_kapton_cutout_inner = new TGeoTube("shape_kapton_cutout_inner",
			0., inner_rad, 1.);
	// finally kapton discs will be cut at the left and right down to 36 degree in phi
	// for that we subtract tube segments
	TGeoTubeSeg* shape_kapton_disc_cut_side = new TGeoTubeSeg(
			"shape_kapton_disc_cut_side", 0., outer_rad, 1.,
			+delta_phi / 2. / pi * 180.,
			-delta_phi / 2. / pi * 180.);
	// before: kapton disc was moved to the displaced position around the z axis
	// now: segments for the cut are moved off centered and kapton disc remains in the center
	TGeoRotation* kapton_rotation = new TGeoRotation("kapton_rotation", 0, 0, 0);
	TGeoTranslation* kapton_translation = new TGeoTranslation("kapton_translation",
			-cvd_disc_dist, 0, 0);
	TGeoCombiTrans* kapton_combtrans = new TGeoCombiTrans(*kapton_translation,
			*kapton_rotation);
	kapton_combtrans->SetName("kapton_combtrans");
	kapton_combtrans->RegisterYourself();
	TGeoCompositeShape
			*shape_kapton_support =
					new TGeoCompositeShape(
							"shape_kapton_support",
							"(shape_kapton_disc-shape_kapton_cutout_inner:kapton_combtrans-shape_kapton_disc_cut_side:kapton_combtrans)");

	TGeoVolume* lmd_vol_kapton_disc = new TGeoVolume("lmd_vol_kapton_disc",
							shape_kapton_support, fgGeoMan->GetMedium("kapton"));
	lmd_vol_kapton_disc->SetLineColor(kRed);
	//	lmd_vol_kapton_disc->SetVisibility(false);
	// *********************************** HV-MAPS *************************************

	// create basic shapes and their positions
	TGeoBBox *shape_maps_active_centered = new TGeoBBox(
			"shape_maps_active_centered", maps_active_width,
			maps_active_height, maps_thickness);
	TGeoCombiTrans* combtrans_maps_active = new TGeoCombiTrans(
			"combtrans_maps_active",
			-maps_width + maps_passive_left * 2.
					+ maps_active_width,
			-maps_height + maps_passive_bottom * 2.
					+ maps_active_height, 0., rot_no);
	combtrans_maps_active->RegisterYourself();
	TGeoBBox *shape_maps_passive_left = new TGeoBBox("shape_maps_passive_left",
			maps_passive_left, maps_height,
			maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_left = new TGeoCombiTrans(
			"combtrans_maps_passive_left",
			-maps_width + maps_passive_left, 0., 0., rot_no);
	combtrans_maps_passive_left->RegisterYourself();
	TGeoBBox *shape_maps_passive_right = new TGeoBBox(
			"shape_maps_passive_right", maps_passive_right,
			maps_height, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_right = new TGeoCombiTrans(
			"combtrans_maps_passive_right",
			maps_width - maps_passive_right, 0., 0., rot_no);
	combtrans_maps_passive_right->RegisterYourself();
	TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top",
			maps_width, maps_passive_top,
			maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_top = new TGeoCombiTrans(
			"combtrans_maps_passive_top", 0.,
			maps_height - maps_passive_top, 0., rot_no);
	combtrans_maps_passive_top->RegisterYourself();
	TGeoBBox *shape_maps_passive_bottom = new TGeoBBox(
			"shape_maps_passive_bottom", maps_width,
			maps_passive_bottom, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_bottom = new TGeoCombiTrans(
			"combtrans_maps_passive_bottom", 0.,
			-maps_height + maps_passive_bottom, 0., rot_no);
	combtrans_maps_passive_bottom->RegisterYourself();

	if (!lmd_box_outer || !lmd_box_inner || !lmd_box_rib || !box_hole_upstream || !box_hole_downstream || !shape_cvd_disc || !shape_cvd_cutout_inner || !shape_cvd_disc_cut_side || !shape_kapton_disc || !shape_kapton_cutout_inner || !shape_kapton_disc || !shape_kapton_cutout_inner || !shape_kapton_disc_cut_side || !shape_maps_active_centered || !shape_maps_passive_left || !shape_maps_passive_right || !shape_maps_passive_top || !shape_maps_passive_bottom)
		cout << " pedantic compiler together with root geometries sucks " << endl;

	TGeoCompositeShape
			*shape_maps_passive =
					new TGeoCompositeShape(
							"shape_maps_passive",
							"(shape_maps_passive_top:combtrans_maps_passive_top+shape_maps_passive_right:combtrans_maps_passive_right+shape_maps_passive_bottom:combtrans_maps_passive_bottom+shape_maps_passive_left:combtrans_maps_passive_left)");

	TGeoCompositeShape
			*shape_maps_active =
					new TGeoCompositeShape("shape_maps_active",
							"(shape_maps_active_centered:combtrans_maps_active-shape_maps_passive)");

	TGeoVolume* _vol_passive =
			new TGeoVolume(
					"LumPassiveRect_",
					shape_maps_passive,
					fgGeoMan->GetMedium("silicon"));
	_vol_passive->SetLineColor(30);

	TGeoVolume* _vol_active =
			new TGeoVolume(
					nav_paths[7].c_str(),
					shape_maps_active,
					fgGeoMan->GetMedium("silicon"));
	_vol_active->SetLineColor(36);
	// **************************************************************

	// ****************************** loops in the luminosity detector ************************
	stringstream name;
	stringstream uniqueid; // seems pandaroot has problems when volumes are not uniquely named
	double _x(0), _y(0), _z(0), _rotphi(0), _rottheta(0), _rotpsi(0);
	double _offset_x(0), _offset_y(0), _offset_z(0),
		_offset_phi(0), _offset_theta(0), _offset_psi(0);
	unsigned int sensor_id(0);
	unsigned int module_id(0);
	for (unsigned int ihalf = 0; ihalf < 2; ihalf++){ // loop over detector halves
		// in order do be able to displace the detector halves those are introduced as
		// separate volume assemblies
		name.str("");
		name << nav_paths[2] << ihalf;
		uniqueid.str("");
		uniqueid << "_" << ihalf;
		TGeoVolumeAssembly* lmd_vol_half_ = new TGeoVolumeAssembly(name.str().c_str());
		//mothervol.AddNode(lmd_vol_half_, 0, rottrans_no);
		for (unsigned int iplane = 0; iplane < n_planes; iplane++){ // loop over planes
			name.str("");
			name << nav_paths[3] << iplane;
			uniqueid.str("");
			uniqueid << "_" << ihalf << iplane;
			TGeoVolumeAssembly* lmd_vol_plane_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
			// move to the position of the corresponding plane
			TGeoMatrix* rottrans_plane = new TGeoCombiTrans(0., 0.,
					plane_pos_z[iplane], rot_no);
			if (misaligned){
				Get_offset(ihalf, iplane, -1, -1, -1, -1,
						_offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
				TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
						_offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
				TGeoCombiTrans* rottrans_plane_offset =
						new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
				//	rottrans_plane = new TGeoHMatrix(*rottrans_plane * *rottrans_plane_offset);
				rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);
			}
			for (unsigned int imodule = 0; imodule < nmodules; imodule++){ // loop over modules
				name.str("");
				name << nav_paths[4] << imodule;
				uniqueid.str("");
				uniqueid << "_" << ihalf << iplane << imodule;
				TGeoVolumeAssembly* lmd_vol_module_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
				double angle = delta_phi/2.+ihalf*pi+imodule*delta_phi;
				double add_z = cvd_disc_even_odd_offset;
				// the offset of the modules in the upper and lower halfs
				// are opposite
				if (((imodule+ihalf)%2)==0) add_z = -add_z;
				_x = cos(angle)*cvd_disc_dist;
				_y = sin(angle)*cvd_disc_dist;
				_z = add_z;
				_rotphi = 0.;
				_rottheta = 0.;
				_rotpsi = angle/pi*180.;
				TGeoRotation* rot_module = new TGeoRotation("rot_module", _rotphi, _rottheta, _rotpsi);
				TGeoMatrix* rottrans_module = new TGeoCombiTrans(_x, _y, _z, rot_module);
				if (misaligned){
					Get_offset(ihalf, iplane, imodule, -1, -1, -1,
							_offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
					TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",
							_offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
					TGeoCombiTrans* rottrans_module_offset =
							new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
					rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
				}
				/*
				if (ihalf == 0 && iplane == 2 && imodule == 3){
					_x += 2.;
					_y += 1.;
					_z += -5.;
					_rotphi += 20.;
					_rottheta += 70.;
				}*/
				// add the cvd disc into that assembly
				//TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
				//						shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
				lmd_vol_module_->AddNode(lmd_vol_cvd_disc, module_id, rottrans_no);
				module_id++;
				for (unsigned int iside = 0; iside < 2; iside++){ // loop over the two sides of the modules
					name.str("");
					name << nav_paths[5] << iside;
					uniqueid.str("");
					uniqueid << "_" << ihalf << iplane << imodule << iside;
					TGeoVolumeAssembly* lmd_vol_side_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
					// rotation around the y axis for the downstream side
					_x = 0.; _y = 0.; _z = 0.;
					if (iside == 0) {_rotphi = 0.; _rottheta = 0.; _rotpsi = 0.;}
					else {_rotphi = 0.; _rottheta = 180.; _rotpsi = 0.;}
					TGeoRotation* rot_side = new TGeoRotation("rot_side", _rotphi, _rottheta, _rotpsi);
					TGeoMatrix* rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);
					if (misaligned){
						Get_offset(ihalf, iplane, imodule, iside, -1, -1,
								_offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
						TGeoRotation* rot_side_offset = new TGeoRotation("rot_side_offset",
								_offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
						TGeoCombiTrans* rottrans_side_offset =
								new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
						rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
					}
					// glue the HV-MAPS to the cvd surface
					for (unsigned int idie = 0; idie < 2; idie++){ // loop over dies
						name.str("");
						name << nav_paths[6] << idie;
						uniqueid.str("");
						uniqueid << "_" << ihalf << iplane << imodule << iside << idie;
						TGeoVolumeAssembly* lmd_vol_die_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
						// rotation to the cut side of the cvd_disc
						// the origin is the inner edge
						const double _sinhalf = sin (delta_phi/2.);
						//const double _coshalf = cos (delta_phi/2.);
						const double _edge_y = -_sinhalf*inner_rad;
						// angle between the edge and the half of the cvd disc
						const double _edgeangle = asin(-_edge_y/cvd_disc_rad);
						const double _edge_x = - cvd_disc_rad * cos(_edgeangle);
						_x = _edge_x;
						_y = _edge_y;
						_z = - cvd_disc_thick_half - maps_thickness; // move to the surface
						_rotphi = - delta_phi/2./pi*180.; _rottheta = 0.; _rotpsi = 0.; // rotate to the cut edge
						TGeoRotation* rot_die = new TGeoRotation("rot_die", _rotphi, _rottheta, _rotpsi);
						TGeoCombiTrans* rottrans_die = new TGeoCombiTrans(_x, _y, _z, rot_die);
						// construct now the sensors on the two dies
						for (unsigned int isensor = 0; isensor < 3; isensor++){ // loop over sensors
							// the 0th die is oriented at the inner edge
							_x = maps_width + maps_width * 2. * isensor;
							_y = maps_height;
							_z = 0.;
							// next row
							if (idie == 1){
								if (isensor == 0) continue;
								_y += die_gap + 2. * maps_height;
								_rotphi = 0.;
							} else {
								_rotphi = 180.;
							}
							 _rottheta = 0.; _rotpsi = 0.;
							//"LumActiveRect" is the keyword for digitization of hits
							/*
							name.str("");
							name << nav_paths[7] << isensor;
							uniqueid.str("");
							uniqueid << "_" << ihalf << iplane << imodule << iside << idie << isensor;
							TGeoVolume* _vol_active =
									new TGeoVolume(
											(name.str()+uniqueid.str()).c_str(),
											shape_maps_active,
											fgGeoMan->GetMedium("silicon"));
							_vol_active->SetLineColor(36);
							*/
							/*
							name.str("");
							name << "LumPassiveRect_" << isensor;
							TGeoVolume* _vol_passive =
									new TGeoVolume(
											(name.str()+uniqueid.str()).c_str(),
											shape_maps_passive,
											fgGeoMan->GetMedium("silicon"));
							_vol_passive->SetLineColor(30);
							*/
							TGeoRotation* rot_sensor = new TGeoRotation("rot_sensor", _rotphi, _rottheta, _rotpsi);
							TGeoCombiTrans* rottrans_sensor = new TGeoCombiTrans(_x, _y, _z, rot_sensor);
							lmd_vol_die_->AddNode(_vol_active, sensor_id, rottrans_sensor);
							lmd_vol_die_->AddNode(_vol_passive, sensor_id, rottrans_sensor);
							// save the transformation from the cvd_side reference frame
							// into the local frame of the sensors
							transformation_matrices[Generate_key(ihalf, iplane, imodule, iside, idie, isensor)] =
									new TGeoHMatrix((*rottrans_die) * (*rottrans_sensor));

							if (0) { // some tests for debugging
								unsigned int _sensor_id = Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
								if (sensor_id != _sensor_id){
									cout << " wrong sensor id " << _sensor_id << " != " << sensor_id << endl;
								}
								int _ihalf, _iplane, _imodule, _iside, _idie, _isensor;
								Get_sensor_by_id(sensor_id, _ihalf, _iplane, _imodule, _iside, _idie, _isensor);
								if ((signed)ihalf != _ihalf) cout << " wrong half " << _ihalf << endl;
								if ((signed)iplane != _iplane) cout << " wrong plane " << _iplane << endl;
								if ((signed)imodule != _imodule) cout << " wrong module " << _imodule << endl;
								if ((signed)iside != _iside) cout << " wrong side " << _iside << endl;
								if ((signed)idie != _idie) cout << " wrong die " << _idie << endl;
								if ((signed)isensor != _isensor) cout << " wrong sensor " << _isensor << endl;
							}
							sensor_id++;
						} // loop over sensors
						_x = 0;
						_y = 0;
						_z = - cvd_disc_thick_half - maps_thickness * 2. - kapton_disc_thick_half; // move to the surface
						_rotphi = 0.; _rottheta = 0.; _rotpsi = 0.; // rotate to the cut edge
						TGeoRotation* rot_kapton = new TGeoRotation("rot_kapton", _rotphi, _rottheta, _rotpsi);
						TGeoCombiTrans* rottrans_kapton = new TGeoCombiTrans(_x, _y, _z, rot_kapton);
						lmd_vol_side_->AddNode(lmd_vol_kapton_disc, 0, rottrans_kapton);
						lmd_vol_side_->AddNode(lmd_vol_die_, 0, rottrans_die);
					} // loop over dies
					lmd_vol_module_->AddNode(lmd_vol_side_, 0, rottrans_side);
					// save the transformation from the lumi reference frame
					// into the local cvd side reference frame
					transformation_matrices[Generate_key(ihalf, iplane, imodule, iside, -1, -1)] =
							new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));
				} // loop over the two sides of the modules
				lmd_vol_plane_->AddNode(lmd_vol_module_, 0, rottrans_module);
				// transformation_matrices[Generate_key(ihalf, iplane, imodule, -1, -1, -1)] =
				//   new TGeoHMatrix((*rottrans_plane) * (*rottrans_module));
			} // loop over modules
			lmd_vol_half_->AddNode(lmd_vol_plane_, 0, rottrans_plane);
			// // save the transformation from the lumi reference frame
			// // into the plane reference frame
			// transformation_matrices[Generate_key(ihalf, iplane, -1, -1, -1, -1)] =
			//   new TGeoHMatrix((*rottrans_plane));
		} // loop over planes
		lmd_vol_ref_sys->AddNode(lmd_vol_half_, 0, rottrans_no);
		// save the transformation into the lumi reference frame
		transformation_matrices[Generate_key(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
	} // loop over detector halves
	mothervol.AddNode(lmd_vol_vac, 0, lmd_transrot);

	/*
		gGeoMan->CloseGeometry();
		//gGeoMan->Get
		//cout << gGeoMan->InitTrack(0,30,1050,0,0,1)->GetName() << endl;
		const double* current_point = gGeoMan->GetCurrentPoint();
		cout << current_point[0] << " " << current_point[1] << " " << current_point[2] << endl;
		double new_point[3] = {8.7, 1., 29.7375};
		gGeoMan->SetCurrentPoint(new_point);
		const double* new_current_point = gGeoMan->GetCurrentPoint();
		cout << new_current_point[0] << " " << new_current_point[1] << " " << new_current_point[2] << endl;
		cout << gGeoMan->GetCurrentNode()->GetName() << endl;
		gGeoMan->FindNode();
		cout << gGeoMan->GetCurrentNode()->GetName() << endl;

		string path = "/lmd_HV_MAPS_1/vol_lmd_vac_0/Lumi_HV-MAPS_0/vol_LumPassive_cvd_disc_plane_1_disc_1_0";
		gGeoMan->cd(path.c_str());///LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0");
		cout << gGeoMan->GetPath()<< endl;
		TGeoPhysicalNode* node =
				gGeoMan->MakePhysicalNode((path + "/LumActivePixelRect_plane_0_disc_0_side_0_col_0_row_0_0").c_str());///Lumi_HV-MAPS/LumPassive_cvd_disc_plane_2_disc_3");
		if (node){
			//TGeoCombiTrans* align_trans = new TGeoCombiTrans(1.,0.,0.,georot_no);
			//node->Align(align_trans);
			//gGeoMan->CloseGeometry();
			TGeoHMatrix* matrix = node -> GetMatrix();
			matrix -> Print();
		}
		//gGeoMan->Set
*/
}



//void PndLmdDim::Create_transformation_matrices_aligned(string filename_in, string filename_out){
void PndLmdDim::reCreate_transformation_matrices(){
	// no translation nor rotation
	TGeoRotation* rot_no = new TGeoRotation("rot_no", 0., 0., 0.); // no rotation
	//TGeoCombiTrans* rottrans_no = new TGeoCombiTrans("rottrans_no", 0., 0.,
	//			0., rot_no);

  double _x(0), _y(0), _z(0), _rotphi(0), _rottheta(0), _rotpsi(0);
  double _offset_x(0), _offset_y(0), _offset_z(0),
    _offset_phi(0), _offset_theta(0), _offset_psi(0);

  for (unsigned int ihalf = 0; ihalf < 2; ihalf++){ // loop over detector halves
    for (unsigned int iplane = 0; iplane < n_planes; iplane++){ // loop over planes
      // move to the position of the corresponding plane
      TGeoMatrix* rottrans_plane = new TGeoCombiTrans(0., 0.,plane_pos_z[iplane], rot_no);
      
	// Get_offset(ihalf, iplane, -1, -1, -1, -1,
	// 	   _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	// TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
	// 						  _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	// TGeoCombiTrans* rottrans_plane_offset =
	//   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
	// //	rottrans_plane = new TGeoHMatrix(*rottrans_plane * *rottrans_plane_offset);
	// rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);
      
      for (unsigned int imodule = 0; imodule < nmodules; imodule++){ // loop over modules
	double angle = delta_phi/2.+ihalf*pi+imodule*delta_phi;
	double add_z = cvd_disc_even_odd_offset;
	// the offset of the modules in the upper and lower halfs
	// are opposite
	if (((imodule+ihalf)%2)==0) add_z = -add_z;
	_x = cos(angle)*cvd_disc_dist;
	_y = sin(angle)*cvd_disc_dist;
	_z = add_z;
	_rotphi = 0.;
	_rottheta = 0.;
	_rotpsi = angle/pi*180.;
	TGeoRotation* rot_module = new TGeoRotation("rot_module", _rotphi, _rottheta, _rotpsi);
	TGeoMatrix* rottrans_module = new TGeoCombiTrans(_x, _y, _z, rot_module);
	Get_offset(ihalf, iplane, imodule, -1, -1, -1,
		   _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	// TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",
	// 						   _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	//	cout<<"_rot_x: "<<_offset_phi<<" rad"<<endl;
	TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",0,0,0);
	// // ///TEST
	// rot_module_offset->RotateZ(-_offset_psi/pi*180.);
	// rot_module_offset->RotateY(_offset_theta/pi*180.);
	// rot_module_offset->RotateX(-_offset_phi/pi*180.);

	rot_module_offset->RotateZ(_offset_psi/pi*180.);
	rot_module_offset->RotateY(_offset_theta/pi*180.);
	rot_module_offset->RotateX(_offset_phi/pi*180.);
	//	rot_module_offset->Print();


	//	cout<<"_rot_x_theta: "<<_offset_phi/pi*180.<<" degree"<<endl;
	TGeoCombiTrans* rottrans_module_offset =
	  new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
	//	rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
	rottrans_module = new TGeoHMatrix(*rottrans_module * *rottrans_module_offset);
	for (int iside = 0; iside < 2; iside++){ // loop over the two sides of the modules
	  // rotation around the y axis for the downstream side
	  _x = 0.; _y = 0.; _z = 0.;
	  if (iside == 0) {_rotphi = 0.; _rottheta = 0.; _rotpsi = 0.;}
	  else {_rotphi = 0.; _rottheta = 180.; _rotpsi = 0.;}
	  TGeoRotation* rot_side = new TGeoRotation("rot_side", _rotphi, _rottheta, _rotpsi);
	  TGeoMatrix* rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);
	  
	    // Get_offset(ihalf, iplane, imodule, iside, -1, -1,
	    // 	       _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	    // TGeoRotation* rot_side_offset = new TGeoRotation("rot_side_offset",
	    // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	    // TGeoCombiTrans* rottrans_side_offset =
	    //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
	    // rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
	    // save the transformation from the lumi reference frame
	    // into the local cvd side reference frame
	    transformation_matrices[Generate_key(ihalf, iplane, imodule, iside, -1, -1)] =
	      new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));
	} // loop over the two sides of the modules
	// transformation_matrices[Generate_key(ihalf, iplane, imodule, -1, -1, -1)] =
	//       new TGeoHMatrix((*rottrans_plane) * (*rottrans_module));
      } // loop over modules
    } // loop over planes
    // // save the transformation into the lumi reference frame
    // transformation_matrices[Generate_key(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
  } // loop over detector halves
}

void PndLmdDim::Correct_transformation_matrices(){
  // Read_transformation_matrices(filename_in,true);//read initial matrices

	// ****************************** loops in the luminosity detector ************************
	double _offset_x(0), _offset_y(0), _offset_z(0),
	  _offset_phi(0), _offset_theta(0), _offset_psi(0);
	for (unsigned int ihalf = 0; ihalf < 2; ihalf++){ // loop over detector halves
	  //TODO: "Transformation matrix not existent!" =
	  // TGeoMatrix* rottrans_half = Get_matrix(ihalf, -1, -1 , -1, -1, -1,  false);
	  // Get_offset(ihalf, -1, -1, -1, -1, -1,
	  // 	     _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	  // TGeoRotation* rot_half_offset = new TGeoRotation("rot_half_offset",
	  // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	  // TGeoCombiTrans* rottrans_half_offset =
	  //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_half_offset);
	  // rottrans_half = new TGeoHMatrix(*rottrans_half_offset * *rottrans_half);
	  
	  for (unsigned int iplane = 0; iplane < n_planes; iplane++){ // loop over planes
	    // TODO: "Transformation matrix not existent!" =
	    //   cout<<"Correct matrix plane #"<<iplane<<endl;
	    //   TGeoMatrix* rottrans_plane = Get_matrix(ihalf, iplane, -1 , -1, -1, -1,  false);
	    // Get_offset(ihalf, iplane, -1, -1, -1, -1,
	    // 	       _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	    // TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
	    // 						     _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	    // TGeoCombiTrans* rottrans_plane_offset =
	    //   new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_plane_offset);
	    // rottrans_plane = new TGeoHMatrix(*rottrans_plane_offset * *rottrans_plane);
	    
	    for (unsigned int imodule = 0; imodule < nmodules; imodule++){ // loop over modules
	      //TODO: "Transformation matrix not existent!" =
	      // cout<<"Correct matrix module #"<<imodule<<" "<<ihalf<<iplane<<imodule<<"-1-1-1"<<endl;
	      //TGeoMatrix* rottrans_module = Get_matrix(ihalf, iplane, imodule, -1, -1, -1,  false);
	      // Get_offset(ihalf, iplane, imodule, -1, -1, -1,
	      // 		 _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
	      // cout<<"offsets:"<<_offset_x<<" "<<_offset_y<<" "<<_offset_z<<" "<<_offset_phi<<" "<<_offset_theta<<" "<<_offset_psi<<endl;
	      // TGeoRotation* rot_module_offset = new TGeoRotation("rot_module_offset",
	      // 							 _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
	      // TGeoCombiTrans* rottrans_module_offset =
	      // 	new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_module_offset);
	      // rottrans_module = new TGeoHMatrix(*rottrans_module_offset * *rottrans_module);
	      // //   rottrans_module = new TGeoHMatrix(*rottrans_module * *rottrans_module_offset);
	      	     	for (unsigned int iside = 0; iside < 2; iside++){ // loop over the two sides of the modules
			  TGeoMatrix* rottrans_side =  Get_matrix(ihalf, iplane, imodule , iside, -1, -1,  false);
			  Get_offset(ihalf, iplane, imodule, iside, -1, -1,
				     _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi);
			  //cout<<"offsets:"<<_offset_x<<" "<<_offset_y<<" "<<_offset_z<<" "<<_offset_phi<<" "<<_offset_theta<<" "<<_offset_psi<<endl;
			  TGeoRotation* rot_side_offset = new TGeoRotation("rot_side_offset",
								 _offset_phi/pi*180., _offset_theta/pi*180., _offset_psi/pi*180.);
			  TGeoCombiTrans* rottrans_side_offset =
			    new TGeoCombiTrans(_offset_x, _offset_y, _offset_z, rot_side_offset);
			  rottrans_side = new TGeoHMatrix(*rottrans_side_offset * *rottrans_side);
			  //	  rottrans_side = new TGeoHMatrix(*rottrans_side * *rottrans_side_offset);

			  // // save the transformation from the lumi reference frame
			  // // into the local cvd side reference frame
			  // transformation_matrices[Generate_key(ihalf, iplane, imodule, iside, -1, -1)] =
			  //   new TGeoHMatrix((*rottrans_plane) * (*rottrans_module) * (*rottrans_side));

			  // save the transformation from the lumi reference frame
			  // into the local cvd side reference frame
			  transformation_matrices[Generate_key(ihalf, iplane, imodule, iside, -1, -1)] =
			    new TGeoHMatrix((*rottrans_side)); //TODO: ????


			} // loop over the two sides of the modules
	      // transformation_matrices[Generate_key(ihalf, iplane, imodule, -1, -1, -1)] =
	      // 	new TGeoHMatrix((*rottrans_plane) * (*rottrans_module)); //TODO: ????
	    } // loop over modules
	  } // loop over planes
	  // // save the transformation into the lumi reference frame
	  // transformation_matrices[Generate_key(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix((*lmd_transrot) * (*rottrans_lmd_in_box));
	} // loop over detector halves
}



#include <fstream>

void PndLmdDim::Read_transformation_matrices(string filename, bool aligned){
	map<string, TGeoMatrix* >* matrices = NULL;
	if (aligned){
		matrices = &transformation_matrices_aligned;
	} else {
		matrices = &transformation_matrices;
	}
	// kill existing matrices
	for (it_transformation_matrices = matrices->begin(); it_transformation_matrices != matrices->end(); it_transformation_matrices++){
		delete(it_transformation_matrices->second);
	}
	matrices->clear();
	string dir = getenv("VMCWORKDIR");
	if (filename == "") filename = dir+"/input/trafo_matrices_lmd.dat";
	ifstream file(filename.c_str());
	int matrices_counter(0);
	if (file.is_open()){
		while (1){
			// read the key
			string key;
			getline (file, key);
			//cout << "next key " << key << endl;
			//char newline;
			if (file.good()){
				// generate rotation and translation matrices with it
				// error treatment is not foreseen yet
				string line1;
				getline (file, line1);
				istringstream inputstream1(line1);
				double translation[3];
				for (int i = 0 ; i < 3 ; i++){
					inputstream1 >> translation[i];
				}
				string line2;
				getline (file, line2);
				istringstream inputstream2(line2);
				double rotation[9];
				for (int i = 0 ; i < 9 ; i++){
					inputstream2 >> rotation[i];
				}
				TGeoRotation* rot = new TGeoRotation("rotation");
				rot->SetMatrix(rotation);
				TGeoCombiTrans* rottrans =
						new TGeoCombiTrans(translation[0], translation[1], translation[2], rot);
				(*matrices)[key] = rottrans;
				matrices_counter++;
			} else {
				file.close();
				break;
			}
		}
		file.close();
		cout << " Read " << matrices_counter << " matrices from " << filename << endl;
	} else {
		cout << " Error in PndLmdDim::Read_transformation_matrices: could not read from " << filename << endl;
	}
}

#include <iomanip>

void PndLmdDim::Write_transformation_matrices(string filename, bool aligned){
	map<string, TGeoMatrix* >* matrices = NULL;
	if (aligned){
		matrices = &transformation_matrices_aligned;
	} else {
		matrices = &transformation_matrices;
	}
	int matrices_counter(0);
	ofstream file(filename.c_str());
	if (file.is_open()){
		for (it_transformation_matrices = matrices->begin(); it_transformation_matrices != matrices->end(); it_transformation_matrices++){
			// write the key
			file << it_transformation_matrices->first << '\n';
			const double * translation = it_transformation_matrices->second->GetTranslation();
			const double * rotation = it_transformation_matrices->second->GetRotationMatrix();
			// write the numbers for the translation
			for (int i = 0 ; i < 3 ; i++){
				file << setw( 14 ) << scientific << translation[i];
				if (i == 2) file << '\n'; else file << ' ';
			}
			// write the numbers for the rotation
			for (int i = 0 ; i < 9 ; i++){
				file << setw( 14 ) << scientific << rotation[i];
				if (i == 8) file << '\n'; else file << ' ';
			}
			matrices_counter++;
		}
		file.close();
		cout << matrices_counter << " Transformation matrices were written to " << filename << endl;
	}
	else cout << " Error in PndLmdDim::Write_transformation_matrices: could not write to " << filename << endl;
}

void PndLmdDim::Cleanup(){
	for (it_transformation_matrices = transformation_matrices.begin(); it_transformation_matrices != transformation_matrices.end(); it_transformation_matrices++){
		delete(it_transformation_matrices->second);
	}
	transformation_matrices.clear();
	for (it_transformation_matrices = transformation_matrices_aligned.begin(); it_transformation_matrices != transformation_matrices_aligned.end(); it_transformation_matrices++){
		delete(it_transformation_matrices->second);
	}
	transformation_matrices_aligned.clear();
}

void PndLmdDim::Read_DB_offsets(PndLmdAlignPar *lmdalignpar){
  // ana = FairRun::Instance();
  // rtdb=ana->GetRuntimeDb();
  // TList* fAlignParamList = new TList();// alignment params fom DB
  double fShiftX[40],fShiftY[40],fShiftZ[40];
  double fRotateX[40],fRotateY[40],fRotateZ[40];
  // //read params for lumi alignment
  // PndLmdContFact* thelmdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
  // TList* theAlignLMDContNames = thelmdcontfact->GetAlignParNames();
  // Info("SetParContainers()","AlignLMD The container names list contains %i entries",theAlignLMDContNames->GetEntries());
  // TIter cfAlIter(theAlignLMDContNames);
  // while (TObjString* contname = (TObjString*)cfAlIter()) {
  //   TString parsetname = contname->String();
  //   Info("SetParContainers()",parsetname.Data());
  //   PndLmdAlignPar *lmdalignpar = (PndLmdAlignPar*)(rtdb->getContainer(parsetname.Data()));
  //   if(!lmdalignpar) Fatal("SetParContainers","No ALIGN parameter found: %s",parsetname.Data());
  //   fAlignParamList->Add(lmdalignpar); 
  // }

  // TIter alignparams(fAlignParamList); 
  // PndLmdAlignPar* lmdalignpar=(PndLmdAlignPar*)alignparams();
  // //  lmdalignpar->getParams(alignparams);
  //  lmdalignpar->Print();
  if(0==lmdalignpar) { 
    Error("PndLmdStripClusterTask::SetCalculators()","A ALIGN Parameter Set does not exist properly.");
  } 
  else{
    // cout<<"&&&&& it's from LmdDim &&&&&"<<endl;
    // lmdalignpar->Print();
    for(int ik=0;ik<40;ik++){
      //      cout<<"ik = "<<ik<<endl;
      fShiftX[ik] = lmdalignpar->GetShiftX(ik);
      //      cout<<"fShiftX[ik] = "<<fShiftX[ik]<<endl;
      fShiftY[ik] = lmdalignpar->GetShiftY(ik);
      fShiftZ[ik] = lmdalignpar->GetShiftZ(ik);
      fRotateX[ik] = lmdalignpar->GetRotateX(ik);
      fRotateY[ik] = lmdalignpar->GetRotateY(ik);
      fRotateZ[ik] = lmdalignpar->GetRotateZ(ik);
      //      if (fVerbose > 2) cout<<"fShiftX["<<ik<<"]="<<fShiftX[ik]<<" fRotateX["<<ik<<"]="<<fRotateX[ik]
      //		    <<" fRotateY["<<ik<<"]="<<fRotateY[ik]<<" fRotateZ["<<ik<<"]="<<fRotateZ[ik]<<endl;
    }
  }
  //  lmdalignpar->Print();

  //so far for modules alignment only
  //TODO: individual sensor and\or die alignment???
  for (unsigned int ihalf = 0; ihalf < 2; ihalf++){ // loop over detector halves
    for (unsigned int iplane = 0; iplane < n_planes; iplane++){ // loop over planes
      for (unsigned int imodule = 0; imodule < nmodules; imodule++){ // loop over modules
	string key = Generate_key(ihalf, iplane, imodule, -1, -1, -1);
	int ikey = (ihalf*n_planes*nmodules)+(iplane*nmodules)+imodule;
	//	cout<<"for: "<<ihalf<<iplane<<imodule<<": ikey="<<ikey<<endl;
	offsets[key].push_back(fShiftX[ikey]);
	offsets[key].push_back(fShiftY[ikey]);
	offsets[key].push_back(fShiftZ[ikey]);
	offsets[key].push_back(fRotateX[ikey]);
	offsets[key].push_back(fRotateY[ikey]);
	offsets[key].push_back(fRotateZ[ikey]);

	string key2 = Generate_key(ihalf, iplane, imodule, 0, -1, -1);//top side
	//	cout<<"for: "<<ihalf<<iplane<<imodule<<": ikey="<<ikey<<endl;
	offsets[key2].push_back(fShiftX[ikey]);
	offsets[key2].push_back(fShiftY[ikey]);
	offsets[key2].push_back(fShiftZ[ikey]);
	offsets[key2].push_back(fRotateX[ikey]);
	offsets[key2].push_back(fRotateY[ikey]);
	offsets[key2].push_back(fRotateZ[ikey]);

	// //TODO: is it correct???
	string key3 = Generate_key(ihalf, iplane, imodule, 1, -1, -1);// bottom side
	offsets[key3].push_back(fShiftX[ikey]);
	offsets[key3].push_back(fShiftY[ikey]);
	offsets[key3].push_back(fShiftZ[ikey]);
	offsets[key3].push_back(fRotateX[ikey]);
	offsets[key3].push_back(fRotateY[ikey]);
	offsets[key3].push_back(fRotateZ[ikey]);
      }
    }
  }
}

void PndLmdDim::Get_offset(int ihalf, int iplane, int imodule, int iside, int idie, int isensor,
		double& x, double& y, double& z,
		double& rotphi, double& rottheta, double& rotpsi){
	string key = Generate_key(ihalf, iplane, imodule, iside, idie, isensor);
	//cout<<"setting Offset for: "<<ihalf<<", "<<iplane<<", "<<imodule<<", "<<iside<<", "<<idie<<", "<<isensor<<endl;
	itoffset = offsets.find(key);
	// if (itoffset != offsets.end()){
	// 	x = itoffset->second[0];
	// 	y = itoffset->second[1];
	// 	z = itoffset->second[2];
	// 	rotphi = itoffset->second[3];
	// 	rottheta = itoffset->second[4];
	// 	rotpsi = itoffset->second[5];
	// } else {
	// 	cout << " generating offset for";
	// 	// the precision of dies containing sensors is requested
	// 	// when requested offset applies to all sensors
	// 	// on one side
	// 	if (ihalf >= 0 && iplane >= 0 && imodule >= 0 && iside >= 0 &&
	// 			idie < 0 && isensor < 0 ){
	// 		cout << " one module side " << endl;
	// 		x = gRandom->Gaus(0, side_offset_x);
	// 		y = gRandom->Gaus(0, side_offset_y);
	// 		z = gRandom->Gaus(0, side_offset_z);
	// 		rottheta = gRandom->Gaus(0, side_tilt_phi);
	// 		rotphi = gRandom->Gaus(0, side_tilt_theta);
	// 		rotpsi = gRandom->Gaus(0, side_tilt_psi);
	// 	}
	// 	// the precision of cvd discs is requested
	// 	// when requested offset applies also to the sensors
	// 	// on both sides
	// 	if (ihalf >= 0 && iplane >= 0&& imodule >= 0 &&
	// 			iside < 0 && idie < 0 && isensor < 0){
	// 		cout << " one module " << endl;
	// 		x = gRandom->Gaus(0, cvd_offset_x);
	// 		y = gRandom->Gaus(0, cvd_offset_y);
	// 		z = gRandom->Gaus(0, cvd_offset_z);
	// 		rotphi = gRandom->Gaus(0, cvd_tilt_phi);
	// 		rottheta = gRandom->Gaus(0, cvd_tilt_theta);
	// 		rotpsi = gRandom->Gaus(0, cvd_tilt_psi);
	// 		//x = (cvd_offset_x);
	// 		//y = (cvd_offset_y);
	// 		//z = (cvd_offset_z);
	// 		//rotphi = (cvd_tilt_phi);
	// 		//rottheta = (cvd_tilt_theta);
	// 		//rotpsi = (cvd_tilt_psi);
	// 	}
	// 	// the precision of plane halves containing sensors is requested
	// 	// when requested offset applies to all modules
	// 	if (ihalf >= 0 && iplane >= 0 &&
	// 			imodule < 0 && iside < 0 && idie < 0 && isensor < 0 ){
	// 		cout << " one plane half " << endl;
	// 		x = gRandom->Gaus(0, plane_half_offset_x);
	// 		y = gRandom->Gaus(0, plane_half_offset_y);
	// 		z = gRandom->Gaus(0, plane_half_offset_z);
	// 		rottheta = gRandom->Gaus(0, plane_half_tilt_phi);
	// 		rotphi = gRandom->Gaus(0, plane_half_tilt_theta);
	// 		rotpsi = gRandom->Gaus(0, plane_half_tilt_psi);
	// 	}
	// 	// the precision of halves of planes is requested
	// 	// when requested offset applies to module supports
	// 	if (ihalf >= 0 &&
	// 			iplane < 0 && imodule < 0 && iside < 0 &&
	// 			idie < 0 && isensor < 0 ){
	// 		cout << " one half " << endl;
	// 		x = gRandom->Gaus(0, half_offset_x);
	// 		y = gRandom->Gaus(0, half_offset_y);
	// 		z = gRandom->Gaus(0, half_offset_z);
	// 		rottheta = gRandom->Gaus(0, half_tilt_phi);
	// 		rotphi = gRandom->Gaus(0, half_tilt_theta);
	// 		rotpsi = gRandom->Gaus(0, half_tilt_psi);
	// 	}
	// 	// the precision of the luminosity detector is requested
	// 	// when no degrees of freedom are available to the rest
	// 	if (ihalf < 0 && imodule < 0 && iplane < 0 && iside < 0 &&
	// 			idie < 0 && isensor < 0 ){
	// 		cout << " the luminosity detector " << endl;
	// 		// not implemented yet
	// 		x = gRandom->Gaus(0, 0);
	// 		y = gRandom->Gaus(0, 0);
	// 		z = gRandom->Gaus(0, 0);
	// 		rottheta = gRandom->Gaus(0, 0);
	// 		rotphi = gRandom->Gaus(0, 0);
	// 		rotpsi = gRandom->Gaus(0, 0);
	// 	}


	  x = itoffset->second[0];
	  y = itoffset->second[1];
	  z = itoffset->second[2];
	  rotphi = itoffset->second[3];
	  rottheta = itoffset->second[4];
	  rotpsi = itoffset->second[5];

	  offsets[key].push_back(x);
	  offsets[key].push_back(y);
	  offsets[key].push_back(z);
	  offsets[key].push_back(rotphi);
	  offsets[key].push_back(rottheta);
	  offsets[key].push_back(rotpsi);
//	  cout<<"x,y,z,rotphi,rottheta,rotpsi: "<<x<<", "<<y<<", "<<z<<", "<<rotphi<<", "<<rottheta<<", "<<rotpsi<<endl;
}

void PndLmdDim::Transform_global_to_lmd_local(double& x, double& y, double& z, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_global_to_lmd_local(aligned);
	double from[3] = {x,y,z};
	//cout << x << " " << y << " " << z << endl;
	double to[3];
	matrix.MasterToLocal(from, to);
	x = to[0]; y = to[1]; z = to[2];
	//cout << x << " " << y << " " << z << endl << endl;
}

void PndLmdDim::Transform_global_to_lmd_local_vect(double& x, double& y, double& z, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_global_to_lmd_local(aligned);
	double from[3] = {x,y,z};
	//cout << x << " " << y << " " << z << endl;
	double to[3];
	matrix.MasterToLocalVect(from, to);
	x = to[0]; y = to[1]; z = to[2];
	//cout << x << " " << y << " " << z << endl << endl;
}

map<string, TGeoMatrix* >* PndLmdDim::Get_matrices(bool aligned){
	map<string, TGeoMatrix* >* matrices = NULL;
	if (aligned){
		matrices = &transformation_matrices_aligned;
	} else {
		matrices = &transformation_matrices;
	}
	if (matrices->size() == 0) {
		cout << " Error in PndLmdDim::Get_matrices: No transformation matrices loaded! " << endl;
		return NULL;
	} else {
		return matrices;
	}
}

TGeoMatrix* PndLmdDim::Get_matrix(int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	map<string, TGeoMatrix* >* matrices = Get_matrices(aligned);
	if (!matrices) return NULL;
	it_transformation_matrices = matrices->find(Generate_key(ihalf, iplane, imodule, iside, idie, isensor));
	if (it_transformation_matrices == matrices->end()) {
		cout << " Error in PndLmdDim::Get_matrix: Transformation matrix not existent! " << endl;
		return NULL;
	} else {
		return it_transformation_matrices->second;
	}
}

TGeoMatrix* PndLmdDim::Get_matrix_global_to_lmd_local(bool aligned){
	return Get_matrix(-1,-1,-1,-1,-1,-1, aligned);
}

// TGeoMatrix* PndLmdDim::Get_matrix_global_to_sector_local(int ihalf, int iplane, int imodule, bool aligned){
//   return Get_matrix(ihalf, iplane, imodule,-1,-1,-1, aligned);
// }

TGeoMatrix* PndLmdDim::Get_matrix_lmd_local_to_module_side(int ihalf, int iplane, int imodule, int iside, bool aligned){
	return Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
}

TGeoMatrix* PndLmdDim::Get_matrix_module_side_to_sensor(int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	return Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
}

TGeoHMatrix PndLmdDim::Get_transformation_global_to_lmd_local(bool aligned){
	TGeoMatrix* matrix = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
	if (!matrix) return TGeoHMatrix();
	return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_module_side(
		int ihalf, int iplane, int imodule, int iside, bool aligned){
	TGeoMatrix* matrix = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	if (!matrix) return TGeoHMatrix();
	return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_module_side_to_sensor(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	TGeoMatrix* matrix = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix) TGeoHMatrix();
	return TGeoHMatrix(*matrix);
}

TGeoHMatrix PndLmdDim::Get_transformation_global_to_sensor(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	// instead of using the 3 methods to transform between the reference frames
	// pointers to the matrices are retrieved directly in order to gain a little
	// bit of performance (no construction and deletion of new matrices needed)
	TGeoMatrix* matrix1 = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
	TGeoMatrix* matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	TGeoMatrix* matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix1 || !matrix2 || !matrix3) return TGeoHMatrix();
	return TGeoHMatrix((*matrix1) * (*matrix2) * (*matrix3));
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_sensor(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	TGeoMatrix* matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	TGeoMatrix* matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix2 || !matrix3) return TGeoHMatrix();
	return TGeoHMatrix((*matrix2) * (*matrix3));
}

TGeoHMatrix PndLmdDim::Get_transformation_lmd_local_to_global(bool aligned){
	TGeoMatrix* matrix = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
	if (!matrix) return TGeoHMatrix();
	return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_module_side_to_lmd_local(
		int ihalf, int iplane, int imodule, int iside, bool aligned){
	TGeoMatrix* matrix = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	if (!matrix) TGeoHMatrix();
	return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_module_side(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	TGeoMatrix* matrix = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix) return TGeoHMatrix();
	return TGeoHMatrix(matrix->Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_global(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	// instead of using the 3 methods to transform between the reference frames
	// pointers to the matrices are retrieved directly in order to gain a little
	// bit of performance (no construction and deletion of new matrices needed)
	TGeoMatrix* matrix1 = Get_matrix(-1, -1, -1, -1, -1, -1, aligned);
	TGeoMatrix* matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	TGeoMatrix* matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix1 || !matrix2 || !matrix3) TGeoHMatrix();
	return TGeoHMatrix(((*matrix1) * (*matrix2) * (*matrix3)).Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_lmd_local(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor,  bool aligned){
	TGeoMatrix* matrix2 = Get_matrix(ihalf, iplane, imodule, iside, -1, -1, aligned);
	TGeoMatrix* matrix3 = Get_matrix(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	if (!matrix2 || !matrix3) TGeoHMatrix();
	return TGeoHMatrix(((*matrix2) * (*matrix3)).Inverse());
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_to_sensor_aligned(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
	// to do: optimize like in Get_transformation_sensor_to_global
	const TGeoHMatrix& matrix = Get_transformation_sensor_to_global(ihalf, iplane, imodule, iside, idie, isensor, false);
	const TGeoHMatrix& matrix_aligned = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, true);
	return TGeoHMatrix(matrix*matrix_aligned);
}

TGeoHMatrix PndLmdDim::Get_transformation_sensor_aligned_to_sensor(
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
	// to do: optimize like in Get_transformation_sensor_to_global
	const TGeoHMatrix& matrix_aligned = Get_transformation_sensor_to_global(ihalf, iplane, imodule, iside, idie, isensor, true);
	const TGeoHMatrix& matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, false);
	return TGeoHMatrix(matrix_aligned*matrix);
}

TVector3 PndLmdDim::Transform_global_to_lmd_local(const TVector3& point, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_global_to_lmd_local(aligned);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}

TVector3 PndLmdDim::Transform_lmd_local_to_module_side(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_lmd_local_to_module_side(ihalf, iplane, imodule, iside, aligned);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}

TVector3 PndLmdDim::Transform_module_side_to_sensor(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_module_side_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}

TVector3 PndLmdDim::Transform_global_to_sensor(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}


TVector3 PndLmdDim::Transform_lmd_local_to_global(const TVector3& point, bool isvector, bool aligned){
  // I think Local to Master calculation is faster than the getter of the inverse matrix
  const TGeoHMatrix& matrix = Get_transformation_global_to_lmd_local(aligned);
  double local[3];
  point.GetXYZ(local);
  double master[3];
  if (isvector) matrix.LocalToMasterVect(local, master);
  else matrix.LocalToMaster(local, master);
  return TVector3(master);
}

TVector3 PndLmdDim::Transform_lmd_local_to_sensor(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_lmd_local_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double local[3];
	point.GetXYZ(local);
	double master[3];
	if (isvector) matrix.LocalToMasterVect(local, master);
	else matrix.LocalToMaster(local, master);
	return TVector3(master);
}

TVector3 PndLmdDim::Transform_module_side_to_lmd_local(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, bool isvector, bool aligned){
	// I think Local to Master calculation is faster than the getter of the inverse matrix
	const TGeoHMatrix& matrix = Get_transformation_lmd_local_to_module_side(ihalf, iplane, imodule, iside, aligned);
	double local[3];
	point.GetXYZ(local);
	double master[3];
	if (isvector) matrix.LocalToMasterVect(local, master);
	else matrix.LocalToMaster(local, master);
	return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_module_side(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	// I think Local to Master calculation is faster than the getter of the inverse matrix
	const TGeoHMatrix& matrix = Get_transformation_module_side_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double local[3];
	point.GetXYZ(local);
	double master[3];
	if (isvector) matrix.LocalToMasterVect(local, master);
	else matrix.LocalToMaster(local, master);
	return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_global(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	// I think Local to Master calculation is faster than the getter of the inverse matrix
	const TGeoHMatrix& matrix = Get_transformation_global_to_sensor(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double local[3];
	point.GetXYZ(local);
	double master[3];
	if (isvector) matrix.LocalToMasterVect(local, master);
	else matrix.LocalToMaster(local, master);
	return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_lmd_local(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector, bool aligned){
	const TGeoHMatrix& matrix = Get_transformation_sensor_to_lmd_local(ihalf, iplane, imodule, iside, idie, isensor, aligned);
	double local[3];
	point.GetXYZ(local);
	double master[3];
	if (isvector) matrix.LocalToMasterVect(local, master);
	else matrix.LocalToMaster(local, master);
	return TVector3(master);
}

TVector3 PndLmdDim::Transform_sensor_to_sensor_aligned(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector){
	const TGeoHMatrix& matrix = Get_transformation_sensor_to_sensor_aligned(ihalf, iplane, imodule, iside, idie, isensor);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}

TVector3 PndLmdDim::Transform_sensor_aligned_to_sensor(const TVector3& point,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool isvector){
	const TGeoHMatrix& matrix = Get_transformation_sensor_aligned_to_sensor(ihalf, iplane, imodule, iside, idie, isensor);
	double master[3];
	point.GetXYZ(master);
	double local[3];
	if (isvector) matrix.MasterToLocalVect(master, local);
	else matrix.MasterToLocal(master, local);
	return TVector3(local);
}

TMatrixD PndLmdDim::Transform_global_to_lmd_local(const TMatrixD& matrix, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_global_to_lmd_local(aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_module_side(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_lmd_local_to_module_side(
					ihalf, iplane, imodule, iside, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_module_side_to_sensor(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_module_side_to_sensor(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_global_to_sensor(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_global_to_sensor(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_sensor(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_lmd_local_to_sensor(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_lmd_local_to_global(const TMatrixD& matrix, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_lmd_local_to_global(aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_module_side_to_lmd_local(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_module_side_to_lmd_local(
					ihalf, iplane, imodule, iside, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_module_side(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_sensor_to_module_side(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_global(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_sensor_to_global(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_lmd_local(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor, bool aligned){
	TMatrixD rotmatrix(3,3,
			Get_transformation_sensor_to_lmd_local(
					ihalf, iplane, imodule, iside, idie, isensor, aligned).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_to_sensor_aligned(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
	TMatrixD rotmatrix(3,3,
			Get_transformation_sensor_to_sensor_aligned(ihalf, iplane, imodule, iside, idie, isensor).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

TMatrixD PndLmdDim::Transform_sensor_aligned_to_sensor(const TMatrixD& matrix,
		int ihalf, int iplane, int imodule, int iside, int idie, int isensor){
	TMatrixD rotmatrix(3,3,
			Get_transformation_sensor_aligned_to_sensor(ihalf, iplane, imodule, iside, idie, isensor).GetRotationMatrix());
	return TMatrixD(rotmatrix*TMatrixD(matrix,TMatrixD::kMultTranspose,rotmatrix));
}

//

