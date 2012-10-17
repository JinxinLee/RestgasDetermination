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

PndLmdDim* PndLmdDim::pinstance = 0;


#include <TROOT.h>
PndLmdDim::PndLmdDim()
{
	// pi
	pi = 3.141592654;
	// number of detector planes
	n_planes = 4;
	// number of modules per plane half
	nmodules = 5;
	// position of planes where the first plane defines the origin
	plane_pos_z = new double[4];
	plane_pos_z[0] = 0.0; plane_pos_z[1] = 20.0; plane_pos_z[2] = 30.0; plane_pos_z[3] = 40.0;
	// ****************************** cvd cooling support discs ************************
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

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation

	// x is radial to the beam pipe
	cvd_offset_x = 0.01;//2.01;
	// y is tangent to the beam pipe
	cvd_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	cvd_offset_z = 0.01;//1.01;
	// x is a rotation around the radial component of the beam pipe
	cvd_tilt_x = 0.; // please do not use yet
	// y is a rotation around the tangent component of the beam pipe
	cvd_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	cvd_tilt_z = 0.001;//1e0;
	// *********************************** HV-MAPS *************************************
	//
	//            left   right
	//
	//  |---------|----------|----------| top
	//  ||------|-||-------|-||-------|-|
	//  ||      | ||       | ||       | |
	//  ||  0   | ||   1   | ||   2   | |     row 1
	//  ||	    | || active| ||       | |
	//  ||------|-||-------|-||-------|-|
	//  |         | passive  |          |
	//  |---------|----------|----------| bottom
	//  gap
	//            |----------|----------| bottom
	//            | passive  |          |
	//            ||-------|-||-------|-|
	//            ||       | ||       | |
	//      3     ||   4   | ||   5   | |     row 2
	//            ||       | ||       | |
	//            ||-------|-||-------|-|
	//            |----------|----------| top
	//
	//            right   left
	//
	//                A   y; maps_n_row; height
	//                |
	//                |
	//                --> x; maps_n_col; width
	//
	// the current design foresees a rotation of the first parameters

	// even when several maps are placed on one die
	// those will be placed in the simulation next to
	// each other as separate detectors
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
	maps_passive_top    = 0.1 / 2.;//*12000.;
	maps_passive_bottom = 0.4  / 2.;
	maps_passive_left   = 0.2 / 2.;//*6000.;
	maps_passive_right  = 0.05 / 2.;//*3000.;
	maps_active_width   = 2.0  / 2. - maps_passive_left - maps_passive_right;
	maps_active_height  = 2.0  / 2. - maps_passive_top  - maps_passive_bottom;

	maps_width  = maps_passive_left + maps_active_width  + maps_passive_right ;
	maps_height = maps_passive_top  + maps_active_height + maps_passive_bottom;

	maps_active_offset_x   = (maps_passive_left - maps_passive_right )/2.;
	maps_active_offset_y   = (maps_passive_top  - maps_passive_bottom)/2.;


	die_gap = 0.1; // (cm)

	maps_die_width  = maps_width  * maps_n_col;
	maps_die_height = maps_height * maps_n_row + die_gap * maps_n_row - 1;

	// the mechanical alignment precision is defined as an offset of and tilt around
	// the middle of the cvd diamond.
	// Values are standard deviation.
	// first comes translation than rotation\
	// translations along z as well as rotations around x and y are
	// negligible for dies glued on a cvd diamond
	// rotation around z is not working yet

	// x is along the edge of the cvd disc
	die_offset_x = 0.01;//2.01;
	// y is orthogonal to the edge of the cvd disc
	die_offset_y = 0.01;//1.01;
	// z is along the beam pipe
	die_offset_z = 0.; //should not be used -> crashing volumes;
	// x is a rotation around the edge of the cvd disc
	die_tilt_x = 0.; // please do not use yet
	// y is a rotation around the orthogonal component of the edge of the cvd disc
	die_tilt_y = 0.; // please do not use yet
	// z is a rotation around an axis parallel to the along the beam pipe
	die_tilt_z = 0.;// please do not use yet;
	//*********************************** lumi box parameters ***********************************
	// see CAD files for details
	// https://edms.cern.ch/nav/P:FAIR-000000719:V0/P:FAIR-000000726:V0/TAB3
	// width
	box_size_x = 36./2.;
	// height
	box_size_y = 50./2.;
	// length
	box_size_z = 100./2.;
	// thickness of the V2A steel plates
	box_thickness = 0.5;
	// position of the inner rib
	pos_rib = 10.;
	// beam pipe radius at entrance
	rad_entrance = 9.;
	// beam pipe radius at exit
	rad_exit = 4.5;
	// beam pipe separating non interacting paricles
	rad_pipe = 3.5;
	// beam pipe thickness;
	pipe_thickness = 0.1;
	// cone height of the transition region
	length_transision = 30.;
	// length of the inner pipe
	length_pipe = 60.;
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
	pos_plane_0 = 70.;
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
}



PndLmdDim::~PndLmdDim()
{
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
	FairGeoLoader* geoLoad =  FairGeoLoader::Instance();
	if (!geoLoad){
		geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
		cout << " creating FairGeoLoader instance " << endl;
	}
	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile("../../geometry/media_pnd.geo");//("${VMCWORKDIR}/geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();

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

	// no translation nor rotation
	TGeoRotation* rot_no = new TGeoRotation("rot_no", 0., 0., 0.); // no rotation
	TGeoCombiTrans* rottrans_no = new TGeoCombiTrans("rottrans_no", 0., 0.,
				0., rot_no);
	rottrans_no->RegisterYourself();

	// ************ create the luminosity monitor box ***********
	// create the bounding box
	double tube_upstream_length = 25./2.;
	double tube_upstream_rad_out = 9.;
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
	double x, y, z, rotx, roty, rotz;
	Get_pos_lmd_global(x, y, z, rotx, roty, rotz);
	TGeoRotation* lmd_rot = new TGeoRotation("lmd_rot");
	lmd_rot->RotateX(rotx/pi*180.);
	lmd_rot->RotateY(roty/pi*180.);
	lmd_rot->RotateZ(rotz/pi*180.);
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
			box_thickness);
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
	// compose all the parts into one luminosity vacuum box
	TGeoCompositeShape *shape_lmd_box = new TGeoCompositeShape("shape_lmd_box",
			"(lmd_box_outer-lmd_box_inner)-box_hole_upstream:comb_trans_cut_pipe_upstream-box_hole_downstream:comb_trans_cut_pipe_downstream");
	TGeoVolume *lmd_vol_box = new TGeoVolume("lmd_vol_box", shape_lmd_box,
				fgGeoMan->GetMedium("steel"));
	lmd_vol_box->SetLineColor(11);
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
	double cone_height = 30./2.;
	double cone_r_in_upstream = 20.4/2.;
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
	lmd_vol_vac->AddNode(vlum_CaptonCone, 0, lmd_trans_cap_co);
	// beam pipe to shield the sensors
	double pipe_inner_r_in = 7./2.;
	double pipe_inner_length = 60./2.;
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
	unsigned int sensor_id(0);
	unsigned int module_id(0);
	for (int ihalf = 0; ihalf < 2; ihalf++){ // loop over detector halves
		// in order do be able to displace the detector halves those are introduced as
		// separate volume assemblies
		name.str("");
		name << nav_paths[2] << ihalf;
		uniqueid.str("");
		uniqueid << "_" << ihalf;
		TGeoVolumeAssembly* lmd_vol_half_ = new TGeoVolumeAssembly(name.str().c_str());
		//mothervol.AddNode(lmd_vol_half_, 0, rottrans_no);
		for (int iplane = 0; iplane < n_planes; iplane++){ // loop over planes
			name.str("");
			name << nav_paths[3] << iplane;
			uniqueid.str("");
			uniqueid << "_" << ihalf << iplane;
			TGeoVolumeAssembly* lmd_vol_plane_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
			// move to the position of the corresponding plane
			TGeoCombiTrans* rottrans_plane = new TGeoCombiTrans(0., 0.,
					plane_pos_z[iplane], rot_no);
			for (int imodule = 0; imodule < nmodules; imodule++){ // loop over modules
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
				/*
				if (ihalf == 0 && iplane == 2 && imodule == 3){
					_x += 2.;
					_y += 1.;
					_z += -5.;
					_rotphi += 20.;
					_rottheta += 70.;
				}*/
				TGeoRotation* rot_module = new TGeoRotation("rot_module", _rotphi, _rottheta, _rotpsi);
				TGeoCombiTrans* rottrans_module = new TGeoCombiTrans(_x, _y, _z, rot_module);
				// add the cvd disc into that assembly
				//TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
				//						shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
				lmd_vol_module_->AddNode(lmd_vol_cvd_disc, module_id, rottrans_no);
				module_id++;
				for (int iside = 0; iside < 2; iside++){ // loop over the two sides of the modules
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
					TGeoCombiTrans* rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);
					// glue the HV-MAPS to the cvd surface
					for (int idie = 0; idie < 2; idie++){ // loop over dies
						name.str("");
						name << nav_paths[6] << idie;
						uniqueid.str("");
						uniqueid << "_" << ihalf << iplane << imodule << iside << idie;
						TGeoVolumeAssembly* lmd_vol_die_ = new TGeoVolumeAssembly((name.str()+uniqueid.str()).c_str());
						// rotation to the cut side of the cvd_disc
						// the origin is the inner edge
						const double _sinhalf = sin (delta_phi/2.);
						const double _coshalf = cos (delta_phi/2.);
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
						for (int isensor = 0; isensor < 3; isensor++){ // loop over sensors
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
							if (1) { // some tests for debugging
								int _sensor_id = Get_sensor_id(ihalf, iplane, imodule, iside, idie, isensor);
								if (sensor_id != _sensor_id){
									cout << " wrong sensor id " << _sensor_id << " != " << sensor_id << endl;
								}
								int _ihalf, _iplane, _imodule, _iside, _idie, _isensor;
								Get_sensor_by_id(sensor_id, _ihalf, _iplane, _imodule, _iside, _idie, _isensor);
								if (ihalf != _ihalf) cout << " wrong half " << _ihalf << endl;
								if (iplane != _iplane) cout << " wrong plane " << _iplane << endl;
								if (imodule != _imodule) cout << " wrong module " << _imodule << endl;
								if (iside != _iside) cout << " wrong side " << _iside << endl;
								if (idie != _idie) cout << " wrong die " << _idie << endl;
								if (isensor != _isensor) cout << " wrong sensor " << _isensor << endl;
							}
							sensor_id++;
						} // loop over sensors
						lmd_vol_side_->AddNode(lmd_vol_die_, 0, rottrans_die);
					} // loop over dies
					lmd_vol_module_->AddNode(lmd_vol_side_, 0, rottrans_side);
				} // loop over the two sides of the modules
				lmd_vol_plane_->AddNode(lmd_vol_module_, 0, rottrans_module);
			} // loop over modules
			lmd_vol_half_->AddNode(lmd_vol_plane_, 0, rottrans_plane);
		} // loop over planes
		lmd_vol_ref_sys->AddNode(lmd_vol_half_, 0, rottrans_no);
	} // loop over detector halves
	mothervol.AddNode(lmd_vol_vac, 0, lmd_transrot);
/*


		TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Lumi_HV-MAPS");

		int sensor_id = 0;
		for (int i_plane = 0; i_plane < 4; i_plane++) {
			std::stringstream plane_name;
			plane_name << "plane_" << (i_plane + 1);
			for (int i_cvd_disc = 0; i_cvd_disc < n_cvd_discs; i_cvd_disc++) {
				std::stringstream disc_name;
				disc_name << "LumPassive_cvd_disc_" << plane_name.str() << "_disc_"
						<< (i_cvd_disc + 1);
				std::stringstream rot_name;
				rot_name << "rot_" << plane_name.str() << "_disc_" << (i_cvd_disc
						+ 1);
				TGeoVolume* tmpvol = new TGeoVolume(disc_name.str().c_str(),
						shape_cvd_support, gGeoMan->GetMedium("HYPdiamond"));//HYPdiamond"));
				tmpvol->SetLineColor(16);
				double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
				Get_pos_mod_local(i_plane, i_cvd_disc, _x, _y, _z, _rotx,
						_roty, _rotz, misalign);
				TGeoRotation* rot = new TGeoRotation(rot_name.str().c_str());
				rot->RotateX(_rotx / pi * 180.);
				rot->RotateY(_roty / pi * 180.);
				rot->RotateZ(_rotz / pi * 180.);
				TGeoTranslation* trt = new TGeoTranslation(_x, _y, _z);//pos_z + plane_pos_z[i_plane]  - pos_rot_z);
				TGeoCombiTrans* trctmp = new TGeoCombiTrans(*trt, *rot);
				trctmp->SetName((rot_name.str() + "_comb").c_str());
				trctmp->RegisterYourself();
				delete rot;
				delete trt;
				TGeoVolumeAssembly* _SubunitVol = new TGeoVolumeAssembly(("vol_"+disc_name.str()).c_str());
				_SubunitVol->AddNode(tmpvol, i_cvd_disc + 1, trctmp);
				SubunitVol->AddNode(_SubunitVol, 0, comb_trans_no);
				//break;
				for (int iside = 0; iside < 2; iside++) {
					// modules are placed on both sides
					// aligned to the cut edges
					int isensor = -1;
					for (int maps_col = 0; maps_col < maps_n_col; maps_col++) {
						for (int maps_row = 0; maps_row < maps_n_row; maps_row++) {
							if (enabled[maps_row][maps_col]) isensor++; else continue;
							// place active and passive volume
							std::stringstream _module_name;
							_module_name << "_plane_" << i_plane << "_disc_"
									<< i_cvd_disc;
							_module_name << "_side_" << iside ;//<< "_die_" << idie;
							_module_name << "_col_" << maps_col << "_row_"
									<< maps_row;
							//"LumActiveRect" is the keyword for digitization of hits
							TGeoVolume* _vol_active =
									new TGeoVolume(
											("LumActivePixelRect"
													+ _module_name.str()).c_str(),
											shape_maps_active,
											gGeoMan->GetMedium("silicon"));
							_vol_active->SetLineColor(36);
							TGeoVolume
									* _vol_passive =
											new TGeoVolume(
													("LumPassiveRect"
															+ _module_name.str()).c_str(),
													shape_maps_passive,
													gGeoMan->GetMedium("silicon"));
							_vol_passive->SetLineColor(30);
							double _rot_x(0.);
							double _rot_y(0.);
							double _rot_z(0.);
							double _trans_z(0.);
							double _trans_y(0.);
							double _trans_x(0.);
							Get_pos_sens_local(i_plane, i_cvd_disc, iside,
									isensor, _trans_x, _trans_y, _trans_z, _rot_x,
									_rot_y, _rot_z, misalign);

							TGeoRotation* _rot = new TGeoRotation(
									("rot_" + _module_name.str()).c_str());
							_rot->RotateX(_rot_x / pi * 180.);
							_rot->RotateY(_rot_y / pi * 180.);
							_rot->RotateZ(_rot_z / pi * 180.);
							TGeoTranslation* _trans = new TGeoTranslation(_trans_x,
									_trans_y, _trans_z);
							TGeoCombiTrans* _combtrans = new TGeoCombiTrans(
									*_trans, *_rot);
							_combtrans->SetName(
									("combtrans_" + _module_name.str()).c_str());
							_combtrans->RegisterYourself();
							delete _rot;
							delete _trans;
							_SubunitVol->AddNode(_vol_active, 0, _combtrans);
							_SubunitVol->AddNode(_vol_passive, 0, _combtrans);
							// some checks
							int sensor_id_check = Get_sensor_id(i_plane, i_cvd_disc, iside, isensor);
							if ( sensor_id_check != sensor_id){
								cout << "Error: Sensor id " << sensor_id_check << " does not correspond to the constructed order " << sensor_id << " !"  << endl;
							}
							int _iplane, _imodule, _iside, _isensor;
							Get_sensor_by_id(sensor_id, _iplane, _imodule, _iside, _isensor);
							if ( _iplane != i_plane || _imodule != i_cvd_disc || _iside != iside || _isensor != isensor){
								cout << "Error: wrong Sensor returned by id" << endl;
							}
							sensor_id++;
						}
					}
				}
			}
		}

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

void PndLmdDim::Init_transrot_matrices(){
	if (!fgGeoMan){
		cout << "Error in PndLmdDim::Init_transrot_matrices: Geo manager not available" << endl;
		return;
	}
	// Find the Node containing the reference frame name by
	// asking for the node at it's position


	//TGeoVolume* fgGeoMan->FindVolumeFast(nav_paths[1].c_str());
	//nav_paths[1];
}

void PndLmdDim::Transform_global_to_lmd_local(double& x, double& y, double& z){
	;
}
