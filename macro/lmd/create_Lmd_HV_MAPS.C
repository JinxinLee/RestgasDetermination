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

#include<iostream>
#include<cmath>
#include<sstream>

//using namespace LmdDim;

// to check the position and dimensions
// one may enable a beam pipe dummy to be displayed
const bool show_beam_pipe_dummy = false;

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

	FairGeoMedium *FairMediumSilicon = Media->getMedium("silicon");

	Int_t nmed = geobuild->createMedium(FairMediumSilicon);

	TGeoManager* gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");

	TGeoVolume *top = new TGeoVolumeAssembly("lmd_HV_MAPS");

	gGeoMan->SetTopVolume(top);

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
	// ****************************** beam pipe dummy **********************************
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
	if (show_beam_pipe_dummy) {
		// create the shapes
		const double length_dipole_lmd = (LmdDim::pos_z - LmdDim::end_seg_bend);
		shape_beam_pipe_target_dipole = new TGeoTube(
				"shape_beam_pipe_target_dipole", 3.5, 3.6,
				LmdDim::end_seg_upstream / 2.);
		shape_beam_pipe_dipole = new TGeoTorus("shape_beam_pipe_dipole",
				LmdDim::r_bend, 3.5, 3.6, 0.,
				LmdDim::phi_bend / LmdDim::pi * 180.);
		shape_beam_pipe_dipole_lmd = new TGeoTube("shape_beam_pipe_dipole_lmd",
				3.5, 3.6, length_dipole_lmd / 2.);
		shape_ip_sphere = new TGeoSphere("shape_ip_sphere", 0., 3.5);

		// the corresponding transformations
		comb_trans_target_dipole = new TGeoCombiTrans(
				"comb_trans_target_dipole", 0., 0.,
				LmdDim::end_seg_upstream / 2., georot_no);
		comb_trans_target_dipole->RegisterYourself();
		comb_trans_dipole = new TGeoCombiTrans("comb_trans_dipole",
				LmdDim::r_bend, 0., LmdDim::end_seg_upstream, georot_270x_180y);
		comb_trans_dipole->RegisterYourself();
		georot_dipole_lmd = new TGeoRotation("georot_dipole_lmd", 90.,
				LmdDim::phi_bend / LmdDim::pi * 180., -90.);
		double _trans_x = LmdDim::r_bend * (1. - cos(LmdDim::phi_bend))
				+ length_dipole_lmd / 2. * LmdDim::phi_bend;
		comb_trans_dipole_lmd = new TGeoCombiTrans("comb_trans_dipole_lmd",
				_trans_x, 0., LmdDim::end_seg_bend + length_dipole_lmd / 2.,
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
	}

	// ****************************** cvd cooling support discs ************************

	// the cvd disc shape
	TGeoTube* shape_cvd_disc = new TGeoTube("shape_cvd_disc", 0.,
			LmdDim::cvd_disc_rad, LmdDim::cvd_disc_thick_half);
	// The inner beam pipe defines the inner acceptance region for the cvd cut_out
	TGeoTube* shape_cvd_cutout_inner = new TGeoTube("shape_cvd_cutout_inner",
			0., LmdDim::inner_rad, 1.);
	// finally cvd discs will be cut at the left and right down to 36 degree in phi
	// for that we subtract tube segments
	TGeoTubeSeg* shape_cvd_disc_cut_side = new TGeoTubeSeg(
			"shape_cvd_disc_cut_side", 0., LmdDim::outer_rad, 1.,
			+LmdDim::delta_phi / 2. / LmdDim::pi * 180.,
			-LmdDim::delta_phi / 2. / LmdDim::pi * 180.);
	// before: cvd disc was moved to the displaced position around the z axis
	// now: segments for the cut are moved off centered and cvd disc remains in the center
	TGeoRotation* cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
	TGeoTranslation* cvd_translation = new TGeoTranslation("cvd_translation",
			-LmdDim::cvd_disc_dist, 0, 0);
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
			"shape_maps_active_centered", LmdDim::maps_active_width,
			LmdDim::maps_active_height, LmdDim::maps_thickness);
	TGeoCombiTrans* combtrans_maps_active = new TGeoCombiTrans(
			"combtrans_maps_active",
			-LmdDim::maps_width + LmdDim::maps_passive_left * 2.
					+ LmdDim::maps_active_width,
			-LmdDim::maps_height + LmdDim::maps_passive_bottom * 2.
					+ LmdDim::maps_active_height, 0., georot_no);
	combtrans_maps_active->RegisterYourself();
	TGeoBBox *shape_maps_passive_left = new TGeoBBox("shape_maps_passive_left",
			LmdDim::maps_passive_left, LmdDim::maps_height,
			LmdDim::maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_left = new TGeoCombiTrans(
			"combtrans_maps_passive_left",
			-LmdDim::maps_width + LmdDim::maps_passive_left, 0., 0., georot_no);
	combtrans_maps_passive_left->RegisterYourself();
	TGeoBBox *shape_maps_passive_right = new TGeoBBox(
			"shape_maps_passive_right", LmdDim::maps_passive_right,
			LmdDim::maps_height, LmdDim::maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_right = new TGeoCombiTrans(
			"combtrans_maps_passive_right",
			LmdDim::maps_width - LmdDim::maps_passive_right, 0., 0., georot_no);
	combtrans_maps_passive_right->RegisterYourself();
	TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top",
			LmdDim::maps_width, LmdDim::maps_passive_top,
			LmdDim::maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_top = new TGeoCombiTrans(
			"combtrans_maps_passive_top", 0.,
			LmdDim::maps_height - LmdDim::maps_passive_top, 0., georot_no);
	combtrans_maps_passive_top->RegisterYourself();
	TGeoBBox *shape_maps_passive_bottom = new TGeoBBox(
			"shape_maps_passive_bottom", LmdDim::maps_width,
			LmdDim::maps_passive_bottom, LmdDim::maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_bottom = new TGeoCombiTrans(
			"combtrans_maps_passive_bottom", 0.,
			-LmdDim::maps_height + LmdDim::maps_passive_bottom, 0., georot_no);
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
		for (int i_cvd_disc = 0; i_cvd_disc < LmdDim::n_cvd_discs; i_cvd_disc++) {
			std::stringstream disc_name;
			disc_name << "LumPassive_cvd_disc_" << plane_name.str() << "_disc_"
					<< (i_cvd_disc + 1);
			std::stringstream rot_name;
			rot_name << "rot_" << plane_name.str() << "_disc_" << (i_cvd_disc
					+ 1);
			TGeoVolume* tmpvol = new TGeoVolume(disc_name.str().c_str(),
					shape_cvd_support, gGeoMan->GetMedium("silicon"));//HYPdiamond"));
			tmpvol->SetLineColor(16);
			double _x(0), _y(0), _z(0), _rotx(0), _roty(0), _rotz(0);
			LmdDim::Get_pos_mod_local(i_plane, i_cvd_disc, _x, _y, _z, _rotx,
					_roty, _rotz, misalign);
			TGeoRotation* rot = new TGeoRotation(rot_name.str().c_str());
			rot->RotateX(_rotx / LmdDim::pi * 180.);
			rot->RotateY(_roty / LmdDim::pi * 180.);
			rot->RotateZ(_rotz / LmdDim::pi * 180.);
			TGeoTranslation* trt = new TGeoTranslation(_x, _y, _z);//LmdDim::pos_z + LmdDim::plane_pos_z[i_plane]  - LmdDim::pos_rot_z);
			TGeoCombiTrans* trctmp = new TGeoCombiTrans(*trt, *rot);
			trctmp->SetName((rot_name.str() + "_comb").c_str());
			trctmp->RegisterYourself();
			delete rot;
			delete trt;
			SubunitVol->AddNode(tmpvol, 0, trctmp);
			//break;
			for (int iside = 0; iside < 2; iside++) {
				// modules are placed on both sides
				// aligned to the cut edges
				int isensor = -1;
				for (int maps_col = 0; maps_col < LmdDim::maps_n_col; maps_col++) {
					for (int maps_row = 0; maps_row < LmdDim::maps_n_row; maps_row++) {
						if (LmdDim::enabled[maps_row][maps_col]) isensor++; else continue;
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
						LmdDim::Get_pos_sens_local(i_plane, i_cvd_disc, iside,
								isensor, _trans_x, _trans_y, _trans_z, _rot_x,
								_rot_y, _rot_z, misalign);

						TGeoRotation* _rot = new TGeoRotation(
								("rot_" + _module_name.str()).c_str());
						_rot->RotateX(_rot_x / LmdDim::pi * 180.);
						_rot->RotateY(_rot_y / LmdDim::pi * 180.);
						_rot->RotateZ(_rot_z / LmdDim::pi * 180.);
						TGeoTranslation* _trans = new TGeoTranslation(_trans_x,
								_trans_y, _trans_z);
						TGeoCombiTrans* _combtrans = new TGeoCombiTrans(
								*_trans, *_rot);
						_combtrans->SetName(
								("combtrans_" + _module_name.str()).c_str());
						_combtrans->RegisterYourself();
						delete _rot;
						delete _trans;
						SubunitVol->AddNode(_vol_active, 0, _combtrans);
						SubunitVol->AddNode(_vol_passive, 0, _combtrans);
						// some checks
						int sensor_id_check = LmdDim::Get_sensor_id(i_plane, i_cvd_disc, iside, isensor);
						if ( sensor_id_check != sensor_id){
							cout << "Error: Sensor id " << sensor_id_check << " does not correspond to the constructed order " << sensor_id << " !"  << endl;
						}
						int _iplane, _imodule, _iside, _isensor;
						LmdDim::Get_sensor_by_id(sensor_id, _iplane, _imodule, _iside, _isensor);
						if ( _iplane != i_plane || _imodule != i_cvd_disc || _iside != iside || _isensor != isensor){
							cout << "Error: wrong Sensor returned by id" << endl;
						}
						sensor_id++;
					}
				}
			}
		}
	}

	double x, y, z, rotx, roty, rotz;
	LmdDim::Get_pos_lmd_global(x, y, z, rotx, roty, rotz);
	TGeoRotation* global_rot = new TGeoRotation("global_rot");
	global_rot->RotateX(rotx/LmdDim::pi*180.);
	global_rot->RotateY(roty/LmdDim::pi*180.);
	global_rot->RotateZ(rotz/LmdDim::pi*180.);
	TGeoCombiTrans* global_transrot = new TGeoCombiTrans(x, y, z, global_rot);
	global_transrot->SetName("global_transrot");
	global_transrot->RegisterYourself();
	if (vol_beam_pipe)
		top->AddNode(vol_beam_pipe, 0, comb_trans_no);
	top->AddNode(SubunitVol, 0, global_transrot);

	gGeoMan->CloseGeometry();
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
