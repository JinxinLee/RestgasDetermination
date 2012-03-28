// compile with
// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/buildPanda/lib -lGeoBase create_HV_MAPS.C -o /tmp/create_geometry
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
#include<TMath.h>

#include<iostream>
#include<cmath>
#include<sstream>

void create_HV_MAPS() {
	// Lumi Si-Set Parameter
	//-----------------------------

	//vars for trap-builder
	/*
	const Int_t kNumTraps = 12;
	const Double_t kTrapHalfAngle = TMath::Pi() / (Double_t) kNumTraps; // (rad)

	const Double_t kShortSide = 1.673 / 2.;//kTrapInnerDist * tan(kTrapHalfAngle);
	const Double_t kLongSide = 5.921 / 2.;//kTrapOuterDist * tan(kTrapHalfAngle);

	const Double_t kTrapInnerDist = 3.2;//kShortSide / tan(kTrapHalfAngle);;//2.6;   // (cm) //Dimension of Trapezoid
	const Double_t kTrapOuterDist = kTrapInnerDist + 7.927;//kLongSide / tan(kTrapHalfAngle);;//11.;   // (cm)

	const Double_t kTrapHigh = 7.927;//kTrapOuterDist - kTrapInnerDist;
*/
	/*    DIGI PAR
	 top_pitch:Double_t 0.005
	 bot_pitch:Double_t 0.00625
	 orient:Double_t 1.308995833				75deg=90deg-kTrapHalfAngle	orientation
	 skew:Double_t 0.523598333				30deg=2*kTrapHalfAngle		stereo angle
	 top_anchor_x:Double_t -2.8125  #-4.142135624	-kLongSide+DeadZone
	 top_anchor_y:Double_t 3.96 #3.5				kTrapHigh/2
	 bot_anchor_x:Double_t -2.8125  #-4.142135624
	 bot_anchor_y:Double_t 3.96 #3.5
	 */

	//(pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);

	Double_t kPlaneZPosition[4];
	kPlaneZPosition[0] = 0.0; // (cm) //Dist to IP before rotating
	kPlaneZPosition[1] = 20.0; // (cm) //
	kPlaneZPosition[2] = 30.0; // (cm) //
	kPlaneZPosition[3] = 40.0; // (cm) //

	//const Double_t kHalfFoilThickness = 0.0075; // Thickness of sensitive foil (cm)
	// const Double_t  kHalfFoilThickness  = 0.0035; // Thickness of sensitive foil (cm) TEST
	double end_seg_upstream = 360.1; // where bending starts with
	double r_bend = 5750.; // the bending radius
	double phi_bend = 40.068e-3; // and the angle of the circle path

	const Double_t kRot = phi_bend/3.141*180.;//=2.295727;//2.326; //(deg) //Rotate to dipol
	// the point where both tangents of the straight beam pipe tubes meet is
	const Double_t kRotUmZ = end_seg_upstream + tan(phi_bend/2.)*r_bend;//476.03; //(cm) //z-point to rotate
	//const Double_t  kRot = 0; //(deg) //Rotate to dipol TEST
	//  const Double_t  kRotUmZ = 0; //(cm) //z-point to rotate TEST

	const Double_t kTransZ = 1130.; //(cm) //move at z-position

	//double end_seg_bend = end_seg_upstream+sin(phi_bend)*r_bend;
	//(pos_z - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend);
	const Double_t kTransX = (kTransZ - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend); // 25 (cm) //move at x-position
	std::cout << " translation in x by " << kTransX << std::endl;

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

	// ****************************** cvd cooling support discs ************************

	// cvd_diamond is cut out of 80 mm discs of 200 micron thickness
	const double inner_rad = 3.6; //4.5;
	const double outer_rad = 9.;
	const int n_cvd_discs = 6;
	const double cvd_disc_rad = 4.;
	const double cvd_disc_thick_half = 0.01;
	// even and odd discs in a plane will be shifted in z in order to prevent
	// mechanical damage during assembly
	const double cvd_disc_even_odd_offset = 0.25;
	// angle from the division of a circle into n_cvd_discs
	const double delta_phi = TMath::TwoPi() / ((double) (n_cvd_discs));
	// a polygon of n_cvd_discs sides fitting a radius of inner_rad
	// has a side length pol_side_lg of
	const double pol_side_lg_half = inner_rad * sin(delta_phi / 2.);
	// the minimum distance to the center of the polygone is given by
	const double pol_side_dist_min = inner_rad * cos(delta_phi / 2.);
	// the cvd disc has to be placed such that the disc crosses
	// the inner ring at an angle of 0 and delta_phi
	// this defines the distance to the center according to pythagoras
	const double cvd_disc_dist = pol_side_dist_min + sqrt(
			cvd_disc_rad * cvd_disc_rad - pol_side_lg_half * pol_side_lg_half);
	//std::cout << cvd_disc_dist << std::endl;

	// the cvd disc shape
	TGeoTube* shape_cvd_disc = new TGeoTube("shape_cvd_disc", 0., cvd_disc_rad,
			cvd_disc_thick_half);
	// The inner beam pipe defines the inner acceptance region for the cvd cut_out
	TGeoTube* shape_cvd_cutout_inner = new TGeoTube("shape_cvd_cutout_inner",
			0., inner_rad, 1.);
	// finally cvd discs will be cut at the left and right down to 60 degree in phi
	// for that we subtract tube segments
	TGeoTubeSeg* shape_cvd_disc_cut_left =
		  new TGeoTubeSeg("shape_cvd_disc_cut_left",0., outer_rad, 1., 0.,-delta_phi/2./TMath::Pi()*180.+90.);
	TGeoTubeSeg* shape_cvd_disc_cut_right =
			  new TGeoTubeSeg("shape_cvd_disc_cut_right",0., outer_rad, 1.,+delta_phi/2./TMath::Pi()*180.+90., +180.);
	// move the cvd disc to the upper position in the circular arrangement
	TGeoRotation* cvd_rotation = new TGeoRotation("cvd_rotation", 0, 0, 0);
	TGeoTranslation* cvd_translation = new TGeoTranslation("cvd_translation", 0, cvd_disc_dist, 0);
	TGeoCombiTrans* cvd_combtrans = new TGeoCombiTrans(*cvd_translation, *cvd_rotation);
	cvd_combtrans->SetName("cvd_combtrans");
	cvd_combtrans->RegisterYourself();
	// move boxes to cut at the angles
	//TGeoRotation* cut_rotation_plus  = new TGeoRotation("cut_rotation_plus ", 0., 0., +delta_phi/2./TMath::Pi()*180.);
	//TGeoRotation* cut_rotation_minus = new TGeoRotation("cut_rotation_minus", 0., 0., -delta_phi/2./TMath::Pi()*180.);
	//TGeoTranslation* cut_translation_plus  = new TGeoTranslation("cut_translation_plus" , +5., 0, 0);
	//TGeoTranslation* cut_translation_minus = new TGeoTranslation("cut_translation_minus", -5., 0, 0);
	//TGeoCombiTrans* cut_combtrans_plus = new TGeoCombiTrans(*cut_translation_plus, *cut_rotation_plus);
	//cut_combtrans_plus->SetName("cut_combtrans_plus");
	//cut_combtrans_plus->RegisterYourself();
	//TGeoCombiTrans* cut_combtrans_minus = new TGeoCombiTrans(*cut_translation_minus, *cut_rotation_minus);
	//cut_combtrans_minus->SetName("cut_combtrans_minus");
	//cut_combtrans_minus->RegisterYourself();
	// construct the cut cvd diamond disc
	TGeoCompositeShape *shape_cvd_support = new TGeoCompositeShape(
			"shape_cvd_support", "(shape_cvd_disc:cvd_combtrans-shape_cvd_cutout_inner-shape_cvd_disc_cut_left-shape_cvd_disc_cut_right)");

	// *********************************** HV-MAPS *************************************

	//  x = origin
	//
	//  |---------|----------|----------| top
	//  ||------|-||-------|-||-------|-|
	//  ||      | ||       | ||       | |
	//  ||      | ||   x   | ||       | |
    //  ||	    | || active| ||       | |
	//  ||------|-||-------|-||-------|-|
	//  |         | passive  |          |
	//  |---------|----------|----------| bottom
	//             left  right
	//
	//                A   y; maps_n_row; height
	//                |
	//                |
	//                --> x; maps_n_col; width
	//

	// NOTE: MOST of the following VARIABLES are HALF of it
	// due to geometry construction in GEANT
	const double maps_thickness      = 0.005  / 2.;
	const double maps_active_width   = 2.0    / 2.;
	const double maps_active_height  = 1.9    / 2.;
	const double maps_passive_top    = 0.01 / 2.;//*12000.;
	const double maps_passive_bottom = 0.1    / 2.;
	const double maps_passive_left   = 0.01 / 2.;//*6000.;
	const double maps_passive_right  = 0.01 / 2.;//*3000.;

	const double maps_width  = maps_passive_left + maps_active_width  + maps_passive_right ;
	const double maps_height = maps_passive_top  + maps_active_height + maps_passive_bottom;

	// even when several maps are placed on one die
	// those will be placed in the simulation next to
	// each other as separate detectors
	const int maps_n_col = 3;
	const int maps_n_row = 1;

	const double maps_die_widht  = maps_width  * maps_n_col;
	const double maps_die_height = maps_height * maps_n_row;

	TGeoRotation* no_rotation = new TGeoRotation("no_rotation", 0, 0, 0);

	// create basic shapes and their positions
	TGeoBBox *shape_maps_active_centered = new TGeoBBox("shape_maps_active_centered", maps_active_width, maps_active_height, maps_thickness);
	TGeoCombiTrans* combtrans_maps_active =
			new TGeoCombiTrans("combtrans_maps_active",
					- maps_width  + maps_passive_left*2. + maps_active_width,
					- maps_height + maps_passive_bottom*2. + maps_active_height, 0., no_rotation);
	combtrans_maps_active->RegisterYourself();
	TGeoBBox *shape_maps_passive_left  = new TGeoBBox("shape_maps_passive_left", maps_passive_left, maps_height, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_left =
			new TGeoCombiTrans("combtrans_maps_passive_left", - maps_width + maps_passive_left, 0., 0., no_rotation);
	combtrans_maps_passive_left->RegisterYourself();
	TGeoBBox *shape_maps_passive_right = new TGeoBBox("shape_maps_passive_right", maps_passive_right, maps_height, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_right =
			new TGeoCombiTrans("combtrans_maps_passive_right", maps_width - maps_passive_right, 0., 0., no_rotation);
	combtrans_maps_passive_right->RegisterYourself();
	TGeoBBox *shape_maps_passive_top = new TGeoBBox("shape_maps_passive_top", maps_width, maps_passive_top, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_top =
			new TGeoCombiTrans("combtrans_maps_passive_top", 0., maps_height - maps_passive_top, 0., no_rotation);
	combtrans_maps_passive_top->RegisterYourself();
	TGeoBBox *shape_maps_passive_bottom = new TGeoBBox("shape_maps_passive_bottom", maps_width, maps_passive_bottom, maps_thickness);
	TGeoCombiTrans* combtrans_maps_passive_bottom =
			new TGeoCombiTrans("combtrans_maps_passive_bottom", 0., - maps_height + maps_passive_bottom, 0., no_rotation);
	combtrans_maps_passive_bottom->RegisterYourself();

	TGeoCompositeShape *shape_maps_passive = new TGeoCompositeShape(
			"shape_maps_passive", "(shape_maps_passive_top:combtrans_maps_passive_top+shape_maps_passive_right:combtrans_maps_passive_right+shape_maps_passive_bottom:combtrans_maps_passive_bottom+shape_maps_passive_left:combtrans_maps_passive_left)");

	TGeoCompositeShape *shape_maps_active = new TGeoCompositeShape(
			"shape_maps_active", "(shape_maps_active_centered:combtrans_maps_active-shape_maps_passive)");


	// **************************************************************



	//TGeoVolumeAssembly* SubunitVol_active = new TGeoVolumeAssembly("Lumi_HV-MAPS_active");
	//TGeoVolumeAssembly* SubunitVol_passive = new TGeoVolumeAssembly("Lumi_HV-MAPS_passive");
	TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Lumi_HV-MAPS");

	for (int i_plane = 0; i_plane < 4; i_plane++) {
		std::stringstream plane_name;
		plane_name << "plane_" << (i_plane + 1);
		for (int i_cvd_disc = 0; i_cvd_disc < n_cvd_discs; i_cvd_disc++) {
			double angle = /*delta_phi / 2. +*/ delta_phi * i_cvd_disc;
			double offset_z = cvd_disc_even_odd_offset;
			if ((i_cvd_disc%2)==0) offset_z = -offset_z;
			std::stringstream disc_name;
			disc_name << "LumPassive_cvd_disc_" << plane_name.str() << "_disc_"
					<< (i_cvd_disc + 1);
			std::stringstream rot_name;
			rot_name << "rot_" << plane_name.str() << "_disc_" << (i_cvd_disc
					+ 1);
			Double_t dreh = angle/TMath::Pi()*180;// -i_cvd_disc * 360. / n_cvd_discs;
			TGeoVolume* tmpvol = new TGeoVolume(disc_name.str().c_str(),
					shape_cvd_support, gGeoMan->GetMedium("silicon"));//HYPdiamond"));
			tmpvol->SetLineColor(16);
			TGeoRotation* rot = new TGeoRotation(rot_name.str().c_str(), dreh,
					0, 0);
			TGeoTranslation* trt = new TGeoTranslation(
					0, 0,
					kTransZ + kPlaneZPosition[i_plane] + offset_z - kRotUmZ);
			TGeoCombiTrans* trctmp = new TGeoCombiTrans(*trt, *rot);
			trctmp->SetName((rot_name.str() + "_comb").c_str());
			trctmp->RegisterYourself();
			delete rot;
			delete trt;
			//SubunitVol_passive->AddNode(tmpvol, 0, trctmp);
			SubunitVol->AddNode(tmpvol, 0, trctmp);
			//break;
			for (int iside = 0; iside < 2; iside++){ // modules are placed on both sides
				// the modules at the back are aligned at the cut edges
				// of the cvd discs
				// the modules at the front are centered
				for (int idie = 0; idie < 2; idie++){
					for (int maps_col = 0; maps_col < maps_n_col; maps_col++){
						for (int maps_row = 0; maps_row < maps_n_row; maps_row++){
							// place active and passive volume
							std::stringstream _module_name;
							_module_name << "_plane_" << i_plane << "_disc_"<< i_cvd_disc;
							_module_name << "_side_" << iside << "_die_" << idie;
							_module_name << "_col_" << maps_col << "_row_" << maps_row;
							//"LumActiveRect" is the keyword for digitization of hits
							TGeoVolume* _vol_active = new TGeoVolume(("LumActivePixelRect"+_module_name.str()).c_str(),
									shape_maps_active, gGeoMan->GetMedium("silicon"));
							_vol_active->SetLineColor(36);
							TGeoVolume* _vol_passive = new TGeoVolume(("LumPassiveRect"+_module_name.str()).c_str(),
									shape_maps_passive, gGeoMan->GetMedium("silicon"));
							_vol_passive->SetLineColor(30);
							double _rot_z(0.);
							double _trans_z(0.);
							double _trans_y(0.);
							double _trans_x(0.);
							if (iside == 0){
								_trans_z = - cvd_disc_thick_half - maps_thickness;
								_trans_y = inner_rad + maps_col * maps_width*2. + maps_width;
								if (idie == 0){
									_rot_z = -90.;
									_trans_x = maps_height + maps_row * maps_height*2.;
									// rotate now to the right cvd disc angle
									double temp_trans_x = _trans_x;
									_trans_x = _trans_x * cos(angle) - _trans_y * sin(angle);
									_trans_y = temp_trans_x * sin(angle) + _trans_y * cos(angle);
								} else {
									_rot_z = +90.;
									_trans_x = - maps_height - maps_row * maps_height*2.;
									// rotate now to the right edge
									double temp_trans_x = _trans_x;
									_trans_x = _trans_x * cos(angle) - _trans_y * sin(angle);
									_trans_y = temp_trans_x * sin(angle) + _trans_y * cos(angle);
								}
							} else {
								_trans_z = + cvd_disc_thick_half + maps_thickness;
								_trans_y = inner_rad + maps_col * maps_width*2. + maps_width;
								if (idie == 0){ // align to the cutted edges
									_rot_z = -90. -delta_phi/2./TMath::Pi()*180.;
									// place it left of the middle line
									_trans_x = - maps_height - maps_row * maps_height*2.;
									// rotate now to the right edge
									double temp_trans_x = _trans_x;
									_trans_x = _trans_x * cos(-delta_phi/2.+angle) - _trans_y * sin(-delta_phi/2.+angle);
									_trans_y = temp_trans_x * sin(-delta_phi/2.+angle) + _trans_y * cos(-delta_phi/2.+angle);
								} else {
									_rot_z = +90. +delta_phi/2./TMath::Pi()*180.;
									// place it right of the middle line
									_trans_x = maps_height + maps_row * maps_height*2.;
									// rotate now to the left edge
									double temp_trans_x = _trans_x;
									_trans_x = _trans_x * cos(delta_phi/2.+angle) - _trans_y * sin(delta_phi/2.+angle);
									_trans_y = temp_trans_x * sin(delta_phi/2.+angle) + _trans_y * cos(delta_phi/2.+angle);
								}
							}

							TGeoRotation* _rot = new TGeoRotation(("rot_"+_module_name.str()).c_str(), _rot_z + dreh,
									0, 0);
							TGeoTranslation* _trans = new TGeoTranslation(
									_trans_x, _trans_y,
									kTransZ + kPlaneZPosition[i_plane] + offset_z - kRotUmZ + _trans_z);
							TGeoCombiTrans* _combtrans = new TGeoCombiTrans(*_trans, *_rot);
							_combtrans->SetName(("combtrans_"+_module_name.str()).c_str());
							_combtrans->RegisterYourself();
							delete _rot;
							delete _trans;
							//SubunitVol_active->AddNode(_vol_active, 0, _combtrans);
							//SubunitVol_passive->AddNode(_vol_passive, 0, _combtrans);
							SubunitVol->AddNode(_vol_active, 0, _combtrans);
							SubunitVol->AddNode(_vol_passive, 0, _combtrans);
						}
					}
				}
			}
			//break;
		}
	}

/*
	TGeoVolume* tmpvol = new TGeoVolume("tmpvol", shape_maps_passive,
			gGeoMan->GetMedium("silicon"));
	tmpvol->SetLineColor(32);
	TGeoRotation* rot = new TGeoRotation("arotation", 0, 0, 0);
	TGeoTranslation* trt = new TGeoTranslation(0, 0, kTransZ - kRotUmZ);
	TGeoCombiTrans* trctmp = new TGeoCombiTrans(*trt, *rot);
	trctmp->SetName("comb");
	SubunitVol->AddNode(tmpvol, 0, trctmp);
	TGeoVolume* tmpvol2 = new TGeoVolume("tmpvol2", shape_maps_active,
			gGeoMan->GetMedium("silicon"));
	tmpvol2->SetLineColor(36);
	SubunitVol->AddNode(tmpvol2, 0, trctmp);*/

	TGeoRotation* rotf = new TGeoRotation("final", 90, kRot, 0);
	//TGeoRotation* rotf = new TGeoRotation("final", 0, 0, 0);
	TGeoTranslation* trtf = new TGeoTranslation(0, 0, kRotUmZ);
	TGeoCombiTrans *trcf = new TGeoCombiTrans(*trtf, *rotf);
	//top->AddNode(SubunitVol_active, 0, trcf);
	//top->AddNode(SubunitVol_passive, 0, trcf);
	top->AddNode(SubunitVol, 0, trcf);

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
