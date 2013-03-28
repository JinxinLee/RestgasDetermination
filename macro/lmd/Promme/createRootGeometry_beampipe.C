// =============================================================================
//
// macro to create the PANDA beam pipe geometry
// Author: Paul Buehler, paul.buehler@oeaw.ac.at
//
// Revisions:
// v1: October, 2011
// v1.1: January, 2012, jasinski
// 	- playing around to include the lumi
//
// This files is based on the CAD drawing which is available at the PANDA EDMS
// system (https://edms.cern.ch/project/FAIR-000000770) and which was created by
// Alexander Gruber (a9302331@unet.univie.ac.at).
//
// The beam pipe is equiped with sensors along the beam pipe in order to specify the needs
// to the beam pipe it self
// 
// to compile and run this macro do:
// g++ $(root-config --cflags --glibs) -lGeom -I${PANDAROOT}/geobase -L${PANDAROOT}/buildPanda/lib -lGeoBase createRootGeometry_beampipe.C -o /tmp/create_geometry
// /tmp/create_geometry
//
// =============================================================================

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
#include<TSystem.h>
#include<TFile.h>
#include<TGeoPcon.h>
#include<TGeoManager.h>
#include<TGeoTorus.h>
#include<TGeoCone.h>
#include<vector>
#include<sstream>

#include<iostream>
#include<cmath>

void createRootGeometry_beampipe() {

	// basic constants concerning the path of the beam pipe
	// downstream of the target
	const double bend_begin = 360.; // where bending has to start
	const double bend_radius = 5750.; // bending radius
	const double bend_angle = 40.068e-3; // bending angle
	const double bend_end = bend_begin+sin(bend_angle)*bend_radius; // z position of the bend end

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	TString vmcWorkdir = gSystem->Getenv("VMCWORKDIR");

	// if you do not want sensors in your beam pipe,
	// so a simple beam pipe only, set it to false
	bool create_sensors = false;
	// key z positions are stored in the following vector
	std::vector< double > sensor_positions;

	// materials and media
	FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	geoFace->setMediaFile(vmcWorkdir + "/geometry/media_pnd.geo");
	geoFace->readMedia();
	geoFace->print();
	FairGeoMedia* geoMedia = geoFace->getMedia();
	FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

	std::string str_ti = "Aluminum"; // "titanium" is in Panda software not yet implemented
	FairGeoMedium* FairMediumAir = geoMedia->getMedium("air");
	FairGeoMedium* FairMediumSteel = geoMedia->getMedium("steel");
	FairGeoMedium* FairMediumTi = geoMedia->getMedium(str_ti.c_str()); //titanium"); not found in media_pnd.geo !
	FairGeoMedium* FairMediumKapton = geoMedia->getMedium("mylar"); // mylar properties are nearly the same as for the non existing Kapton

	if (!FairMediumAir || !FairMediumSteel || !FairMediumTi || !FairMediumKapton) {
		std::cout << " warning: not all media found " << std::endl;
	}

	geoBuild->createMedium(FairMediumAir);
	geoBuild->createMedium(FairMediumSteel);
	geoBuild->createMedium(FairMediumTi);
	geoBuild->createMedium(FairMediumKapton);
	std::cout << " done " << std::endl;
	// open output file
	TFile* fi;
	TString fGeoFile;
	if (create_sensors){
		fGeoFile = "beampipe_201210_active.root";

	} else {
		fGeoFile = "beampipe_201210.root";
	}
	fi = new TFile(fGeoFile, "RECREATE");


	// define cave
	/*
	TGeoVolume* cave;
	TGeoBBox* lTop = new TGeoBBox(200, 200, 300);
	cave = new TGeoVolume("BeamPipe", lTop, gGeoManager->GetMedium("air"));
	gGeoManager->SetTopVolume(cave);
    */
	TGeoVolume *cave = new TGeoVolumeAssembly("pipeassembly");//gGeoManager->GetTopVolume();
	gGeoManager->SetTopVolume(cave);
	//cave->AddNode(beamPipe, 1);

	// ---------------------------------------------------------------------------
	// the full geometry is built-up by adding volumes starting from the most
	// upstream position, variable currentz is continuously updated and always
	// contains the z-coordinate of the most down-stream point of the geometry 
	Double_t currentz = 0.0;

	// z-position of very first point
	Double_t z0 = -459.9328; // with this z0 the target is at z = 0.

	// Definition of some constants, dummy shapes, transformations
	Double_t const rad = 3.1415926 / 180.;
	Double_t const delta = 1.E-6;

	TGeoRotation* r1 = new TGeoRotation("r1", 0., 0., 0.); // no rotation
	TGeoRotation* r2 = new TGeoRotation("r2", 0., 90., 180.); //  90 deg about x-axis and 180 deg about y axis
	TGeoRotation* r3 = new TGeoRotation("r3", 0., -90., 0); // -90 deg about x-axis
	TGeoRotation* r4 = new TGeoRotation("r4", 90., 0., 0); //  90 deg about z-axis

	TGeoCombiTrans* tr0 = new TGeoCombiTrans("tra0", 0., 0., z0, r1);
	tr0->RegisterYourself();

	// ---------------------------------------------------------------------------
	// define valves and pumps which are used at different places

	TGeoCombiTrans* trv1 = new TGeoCombiTrans("trv1", 0., 0., 1.075, r1);
	trv1->RegisterYourself();
	TGeoCombiTrans* trv2 = new TGeoCombiTrans("trv2", 0., 0., 3.53875, r1);
	trv2->RegisterYourself();
	TGeoCombiTrans* trv3 = new TGeoCombiTrans("trv3", 0., 0., 6.0075, r1);
	trv3->RegisterYourself();

	// VAT -Gate Valve CF63
	// width in z is 7.
	Double_t ov1[3] = { 0., 5.7, 0. };
	TGeoBBox* VATvalve63a = new TGeoBBox("VATvalve63a", 5.6, 12.45, 1.35, ov1);
	TGeoTube* VATvalve63b = new TGeoTube("VATvalve63b", 3.5, 6.5, 1.075);
	TGeoTube* tv1 = new TGeoTube("tv1", 0., 3.5, 1.35 + delta);

	TGeoCompositeShape *SVATvalve630 = new TGeoCompositeShape("VATvalve630",
			"VATvalve63a-tv1");
	TGeoCompositeShape *SVATvalve63 = new TGeoCompositeShape("VATvalve63",
			"VATvalve63b:trv1+VATvalve630:trv2+VATvalve63b:trv3");

	// VAT -Gate Valve CF100
	// width in z is 7.
	Double_t ov2[3] = { 0., 7.3, 0. };
	TGeoBBox* VATvalve100a = new TGeoBBox("VATvalve100a", 7.2, 16.0, 1.35, ov2);
	TGeoTube* VATvalve100b = new TGeoTube("VATvalve100b", 5.0, 8.25, 1.075);
	TGeoTube* tv2 = new TGeoTube("tv2", 0., 5.0, 1.35 + delta);

	TGeoCompositeShape *SVATvalve1000 = new TGeoCompositeShape("VATvalve1000",
			"VATvalve100a-tv2");
	TGeoCompositeShape *SVATvalve100 = new TGeoCompositeShape("VATvalve100",
			"VATvalve100b:trv1+VATvalve1000:trv2+VATvalve100b:trv3");

	// VAT -Gate Valve CF160
	// width in z is 7.
	Double_t ov3[3] = { 0., 9.9, 0. };
	TGeoBBox* VATvalve160a =
			new TGeoBBox("VATvalve160a", 9.1, 21.75, 1.35, ov3);
	TGeoTube* VATvalve160b = new TGeoTube("VATvalve160b", 7.5, 11.25, 1.075);
	TGeoTube* tv3 = new TGeoTube("tv3", 0., 7.5, 1.35 + delta);

	TGeoCompositeShape *SVATvalve1600 = new TGeoCompositeShape("VATvalve1600",
			"VATvalve160a-tv3");
	TGeoCompositeShape *SVATvalve160 = new TGeoCompositeShape("VATvalve160",
			"VATvalve160b:trv1+VATvalve1600:trv2+VATvalve160b:trv3");

	// Turbovac_SL700_CF160
	Double_t psv1[27] = { 0., 360., 8, .0, 6.0, 10.85, 7.6, 6.0, 10.85, 7.6,
			6.0, 10.15, 16.6, 6.0, 10.15, 16.6, 6.0, 9.0, 24.0, 6.0, 9.0, 24.0,
			0., 9.0, 24.2, 0., 9.0 };
	TGeoPcon* TVP700 = new TGeoPcon("TVP700", 0., 360., 8);
	TVP700->SetDimensions(psv1);

	// Turbovac_1000C_CF160
	Double_t psv2[39] = { 0., 360., 12, 0.0, 7.5, 11.25, 2.45, 7.5, 11.25,
			2.45, 7.5, 7.965, 3.45, 7.5, 7.965, 6.45, 7.5, 12.9, 8.45, 0.,
			12.9, 8.45, 0., 12.05, 24.00, 0., 12.05, 24.00, 0., 12.9, 26.00,
			0., 12.9, 26.00, 0., 7.5, 36.85, 0., 7.5 };
	TGeoPcon* TVP1000 = new TGeoPcon("TVP1000", 0., 360., 12);
	TVP1000->SetDimensions(psv2);

	// Ion getter pump 1000l/s
	// is approximated as a cylinder
	Double_t psv3[21] = { 0., 360., 6, 0., 7.5, 10.125, 2.0, 7.5, 10.125, 2.0,
			7.5, 7.7, 10.0, 7.5, 7.7, 10.0, 0., 20.0, 60.7, 0., 20.0 };
	TGeoPcon* IGP1000 = new TGeoPcon("IGP1000", 0., 360., 6);
	IGP1000->SetDimensions(psv3);

	// ---------------------------------------------------------------------------
	// Definition of the volumes from up- to down-stream
	// The naming of the shapes and volumes follows the naming in the CATIA
	// drawing
	//
	// ---------------------------------------------------------------------------
	// a - GV to HESR (GV to HESR.1)

	// VAT -Gate Valve CF100
	TGeoVolume *Vgvhesr = new TGeoVolume("gvhesr", SVATvalve100,
			gGeoManager->GetMedium("steel"));
	Vgvhesr->SetLineColor(30);

	// last z-position: 7.
	currentz += 7.;
	fprintf(stderr, "currentz a: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// b - pipe upstream (pipe upstream.1)

	Double_t psb1[27] = { 0., 360., 8, 0., 4.35, 7.58, 2.0, 4.35, 7.58, 2.0,
			4.35, 4.45, 20.0, 4.35, 4.45, 31.3328, 7.50, 7.60, 156.0328, 7.50,
			7.60, 156.0328, 7.50, 10.125, 158.2328, 7.50, 10.125 };
	TGeoPcon* pipeup = new TGeoPcon("pipeup", 0., 360., 8);
	pipeup->SetDimensions(psb1);

	TGeoCombiTrans* trb1 =
			new TGeoCombiTrans("trb1", 0., 0., z0 + currentz, r1);
	trb1->RegisterYourself();
	//sensor_positions.push_back(z0 + currentz);

	TGeoVolume *Vpipeup = new TGeoVolume("pipeup", pipeup,
			gGeoManager->GetMedium("steel"));
	Vpipeup->SetLineColor(31);

	// last z-position: 165.2328
	currentz += 158.2328;
	fprintf(stderr, "currentz b: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// c - KreuzTMpump (KreuzTMpump.1)

	// horizontal/vertical bar of the cross
	Double_t psc1[21] = { 0., 360., 6, -16.7, 7.50, 10.125, -14.5, 7.50,
			10.125, -14.5, 7.50, 7.70, 14.5, 7.50, 7.70, 14.5, 7.50, 10.125,
			16.7, 7.50, 10.125 };
	TGeoPcon* ktmpump0 = new TGeoPcon("ktmpump0", 0., 360., 6);
	ktmpump0->SetDimensions(psc1);
	TGeoTube* tc1 = new TGeoTube("tc1", 0., 7.7, 7.7);

	TGeoCombiTrans* trc1 = new TGeoCombiTrans("trc1", 0., 0., 0, r2);
	trc1->RegisterYourself();
	TGeoCombiTrans* trc2 = new TGeoCombiTrans("trc2", 0., 0.,
			z0 + currentz + 16.7, r1);
	trc2->RegisterYourself();

	TGeoCompositeShape *csc1 = new TGeoCompositeShape("csc1",
			"(ktmpump0-tc1:trc1)");
	TGeoCompositeShape *Sktmpump = new TGeoCompositeShape("ktmpump",
			"csc1+(csc1:trc1)");

	TGeoVolume *Vktmpump = new TGeoVolume("ktmpump", Sktmpump,
			gGeoManager->GetMedium("steel"));
	Vktmpump->SetLineColor(32);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 181.932800
	currentz += 16.7;
	fprintf(stderr, "currentz c1: %f\n", currentz);

	// ...........................................................................

	// add Turbovac_1000C_CF160 and GV_CF160
	TGeoCombiTrans* trc3 = new TGeoCombiTrans("trc3", 0., 0., 7.0, r1);
	trc3->RegisterYourself();
	TGeoCombiTrans* trc4 = new TGeoCombiTrans("trc4", 0., -16.7, currentz, r2);
	trc4->RegisterYourself();
	TGeoCombiTrans* trc5 = new TGeoCombiTrans("trc5", 0., 16.7, currentz, r3);
	trc5->RegisterYourself();

	TGeoCompositeShape *STVPwValve = new TGeoCompositeShape("TVPwValve",
			"(VATvalve160+TVP1000:trc3)");
	TGeoCompositeShape *STpumps = new TGeoCompositeShape("Tpumps",
			"TVPwValve:trc4+TVPwValve:trc5");

	TGeoVolume *VTpumps = new TGeoVolume("Tpump", STpumps,
			gGeoManager->GetMedium("steel"));
	VTpumps->SetLineColor(33);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 198.6328
	currentz += 16.7;
	fprintf(stderr, "currentz c2: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// d - pipeTSup.1, special_flange.1, coneExtensionUpstreamSteel.1

	Double_t psd1[27] = { 0., 360., 8, 0.0, 7.5, 10.125, 2.20, 7.5, 10.125,
			2.20, 7.5, 7.70, 148.80, 7.5, 7.70, 148.80, 7.5, 10.125, 153.80,
			7.5, 10.125, 153.80, 7.5, 7.70, 231.30, 7.5, 7.70 };
	TGeoPcon* pipeTSup = new TGeoPcon("pipeTSup", 0., 360., 8);
	pipeTSup->SetDimensions(psd1);

	TGeoCombiTrans* trd1 =
			new TGeoCombiTrans("trd1", 0., 0., z0 + currentz, r1);
	trd1->RegisterYourself();

	TGeoVolume *VpipeTSup = new TGeoVolume("pipeTSup", pipeTSup,
			gGeoManager->GetMedium("steel"));
	VpipeTSup->SetLineColor(34);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 429.9328
	currentz += 231.3;
	fprintf(stderr, "currentz d: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// e - TargetKreuz.1

	// target position
	Double_t tz = 30.0;
	fprintf(stderr, "Target position: 0 / 0 / %f\n", z0 + currentz + tz);

	// target cross - horizontal
	Double_t pse1[24] = { 0., 360., 7, 0., 7.5, 7.55, 10.45, 7.5, 7.55, 26.8,
			1.25, 1.30, 26.8, 1.25, 1.27, 29.98, 1.25, 1.27, 29.98, 0., 1.27,
			tz, 0., 1.27 };
	TGeoPcon* Tcross1 = new TGeoPcon("Tcross1", 0., 360., 7);
	Tcross1->SetDimensions(pse1);

	TGeoTube* Tcross2 = new TGeoTube("Tcross2", 1.0, 1.02, 11.5);

	// target cross vertical
	Double_t pse2[63] = { 0., 360., 20, -182.0, 7.5, 7.7, -135.5, 7.5, 7.7,
			-125.5, 4.5, 4.6, -95.5, 4.5, 4.6, -95.5, 3.0, 3.1, -54.0, 3.0,
			3.1, -54.0, 2.0, 2.1, -42.1, 2.0, 2.1, -17.0, 2.0, 2.1, -17.0, 1.0,
			1.02, 17.0, 1.0, 1.02, 17.0, 2.0, 2.1, 42.1, 2.0, 2.1, 54.0, 2.0,
			2.1, 54.0, 3.0, 3.1, 95.5, 3.0, 3.1, 95.5, 4.5, 4.6, 133.5, 4.5,
			4.6, 143.5, 7.85, 7.95, 168.69, 7.85, 7.95,

	};
	TGeoPcon* Tcross3 = new TGeoPcon("Tcross3", 0., 360., 20);
	Tcross3->SetDimensions(pse2);

	TGeoTube* te1 = new TGeoTube("te1", 0., 1.25, 0.51 + delta);
	TGeoTube* te2 = new TGeoTube("te2", 0., 1.00, 0.51 + delta);
	TGeoTube* te3 = new TGeoTube("te3", 0., 1.00, 1.27 + delta);
	TGeoTube* te4 = new TGeoTube("te4", 0., 1.00, 1.02 + delta);

	TGeoCombiTrans* tre1 = new TGeoCombiTrans("tre1", 0., 0., tz, r2);
	tre1->RegisterYourself();
	TGeoCombiTrans* tre2 = new TGeoCombiTrans("tre2", 0., 0., -11.5, r2);
	tre2->RegisterYourself();
	TGeoCombiTrans* tre3 = new TGeoCombiTrans("tre3", 0., 0., 0., r2);
	tre3->RegisterYourself();
	TGeoCombiTrans* tre4 = new TGeoCombiTrans("tre4", 0., 0., -0.51, r1);
	tre4->RegisterYourself();
	TGeoCombiTrans* tre5 = new TGeoCombiTrans("tre5", 0., 0., 0.51, r1);
	tre5->RegisterYourself();
	TGeoCombiTrans* tre6 = new TGeoCombiTrans("tre6", 0., 0., 41.5, r1);
	tre6->RegisterYourself();
	TGeoCombiTrans* tre7 = new TGeoCombiTrans("tre7", 0., 0., tz, r1);
	tre7->RegisterYourself();
	TGeoCombiTrans* tre8 = new TGeoCombiTrans("tre8", 0., -175.0, tz, r3);
	tre8->RegisterYourself();

	TGeoCombiTrans* tre9 =
			new TGeoCombiTrans("tre9", 0., 0., z0 + currentz, r1);
	tre9->RegisterYourself();

	TGeoCompositeShape *STcross0a = new TGeoCompositeShape("Tcross0a",
			"Tcross1-te3:tre1");
	TGeoCompositeShape *STcross0b = new TGeoCompositeShape("Tcross0b",
			"Tcross2-te4:tre2");
	TGeoCompositeShape *STcross0c = new TGeoCompositeShape("Tcross0c",
			"Tcross3:tre3-te1:tre4-te2:tre5");

	TGeoCompositeShape *STcross = new TGeoCompositeShape("Tcross",
			"Tcross0a+Tcross0b:tre6+Tcross0c:tre7+VATvalve160:tre8");

	TGeoVolume *VTcross = new TGeoVolume("Tcross", STcross,
			gGeoManager->GetMedium(str_ti.c_str()));
	VTcross->SetLineColor(35);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 482.9328
	currentz += 53.0;
	fprintf(stderr, "currentz e: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// f - pipeTSdown

	Double_t psf1[51] = { 0., 360., 16, 0.000, 1.0, 1.02, 3.7321, 2.0, 2.02,
			3.7321, 2.0, 2.05, 93.0000, 2.0, 2.05, 97.4785, 3.2, 3.25,
			101.0000, 3.2, 3.25, 101.0000, 3.2, 3.4, 106.0225, 3.2, 3.4,
			106.0225, 3.2, 3.27, 241.8225, 3.2, 3.27, 241.8225, 3.2, 3.35,
			244.8225, 3.2, 3.35, 244.8225, 3.2, 3.27, 261.0725, 3.2, 3.27,
			261.0725, 3.2, 5.675, 262.8225, 3.2, 5.675 };
	TGeoPcon* pipeTSdown = new TGeoPcon("pipeTSdown", 0., 360., 16);
	pipeTSdown->SetDimensions(psf1);

	TGeoCombiTrans* trf1 =
			new TGeoCombiTrans("trf1", 0., 0., z0 + currentz, r1);
	trf1->RegisterYourself();

	TGeoVolume *VpipeTSdown = new TGeoVolume("pipeTSdown", pipeTSdown,
			gGeoManager->GetMedium(str_ti.c_str()));
	VpipeTSdown->SetLineColor(36);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 745.7553
	currentz += 262.8225;
	fprintf(stderr, "currentz f: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// g - cross_TS_TMPs

	// horizontal part
	Double_t psg1[27] = { 0., 360., 8, 0.000, 3.2, 5.675, 1.7200, 3.2, 5.675,
			1.7200, 3.2, 3.27, 3.3653, 3.2, 3.27, 10.0830, 5.0, 5.07, 51.0,
			5.0, 5.07, 51.0, 5.0, 7.60, 53.0, 5.0, 7.60 };
	TGeoPcon* crossTS1 = new TGeoPcon("crossTS1", 0., 360., 8);
	crossTS1->SetDimensions(psg1);

	TGeoTube* tg1 = new TGeoTube("tg1", 0., 6.07, 5.07 + delta);
	TGeoCombiTrans* trg1 = new TGeoCombiTrans("trg1", 0., 0., 26.0, r2);
	trg1->RegisterYourself();
	TGeoCompositeShape *ScrossTSa = new TGeoCompositeShape("crossTSa",
			"crossTS1-tg1:trg1");

	// pipe perpendicular
	Double_t psg2[21] = { 0., 360., 6, -55.0, 6.0, 10.125, -52.8, 6.0, 10.125,
			-52.8, 6.0, 6.07, 52.8, 6.0, 6.07, 52.8, 6.0, 10.125, 55.0, 6.0,
			10.125 };
	TGeoPcon* crossTS2 = new TGeoPcon("crossTS2", 0., 360., 22);
	crossTS2->SetDimensions(psg2);

	TGeoTube* tg2 = new TGeoTube("tg2", 0., 5.07, 6.07 + delta);
	TGeoCombiTrans* trg2 = new TGeoCombiTrans("trg2", 0., -55.0, 0., r2);
	trg2->RegisterYourself();
	TGeoCombiTrans* trg3 = new TGeoCombiTrans("trg3", 0., 55.0, 0., r3);
	trg3->RegisterYourself();
	TGeoCombiTrans* trg4 = new TGeoCombiTrans("trg4", 0., 0., 7.0775, r1);
	trg4->RegisterYourself();
	TGeoCombiTrans* trg5 = new TGeoCombiTrans("trg5", 0., 0., 33.0775, r1);
	trg5->RegisterYourself();

	// put it together
	TGeoCompositeShape *ScrossTSb = new TGeoCompositeShape("crossTSb",
			"crossTS2:r2-tg2+TVP700:trg2+TVP700:trg3");
	TGeoCompositeShape *ScrossTS = new TGeoCompositeShape("crossTS",
			"crossTSa:trg4+crossTSb:trg5");

	TGeoCombiTrans* trg6 = new TGeoCombiTrans("trg6", 0., 0., 60.0775, r1);
	trg6->RegisterYourself();
	TGeoCombiTrans* trg7 =
			new TGeoCombiTrans("trg7", 0., 0., z0 + currentz, r1);
	trg7->RegisterYourself();

	TGeoCompositeShape *ScrossTSTMPs = new TGeoCompositeShape("crossTSTMPs",
			"VATvalve63+crossTS+VATvalve100:trg6");

	TGeoVolume *VcrossTSTMPs = new TGeoVolume("crossTSTMPs", ScrossTSTMPs,
			gGeoManager->GetMedium("steel"));
	VcrossTSTMPs->SetLineColor(37);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 812.9103
	currentz += 67.155;
	fprintf(stderr, "currentz g: %f\n", currentz);

	// ---------------------------------------------------------------------------
	// h - BeamPipe Dipole
	// The dipole pipe is bent with a radius of 60 m. All elements downstream of this
	// element need to be rotated and shifted accordingly in the xz-plane.
	//Double_t R = 6000.0; // bending radius of the pipe
	Double_t R = bend_radius; // fit results (P.Jasinski) with fixed deflection angle
	//Double_t dz0 = 241.1985; // this is the length of the pipe along z
	Double_t dz0 =  bend_end-bend_begin; // fit results (P.Jasinski)
	Double_t dphi = asin(dz0 / R); // what is indeed bend_radius, as it should be
	Double_t dx0 = R * (1. - cos(dphi));
	TGeoRotation *dipolerot = new TGeoRotation("dipolerot", 90., dphi / rad,
			-90.); // rotation to accout for bendig
	fprintf(stderr, "Rotation due to dipole magnet: %f\n", dphi / rad);

	// shifts need to be computed individually
	// dz = dz0 + s*cos(dphi)    where dx,dz0 is the point where the bended pipe ends
	// dx = dx0 - s*sin(dphi)          s is the length of the added elements
	// combined transformations are then defined with ...
	// TGeoCombiTrans* trpr = new TGeoCombiTrans("trpr", dx, 0., dz, dipolerot); trpr->RegisterYourself();
	Double_t s = 0.;

	// horizontal pipe, first part
	Double_t psh1[15] = { 0., 360., 4, 0., 5.0, 7.6, 2.0, 5.0, 7.6, 2.0, 5.0,
			5.2, 7.1225, 5.0, 5.2, };
	TGeoPcon* Dippip1 = new TGeoPcon("Dippip1", 0., 360., 4);
	Dippip1->SetDimensions(psh1);

	// horizontal pipe, bent part
	TGeoTorus *Dippip2 = new TGeoTorus("Dippip2", R, 5.0, 5.2, 0., dphi / rad);
	//Double_t psh2[24] = { 0., 360., 7, 0., 5.0, 5.2, 5.0225, 5.0, 5.2, 19.3385
	Double_t psh2[24] = { 0., 360., 7, 0., 6.0, 6.2, 5.0225, 6.0, 6.2, 19.3385,
			9.0, 9.2, 19.3385, 9.0, 9.3, 439.3572 + 11.1, 9.0, 9.3, 439.3572 + 11.1, 9.0,
			12.65, 441.7572 + 11.1, 9.0, 12.65, }; //  + 11.1 cm needed with the fit radius by P.Jasinski

	// horizontal pipe, third part
	TGeoPcon* Dippip3 = new TGeoPcon("Dippip3", 0., 360., 7);
	Dippip3->SetDimensions(psh2);

	TGeoCombiTrans* trh1 = new TGeoCombiTrans("trh1", R, 0., 7.1225,
			new TGeoRotation("a", 0., -90., 180.));
	trh1->RegisterYourself();
	TGeoCombiTrans* trh2 = new TGeoCombiTrans("trh2", dx0, .0, 7.1225 + dz0,
			dipolerot);
	//sensor_positions.push_back(7.1225 + dz0);
	trh2->RegisterYourself();
	s = s + 7.0775;
	TGeoCombiTrans* trh3 = new TGeoCombiTrans("trh3", dx0 + s * sin(dphi), 0.,
			7.1225 + dz0 + s * cos(dphi), dipolerot);
	trh3->RegisterYourself();
	s = s + 441.7572 + 11.1; // 11.1 cm needed with the fit radius by P.Jasinski
	TGeoCombiTrans* trh4 =
			new TGeoCombiTrans("trh4", 0., 0., z0 + currentz, r1);
	trh4->RegisterYourself();

	// put it together including VATvalve100
	TGeoCompositeShape *SDippip = new TGeoCompositeShape("Dippip",
			"Dippip1+Dippip2:trh1+VATvalve100:trh2+Dippip3:trh3");//"Dippip1+Dippip3:trh3");//"Dippip1+Dippip2:trh1+VATvalve100:trh2+Dippip3:trh3");

	TGeoVolume *VDipolePip = new TGeoVolume("DipolePip", SDippip,
			gGeoManager->GetMedium("steel"));
	VDipolePip->SetLineColor(38);

	// last z-position: 1498.836617 was 1509.7031905 (s = 448.83470)
	currentz += 7.1225 + dz0 + s * cos(dphi);
	fprintf(stderr, "currentz/s h: %f %f\n", currentz, s);
	std::cout << " luminosity monitor starts at " << currentz + z0 << std::endl;

	// ---------------------------------------------------------------------------
	// i - Luminosity monitor

	TGeoTube* lmd_beampipe_upstr = new TGeoTube(
			"lmd_beampipe_upstr", 9., 9.2, 25.);
	TGeoCombiTrans* lmd_trans_b_up = new TGeoCombiTrans("lmd_trans_b_up", 0., 0., 25.+delta, r1);
	lmd_trans_b_up->RegisterYourself();
	TGeoTube* lmd_flange_upstr = new TGeoTube(
			"lmd_flange_upstr", 9.2, 25.3/2., 1.2);
	TGeoCombiTrans* lmd_trans_fl_up = new TGeoCombiTrans("lmd_trans_fl_up", 0., 0., 1.2+delta, r1);
	lmd_trans_fl_up->RegisterYourself();
	// upstream flange holding the kapton cone
	TGeoTube* lmd_cone_flange_upstr = new TGeoTube(
			"lmd_cone_flange_upstr", 9.2, 25.3/2., 1.5);
	TGeoCombiTrans* lmd_trans_co_fl_up = new TGeoCombiTrans("lmd_trans_co_fl_up", 0., 0., -1.5+50.-delta, r1);
	lmd_trans_co_fl_up->RegisterYourself();

	// 20 mu thick kapton foil cone
	TGeoCone* lmd_capton_cone = new TGeoCone("lmd_capton_cone", 23.386/2., 20.4/2., 20.4/2.+0.002/2., 7./2., 7./2.+0.002/2.);
	TGeoCombiTrans* lmd_trans_cap_co = new TGeoCombiTrans("lmd_trans_cap_co", 0., 0., 50.+23.386/2., r1);
	lmd_trans_cap_co->RegisterYourself();

	// flange holding the kapton cone downstream
	TGeoCone* lmd_cone_flange_downstr = new TGeoCone("lmd_cone_flange_downstr", 3.12/2., 3.1, 8.6/2., 3.1, 3.2);
	TGeoCombiTrans* lmd_trans_co_fl_do = new TGeoCombiTrans("lmd_trans_co_fl_do", 0., 0., 50.+23.386+3.12/2., r1);
	lmd_trans_co_fl_do->RegisterYourself();
	// beam pipe to shield the sensors
	TGeoTube* lmd_beam_pipe = new TGeoTube("lmd_beam_pipe", 3.5, 3.6, 50./2.);
	TGeoCombiTrans* lmd_trans_p = new TGeoCombiTrans("lmd_trans_p", 0., 0., 50.+23.386+50./2., r1);
	lmd_trans_p->RegisterYourself();
	// beam pipe cone downstream
	TGeoCone* lmd_cone_downstr = new TGeoCone("lmd_cone_downstr", 20./2., 3.5, 3.7, 9./2., 9.2/2.);
	TGeoCombiTrans* lmd_trans_co_do = new TGeoCombiTrans("lmd_trans_co_do", 0., 0., 50.+23.386+50.+20./2., r1);
	lmd_trans_co_do->RegisterYourself();
	// beam pipe downstream
	TGeoTube* lmd_beam_pipe_downstream = new TGeoTube("lmd_beam_pipe_downstream", 9./2., 9.2/2., 56./2.);
	TGeoCombiTrans* lmd_trans_p_down = new TGeoCombiTrans("lmd_trans_p_down", 0., 0., 50.+23.386+50.+20.+56./2., r1);
	lmd_trans_p_down->RegisterYourself();
	// main body
	//Double_t psi1[33] = { 0., 360., 10, 0., 9.0, 12.65, 2.4, 9.0, 12.65, 2.4,
	//		9.0, 9.2, 4.4343, 9.0, 9.2, 17.5, 12.5, 12.7, 177.0, 12.5, 12.7,
	//		195.66, 7.5, 7.7, 198.0, 7.5, 7.7, 198.0, 7.5, 10.125, 200.0, 7.5,
	//		10.125 };
	//TGeoPcon* LumMon0 = new TGeoPcon("LumMon0", 0., 360., 10);
	//LumMon0->SetDimensions(psi1);

	// nozzle
	//Double_t psi2[21] = { 0., 360., 6, -27.0, 5.2, 7.6, -25.0, 5.2, 7.6, -25.0,
	//		5.2, 5.4, 25.0, 5.2, 5.4, 25.0, 5.2, 7.6, 27.0, 5.2, 7.6 };
	//TGeoPcon* nozzle0 = new TGeoPcon("nozzle0", 0., 360., 6);
	//nozzle0->SetDimensions(psi2);

	//TGeoTube* ti1 = new TGeoTube("ti1", 0., 12.7, 5.4 + delta);
	//TGeoTube* ti2 = new TGeoTube("ti2", 0., 5.0, 12.7 + delta);

	//TGeoCombiTrans* tri1 = new TGeoCombiTrans("tri1", 0., 0., 77.8, r2);
	//tri1->RegisterYourself();
	//TGeoCombiTrans* tri2 = new TGeoCombiTrans("tri2", 0., 0., 97.8, r2);
	//tri2->RegisterYourself();
	//TGeoCombiTrans* tri3 = new TGeoCombiTrans("tri3", 0., 0., 117.8, r2);
	//tri3->RegisterYourself();
	//TGeoCombiTrans* tri4 = new TGeoCombiTrans("tri4", 0., 0., 137.8, r2);
	//tri4->RegisterYourself();
	//TGeoCombiTrans* tri5 = new TGeoCombiTrans("tri5", 0., 0., 0., r4);
	//tri5->RegisterYourself();

	// put it together
	//TGeoCompositeShape *Snozzle1 = new TGeoCompositeShape("nozzle1",
	//		"(nozzle0-ti1:r2)");
	//TGeoCompositeShape *Snozzles = new TGeoCompositeShape("nozzles",
	//		"nozzle1:tri1+nozzle1:tri2+nozzle1:tri3+nozzle1:tri4");
	//TGeoCompositeShape *Sholes = new TGeoCompositeShape("holes",
	//		"ti1:tri1+ti1:tri2+ti1:tri3+ti1:tri4");
	//TGeoCompositeShape *SLumMon1 = new TGeoCompositeShape("LumMon1",
	//		"LumMon0-holes-(holes:tri5)");
	TGeoCompositeShape *slum_beampipe_upstream = new TGeoCompositeShape("slum_beampipe_upstream",
			"lmd_flange_upstr:lmd_trans_fl_up+lmd_beampipe_upstr:lmd_trans_b_up+lmd_cone_flange_upstr:lmd_trans_co_fl_up");
	TGeoCompositeShape *slum_CaptonCone = new TGeoCompositeShape("slum_CaptonCone", "lmd_capton_cone:lmd_trans_cap_co-lmd_cone_flange_downstr:lmd_trans_co_fl_do-lmd_cone_flange_upstr:lmd_trans_co_fl_up");
	TGeoCompositeShape *slum_beampipe_downstream = new TGeoCompositeShape("slum_beampipe_downstream",
				"lmd_beam_pipe:lmd_trans_p+lmd_cone_downstr:lmd_trans_co_do+lmd_beam_pipe_downstream:lmd_trans_p_down");/*+lmd_cone_flange_downstr:lmd_trans_co_fl_do*/
	//		"nozzles+(nozzles:tri5)");

	TGeoCombiTrans* tri6 = new TGeoCombiTrans("tri6", dx0 + s * sin(dphi), 0.,
			z0 + currentz, dipolerot);

	//std::cout << " rotation for the lumi monitor is " << std::endl;
	//tri6->Print();

	tri6->RegisterYourself();
	s = s + 200.00;

	TGeoVolume *vlum_beampipe_upstream = new TGeoVolume("vlum_beampipe_upstream", slum_beampipe_upstream,
			gGeoManager->GetMedium("steel"));
	vlum_beampipe_upstream->SetLineColor(39);//39);
	TGeoVolume *vlum_CaptonCone = new TGeoVolume("vlum_CaptonCone", slum_CaptonCone,
			gGeoManager->GetMedium("mylar"));
	vlum_CaptonCone->SetLineColor(kRed);//39);
	TGeoVolume *vlum_beampipe_downstream = new TGeoVolume("vlum_beampipe_downstream", slum_beampipe_downstream,
			gGeoManager->GetMedium("steel"));
	vlum_beampipe_downstream->SetLineColor(39);//39);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 1709.5415231 (s = 648.83470)
	currentz += 200.00 * cos(dphi);
	fprintf(stderr, "currentz/s i: %f %f\n", currentz, s);

	// ---------------------------------------------------------------------------
	// j - end cross

	// horizontal pipe, first part
	Double_t psj1[15] = { 0., 360., 4, 0., 7.5, 10.125, 2.0, 7.5, 10.125, 2.0,
			7.5, 7.7, 16.7, 7.5, 7.7 };
	TGeoPcon* EndCross0a = new TGeoPcon("EndCross0a", 0., 360., 4);
	EndCross0a->SetDimensions(psj1);

	// horizontal pipe, second part
	Double_t psj2[15] = { 0., 360., 4, 16.7, 4.5, 4.7, 34.7, 4.5, 4.7, 34.7,
			4.5, 7.6, 36.7, 4.5, 7.6 };
	TGeoPcon* EndCross0b = new TGeoPcon("EndCross0b", 0., 360., 4);
	EndCross0b->SetDimensions(psj2);

	// vertical pipe
	Double_t psj3[21] = { 0., 360., 6, -16.7, 7.5, 10.125, -14.7, 7.5, 10.125,
			-14.7, 7.5, 7.7, 14.7, 7.5, 7.7, 14.7, 7.5, 10.125, 16.7, 7.5,
			10.125 };
	TGeoPcon* EndCross1 = new TGeoPcon("EndCross1", 0., 360., 6);
	EndCross1->SetDimensions(psj3);

	TGeoTube* tj1 = new TGeoTube("tj1", 0., 7.7, 10.125 + delta);
	TGeoTube* tj2 = new TGeoTube("tj2", 0., 7.7, 3.85 + delta);
	TGeoTube* tj3 = new TGeoTube("tj3", 0., 4.7, 3.85 + delta);

	TGeoCombiTrans* trj1 = new TGeoCombiTrans("trj1", 0., 0., 16.7, r2);
	trj1->RegisterYourself();
	TGeoCombiTrans* trj2 =
			new TGeoCombiTrans("trj2", 0., 0., 12.85 - delta, r1);
	trj2->RegisterYourself();
	TGeoCombiTrans* trj3 =
			new TGeoCombiTrans("trj3", 0., 0., 20.55 + delta, r1);
	trj3->RegisterYourself();
	TGeoCombiTrans* trj4 = new TGeoCombiTrans("trj4", 0., 0., 36.7, r1);
	trj4->RegisterYourself();
	TGeoCombiTrans* trj5 = new TGeoCombiTrans("trj5", 0., 16.7, 16.7, r3);
	trj5->RegisterYourself();
	TGeoCombiTrans* trj6 = new TGeoCombiTrans("trj6", 0., -23.7, 16.7, r3);
	trj6->RegisterYourself();
	TGeoCombiTrans* trj7 = new TGeoCombiTrans("trj7", 0., -23.7, 16.7, r2); // here was a typo
	trj7->RegisterYourself();

	// adding valves and pumps
	TGeoCompositeShape *EndCross2 = new TGeoCompositeShape("EndCross2",
			"EndCross0a+EndCross0b-tj1:trj1");
	TGeoCompositeShape *EndCross3 = new TGeoCompositeShape("EndCross3",
			"EndCross1:trj1-tj2:trj2-tj3:trj3");
	TGeoCompositeShape *EndCross4 = new TGeoCompositeShape("EndCross4",
			"TVPwValve:trj5+VATvalve160:trj6+IGP1000:trj7");
	TGeoCompositeShape *EndCross = new TGeoCompositeShape("EndCross",
			"EndCross2+EndCross3+EndCross4+VATvalve100:trj4");

	TGeoCombiTrans* trj8 = new TGeoCombiTrans("trj8", dx0 + s * sin(dphi), 0.,
			z0 + currentz, dipolerot);
	trj8->RegisterYourself();
	s = s + 43.7;

	TGeoVolume *VEndCross = new TGeoVolume("EndCross", EndCross,
			gGeoManager->GetMedium("steel"));
	VEndCross->SetLineColor(40);

	//sensor_positions.push_back(z0 + currentz);
	// last z-position: 1753.2061988 (s = 692.53470)
	currentz += 43.7 * cos(dphi);
	fprintf(stderr, "currentz/s j: %f %f\n", currentz, s);
	//sensor_positions.push_back(z0 + currentz);

	// ---------------------------------------------------------------------------
	// put all volumes together -> beamPipe
	TGeoVolume *beamPipe = new TGeoVolumeAssembly("BeamPipe");
	//gGeoManager->SetTopVolume(beamPipe);
	beamPipe->AddNode(Vgvhesr, 0, tr0); // a
	beamPipe->AddNode(Vpipeup, 0, trb1); // b
	beamPipe->AddNode(Vktmpump, 0, trc2); // c1
	beamPipe->AddNode(VTpumps, 0, tr0); // c2
	beamPipe->AddNode(VpipeTSup, 0, trd1); // d
	beamPipe->AddNode(VTcross, 0, tre9); // e
	beamPipe->AddNode(VpipeTSdown, 0, trf1); // f
	beamPipe->AddNode(VcrossTSTMPs, 0, trg7); // g
	beamPipe->AddNode(VDipolePip, 0, trh4); // h
	//beamPipe->AddNode(VLumMon,        0, tri6);       // i
	//beamPipe->AddNode(vlum_beampipe_upstream, 0, tri6);       // i
	//beamPipe->AddNode(vlum_CaptonCone, 0, tri6);       // i
	//beamPipe->AddNode(vlum_beampipe_downstream, 0, tri6);       // i
	//beamPipe->AddNode(VEndCross,      0, trj8);       // j

	// create an array of z positions for the sensors in 10 cm distances
	for (int iposz = 0; iposz < 1200; iposz+=10) sensor_positions.push_back(iposz);
	// construct the sensors
	if (create_sensors){
		for (unsigned int isensor = 1; isensor < sensor_positions.size(); isensor++){
			std::stringstream volume_name;
			volume_name << "_" << isensor;
			// create a sensor with a radius of 40 cm
			TGeoTube* disc = new TGeoTube(("disc"+volume_name.str()).c_str(), 0., 40., 0.0000001);
			TGeoVolume* Vdisc = new TGeoVolume(("LumActive_pipe_sensor_"+volume_name.str()).c_str(), disc,
					gGeoManager->GetMedium("mylar"));
			TGeoCombiTrans* tre_disc =
						new TGeoCombiTrans(("tre_"+volume_name.str()).c_str(), 0., 0., sensor_positions[isensor], r1);
			tre_disc->RegisterYourself();
			Vdisc->SetLineColor(2);
			beamPipe->AddNode(Vdisc,      0, tre_disc);
		}
	}

	/*
	TGeoVolume *VTest = new TGeoVolume("VTest", SDippip,
			gGeoManager->GetMedium("steel"));
			VTest->SetLineColor(37);
			*/

	// add pipe to cave
	cave->AddNode(beamPipe, 1);
	//beamPipe->AddNode(VTest, 0, trv1);
	gGeoManager->CloseGeometry();

	// check geometry
	//cave->CheckOverlaps(0.1, "");
	//gGeoManager->CheckOverlaps(0.001); // [cm]
	//gGeoManager->CheckGeometryFull();

	// save geometry

	//cave->Write();
	cave->Write();
	fi->Close();

	// plot geometry
	cave->Draw("ogl");
	//cave->Draw("ogl");

	// done
	fprintf(stderr, "\n");
	fprintf(stderr, "<I> Geometry of PANDA beam pipe written to %s\n",
			fGeoFile.Data());
	fprintf(stderr, "\n");

}

#include<TApplication.h>

int main(){
	TApplication myapp("myapp",0,0);
	createRootGeometry_beampipe();
	myapp.Run();
	return 0;
}
