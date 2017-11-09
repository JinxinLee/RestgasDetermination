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
				(TGeoManager*) gROOT->FindObject("FAIRGeom")), active_sensor_volume(
		nullptr), passive_sensor_volume(nullptr), aluminum_support_volume(
		nullptr), cvd_disc_volume(nullptr), sensor_cables_volume(nullptr), global_sensor_id_counter(0), global_aluminum_support_counter(
				0), global_cvd_disc_counter(0) {

	auto pt_general = geometry_property_tree.get_child("general");
	for (ptree::value_type &nav_path : pt_general.get_child("navigation_paths")) {
		navigation_paths.push_back(nav_path.second.get_value<std::string>());
	}
}

PndLmdGeometryFactory::~PndLmdGeometryFactory() {
}

void PndLmdGeometryFactory::init(FairGeoLoader* geoLoad) {
	retrieveMaterial(geoLoad);

	generateSensor();
	generateCoolingStructures();
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
	generateDetectorHalf(true);

	// generate lower detector half
	generateDetectorHalf(false);

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
	double rib_inner_distance_to_front_plate = pt_vac_box.get_child(
			"zplane_reinforcement_rib").get<double>(
			"inner_distance_to_box_front_plate");
	TGeoTranslation* comb_trans_rib = new TGeoTranslation("comb_trans_rib", 0.,
			0.,
			-box_dim_z / 2.0 + box_thickness + rib_inner_distance_to_front_plate
					+ rib_thickness / 2.0);
	comb_trans_rib->RegisterYourself();

	// the horizontal clash plates for the detector halves
	auto pt_clash_plates = geometry_property_tree.get_child(
			"vacuum_box.horizontal_detector_half_clash_plates");
	double clash_plate_dim_x = pt_clash_plates.get<double>("width");
	double clash_plate_dim_y = pt_clash_plates.get<double>("thickness");
	double clash_plate_dim_z = box_dim_z - rib_inner_distance_to_front_plate
			- rib_thickness - box_thickness;

	double origin_left[3] = { -box_dim_x / 2.0 + clash_plate_dim_x / 2.0
			+ box_thickness, 0., -box_dim_z / 2.0 + box_thickness
			+ rib_inner_distance_to_front_plate + rib_thickness
			+ clash_plate_dim_z / 2.0 };
	TGeoBBox* lmd_box_clash_rod_left = new TGeoBBox("lmd_box_clash_rod_left",
			clash_plate_dim_x / 2.0, clash_plate_dim_y / 2.0, clash_plate_dim_z / 2.0,
			origin_left);
	double origin_right[3] = { +box_dim_x / 2.0 - clash_plate_dim_x / 2.0
			- box_thickness, 0., -box_dim_z / 2.0 + box_thickness
			+ rib_inner_distance_to_front_plate + rib_thickness
			+ clash_plate_dim_z / 2.0 };
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

	TGeoVolume* steel_cone_clamp_vol = new TGeoVolume("steel_cone_clamp_vol",
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

	TGeoVolume* alu_cone_clamp_vol = new TGeoVolume("alu_cone_clamp_vol",
			alu_cone_clamp, gGeoMan->GetMedium("Aluminum"));
	alu_cone_clamp_vol->SetLineColor(kGray);
	mother_volume.AddNode(alu_cone_clamp_vol, 0, lmd_trans_lmd_cone_clamp);

	// transition cone
	auto pt_cone = pt_beam_pipe.get_child("transition_cone");
	double alu_cone_thickness(pt_cone.get<double>("aluminum_thickness"));
	double polyamide_cone_thickness(pt_cone.get<double>("polyamide_thickness"));
	auto pt_inner_pipe = pt_beam_pipe.get_child("inner_pipe");
	double inner_beampipe_outer_radius(pt_inner_pipe.get<double>("outer_radius"));
	double inner_beampipe_thickness(pt_inner_pipe.get<double>("thickness"));
	double inner_beampipe_inner_radius(
			inner_beampipe_outer_radius - inner_beampipe_thickness);
	double cone_length = (entrance_inner_radius - inner_beampipe_inner_radius)
			/ std::tan(cone_angle);

	TGeoCone* lmd_al_cone = new TGeoCone("lmd_al_cone", cone_length / 2.0,
			entrance_inner_radius, entrance_inner_radius + alu_cone_thickness,
			inner_beampipe_inner_radius,
			inner_beampipe_inner_radius + alu_cone_thickness);
	TGeoVolume *vol_al_cone = new TGeoVolume("al_cone", lmd_al_cone,
			gGeoMan->GetMedium("Aluminum"));
	vol_al_cone->SetLineColor(kGray); //39);
	TGeoTranslation* lmd_trans_cone = new TGeoTranslation("lmd_trans_cone", 0.,
			0.,
			cone_clamp_start_pos_z + steel_part_thickness + alu_part_thickness
					+ cone_length / 2.0);
	lmd_trans_cone->RegisterYourself();
	mother_volume.AddNode(vol_al_cone, 0, lmd_trans_cone);

	TGeoCone* lmd_capton_cone = new TGeoCone("lmd_capton_cone", cone_length / 2.0,
			entrance_inner_radius + alu_cone_thickness,
			entrance_inner_radius + alu_cone_thickness + polyamide_cone_thickness,
			inner_beampipe_inner_radius + alu_cone_thickness,
			inner_beampipe_inner_radius + alu_cone_thickness
					+ polyamide_cone_thickness);
	TGeoVolume *vol_capton_cone = new TGeoVolume("capton_cone", lmd_capton_cone,
			gGeoMan->GetMedium("kapton"));
	vol_capton_cone->SetLineColor(kRed); //39);
	mother_volume.AddNode(vol_capton_cone, 0, lmd_trans_cone);

	// inner beam pipe part
	double inner_beampipe_length(pt_inner_pipe.get<double>("length"));
	double inner_beampipe_cone_foil_attach_segement_length(
			pt_inner_pipe.get<double>("length_cone_foil_attach_segment"));
	double outer_radius_cone_foil_glue_segment(
			inner_beampipe_outer_radius
					+ std::tan(cone_angle)
							* inner_beampipe_cone_foil_attach_segement_length);

	TGeoPcon* lmd_inner_pipe = new TGeoPcon(0.0, 360.0, 3);
	lmd_inner_pipe->DefineSection(0, 0.0,
			outer_radius_cone_foil_glue_segment - inner_beampipe_thickness,
			outer_radius_cone_foil_glue_segment);
	lmd_inner_pipe->DefineSection(1,
			inner_beampipe_cone_foil_attach_segement_length,
			inner_beampipe_inner_radius, inner_beampipe_outer_radius);
	lmd_inner_pipe->DefineSection(2,
			inner_beampipe_cone_foil_attach_segement_length + inner_beampipe_length,
			inner_beampipe_inner_radius, inner_beampipe_outer_radius);

	TGeoVolume* vol_inner_pipe = new TGeoVolume("vol_inner_pipe", lmd_inner_pipe,
			gGeoMan->GetMedium("steel"));
	vol_inner_pipe->SetLineColor(11); //39);
	double inner_pipe_start_position_z(
			cone_clamp_start_pos_z + steel_part_thickness + alu_part_thickness
					+ cone_length - inner_beampipe_cone_foil_attach_segement_length);
	TGeoTranslation* lmd_trans_inner_pipe = new TGeoTranslation(
			"lmd_trans_inner_pipe", 0., 0., inner_pipe_start_position_z);
	lmd_trans_inner_pipe->RegisterYourself();
	mother_volume.AddNode(vol_inner_pipe, 0, lmd_trans_inner_pipe);

	// bellow
	auto pt_exit_pipe = pt_beam_pipe.get_child("exit_pipe");
	double inner_exit_radius = pt_exit_pipe.get<double>("inner_radius");

	auto pt_bellow = pt_beam_pipe.get_child("bellow");
	double flange_to_inner_pipe_outer_radius(
			inner_beampipe_inner_radius
					+ pt_bellow.get<double>(
							"flange_to_inner_beampipe.outer_radius_increase"));
	TGeoPcon* beampipe_bellow = new TGeoPcon(0., 360., 6);
	double tempzpos(
			inner_pipe_start_position_z
					+ inner_beampipe_cone_foil_attach_segement_length
					+ inner_beampipe_length);
	beampipe_bellow->DefineSection(0, tempzpos, inner_beampipe_inner_radius,
			flange_to_inner_pipe_outer_radius);
	tempzpos += pt_bellow.get<double>("flange_to_inner_beampipe.thickness");
	beampipe_bellow->DefineSection(1, tempzpos, inner_beampipe_inner_radius,
			flange_to_inner_pipe_outer_radius);
	tempzpos += pt_bellow.get<double>(
			"flange_to_inner_beampipe.outer_radius_increase")
			/ std::tan(
					pt_bellow.get<double>("inner_angle_in_degrees") / 180 * TMath::Pi());
	beampipe_bellow->DefineSection(2, tempzpos, inner_exit_radius,
			inner_exit_radius + pt_bellow.get<double>("thickness"));
	tempzpos += (box_dim_z / 2.0 - pt_vac_box.get<double>("wall_thickness")
			- tempzpos - pt_bellow.get<double>("flange_to_box.thickness"));
	beampipe_bellow->DefineSection(3, tempzpos, inner_exit_radius,
			inner_exit_radius + pt_bellow.get<double>("thickness"));
	beampipe_bellow->DefineSection(4, tempzpos, inner_exit_radius,
			inner_exit_radius + pt_bellow.get<double>("thickness")
					+ pt_bellow.get<double>("flange_to_box.outer_radius_increase"));
	tempzpos += pt_bellow.get<double>("flange_to_box.thickness");
	beampipe_bellow->DefineSection(5, tempzpos, inner_exit_radius,
			inner_exit_radius + pt_bellow.get<double>("thickness")
					+ pt_bellow.get<double>("flange_to_box.outer_radius_increase"));

	TGeoVolume* vol_beampipe_bellow = new TGeoVolume("vol_beampipe_bellow",
			beampipe_bellow, gGeoMan->GetMedium("steel"));
	vol_beampipe_bellow->SetLineColor(11);	//39);
	mother_volume.AddNode(vol_beampipe_bellow, 0);

	// downstream pipe part - outside of lmd
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
	vlum_pipe_box_do->SetLineColor(11);	//39);
	mother_volume.AddNode(vlum_pipe_box_do, 0);
}

TGeoVolume* PndLmdGeometryFactory::generateDetectorHalf(
		bool is_upper_half) const {

	// make four half planes
	// for loop
	// step 1: AddNode of cooling support to mother volume
	// step 2: equip this cooling support with sensors for loop over number of sensors per half plane

}

/*TGeoVolume* PndLmdGeometryFactory::generateDetectorHalfPlane(
		unsigned int plane_index) const {

}*/

void PndLmdGeometryFactory::generateCoolingStructures() {
	if (nullptr == aluminum_support_volume) {
		auto pt_cool_support = geometry_property_tree.get_child("cooling_support");

		double outer_radius(pt_cool_support.get<double>("outer_radius"));
		double inner_radius(pt_cool_support.get<double>("inner_radius"));
		double radius_diff(outer_radius - inner_radius);

		// construct first a tube segment
		TGeoTubeSeg* cool_support_tube_segment = new TGeoTubeSeg(
				"cool_support_tube_segment", inner_radius, outer_radius,
				pt_cool_support.get<double>("thickness") / 2.0, 0, 180);

		// to cut off little bit below to adapt to clash planes
		double clash_plane_thickness(
				geometry_property_tree.get<double>(
						"vacuum_box.horizontal_detector_half_clash_plates.thickness"));

		TGeoBBox* clash_plane_cut = new TGeoBBox("cool_support_clash_plane_cutoff",
				radius_diff + 0.1, clash_plane_thickness / 2.0 + 0.001,
				pt_cool_support.get<double>("thickness") + 0.1);

		/*
		 // We need some cut outs for the modules and the outer structure
		 // we give them a little bit more space for misalignment studies without clashing volumes
		 TGeoTube* shape_module_cutout = new TGeoTube("shape_module_cutout", 0., cvd_disc_rad + 0.05,
		 cvd_disc_thick_half + 2 * kapton_disc_thick_half + 0.01);
		 for (size_t imodule = 0; imodule < nmodules * 2; imodule++) {
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
		 }*/

		TGeoTranslation* trans_clash_cut_left = new TGeoTranslation(
				"trans_clash_cut_left", -inner_radius - radius_diff / 2.0, 0.0, 0.);
		trans_clash_cut_left->RegisterYourself();
		TGeoTranslation* trans_clash_cut_right = new TGeoTranslation(
				"trans_clash_cut_right", +inner_radius + radius_diff / 2.0, 0.0, 0.);
		trans_clash_cut_right->RegisterYourself();

		// construct the support from basic shape and it's cut outs
		TGeoCompositeShape *cool_support =
				new TGeoCompositeShape("cool_support",
						"cool_support_tube_segment-cool_support_clash_plane_cutoff:trans_clash_cut_left-cool_support_clash_plane_cutoff:trans_clash_cut_right");

		aluminum_support_volume = new TGeoVolume("vol_cool_support", cool_support,
				gGeoMan->GetMedium("Aluminum"));
		aluminum_support_volume->SetLineColor(kGray);

		// cvd disc
		auto pt_cvd_disc = pt_cool_support.get_child("cvd_disc");
		unsigned int num_modules_per_plane(
				geometry_property_tree.get<double>("general.modules_per_half_plane"));
		double gap_between_disc_and_support_structure(0.1);
		TGeoTubeSeg* cvd_disc = new TGeoTubeSeg("cvd_disc",
				pt_cvd_disc.get<double>("inner_radius"),
				inner_radius - gap_between_disc_and_support_structure,
				pt_cvd_disc.get<double>("thickness"), 0.0,
				180.0 / num_modules_per_plane);

		cvd_disc_volume = new TGeoVolume("lmd_vol_cvd_disc", cvd_disc,
				gGeoMan->GetMedium("HYPdiamond"));
		cvd_disc_volume->SetLineColor(9);
	}
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


	 // *********************************** HV-MAPS *************************************


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

void PndLmdGeometryFactory::generateSensor() {
	if (nullptr == active_sensor_volume) {
		auto pt_sensors = geometry_property_tree.get_child("sensors");
		auto pt_active_part = pt_sensors.get_child("active_part");

		double sensor_dim_x(pt_sensors.get<double>("dimension_x"));
		double sensor_dim_y(pt_sensors.get<double>("dimension_x"));
		double thickness(pt_sensors.get<double>("thickness"));

		double active_part_dim_x(pt_active_part.get<double>("dimension_x"));
		double active_part_dim_y(pt_active_part.get<double>("dimension_y"));
		double active_part_offset_x(pt_active_part.get<double>("offset_x"));
		double active_part_offset_y(pt_active_part.get<double>("offset_y"));

		TGeoBBox *sensor_full_centered = new TGeoBBox("sensor_full_centered",
				sensor_dim_x / 2.0, sensor_dim_y / 2.0, thickness / 2.0);

		TGeoBBox *sensor_active_centered = new TGeoBBox("sensor_active_centered",
				active_part_dim_x / 2.0, active_part_dim_y / 2.0, thickness / 2.0);

		TGeoTranslation* trans_sensor_active = new TGeoTranslation(
				"trans_sensor_active",
				-sensor_dim_x / 2.0 + active_part_dim_x / 2.0 + active_part_offset_x,
				-sensor_dim_y / 2.0 + active_part_dim_y / 2.0 + active_part_offset_y,
				0.);
		trans_sensor_active->RegisterYourself();

		TGeoCompositeShape *sensor_passive_centered = new TGeoCompositeShape(
				"sensor_passive_centered",
				"(sensor_full_centered-trans_sensor_active:trans_sensor_active)");

		passive_sensor_volume = new TGeoVolume("LumPassiveRect",
				sensor_passive_centered, gGeoMan->GetMedium("silicon"));
		passive_sensor_volume->SetLineColor(30);

		active_sensor_volume = new TGeoVolume(navigation_paths[7].c_str(),
				sensor_active_centered, gGeoMan->GetMedium("silicon"));
		active_sensor_volume->SetLineColor(36);

		// put cable on top of the sensor
		/*TGeoTubeSeg* shape_kapton_disc = new TGeoTubeSeg("shape_kapton_disc",
				inner_rad,
				lmd_cool_sup_inner_rad - gap_between_disc_and_support_structure,
				kapton_disc_thick_half, -delta_phi / 2. / pi * 180.,
				+delta_phi / 2. / pi * 180.);

		TGeoRotation* kapton_rotation = new TGeoRotation("kapton_rotation", 0, 0,
				0);
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
		lmd_vol_kapton_disc->SetVisibility(false);*/

	}
}
