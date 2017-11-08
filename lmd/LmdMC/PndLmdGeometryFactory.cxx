/*
 * PndLmdGeometryFactory.cxx
 *
 *  Created on: Nov 6, 2017
 *      Author: steve
 */

#include "PndLmdGeometryFactory.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoBuilder.h"
#include "FairGeoMedia.h"
#include "FairLogger.h"

#include "TROOT.h"
#include "TGeoManager.h"
#include "TGeoTube.h"
#include "TGeoVolume.h"
#include "TGeoPcon.h"
#include "TGeoCone.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TMath.h"

using boost::property_tree::ptree;

PndLmdGeometryFactory::PndLmdGeometryFactory(
		const ptree& geometry_property_tree_) :
		geometry_property_tree(geometry_property_tree_), gGeoMan(
				(TGeoManager*) gROOT->FindObject("FAIRGeom")) {

	auto pt_general = geometry_property_tree.get_child("general");
	for (ptree::value_type &nav_path : pt_general.get_child("navigation_paths")) {
		navigation_paths.push_back(nav_path.second.get_value<std::string>());
	}
}

PndLmdGeometryFactory::~PndLmdGeometryFactory() {
}

void PndLmdGeometryFactory::retrieveMaterial(FairGeoLoader* geoLoad) {
	FairGeoMedia *Media = geoLoad->getGeoInterface()->getMedia();
	FairGeoBuilder *geobuild = geoLoad->getGeoBuilder();

	// retrieve available media
	FairGeoMedium* FairMediumAir = Media->getMedium("air");
	FairGeoMedium* FairMediumSteel = Media->getMedium("steel");
	FairGeoMedium* FairMediumAl = Media->getMedium("Aluminum");
	FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");
	FairGeoMedium *FairMediumDiamond = Media->getMedium("HYPdiamond");
	FairGeoMedium *FairMediumVacuum = Media->getMedium("vacuum7");
	FairGeoMedium *FairMediumKapton = Media->getMedium("kapton");
	FairGeoMedium *FairMediumCopper = Media->getMedium("copper");

	if (!FairMediumAir || !FairMediumSteel || !FairMediumAl || !FairMediumKapton
			|| !FairMediumSilicon || !FairMediumVacuum || !FairMediumCopper) {
		LOG(ERROR)
									<< "PndLmdGeometryFactory::retrieveMaterial: not all media found ";
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
}

void PndLmdGeometryFactory::generateLmdGeometry(
		TGeoVolume& mother_volume) const {

	auto pt_general = geometry_property_tree.get_child("general");

	TGeoRotation* lmd_rot = new TGeoRotation("lmd_rot");
	lmd_rot->RotateX(0.0);
	lmd_rot->RotateY(
			pt_general.get<double>("pipe_bend_segment_phi") / 3.14 * 180.);
	lmd_rot->RotateZ(0.0);
	lmd_rot->RegisterYourself();

	TGeoCombiTrans *lmd_frame_transformation = new TGeoCombiTrans(
			"lmd_translation", 0., 0.,
			pt_general.get<double>("first_plane_position_z"), lmd_rot);
	lmd_frame_transformation->RegisterYourself();

	// generate vacuum box
	TGeoVolume *lmd_vac_box = generateVacuumBox();
	// generate beam pipe segment
	generateBeamPipe(*lmd_vac_box);

	// generate upper detector half
	//generateDetectorHalf();

	// generate lower detector half
	//generateDetectorHalf();

	// place correctly in mother volume
	mother_volume.AddNode(lmd_vac_box, 1, lmd_frame_transformation);
}

TGeoVolume* PndLmdGeometryFactory::generateVacuumBox() const {
	auto pt_general = geometry_property_tree.get_child("general");
	auto pt_vac_box = geometry_property_tree.get_child("vacuum_box");

	// general remark: watch out! TGeoShape values are often given has half sizes!
	double box_dim_x(pt_vac_box.get<double>("dimension_x"));
	double box_dim_y(pt_vac_box.get<double>("dimension_y"));
	double box_dim_z(pt_vac_box.get<double>("dimension_z"));
	double box_thickness(pt_vac_box.get<double>("wall_thickness"));

	// the lmd box
	TGeoBBox* lmd_box_outer = new TGeoBBox("lmd_box_outer", box_dim_x / 2.0,
			box_dim_y / 2.0, box_dim_z / 2.0);
	TGeoBBox* lmd_box_inner = new TGeoBBox("lmd_box_inner",
			box_dim_x / 2.0 - box_thickness, box_dim_y / 2.0 - box_thickness,
			box_dim_z / 2.0 - box_thickness);

	// the two beampipe holes
	TGeoTube* box_hole_upstream = new TGeoTube("box_hole_upstream", 0.0,
			geometry_property_tree.get_child("beam_pipe.entrance_pipe").get<double>(
					"inner_radius"), box_thickness / 2.0 + 0.1); // 0.1 safety margin
	// move the cut pipe upstream
	TGeoTranslation* comb_trans_cut_pipe_upstream = new TGeoTranslation(
			"comb_trans_cut_pipe_upstream", 0., 0.,
			-box_dim_z / 2.0 + box_thickness / 2.0);
	comb_trans_cut_pipe_upstream->RegisterYourself();

	TGeoTube* box_hole_downstream = new TGeoTube("box_hole_downstream", 0.0,
			geometry_property_tree.get_child("beam_pipe.exit_pipe").get<double>(
					"inner_radius"), box_thickness / 2.0 + 0.1);
	// move the cut pipe downstream
	TGeoTranslation* comb_trans_cut_pipe_downstream = new TGeoTranslation(
			"comb_trans_cut_pipe_downstream", 0., 0.,
			+box_dim_z / 2.0 - box_thickness / 2.0);
	comb_trans_cut_pipe_downstream->RegisterYourself();

	// the z-plane reinforcement rib
	double rib_thickness = pt_vac_box.get_child("zplane_reinforcement_rib").get<
			double>("thickness");
	TGeoBBox* lmd_box_rib = new TGeoBBox("lmd_box_rib",
			box_dim_x / 2.0 - box_thickness, box_dim_y / 2.0 - box_thickness,
			rib_thickness / 2.);
	TGeoTube* rib_hole = new TGeoTube("rib_hole", 0.0,
			geometry_property_tree.get_child("beam_pipe.entrance_pipe").get<double>(
					"inner_radius"), rib_thickness / 2.0 + 0.1);
	// move rib upstream
	double rib_position_z = pt_vac_box.get_child("zplane_reinforcement_rib").get<
			double>("position_z");
	TGeoTranslation* comb_trans_rib = new TGeoTranslation("comb_trans_rib", 0.,
			0., -box_dim_z / 2.0 + rib_position_z);
	comb_trans_rib->RegisterYourself();

	// the horizontal clash plates for the detector halves
	auto pt_clash_plates = geometry_property_tree.get_child(
			"vacuum_box.horizontal_detector_half_clash_plates");
	double clash_plate_dim_x = pt_clash_plates.get<double>("width");
	double clash_plate_dim_y = pt_clash_plates.get<double>("thickness");
	double clash_plate_dim_z = box_dim_z - rib_position_z - rib_thickness / 2.0
			- box_thickness;

	double origin_left[3] = { -box_dim_x / 2.0 + clash_plate_dim_x / 2.0
			+ box_thickness, 0., -box_dim_z / 2.0 + rib_position_z
			+ rib_thickness / 2.0 + clash_plate_dim_z / 2.0 };
	TGeoBBox* lmd_box_clash_rod_left = new TGeoBBox("lmd_box_clash_rod_left",
			clash_plate_dim_x / 2.0, clash_plate_dim_y / 2.0, clash_plate_dim_z / 2.0,
			origin_left);
	double origin_right[3] = { +box_dim_x / 2.0 - clash_plate_dim_x / 2.0
			- box_thickness, 0., -box_dim_z / 2.0 + rib_position_z
			+ rib_thickness / 2.0 + clash_plate_dim_z / 2.0 };
	TGeoBBox* lmd_box_clash_rod_right = new TGeoBBox("lmd_box_clash_rod_right",
			clash_plate_dim_x / 2.0, clash_plate_dim_y / 2.0, clash_plate_dim_z / 2.0,
			origin_right);

	// compose all the parts into one luminosity vacuum box
	TGeoCompositeShape *shape_lmd_box = new TGeoCompositeShape("shape_lmd_box",
			"(lmd_box_outer-lmd_box_inner + ((lmd_box_rib-rib_hole):comb_trans_rib))"
					"-box_hole_upstream:comb_trans_cut_pipe_upstream"
					"-box_hole_downstream:comb_trans_cut_pipe_downstream"
					"+lmd_box_clash_rod_left+lmd_box_clash_rod_right");

	TGeoVolume *lmd_vol_box = new TGeoVolume("lmd_vol_box", shape_lmd_box,
			gGeoMan->GetMedium("steel"));
	lmd_vol_box->SetLineColor(11);

	double origin[3] = { 0., 0., box_dim_z / 2.0 };
	TGeoBBox* lmd_box_vac = new TGeoBBox("lmd_box_vac", box_dim_x / 2.0,
			box_dim_y / 2.0, box_dim_z / 2.0, origin);
	TGeoCompositeShape *shape_lmd_vac = new TGeoCompositeShape("shape_lmd_vac",
			"lmd_box_vac-shape_lmd_box");
	TGeoVolume *lmd_vol_vac = new TGeoVolume(navigation_paths[0].c_str(),
			shape_lmd_vac, gGeoMan->GetMedium("vacuum7"));
	//TGeoTranslation* comb_trans_lmd_box = new TGeoTranslation(
	//		"comb_trans_lmd_box", 0., 0., box_dim_z);
	//comb_trans_pipe_upstream->RegisterYourself();
	lmd_vol_vac->AddNode(lmd_vol_box, 0);
	return lmd_vol_vac;
}

void PndLmdGeometryFactory::generateBeamPipe(TGeoVolume& mother_volume) const {
	auto pt_vac_box = geometry_property_tree.get_child("vacuum_box");
	auto pt_beam_pipe = geometry_property_tree.get_child("beam_pipe");

	double box_dim_z = pt_vac_box.get<double>("dimension_z");

	// upstream pipe part - outside of lmd
	auto pt_ent_pipe = pt_beam_pipe.get_child("entrance_pipe");
	double entrance_inner_radius(pt_ent_pipe.get<double>("inner_radius"));
	TGeoTube* pipe_upstream = new TGeoTube("pipe_upstream", entrance_inner_radius,
			entrance_inner_radius + pt_ent_pipe.get<double>("thickness"),
			pt_ent_pipe.get<double>("length") / 2.0);
	TGeoTranslation* comb_trans_pipe_upstream = new TGeoTranslation(
			"comb_trans_pipe_upstream", 0., 0.,
			-box_dim_z / 2.0 - pt_ent_pipe.get<double>("length") / 2.0);
	TGeoVolume* lmd_vol_pipe_up = new TGeoVolume("lmd_vol_pipe_up", pipe_upstream,
			gGeoMan->GetMedium("steel"));
	lmd_vol_pipe_up->SetLineColor(11);
	mother_volume.AddNode(lmd_vol_pipe_up, 0, comb_trans_pipe_upstream);

	// clamp for transition cone steel and aluminum part
	auto pt_cone_clamp = pt_beam_pipe.get_child("cone_clamp_ring");
	double cone_clamp_outer_radius = pt_cone_clamp.get<double>("outer_radius");
	double steel_part_thickness = pt_cone_clamp.get<double>(
			"steel_part_thickness");
	double alu_part_thickness = pt_cone_clamp.get<double>(
			"aluminum_part_thickness");
	double cone_angle = pt_beam_pipe.get_child("transition_cone").get<double>(
			"angle_in_degrees") * TMath::Pi() / 180;

	TGeoPcon* steel_cone_clamp = new TGeoPcon(0., 360., 4);
	steel_cone_clamp->DefineSection(0, 0.0, entrance_inner_radius,
			cone_clamp_outer_radius);
	steel_cone_clamp->DefineSection(1, steel_part_thickness,
			entrance_inner_radius, cone_clamp_outer_radius);
	steel_cone_clamp->DefineSection(2, steel_part_thickness,
			entrance_inner_radius,
			entrance_inner_radius + alu_part_thickness * std::tan(cone_angle));
	steel_cone_clamp->DefineSection(3, steel_part_thickness + alu_part_thickness,
			entrance_inner_radius, entrance_inner_radius);

	TGeoVolume* steel_cone_clamp_vol = new TGeoVolume("steel_cone_clamp",
			steel_cone_clamp, gGeoMan->GetMedium("steel"));
	steel_cone_clamp_vol->SetLineColor(11);
	double cone_clamp_start_pos_z = -box_dim_z / 2.0
			+ pt_vac_box.get<double>("wall_thickness");
	TGeoTranslation* lmd_trans_lmd_cone_clamp = new TGeoTranslation(
			"lmd_trans_lmd_cone_clamp", 0., 0., cone_clamp_start_pos_z);
	lmd_trans_lmd_cone_clamp->RegisterYourself();
	mother_volume.AddNode(steel_cone_clamp_vol, 0, lmd_trans_lmd_cone_clamp);

	TGeoPcon* alu_cone_clamp = new TGeoPcon(0., 360., 2);
	alu_cone_clamp->DefineSection(0, steel_part_thickness,
			entrance_inner_radius + alu_part_thickness * std::tan(cone_angle),
			cone_clamp_outer_radius);
	alu_cone_clamp->DefineSection(1, steel_part_thickness + alu_part_thickness,
			entrance_inner_radius, cone_clamp_outer_radius);

	TGeoVolume* alu_cone_clamp_vol = new TGeoVolume("steel_cone_clamp",
			alu_cone_clamp, gGeoMan->GetMedium("aluminum"));
	alu_cone_clamp_vol->SetLineColor(kGray);
	mother_volume.AddNode(alu_cone_clamp_vol, 0, lmd_trans_lmd_cone_clamp);

	// transition cone
	auto pt_cone = pt_beam_pipe.get_child("transition_cone");
	double alu_cone_thickness(pt_cone.get<double>("aluminum_thickness"));
	double polyamide_cone_thickness(pt_cone.get<double>("polyamide_thickness"));
	double lmd_beampipe_outer_radius(
			pt_beam_pipe.get_child("inner_pipe").get<double>("outer_radius"));
	double cone_length = (entrance_inner_radius - lmd_beampipe_outer_radius)
			/ std::tan(cone_angle);
	// 10 mu thick kapton foil aluminum coating
	TGeoCone* lmd_al_cone = new TGeoCone("lmd_al_cone", cone_length/2.0,
			entrance_inner_radius, entrance_inner_radius + alu_cone_thickness,
			lmd_beampipe_outer_radius,
			lmd_beampipe_outer_radius + alu_cone_thickness);
	TGeoVolume *vol_al_cone = new TGeoVolume("al_cone", lmd_al_cone,
			gGeoMan->GetMedium("Aluminum"));
	vol_al_cone->SetLineColor(kGray); //39);
	TGeoTranslation* lmd_trans_cone = new TGeoTranslation("lmd_trans_cone",
			0., 0.,
			cone_clamp_start_pos_z + steel_part_thickness + alu_part_thickness + cone_length/2.0);
	lmd_trans_cone->RegisterYourself();
	mother_volume.AddNode(vol_al_cone, 0, lmd_trans_cone);

	TGeoCone* lmd_capton_cone = new TGeoCone("lmd_capton_cone", cone_length/2.0,
			entrance_inner_radius + alu_cone_thickness,
			entrance_inner_radius + alu_cone_thickness + polyamide_cone_thickness,
			lmd_beampipe_outer_radius + alu_cone_thickness,
			lmd_beampipe_outer_radius + alu_cone_thickness
					+ polyamide_cone_thickness);

	TGeoVolume *vol_capton_cone = new TGeoVolume("capton_cone",
			lmd_capton_cone, gGeoMan->GetMedium("kapton"));
	vol_capton_cone->SetLineColor(kRed); //39);
	mother_volume.AddNode(vol_capton_cone, 0, lmd_trans_cone);

	/*
	 double lmd_pipe_params[36];
	 lmd_pipe_params[0] = 0.;
	 lmd_pipe_params[1] = 360.;
	 lmd_pipe_params[2] = 11.;

	 lmd_pipe_params[3] = cA2uiz;
	 lmd_pipe_params[4] = cA2uiy;
	 lmd_pipe_params[5] = cA2uoy;

	 lmd_pipe_params[6] = cA2diz;
	 lmd_pipe_params[7] = cA2diy;
	 lmd_pipe_params[8] = cA2doy;

	 lmd_pipe_params[9] = tA2diz;
	 lmd_pipe_params[10] = tA2diy;
	 lmd_pipe_params[11] = tA2doy;

	 lmd_pipe_params[12] = 0 + lmd_pipe_params[9];
	 lmd_pipe_params[13] = 3.5;
	 lmd_pipe_params[14] = 4.05;

	 lmd_pipe_params[15] = 1.5 + lmd_pipe_params[9];
	 lmd_pipe_params[16] = 3.5;
	 lmd_pipe_params[17] = 4.05;

	 lmd_pipe_params[18] = 3.5 + lmd_pipe_params[9];
	 lmd_pipe_params[19] = 4.2;
	 lmd_pipe_params[20] = 6.;

	 lmd_pipe_params[21] = 10. + lmd_pipe_params[9];
	 lmd_pipe_params[22] = 4.2;
	 lmd_pipe_params[23] = 6.;

	 lmd_pipe_params[24] = 10. + lmd_pipe_params[9];
	 lmd_pipe_params[25] = 4.2;
	 lmd_pipe_params[26] = 4.55;

	 lmd_pipe_params[27] = 20. + lmd_pipe_params[9];
	 lmd_pipe_params[28] = 4.2;
	 lmd_pipe_params[29] = 4.55;

	 lmd_pipe_params[30] = 20. + lmd_pipe_params[9];
	 lmd_pipe_params[31] = 4.2;
	 lmd_pipe_params[32] = 4.55; //7.;

	 lmd_pipe_params[33] = 22 + lmd_pipe_params[9];
	 lmd_pipe_params[34] = 4.2;
	 lmd_pipe_params[35] = 4.55; //7.;

	 TGeoPcon* lmd_V2_pipe = new TGeoPcon(lmd_pipe_params);
	 TGeoVolume* vlum_V2_pipe = new TGeoVolume("vlum_V2_pipe", lmd_V2_pipe,
	 fgGeoMan->GetMedium("steel"));
	 vlum_V2_pipe->SetLineColor(kGray); //39);
	 TGeoTranslation* lmd_trans_lmd_V2_pipe = new TGeoTranslation(
	 "lmd_trans_lmd_V2_pipe", 0., 0., box_inner_up_z);
	 lmd_trans_lmd_V2_pipe->RegisterYourself();
	 lmd_vol_vac->AddNode(vlum_V2_pipe, 0, lmd_trans_lmd_V2_pipe);

	 TGeoPcon* lmd_cond_ring = new TGeoPcon(lmd_conductor_params);
	 TGeoVolume* vlum_cond_ring = new TGeoVolume("vlum_cond_ring", lmd_cond_ring,
	 fgGeoMan->GetMedium("steel")); // should be brass
	 vlum_cond_ring->SetLineColor(kYellow); //39);
	 TGeoTranslation* lmd_trans_lmd_cond_ring = new TGeoTranslation(
	 "lmd_trans_lmd_cond_ring", 0., 0., box_inner_up_z);
	 lmd_trans_lmd_cond_ring->RegisterYourself();
	 lmd_vol_vac->AddNode(vlum_cond_ring, 0, lmd_trans_lmd_cond_ring);*/

	// downstream pipe part - outside of lmd
	auto pt_exit_pipe = pt_beam_pipe.get_child("exit_pipe");
	double inner_exit_radius = pt_exit_pipe.get<double>("inner_radius");
	double outer_exit_radius = inner_exit_radius
			+ pt_exit_pipe.get<double>("thickness");
	double tube_length = pt_exit_pipe.get<double>("length");
	double flange_outer_radius = pt_exit_pipe.get<double>("flange_outer_radius");
	double flange_thickness = pt_exit_pipe.get<double>("flange_thickness");

	TGeoPcon* shape_pipe_box_do = new TGeoPcon(0., 360., 4);
	shape_pipe_box_do->DefineSection(0, box_dim_z / 2.0, inner_exit_radius,
			outer_exit_radius);
	shape_pipe_box_do->DefineSection(1,
			box_dim_z / 2.0 + tube_length - flange_thickness, inner_exit_radius,
			outer_exit_radius);
	shape_pipe_box_do->DefineSection(2,
			box_dim_z / 2.0 + tube_length - flange_thickness, inner_exit_radius,
			flange_outer_radius);
	shape_pipe_box_do->DefineSection(3, box_dim_z / 2.0 + tube_length,
			inner_exit_radius, flange_outer_radius);

	TGeoVolume* vlum_pipe_box_do = new TGeoVolume("vlum_pipe_box_do",
			shape_pipe_box_do, gGeoMan->GetMedium("steel"));
	vlum_pipe_box_do->SetLineColor(kGray);	//39);
	TGeoTranslation* lmd_trans_pipe_box_do = new TGeoTranslation(
			"lmd_trans_pipe_box_do", 0., 0., 0.);
	lmd_trans_pipe_box_do->RegisterYourself();
	mother_volume.AddNode(vlum_pipe_box_do, 0, lmd_trans_pipe_box_do);

}

void PndLmdGeometryFactory::generateDetectorHalf(
		TGeoVolume& mother_volume) const {

}

void PndLmdGeometryFactory::generateCoolingStructure(
		TGeoVolume& mother_volume) const {
	// ****************************** cvd cooling support structure ********************
	/*
	 // construct first a tube
	 new TGeoTube("shape_cool_sup_tube",
	 lmd_cool_sup_inner_rad, lmd_cool_sup_outer_rad, lmd_cool_sup_thick);//TGeoTube* shape_cool_sup_tube =  //[R.K.03/2017] unused variable
	 // to cut off a half + a little bit
	 new TGeoBBox("shape_cool_sup_cut",
	 lmd_cool_sup_outer_rad, lmd_cool_sup_outer_rad, lmd_cool_sup_thick + 0.1);//TGeoBBox* shape_cool_sup_cut =  //[R.K.03/2017] unused variable
	 // set the position for the cut off
	 TGeoTranslation* trans_shape_cool_sup_cut_low = new TGeoTranslation(
	 "trans_shape_cool_sup_cut_low", 0., -lmd_cool_sup_outer_rad + 0.5, 0.); // 0.5 should be 0.3 but for the sake of simplicity not to clash to simple rod description
	 TGeoCombiTrans* combtrans_shape_cool_sup_cut_low = new TGeoCombiTrans(
	 *trans_shape_cool_sup_cut_low, *rot_no);
	 combtrans_shape_cool_sup_cut_low->SetName("combtrans_shape_cool_sup_cut_low");
	 combtrans_shape_cool_sup_cut_low->RegisterYourself();

	 TGeoTranslation* trans_shape_cool_sup_cut_high = new TGeoTranslation(
	 "trans_shape_cool_sup_cut_high", 0., +lmd_cool_sup_outer_rad - 0.5, 0.); // 0.5 should be 0.3 but for the sake of simplicity not to clash to simple rod description
	 TGeoCombiTrans* combtrans_shape_cool_sup_cut_high = new TGeoCombiTrans(
	 *trans_shape_cool_sup_cut_high, *rot_no);
	 combtrans_shape_cool_sup_cut_high->SetName(
	 "combtrans_shape_cool_sup_cut_high");
	 combtrans_shape_cool_sup_cut_high->RegisterYourself();

	 // We need some cut outs for the modules and the outer structure
	 // we give them a little bit more space for misalignment studies without clashing volumes
	 new TGeoTube("shape_module_cutout", 0.,
	 cvd_disc_rad + 0.05,
	 cvd_disc_thick_half + 2 * kapton_disc_thick_half + 0.01);//TGeoTube* shape_module_cutout =  //[R.K.03/2017] unused variable
	 for (size_t imodule = 0; imodule < nmodules * 2 ;
	 imodule++) {
	 double angle = delta_phi / 2. + imodule * delta_phi;
	 double add_z = cvd_disc_even_odd_offset;
	 // the offset of the modules in the upper and lower halfs
	 // are opposite
	 if (((imodule) % 2) == 0)
	 add_z = -add_z;
	 double _x = cos(angle) * cvd_disc_dist;
	 double _y = sin(angle) * cvd_disc_dist;
	 TGeoTranslation* trans_shape_module_cutout = new TGeoTranslation(_x, _y,
	 add_z);
	 TGeoCombiTrans* combtrans_shape_module_cutout = new TGeoCombiTrans(
	 *trans_shape_module_cutout, *rot_no);
	 stringstream _cutout_name;
	 _cutout_name << "rottrans_cutout_" << imodule;
	 combtrans_shape_module_cutout->SetName(_cutout_name.str().c_str());
	 combtrans_shape_module_cutout->RegisterYourself();

	 _x = cos(angle) * (lmd_cool_sup_outer_cut + lmd_cool_sup_outer_rad);
	 _y = sin(angle) * (lmd_cool_sup_outer_cut + lmd_cool_sup_outer_rad);
	 TGeoTranslation* trans_cool_sup_cut_outer = new TGeoTranslation(_x, _y, 0.);
	 stringstream _cutshape_rot_name;
	 _cutshape_rot_name << "cutshaperot_" << imodule;
	 TGeoRotation* rot_cool_sup_cut_outer = new TGeoRotation(
	 _cutshape_rot_name.str().c_str(), angle / pi * 180., 0., 0.);
	 TGeoCombiTrans* combtrans_cool_sup_cut_outer = new TGeoCombiTrans(
	 *trans_cool_sup_cut_outer, *rot_cool_sup_cut_outer);
	 stringstream _cutshape_name;
	 _cutshape_name << "cutshape_" << imodule;
	 combtrans_cool_sup_cut_outer->SetName(_cutshape_name.str().c_str());
	 combtrans_cool_sup_cut_outer->RegisterYourself();
	 }

	 // construct the support from basic shape and it's cut outs
	 TGeoCompositeShape *shape_cool_sup_up = new TGeoCompositeShape(
	 "shape_cool_sup_up",
	 "shape_cool_sup_tube-shape_cool_sup_cut:combtrans_shape_cool_sup_cut_low"
	 "-shape_module_cutout:rottrans_cutout_0"
	 "-shape_module_cutout:rottrans_cutout_1"
	 "-shape_module_cutout:rottrans_cutout_2"
	 "-shape_module_cutout:rottrans_cutout_3"
	 "-shape_module_cutout:rottrans_cutout_4"
	 "-shape_cool_sup_cut:cutshape_0"
	 "-shape_cool_sup_cut:cutshape_1"
	 "-shape_cool_sup_cut:cutshape_2"
	 "-shape_cool_sup_cut:cutshape_3"
	 "-shape_cool_sup_cut:cutshape_4");

	 TGeoVolume* lmd_vol_cool_sup_up = new TGeoVolume("lmd_vol_cool_sup_up",
	 shape_cool_sup_up, fgGeoMan->GetMedium("Aluminum"));
	 lmd_vol_cool_sup_up->SetLineColor(17);

	 // the lower cooling support has to be rotated, we create simply a new volume for it
	 TGeoRotation* rot_shape_cool_sup_down = new TGeoRotation(
	 "rot_shape_cool_sup_down", 180., 180., 0.);
	 TGeoCombiTrans* combtrans_shape_cool_sup_down = new TGeoCombiTrans(*trans_no,
	 *rot_shape_cool_sup_down);
	 combtrans_shape_cool_sup_down->SetName("combtrans_shape_cool_sup_down");
	 combtrans_shape_cool_sup_down->RegisterYourself();
	 // construct the support from basic shape and it's cut outs
	 TGeoCompositeShape *shape_cool_sup_down = new TGeoCompositeShape(
	 "shape_cool_sup_down",
	 "shape_cool_sup_tube-shape_cool_sup_cut:combtrans_shape_cool_sup_cut_high"
	 "-shape_module_cutout:rottrans_cutout_5"
	 "-shape_module_cutout:rottrans_cutout_6"
	 "-shape_module_cutout:rottrans_cutout_7"
	 "-shape_module_cutout:rottrans_cutout_8"
	 "-shape_module_cutout:rottrans_cutout_9"
	 "-shape_cool_sup_cut:cutshape_5"
	 "-shape_cool_sup_cut:cutshape_6"
	 "-shape_cool_sup_cut:cutshape_7"
	 "-shape_cool_sup_cut:cutshape_8"
	 "-shape_cool_sup_cut:cutshape_9");

	 TGeoVolume* lmd_vol_cool_sup_down = new TGeoVolume("lmd_vol_cool_sup_down",
	 shape_cool_sup_down, fgGeoMan->GetMedium("Aluminum"));
	 lmd_vol_cool_sup_down->SetLineColor(17);
	 */
}

void PndLmdGeometryFactory::generateSensorModule(
		TGeoVolume& mother_volume) const {
	/*	// ****************************** luminosity detector reference system ************************
	 // definition of the luminosity detector local system
	 TGeoCombiTrans* rottrans_lmd_in_box = new TGeoCombiTrans(
	 "rottrans_lmd_in_box", 0., 0., pos_plane_0, rot_no);
	 TGeoVolumeAssembly* lmd_vol_ref_sys = new TGeoVolumeAssembly(
	 nav_paths[1].c_str());
	 // definition of the retractable luminosity detector halves


	 // ****************************** cvd cooling support discs ************************
	 TGeoTubeSeg* shape_cvd_disc = new TGeoTubeSeg("shape_cvd_disc", inner_rad,
	 lmd_cool_sup_inner_rad - gap_between_disc_and_support_structure,
	 cvd_disc_thick_half, -delta_phi / 2. / pi * 180.,
	 +delta_phi / 2. / pi * 180.);

	 TGeoRotation* cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
	 TGeoTranslation* cvd_translation = new TGeoTranslation("cvd_translation",
	 -cvd_disc_dist, 0, 0);
	 TGeoCombiTrans* cvd_combtrans = new TGeoCombiTrans(*cvd_translation,
	 *cvd_rotation);
	 cvd_combtrans->SetName("cvd_combtrans");
	 cvd_combtrans->RegisterYourself();

	 //this next line is pretty stupid but it made the work for the better geometry minimal
	 //otherwise I would have to do some deeper digging and reworking...
	 TGeoCompositeShape *shape_cvd_support = new TGeoCompositeShape(
	 "shape_cvd_support",
	 "(shape_cvd_disc:cvd_combtrans+shape_cvd_disc:cvd_combtrans)");

	 TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
	 shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
	 lmd_vol_cvd_disc->SetLineColor(9);
	 // ****************************** kapton flexible circuits to the sensors ************************
	 // the cvd disc shape
	 TGeoTubeSeg* shape_kapton_disc = new TGeoTubeSeg("shape_kapton_disc",
	 inner_rad,
	 lmd_cool_sup_inner_rad - gap_between_disc_and_support_structure,
	 kapton_disc_thick_half, -delta_phi / 2. / pi * 180.,
	 +delta_phi / 2. / pi * 180.);

	 TGeoRotation* kapton_rotation = new TGeoRotation("kapton_rotation", 0, 0, 0);
	 TGeoTranslation* kapton_translation = new TGeoTranslation(
	 "kapton_translation", -cvd_disc_dist, 0, 0);
	 TGeoCombiTrans* kapton_combtrans = new TGeoCombiTrans(*kapton_translation,
	 *kapton_rotation);
	 kapton_combtrans->SetName("kapton_combtrans");
	 kapton_combtrans->RegisterYourself();

	 //this next line is pretty stupid but it made the work for the better geometry minimal
	 //otherwise I would have to do some deeper digging and reworking...
	 TGeoCompositeShape *shape_kapton_support =
	 new TGeoCompositeShape("shape_kapton_support",
	 "(shape_kapton_disc:kapton_combtrans+shape_kapton_disc:kapton_combtrans)");

	 TGeoVolume* lmd_vol_kapton_disc = new TGeoVolume("lmd_vol_kapton_disc",
	 shape_kapton_support, fgGeoMan->GetMedium("Aluminum"));	//kapton")); // changed to equivalent for glue/flex cable etc.
	 lmd_vol_kapton_disc->SetLineColor(kRed);
	 //lmd_vol_kapton_disc->SetTransparency(50);
	 lmd_vol_kapton_disc->SetVisibility(false);
	 // *********************************** HV-MAPS *************************************

	 // create basic shapes and their positions
	 TGeoBBox *shape_maps_active_centered = new TGeoBBox(
	 "shape_maps_active_centered", maps_active_width, maps_active_height,
	 maps_thickness);
	 TGeoCombiTrans* combtrans_maps_active = new TGeoCombiTrans(
	 "combtrans_maps_active",
	 -maps_width + maps_passive_left * 2. + maps_active_width,
	 -maps_height + maps_passive_bottom * 2. + maps_active_height, 0., rot_no);
	 combtrans_maps_active->RegisterYourself();
	 TGeoBBox *shape_maps_passive_left = new TGeoBBox("shape_maps_passive_left",
	 maps_passive_left, maps_height, maps_thickness);
	 TGeoCombiTrans* combtrans_maps_passive_left = new TGeoCombiTrans(
	 "combtrans_maps_passive_left", -maps_width + maps_passive_left, 0., 0.,
	 rot_no);
	 combtrans_maps_passive_left->RegisterYourself();
	 TGeoBBox *shape_maps_passive_right = new TGeoBBox("shape_maps_passive_right",
	 maps_passive_right, maps_height, maps_thickness);
	 TGeoCombiTrans* combtrans_maps_passive_right = new TGeoCombiTrans(
	 "combtrans_maps_passive_right", maps_width - maps_passive_right, 0., 0.,
	 rot_no);
	 combtrans_maps_passive_right->RegisterYourself();
	 TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top",
	 maps_width, maps_passive_top, maps_thickness);
	 TGeoCombiTrans* combtrans_maps_passive_top = new TGeoCombiTrans(
	 "combtrans_maps_passive_top", 0., maps_height - maps_passive_top, 0.,
	 rot_no);
	 combtrans_maps_passive_top->RegisterYourself();
	 TGeoBBox *shape_maps_passive_bottom = new TGeoBBox(
	 "shape_maps_passive_bottom", maps_width, maps_passive_bottom,
	 maps_thickness);
	 TGeoCombiTrans* combtrans_maps_passive_bottom = new TGeoCombiTrans(
	 "combtrans_maps_passive_bottom", 0., -maps_height + maps_passive_bottom,
	 0., rot_no);
	 combtrans_maps_passive_bottom->RegisterYourself();

	 if (!lmd_box_outer || !lmd_box_inner || !lmd_box_rib || !box_hole_upstream
	 || !box_hole_downstream || !shape_cvd_disc || !shape_kapton_disc
	 || !shape_maps_active_centered || !shape_maps_passive_left
	 || !shape_maps_passive_right || !shape_maps_passive_top
	 || !shape_maps_passive_bottom)
	 cout << " pedantic compiler together with root geometries sucks " << endl;

	 TGeoCompositeShape *shape_maps_passive =
	 new TGeoCompositeShape("shape_maps_passive",
	 "(shape_maps_passive_top:combtrans_maps_passive_top+shape_maps_passive_right:combtrans_maps_passive_right+shape_maps_passive_bottom:combtrans_maps_passive_bottom+shape_maps_passive_left:combtrans_maps_passive_left)");

	 TGeoCompositeShape *shape_maps_active = new TGeoCompositeShape(
	 "shape_maps_active",
	 "(shape_maps_active_centered:combtrans_maps_active-shape_maps_passive)");

	 TGeoVolume* _vol_passive = new TGeoVolume("LumPassiveRect",
	 shape_maps_passive, fgGeoMan->GetMedium("silicon"));
	 _vol_passive->SetLineColor(30);

	 TGeoVolume* _vol_active = new TGeoVolume(nav_paths[7].c_str(),
	 shape_maps_active, fgGeoMan->GetMedium("silicon"));
	 _vol_active->SetLineColor(36);
	 // **************************************************************

	 // ****************************** loops in the luminosity detector ************************
	 stringstream name;
	 stringstream uniqueid; // seems pandaroot has problems when volumes are not uniquely named
	 double _x(0), _y(0), _z(0), _rotphi(0), _rottheta(0), _rotpsi(0);
	 double _offset_x(0), _offset_y(0), _offset_z(0), _offset_phi(0),
	 _offset_theta(0), _offset_psi(0);
	 unsigned int sensor_id(0);
	 unsigned int module_id(0);
	 for (unsigned int ihalf = 0; ihalf < 2; ihalf++) { // loop over detector halves
	 // in order do be able to displace the detector halves those are introduced as
	 // separate volume assemblies
	 name.str("");
	 name << nav_paths[2];		// << ihalf;
	 uniqueid.str("");
	 //uniqueid << "_" << ihalf;
	 TGeoVolumeAssembly* lmd_vol_half_ = new TGeoVolumeAssembly(
	 name.str().c_str());
	 //mothervol.AddNode(lmd_vol_half_, 0, rottrans_no);
	 for (unsigned int iplane = 0; iplane < n_planes; iplane++) { // loop over planes
	 name.str("");
	 name << nav_paths[3]; // << iplane;
	 uniqueid.str("");
	 //uniqueid << "_" << ihalf << iplane;
	 TGeoVolumeAssembly* lmd_vol_plane_ = new TGeoVolumeAssembly(
	 (name.str() + uniqueid.str()).c_str());
	 // move to the position of the corresponding plane
	 TGeoMatrix* rottrans_plane = new TGeoCombiTrans(0., 0.,
	 plane_pos_z[iplane], rot_no);
	 if (misaligned) {
	 Get_offset(ihalf, iplane, -1, -1, -1, -1, _offset_x, _offset_y,
	 _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
	 TGeoRotation* rot_plane_offset = new TGeoRotation("rot_plane_offset",
	 _offset_phi / pi * 180., _offset_theta / pi * 180.,
	 _offset_psi / pi * 180.);
	 TGeoCombiTrans* rottrans_plane_offset = new TGeoCombiTrans(_offset_x,
	 _offset_y, _offset_z, rot_plane_offset);
	 //	rottrans_plane = new TGeoHMatrix(*rottrans_plane * *rottrans_plane_offset);
	 rottrans_plane = new TGeoHMatrix(
	 *rottrans_plane_offset * *rottrans_plane);
	 }
	 for (unsigned int imodule = 0; imodule < nmodules; imodule++) { // loop over modules
	 name.str("");
	 name << nav_paths[4]; // << imodule;
	 uniqueid.str("");
	 //uniqueid << "_" << ihalf << iplane << imodule;
	 TGeoVolumeAssembly* lmd_vol_module_ = new TGeoVolumeAssembly(
	 (name.str() + uniqueid.str()).c_str());
	 double angle = delta_phi / 2. + ihalf * pi + imodule * delta_phi;
	 double add_z = cvd_disc_even_odd_offset;
	 // the offset of the modules in the upper and lower halfs
	 // are opposite
	 if (((imodule + ihalf) % 2) == 0)
	 add_z = -add_z;
	 _x = cos(angle) * cvd_disc_dist;
	 _y = sin(angle) * cvd_disc_dist;
	 _z = add_z;
	 _rotphi = 0.;
	 _rottheta = 0.;
	 _rotpsi = angle / pi * 180.;
	 TGeoRotation* rot_module = new TGeoRotation("rot_module", _rotphi,
	 _rottheta, _rotpsi);
	 TGeoMatrix* rottrans_module = new TGeoCombiTrans(_x, _y, _z,
	 rot_module);
	 if (misaligned) {
	 Get_offset(ihalf, iplane, imodule, -1, -1, -1, _offset_x, _offset_y,
	 _offset_z, _offset_phi, _offset_theta, _offset_psi, true);
	 TGeoRotation* rot_module_offset = new TGeoRotation(
	 "rot_module_offset", _offset_phi / pi * 180.,
	 _offset_theta / pi * 180., _offset_psi / pi * 180.);
	 TGeoCombiTrans* rottrans_module_offset = new TGeoCombiTrans(_offset_x,
	 _offset_y, _offset_z, rot_module_offset);
	 rottrans_module = new TGeoHMatrix(
	 *rottrans_module_offset * *rottrans_module);
	 }

	 // add the cvd disc into that assembly
	 //TGeoVolume* lmd_vol_cvd_disc = new TGeoVolume("lmd_vol_cvd_disc",
	 //						shape_cvd_support, fgGeoMan->GetMedium("HYPdiamond"));
	 lmd_vol_module_->AddNode(lmd_vol_cvd_disc, module_id, rottrans_no);
	 module_id++;
	 for (unsigned int iside = 0; iside < 2; iside++) { // loop over the two sides of the modules
	 name.str("");
	 name << nav_paths[5]; // << iside;
	 uniqueid.str("");
	 //uniqueid << "_" << ihalf << iplane << imodule << iside;
	 TGeoVolumeAssembly* lmd_vol_side_ = new TGeoVolumeAssembly(
	 (name.str() + uniqueid.str()).c_str());
	 // rotation around the y axis for the upstream side
	 // side 0 is downstream! what may be not so obvious
	 _x = 0.;
	 _y = 0.;
	 _z = 0.;
	 if (iside == 0) {
	 _rotphi = 0.;
	 _rottheta = 0.;
	 _rotpsi = 0.;
	 } else {
	 _rotphi = 0.;
	 _rottheta = 180.;
	 _rotpsi = 0.;
	 }
	 TGeoRotation* rot_side = new TGeoRotation("rot_side", _rotphi,
	 _rottheta, _rotpsi);
	 TGeoMatrix* rottrans_side = new TGeoCombiTrans(_x, _y, _z, rot_side);
	 if (misaligned) {
	 Get_offset(ihalf, iplane, imodule, iside, -1, -1, _offset_x,
	 _offset_y, _offset_z, _offset_phi, _offset_theta, _offset_psi,
	 true);
	 TGeoRotation* rot_side_offset = new TGeoRotation("rot_side_offset",
	 _offset_phi / pi * 180., _offset_theta / pi * 180.,
	 _offset_psi / pi * 180.);
	 TGeoCombiTrans* rottrans_side_offset = new TGeoCombiTrans(_offset_x,
	 _offset_y, _offset_z, rot_side_offset);
	 rottrans_side = new TGeoHMatrix(
	 *rottrans_side_offset * *rottrans_side);
	 }
	 // glue the HV-MAPS to the cvd surface
	 for (unsigned int idie = 0; idie < 2; idie++) { // loop over dies
	 // it seems we will have only 2 sensors on a die and only a few of them
	 // -> term die will stay as it is, but may be different from the reality
	 name.str("");
	 name << nav_paths[6];						// << idie;
	 uniqueid.str("");
	 //uniqueid << "_" << ihalf << iplane << imodule << iside << idie;
	 TGeoVolumeAssembly* lmd_vol_die_ = new TGeoVolumeAssembly(
	 (name.str() + uniqueid.str()).c_str());
	 // rotation to the cut side of the cvd_disc
	 // the origin is the inner edge
	 const double _sinhalf = sin(delta_phi / 2.);
	 //const double _coshalf = cos (delta_phi/2.);
	 const double _edge_y = +_sinhalf * inner_rad;
	 // angle between the edge and the half of the cvd disc
	 const double _edgeangle = asin(-_edge_y / cvd_disc_rad);
	 const double _edge_x = -cvd_disc_rad * cos(_edgeangle);
	 _x = _edge_x;
	 _y = _edge_y;
	 _z = +cvd_disc_thick_half + maps_thickness; // move to the surface
	 _rotphi = +delta_phi / 2. / pi * 180.;
	 _rottheta = 0.;
	 _rotpsi = 0.; // rotate to the cut edge
	 TGeoRotation* rot_die = new TGeoRotation("rot_die", _rotphi,
	 _rottheta, _rotpsi);
	 TGeoCombiTrans* rottrans_die = new TGeoCombiTrans(_x, _y, _z,
	 rot_die);
	 // construct now the sensors on the two dies
	 for (unsigned int isensor = 0; isensor < 3; isensor++) { // loop over sensors
	 // the 0th die is oriented at the inner edge
	 _x = +maps_width + maps_width * 2. * isensor;
	 _y = -maps_height;
	 _z = 0.;
	 // only the inner two sensors will possibly on one physical die
	 // so take an offset into account
	 if (isensor == 2)
	 _x = _x + die_gap;
	 // next row
	 if (idie == 1) {
	 if (isensor == 0)
	 continue;
	 _y -= die_gap + 2. * maps_height;
	 _rotphi = 0.;
	 } else {
	 _rotphi = 0.;
	 }
	 _rottheta = 0.;
	 _rotpsi = 0.;
	 //"LumActiveRect" is the keyword for digitization of hits

	 TGeoRotation* rot_sensor = new TGeoRotation("rot_sensor", _rotphi,
	 _rottheta, _rotpsi);
	 TGeoMatrix* rottrans_sensor = new TGeoCombiTrans(_x, _y, _z,
	 rot_sensor);
	 if (misaligned) {
	 Get_offset(ihalf, iplane, imodule, iside, idie, isensor,
	 _offset_x, _offset_y, _offset_z, _offset_phi, _offset_theta,
	 _offset_psi, true);
	 TGeoRotation* rot_sensor_offset = new TGeoRotation(
	 "rot_sensor_offset", _offset_phi / pi * 180.,
	 _offset_theta / pi * 180., _offset_psi / pi * 180.);
	 TGeoCombiTrans* rottrans_sensor_offset = new TGeoCombiTrans(
	 _offset_x, _offset_y, _offset_z, rot_sensor_offset);
	 rottrans_sensor = new TGeoHMatrix(
	 *rottrans_sensor_offset * *rottrans_sensor);
	 }
	 lmd_vol_die_->AddNode(_vol_active, sensor_id, rottrans_sensor);
	 lmd_vol_die_->AddNode(_vol_passive, sensor_id, rottrans_sensor);
	 //cout << sensor_id << " " << _vol_active->GetName() << endl << endl;
	 // save the transformation from the cvd_side reference frame
	 // into the local frame of the sensors
	 transformation_matrices[Tkey(ihalf, iplane, imodule, iside, idie,
	 isensor)] = new TGeoHMatrix(
	 (*rottrans_die) * (*rottrans_sensor));

	 if (0) { // some tests for debugging
	 unsigned int _sensor_id = Get_sensor_id(ihalf, iplane, imodule,
	 iside, idie, isensor);
	 if (sensor_id != _sensor_id) {
	 cout << " wrong sensor id " << _sensor_id << " != "
	 << sensor_id << endl;
	 }
	 int _ihalf, _iplane, _imodule, _iside, _idie, _isensor;
	 Get_sensor_by_id(sensor_id, _ihalf, _iplane, _imodule, _iside,
	 _idie, _isensor);
	 if ((signed) ihalf != _ihalf)
	 cout << " wrong half " << _ihalf << endl;
	 if ((signed) iplane != _iplane)
	 cout << " wrong plane " << _iplane << endl;
	 if ((signed) imodule != _imodule)
	 cout << " wrong module " << _imodule << endl;
	 if ((signed) iside != _iside)
	 cout << " wrong side " << _iside << endl;
	 if ((signed) idie != _idie)
	 cout << " wrong die " << _idie << endl;
	 if ((signed) isensor != _isensor)
	 cout << " wrong sensor " << _isensor << endl;
	 }
	 sensor_id++;
	 } // loop over sensors
	 lmd_vol_side_->AddNode(lmd_vol_die_, idie, rottrans_die);
	 } // loop over dies
	 _x = 0;
	 _y = 0;
	 _z = +cvd_disc_thick_half + maps_thickness * 2.
	 + kapton_disc_thick_half; // move to the surface
	 _rotphi = 0.;
	 _rottheta = 0.;
	 _rotpsi = 0.; // rotate to the cut edge
	 TGeoRotation* rot_kapton = new TGeoRotation("rot_kapton", _rotphi,
	 _rottheta, _rotpsi);
	 TGeoCombiTrans* rottrans_kapton = new TGeoCombiTrans(_x, _y, _z,
	 rot_kapton);
	 lmd_vol_side_->AddNode(lmd_vol_kapton_disc, 0, rottrans_kapton); // GGenerate_Tkeyumber(ihalf,iplane, imodule, iside)
	 lmd_vol_module_->AddNode(lmd_vol_side_, iside, rottrans_side);
	 // save the transformation from the lumi reference frame
	 // into the local cvd side reference frame
	 transformation_matrices[Tkey(ihalf, iplane, imodule, iside, -1, -1)] =
	 new TGeoHMatrix(
	 (*rottrans_plane) * (*rottrans_module) * (*rottrans_side));
	 } // loop over the two sides of the modules
	 lmd_vol_plane_->AddNode(lmd_vol_module_, imodule, rottrans_module);
	 // transformation_matrices[Generate_Tkey(ihalf, iplane, imodule, -1, -1, -1)] =
	 //   new TGeoHMatrix((*rottrans_plane) * (*rottrans_module));
	 } // loop over modules
	 if (ihalf == 0)
	 lmd_vol_plane_->AddNode(lmd_vol_cool_sup_up, iplane, rottrans_no);
	 else
	 lmd_vol_plane_->AddNode(lmd_vol_cool_sup_down, iplane, rottrans_no);
	 lmd_vol_half_->AddNode(lmd_vol_plane_, iplane, rottrans_plane);
	 // // save the transformation from the lumi reference frame
	 // // into the plane reference frame
	 // transformation_matrices[Generate_Tkey(ihalf, iplane, -1, -1, -1, -1)] =
	 //   new TGeoHMatrix((*rottrans_plane));
	 } // loop over planes
	 lmd_vol_ref_sys->AddNode(lmd_vol_half_, ihalf, rottrans_no);
	 // save the transformation into the lumi reference frame
	 transformation_matrices[Tkey(-1, -1, -1, -1, -1, -1)] = new TGeoHMatrix(
	 (*lmd_transrot) * (*rottrans_lmd_in_box));
	 } // loop over detector halves
	 // code geometry version in the node title
	 stringstream nodetitle;
	 nodetitle << "version " << geometry_version << endl;
	 lmd_vol_vac->SetTitle(nodetitle.str().c_str());
	 lmd_vol_vac->AddNode(lmd_vol_ref_sys, 0, rottrans_lmd_in_box);
	 mothervol.AddNode(lmd_vol_vac, geometry_version, lmd_transrot);*/
}

void PndLmdGeometryFactory::generateSensor(TGeoVolume& mother_volume) const {

}
