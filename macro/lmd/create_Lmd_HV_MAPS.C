// compile with
// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/buildPanda/lib -lGeoBase create_Lmd_HV_MAPS.C -o /tmp/create_geometry -I${PANDAROOT}/lmd/LmdMC
// and run with
// /tmp/create_geometry


#include<TROOT.h>
#include<FairGeoLoader.h>
#include<FairGeoInterface.h>
#include<FairGeoBuilder.h>
#include<FairGeoPcon.h>
#include<FairGeoMedia.h>
#include<TGeoCompositeShape.h>
#include<TGeoMatrix.h>
#include<TGeoVolume.h>
#include<TGeoTube.h>
#include<TGeoArb8.h>
#include<TGeoPara.h>
#include<TSystem.h>
#include<TFile.h>
#include<TGeoPcon.h>
#include<TGeoManager.h>
#include<TGeoTorus.h>
#include<TGeoSphere.h>
#include<TMath.h>
#include<TRandom.h>
#include<PndLmdDim.h>
#include<TGeoPhysicalNode.h>
#include<TGeoCone.h>

#include<iostream>
#include<cmath>
#include<sstream>

//using namespace LmdDim;

// to check the position and dimensions
// one may enable a beam pipe dummy to be displayed
const bool show_beam_pipe_dummy = false;

const bool include_box = true;

void create_HV_MAPS(bool misalign = false) {
	// ****************************** Parameters of Detector ************************
	//--------------------------------------------------------------------
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	// Load this libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libPndData");
	gSystem->Load("libPassive");

	TString outfile = "geo/HV_MAPS-Design.root";
	TFile* fi = new TFile(outfile, "RECREATE");

	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile("../../geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();

	FairGeoMedia *Media = geoFace->getMedia();
	FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

	// retrieve available media
	std::string str_ti = "Aluminum"; // "titanium" is in Panda software not yet implemented
	FairGeoMedium* FairMediumAir = Media->getMedium("air");
	FairGeoMedium* FairMediumSteel = Media->getMedium("steel");
	FairGeoMedium* FairMediumTi = Media->getMedium(str_ti.c_str()); //titanium"); not found in media_pnd.geo !
	FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");
	FairGeoMedium *FairMediumDiamond = Media->getMedium("HYPdiamond");
	FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum");
	FairGeoMedium *FairMediumKapton = Media->getMedium("kapton");

	if (!FairMediumAir || !FairMediumSteel || !FairMediumTi || !FairMediumKapton || !FairMediumSilicon || !FairMediumVacuum) {
		std::cout << " warning: not all media found " << std::endl;
	}

	int nmed;
	nmed = geobuild->createMedium(FairMediumAir);
	nmed = geobuild->createMedium(FairMediumSteel);
	nmed = geobuild->createMedium(FairMediumTi);
	nmed = geobuild->createMedium(FairMediumKapton);
	nmed = geobuild->createMedium(FairMediumSilicon);
	nmed = geobuild->createMedium(FairMediumDiamond);
	nmed = geobuild->createMedium(FairMediumVacuum);

	TGeoManager* gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	TGeoManager* geom = gGeoMan;

	TGeoVolume *top = new TGeoVolumeAssembly("lmd_HV_MAPS");

	gGeoMan->SetTopVolume(top);

	PndLmdDim& lmddim = PndLmdDim::Get_instance();

	// *********** some useful rotations *****************
	TGeoRotation* georot_no = new TGeoRotation("georot_no", 0., 0., 0.); // no rotation
	TGeoRotation* georot_90x_180y = new TGeoRotation("georot_90x_180y", 0.,
			90., 180.); //  90 deg about x-axis and 180 deg about y axis
	TGeoRotation* georot_270x_180y = new TGeoRotation("georot_270x_180y", 0.,
			-90., 180.); //  -90 deg about x-axis and 180 deg about y axis
	TGeoRotation* georot_270x = new TGeoRotation("georot_270x", 0., -90., 0); // -90 deg about x-axis
	TGeoRotation* georot_90z = new TGeoRotation("georot_90z", 90., 0., 0); //  90 deg about z-axis

	TGeoCombiTrans* comb_trans_no = new TGeoCombiTrans("comb_trans_no", 0., 0.,
			0., georot_no);
	comb_trans_no->RegisterYourself();

	double x, y, z, rotx, roty, rotz;
	lmddim.Get_pos_lmd_global(x, y, z, rotx, roty, rotz);
	TGeoRotation* global_rot = new TGeoRotation("global_rot");
	global_rot->RotateX(rotx/lmddim.pi*180.);
	global_rot->RotateY(roty/lmddim.pi*180.);
	global_rot->RotateZ(rotz/lmddim.pi*180.);
	TGeoCombiTrans* global_transrot = new TGeoCombiTrans(x, y, z, global_rot);
	global_transrot->SetName("global_transrot");
	global_transrot->RegisterYourself();

	// ************ create the luminosity monitor box ***********
	// create the bounding box
	double tube_upstream_length = 25./2.;
	double tube_upstream_rad_out = 9.;
	double tube_upstream_rad_in = tube_upstream_rad_out - 0.2;

	// create a vacuum around the luminosity detector
	double lmd_total_length = lmddim.box_size_z + tube_upstream_length;
	double origin[3] = {0.,0., lmd_total_length};
	TGeoBBox* lmd_box_vacuum =
			new TGeoBBox("lmd_box_vacuum",
					lmddim.box_size_x,
					lmddim.box_size_y,
					lmd_total_length, origin);
	TGeoVolume *vol_lmd_vac = new TGeoVolume("vol_lmd_vac", lmd_box_vacuum,
					gGeoManager->GetMedium("vacuum"));
	//vol_lmd_vac->SetTransparency(20);
	vol_lmd_vac->SetLineColor(3);
	top->AddNode(vol_lmd_vac, 0, global_transrot);

	// pumping station upstream
	TGeoTube* pipe_upstream =
			new TGeoTube("pipe_upstream", tube_upstream_rad_in, tube_upstream_rad_out, tube_upstream_length);
	TGeoCombiTrans* comb_trans_pipe_upstream = new TGeoCombiTrans("comb_trans_pipe_upstream",
			0., 0., tube_upstream_length, georot_no);
	comb_trans_pipe_upstream->RegisterYourself();
	TGeoVolume* vol_lmd_pipe_up =
			new TGeoVolume("vol_lmd_pipe_up", pipe_upstream, gGeoManager->GetMedium("steel"));
	vol_lmd_pipe_up->SetLineColor(11);
	if (include_box)
		vol_lmd_vac->AddNode(vol_lmd_pipe_up, 0, comb_trans_pipe_upstream);
	// the lmd box
	TGeoBBox* lmd_box_outer
		= new TGeoBBox("lmd_box_outer",
				lmddim.box_size_x , lmddim.box_size_y  , lmddim.box_size_z );
	TGeoBBox* lmd_box_inner
		= new TGeoBBox("lmd_box_inner",
				lmddim.box_size_x - lmddim.box_thickness,
				lmddim.box_size_y - lmddim.box_thickness,
				lmddim.box_size_z - lmddim.box_thickness);
	TGeoBBox* lmd_box_rib
		= new TGeoBBox("lmd_box_rib",
			lmddim.box_size_x - lmddim.box_thickness,
			lmddim.box_size_y - lmddim.box_thickness,
			lmddim.box_thickness);
	TGeoTube* box_hole_upstream =
			new TGeoTube("box_hole_upstream", 0.0, lmddim.rad_entrance, lmddim.box_thickness);
	// move the cut pipe upstream
	TGeoCombiTrans* comb_trans_cut_pipe_upstream = new TGeoCombiTrans("comb_trans_cut_pipe_upstream",
			0., 0., -lmddim.box_size_z+lmddim.box_thickness/2., georot_no);
	comb_trans_cut_pipe_upstream->RegisterYourself();
	TGeoTube* box_hole_downstream =
				new TGeoTube("box_hole_downstream", 0.0, lmddim.rad_exit, lmddim.box_thickness);
	// move the cut pipe downstream
	TGeoCombiTrans* comb_trans_cut_pipe_downstream = new TGeoCombiTrans("comb_trans_cut_pipe_downstream",
				0., 0., +lmddim.box_size_z-lmddim.box_thickness/2., georot_no);
	comb_trans_cut_pipe_downstream->RegisterYourself();
	// compose all the parts into one luminosity vacuum box
	TGeoCompositeShape *shape_lmd_box = new TGeoCompositeShape("shape_lmd_box",
			"(lmd_box_outer-lmd_box_inner)-box_hole_upstream:comb_trans_cut_pipe_upstream-box_hole_downstream:comb_trans_cut_pipe_downstream");
	TGeoVolume *vol_lmd_box = new TGeoVolume("vol_lmd_box", shape_lmd_box,
				gGeoManager->GetMedium("steel"));
	vol_lmd_box->SetLineColor(11);
	TGeoCombiTrans* comb_trans_lmd_box = new TGeoCombiTrans("comb_trans_lmd_box",
			0., 0., 2*tube_upstream_length+lmddim.box_size_z, georot_no);
	comb_trans_pipe_upstream->RegisterYourself();
	if (include_box)
		vol_lmd_vac->AddNode(vol_lmd_box, 0, comb_trans_lmd_box);

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
	TGeoCombiTrans* lmd_trans_cap_co = new TGeoCombiTrans("lmd_trans_cap_co", 0., 0., 2*tube_upstream_length+lmddim.box_thickness + cone_height, georot_no);
	lmd_trans_cap_co->RegisterYourself();
	TGeoVolume *vlum_CaptonCone = new TGeoVolume("vlum_CaptonCone", lmd_capton_cone,
			gGeoManager->GetMedium("kapton"));
	vlum_CaptonCone->SetLineColor(kRed);//39);
	if (include_box)
		vol_lmd_vac->AddNode(vlum_CaptonCone, 0, lmd_trans_cap_co);
	// beam pipe to shield the sensors
	double pipe_inner_r_in = 7./2.;
	double pipe_inner_length = 60./2.;
	double pipe_thickness = 0.1;
	TGeoTube* lmd_beam_pipe = new TGeoTube("lmd_beam_pipe", pipe_inner_r_in, pipe_inner_r_in + pipe_thickness, pipe_inner_length);
	TGeoCombiTrans* lmd_trans_p = new TGeoCombiTrans("lmd_trans_p", 0., 0., 2*tube_upstream_length+lmddim.box_thickness + 2*cone_height + pipe_inner_length, georot_no);
	lmd_trans_p->RegisterYourself();
	TGeoVolume *vlum_trans_p = new TGeoVolume("vlum_trans_p", lmd_beam_pipe,
				gGeoManager->GetMedium("steel"));
	if (include_box)
		vol_lmd_vac->AddNode(vlum_trans_p, 0, lmd_trans_p);
	// beam pipe cone downstream
	double cone_p_height = 10./2.;
	double cone_p_r_in_upstream = pipe_inner_r_in;
	double cone_p_r_in_downstream = 9./2.;
	double cone_p_thickness = 0.2;
	TGeoCone* lmd_cone_downstr = new TGeoCone("lmd_cone_downstr", cone_p_height, cone_p_r_in_upstream,
			cone_p_r_in_upstream+cone_p_thickness/2.,
			cone_p_r_in_downstream, cone_p_r_in_downstream+cone_p_thickness/2.);
	TGeoCombiTrans* lmd_trans_co_do = new TGeoCombiTrans("lmd_trans_co_do", 0., 0.,
			2*tube_upstream_length+lmddim.box_thickness + 2*cone_height + 2*pipe_inner_length + cone_p_height, georot_no);
	lmd_trans_co_do->RegisterYourself();
	TGeoVolume *vlum_pipe_inner_cone = new TGeoVolume("vlum_pipe_inner_cone", lmd_cone_downstr,
				gGeoManager->GetMedium("steel"));
	if (include_box)
		vol_lmd_vac->AddNode(vlum_pipe_inner_cone, 0, lmd_trans_co_do);
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
	//	lmd_trans_p_down->RegisterYourself();

	// ****************************** beam pipe dummy **********************************
	if (show_beam_pipe_dummy) {
		TGeoShape* shape_ip_sphere(NULL);
		TGeoShape* shape_beam_pipe_target_dipole(NULL);
		TGeoShape* shape_beam_pipe_dipole(NULL);
		TGeoShape* shape_beam_pipe_dipole_lmd(NULL);
		TGeoShape* shape_beam_pipe(NULL);
		TGeoCombiTrans* comb_trans_target_dipole(NULL);
		TGeoCombiTrans* comb_trans_dipole(NULL);
		TGeoCombiTrans* comb_trans_dipole_lmd(NULL);
		TGeoRotation* georot_dipole_lmd(NULL);
		TGeoVolume* vol_beam_pipe(NULL);
		// create the shapes
		const double length_dipole_lmd = (lmddim.pos_z - lmddim.end_seg_bend);
		shape_beam_pipe_target_dipole = new TGeoTube(
				"shape_beam_pipe_target_dipole", 3.5, 3.6,
				lmddim.end_seg_upstream / 2.);
		shape_beam_pipe_dipole = new TGeoTorus("shape_beam_pipe_dipole",
				lmddim.r_bend, 3.5, 3.6, 0.,
				lmddim.phi_bend / lmddim.pi * 180.);
		shape_beam_pipe_dipole_lmd = new TGeoTube("shape_beam_pipe_dipole_lmd",
				3.5, 3.6, length_dipole_lmd / 2.);
		shape_ip_sphere = new TGeoSphere("shape_ip_sphere", 0., 3.5);

		// the corresponding transformations
		comb_trans_target_dipole = new TGeoCombiTrans(
				"comb_trans_target_dipole", 0., 0.,
				lmddim.end_seg_upstream / 2., georot_no);
		comb_trans_target_dipole->RegisterYourself();
		comb_trans_dipole = new TGeoCombiTrans("comb_trans_dipole",
				lmddim.r_bend, 0., lmddim.end_seg_upstream, georot_270x_180y);
		comb_trans_dipole->RegisterYourself();
		georot_dipole_lmd = new TGeoRotation("georot_dipole_lmd", 90.,
				lmddim.phi_bend / lmddim.pi * 180., -90.);
		double _trans_x = lmddim.r_bend * (1. - cos(lmddim.phi_bend))
				+ length_dipole_lmd / 2. * lmddim.phi_bend;
		comb_trans_dipole_lmd = new TGeoCombiTrans("comb_trans_dipole_lmd",
				_trans_x, 0., lmddim.end_seg_bend + length_dipole_lmd / 2.,
				georot_dipole_lmd);
		comb_trans_dipole_lmd->RegisterYourself();

		// combine the shapes
		shape_beam_pipe
				= new TGeoCompositeShape(
						"shape_beam_pipe",
						"shape_ip_sphere:comb_trans_no+shape_beam_pipe_target_dipole:comb_trans_target_dipole"
							"+shape_beam_pipe_dipole:comb_trans_dipole+shape_beam_pipe_dipole_lmd:comb_trans_dipole_lmd");

		vol_beam_pipe = new TGeoVolume("vol_beam_pipe", shape_beam_pipe,
				gGeoManager->GetMedium("steel"));
		vol_beam_pipe->SetLineColor(31);
		top->AddNode(vol_beam_pipe, 0, comb_trans_no);
	}


	// ****************************** cvd cooling support discs ************************

	// the cvd disc shape
	TGeoTube* shape_cvd_disc = new TGeoTube("shape_cvd_disc", 0.,
			lmddim.cvd_disc_rad, lmddim.cvd_disc_thick_half);
	// The inner beam pipe defines the inner acceptance region for the cvd cut_out
	TGeoTube* shape_cvd_cutout_inner = new TGeoTube("shape_cvd_cutout_inner",
			0., lmddim.inner_rad, 1.);
	// finally cvd discs will be cut at the left and right down to 36 degree in phi
	// for that we subtract tube segments
	TGeoTubeSeg* shape_cvd_disc_cut_side = new TGeoTubeSeg(
			"shape_cvd_disc_cut_side", 0., lmddim.outer_rad, 1.,
			+lmddim.delta_phi / 2. / lmddim.pi * 180.,
			-lmddim.delta_phi / 2. / lmddim.pi * 180.);
	// before: cvd disc was moved to the displaced position around the z axis
	// now: segments for the cut are moved off centered and cvd disc remains in the center
	TGeoRotation* cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
	TGeoTranslation* cvd_translation = new TGeoTranslation("cvd_translation",
			-lmddim.cvd_disc_dist, 0, 0);
	TGeoCombiTrans* cvd_combtrans = new TGeoCombiTrans(*cvd_translation,
			*cvd_rotation);
	cvd_combtrans->SetName("cvd_combtrans");
	cvd_combtrans->RegisterYourself();
	TGeoCompositeShape
			*shape_cvd_support =
					new TGeoCompositeShape(
							"shape_cvd_support",
							"(shape_cvd_disc-shape_cvd_cutout_inner:cvd_combtrans-shape_cvd_disc_cut_side:cvd_combtrans)");

	TGeoVolume* vol_test = new TGeoVolume("vol_test", shape_cvd_support,
			gGeoManager->GetMedium("steel"));
	// *********************************** HV-MAPS *************************************

	// create basic shapes and their positions
	TGeoBBox *shape_maps_active_centered = new TGeoBBox(
			"shape_maps_active_centered", lmddim.maps_active_width,
			lmddim.maps_active_height, lmddim.maps_thickness);
	TGeoCombiTrans* combtrans_maps_active = new TGeoCombiTrans(
			"combtrans_maps_active",
			-lmddim.maps_width + lmddim.maps_passive_left * 2.
					+ lmddim.maps_active_width,
			-lmddim.maps_height + lmddim.maps_passive_bottom * 2.
					+ lmddim.maps_active_height, 0., georot_no);
	combtrans_maps_active->RegisterYourself();
	TGeoBBox *shape_maps_passive_left = new TGeoBBox("shape_maps_passive_left",
			lmddim.maps_passive_left, lmddim.maps_height,
			lmddim.maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_left = new TGeoCombiTrans(
			"combtrans_maps_passive_left",
			-lmddim.maps_width + lmddim.maps_passive_left, 0., 0., georot_no);
	combtrans_maps_passive_left->RegisterYourself();
	TGeoBBox *shape_maps_passive_right = new TGeoBBox(
			"shape_maps_passive_right", lmddim.maps_passive_right,
			lmddim.maps_height, lmddim.maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_right = new TGeoCombiTrans(
			"combtrans_maps_passive_right",
			lmddim.maps_width - lmddim.maps_passive_right, 0., 0., georot_no);
	combtrans_maps_passive_right->RegisterYourself();
	TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top",
			lmddim.maps_width, lmddim.maps_passive_top,
			lmddim.maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_top = new TGeoCombiTrans(
			"combtrans_maps_passive_top", 0.,
			lmddim.maps_height - lmddim.maps_passive_top, 0., georot_no);
	combtrans_maps_passive_top->RegisterYourself();
	TGeoBBox *shape_maps_passive_bottom = new TGeoBBox(
			"shape_maps_passive_bottom", lmddim.maps_width,
			lmddim.maps_passive_bottom, lmddim.maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_bottom = new TGeoCombiTrans(
			"combtrans_maps_passive_bottom", 0.,
			-lmddim.maps_height + lmddim.maps_passive_bottom, 0., georot_no);
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

	// **************************************************************

	TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Lumi_HV-MAPS");

	int sensor_id = 0;
	for (int i_plane = 0; i_plane < 4; i_plane++) {
		std::stringstream plane_name;
		plane_name << "plane_" << (i_plane + 1);
		for (int i_cvd_disc = 0; i_cvd_disc < lmddim.n_cvd_discs; i_cvd_disc++) {
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
			lmddim.Get_pos_mod_local(i_plane, i_cvd_disc, _x, _y, _z, _rotx,
					_roty, _rotz, misalign);
			TGeoRotation* rot = new TGeoRotation(rot_name.str().c_str());
			rot->RotateX(_rotx / lmddim.pi * 180.);
			rot->RotateY(_roty / lmddim.pi * 180.);
			rot->RotateZ(_rotz / lmddim.pi * 180.);
			TGeoTranslation* trt = new TGeoTranslation(_x, _y, _z);//lmddim.pos_z + lmddim.plane_pos_z[i_plane]  - lmddim.pos_rot_z);
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
				for (int maps_col = 0; maps_col < lmddim.maps_n_col; maps_col++) {
					for (int maps_row = 0; maps_row < lmddim.maps_n_row; maps_row++) {
						if (lmddim.enabled[maps_row][maps_col]) isensor++; else continue;
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
						lmddim.Get_pos_sens_local(i_plane, i_cvd_disc, iside,
								isensor, _trans_x, _trans_y, _trans_z, _rot_x,
								_rot_y, _rot_z, misalign);

						TGeoRotation* _rot = new TGeoRotation(
								("rot_" + _module_name.str()).c_str());
						_rot->RotateX(_rot_x / lmddim.pi * 180.);
						_rot->RotateY(_rot_y / lmddim.pi * 180.);
						_rot->RotateZ(_rot_z / lmddim.pi * 180.);
						TGeoTranslation* _trans = new TGeoTranslation(_trans_x,
								_trans_y, _trans_z);
						TGeoCombiTrans* _combtrans = new TGeoCombiTrans(
								*_trans, *_rot);
						_combtrans->SetName(
								("combtrans_" + _module_name.str()).c_str());
						_combtrans->RegisterYourself();
						delete _rot;
						delete _trans;
						_SubunitVol->AddNode(_vol_active, sensor_id + 1, _combtrans);
						_SubunitVol->AddNode(_vol_passive, sensor_id + 1, _combtrans);
						// some checks
						int sensor_id_check = lmddim.Get_sensor_id(i_plane, i_cvd_disc, iside, isensor);
						if ( sensor_id_check != sensor_id){
							cout << "Error: Sensor id " << sensor_id_check << " does not correspond to the constructed order " << sensor_id << " !"  << endl;
						}
						int _iplane, _imodule, _iside, _isensor;
						lmddim.Get_sensor_by_id(sensor_id, _iplane, _imodule, _iside, _isensor);
						if ( _iplane != i_plane || _imodule != i_cvd_disc || _iside != iside || _isensor != isensor){
							cout << "Error: wrong Sensor returned by id" << endl;
						}
						sensor_id++;
					}
				}
			}
		}
	}

	TGeoCombiTrans* comb_trans_lmd_in_box = new TGeoCombiTrans("comb_trans_lmd_in_box", 0., 0.,
			lmddim.pos_plane_0, georot_no);
	//comb_trans_lmd_in_box->RegisterYourself();
	vol_lmd_vac->AddNode(SubunitVol, 0, comb_trans_lmd_in_box);
	//top->AddNode(SubunitVol, 0, global_transrot);
	gGeoMan->CloseGeometry();
	//gGeoMan->Get
	//gGeoMan->cd("/lmd_HV_MAPS_1/vol_lmd_vac_1");
	//cout << gGeoMan->GetPath()<< endl;
	//TGeoPhysicalNode* node =
	//		gGeoMan->MakePhysicalNode("/lmd_HV_MAPS_1");///Lumi_HV-MAPS/LumPassive_cvd_disc_plane_2_disc_3");
	//TGeoCombiTrans* align_trans = new TGeoCombiTrans(1.,0.,0.,georot_no);
	//node->Align(align_trans);
	//gGeoMan->CloseGeometry();
	//gGeoMan->Set

	top->Write();
	fi->Close();
	//   gGeoManager->Export(outfile);

	top->Draw("ogl");
}

#include<TApplication.h>

int main() {
	TApplication myapp("myapp", 0, 0);
	create_HV_MAPS();
	myapp.Run();
	return 0;
}
