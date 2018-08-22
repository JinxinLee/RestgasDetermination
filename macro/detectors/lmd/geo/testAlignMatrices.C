#include <TH1D.h>
#include <TCanvas.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoPhysicalNode.h>
#include "PndLmdGeometryHelper.h"
#include <TGeoNode.h>
#include <TFile.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <PndLmdHitPair.h>

std::map<std::string, TGeoHMatrix> *matricesMisaligned;
std::map<std::string, TGeoHMatrix> *matricesIdeal;
PndLmdGeometryHelper *helper;

using std::cout;
using std::string;

void initDummySimulation() {
	cout << "creating dummy simulation to populate gGeoManager...\n";
	TString geometryFile = "Luminosity-Detector.root";
	TString simOutput = "./dummy.root";
	FairRunSim *fRun = new FairRunSim();
	fRun->SetName("TGeant4");
	fRun->SetOutputFile(simOutput);
	fRun->SetMaterials("media_pnd.geo");
	FairModule *Cave = new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);
	FairModule *Pipe = new PndPipe("PIPE");
	Pipe->SetGeometryFileName("beampipe_201309.root");
	fRun->AddModule(Pipe);
	FairModule *Magnet = new PndMagnet("MAGNET");
	Magnet->SetGeometryFileName("FullSolenoid_V842.root");
	fRun->AddModule(Magnet);
	FairModule *Dipole = new PndMagnet("MAGNET");
	Dipole->SetGeometryFileName("dipole.geo");
	fRun->AddModule(Dipole);
	PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
	Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
	Lum->SetGeometryFileName(geometryFile);
	fRun->AddModule(Lum);
	fRun->Init();
}

void applyMisalignmentToGGeoManager() {
	bool misaligned = true;
	if (misaligned) {
		//load matrices
		// FIXME: set path correctly!
		std::string misMatricesFilePath = "misalignMatrices-SensorsOnly-100u.root";
		TFile *misalignmentMatrixRootfile = new TFile(misMatricesFilePath.c_str(), "READ");

		if (misalignmentMatrixRootfile->IsOpen()) {
			std::map<std::string, TGeoHMatrix> *matrices;

			gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
			misalignmentMatrixRootfile->Close();

			std::cout << matrices->size()
			    << " matrices successfully read from file.\napplying misalignment.\n";

			//iterate over matrices
			for (auto const& entry : *matrices) {
				TString volPath = entry.first;

				gGeoManager->cd(volPath);

				TGeoNode* currentNode = gGeoManager->GetCurrentNode();
				TGeoMatrix* matrixToNode = currentNode->GetMatrix();

				TGeoHMatrix misalignedMatrixToNode = *matrixToNode * entry.second;

				//this is just for clarity, can probably be removed
				TGeoHMatrix* newMatrixToNode = new TGeoHMatrix(misalignedMatrixToNode);  // new matrix, representing real position

				TGeoPhysicalNode* physicalNode = gGeoManager->MakePhysicalNode(volPath);

				physicalNode->Align(newMatrixToNode);
			}
			std::cout << "all misalignments applied.\n";
		}
		else {
			std::cout << "file could not be read\n";
		}
	}
}

//this function is ugly and stiched together into a barely functioning blob of ugly.
//I know. I'm going to fix this (probably, some time in the future)
TGeoHMatrix readMatrixFromDisk(std::string filename) {

	//read to vector< vector<double > >

	int lines = 0;
	stringstream *valueStream = new stringstream();

	ifstream ifs;
	ifs.open(filename.c_str());
	if (!ifs.is_open()) {
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << "\n";
		return NULL;
	}
	string linebuffer;	// = new string();
	while (std::getline(ifs, linebuffer)) {
		*valueStream << linebuffer << "\n";
		lines++;
	}
	ifs.close();

	std::stringstream *ss;
	std::stringstream iss;
	int i = 0;

	ss = valueStream;
	std::string line, token;
	std::vector<std::vector<double> > data;
	while (getline(*ss, line)) {
		iss << line;
		std::vector<double> tempvec;
		while (getline(iss, token, ',')) {
			tempvec.push_back(std::stod(token));
		}
		data.push_back(tempvec);
		++i;
		iss.clear();
	}
	ss->str("");
	delete ss;
	iss.str("");

	//cast to TGeoHMatrix

	if (data.size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		exit(1);
	}
	if (data[0].size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		exit(1);
	}

	double rot[9];
	double trans[3];

	int rows, columns;
	rows = data.size();
	columns = data[0].size();

	if (rows != 4 || columns != 4) {
		cerr << "wrong matrix dimensions.\n";
		exit(1);
	}

	rot[0] = data[0][0];
	rot[1] = data[0][1];
	rot[2] = data[0][2];
	rot[3] = data[1][0];
	rot[4] = data[1][1];
	rot[5] = data[1][2];
	rot[6] = data[2][0];
	rot[7] = data[2][1];
	rot[8] = data[2][2];

	trans[0] = data[0][3];
	trans[1] = data[1][3];
	trans[2] = data[2][3];

	TGeoHMatrix result;

	result.SetRotation(rot);
	result.SetDx(trans[0]);
	result.SetDy(trans[1]);
	result.SetDz(trans[2]);

	//cout << "matrix " << filename << "\n";
	//result.Print();

	return result;
}

std::map<std::string, TGeoHMatrix> * readRootMatrices(TString filename) {
	cout << "reading files from file: " << filename << "\n";

	TFile *misalignmentMatrixRootfile = new TFile(filename, "READ");

	if (misalignmentMatrixRootfile->IsOpen()) {
		std::map<std::string, TGeoHMatrix> *matrices;

		gDirectory->GetObject("PndLmdMisalignMatrices", matrices);
		misalignmentMatrixRootfile->Close();
		cout << matrices->size() << " matrices successfully read from file.\n";

		return matrices;
	}
	else {
		cout << "file could not be read\n";
		return NULL;
	}

}

void printMatrixDiff(TGeoHMatrix mat1, TGeoHMatrix mat2) {

	const double *translationDesign = mat1.GetTranslation();
	const double *translationICP = mat2.GetTranslation();
	const double *rotationDesign = mat1.GetRotationMatrix();
	const double *rotationICP = mat2.GetRotationMatrix();

	double dx = translationDesign[0] - translationICP[0];
	double dy = translationDesign[1] - translationICP[1];
	double da = rotationDesign[1] - rotationICP[1];

	cout << "\n=-=-=-=-=-=-=-=-=-=\n";
	cout << "differences are:\ndx: " << dx * 1e4 << "\ndy: " << dy * 1e4 << "\n";
	cout << "=-=-=-=-=-=-=-=-=-=\n";
}

// this depends on the geometry currently loaded to gGeoManager!
TGeoHMatrix getMatrixSensorToSensor(int sensorOne, int sensorTwo) {

	// the passive transformation that base-transforms a vector in Panda Global to the system of Sensor x
	TGeoHMatrix toSen0 = helper->getMatrixPndGlobalToSensor(sensorOne);
	TGeoHMatrix toSen1 = helper->getMatrixPndGlobalToSensor(sensorTwo);

	return toSen0.Inverse() * toSen1;

}

TGeoHMatrix getOverlapMatrixLikeICP(PndLmdOverlapInfo info) {

	//prepare misalignment matrices
	TGeoHMatrix misalignmentToSensor0 = (*matricesMisaligned)[info.path1];
	TGeoHMatrix misalignmentToSensor1 = (*matricesMisaligned)[info.path2];
	// transform second misaignment Matrix to new base
	TGeoHMatrix Sen0ToSen1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix misSen1inSen0 = baseTransformation(misalignmentToSensor1, Sen0ToSen1);

	return misalignmentToSensor0.Inverse() * misSen1inSen0;
}

void testInCMnew() {

	cout << " ===== Testing in CM =====\n";

	TString misalignedMatrices = "misalignMatrices-SensorsOnly-100u.root";
	TString idealMatrices = "idealMatrices.root";
	std::string path =
	    "/home/arbeit/RedPro3TB/simulationData/2018-05-07-misalign-100u/LMDmatrices-inSensorOne";

	matricesMisaligned = readRootMatrices(misalignedMatrices);
	matricesIdeal = readRootMatrices(idealMatrices);

	if (!matricesMisaligned || !matricesIdeal) {
		cout << "files not read, aborting.\n";
		exit(1);
	}

	PndLmdGeometryHelper &helper = PndLmdGeometryHelper::getInstance();
	auto overlaps = helper.getAllOverlapInfos();

	string filename;

	double dalpha, dx, dy, dxTarget, dyTarget, daTarget, dxICP, dyICP, daICP;
	TH1D histA("h1", "h1", 30, -1, -1);
	TH1D histX("h2", "h2", 30, -1, -1);
	TH1D histY("h3", "h3", 30, -1, -1);

	cout << "size of overlaps: " << overlaps.size() << "\n";
	for (auto &overlap : overlaps) {

		auto thisDiff = getMatrixDiffCM(overlap);

		// hist das shizzle
		histA.Fill(thisDiff[2] * 1e6);
		histX.Fill(thisDiff[0] * 1e4);
		histY.Fill(thisDiff[1] * 1e4);
	}

	//name histograms, clean up later
	histA.GetXaxis()->SetTitle("d#alpha [#murad]");
	histX.GetXaxis()->SetTitle("dx [#mum]");
	histY.GetXaxis()->SetTitle("dy [#mum]");
	histA.GetYaxis()->SetTitle("Entries");
	histX.GetYaxis()->SetTitle("Entries");
	histY.GetYaxis()->SetTitle("Entries");
	histA.SetTitle("d#alpha");
	histX.SetTitle("dx");
	histY.SetTitle("dy");

	TCanvas canvas("c1", "c1", 800, 600);
	canvas.cd();
	histA.Draw();
	canvas.Print("dAlpha.pdf");

	histX.Draw();
	canvas.Print("dx.pdf");

	histY.Draw();
	canvas.Print("dy.pdf");

}

PndLmdOverlapInfo& getSmallOverlapInfo(std::vector<PndLmdOverlapInfo> &infos, int smallOverlap) {
	//PndLmdOverlapInfo result;
	for (auto &info : infos) {
		int smallOverlapHere = info.overlapID % 10;
		if (smallOverlap == smallOverlapHere) {
			return info;
		}
	}
	return infos[0];
}

// WARNING. the geometry inside gGeomanager must be aligned for this to work!
TGeoHMatrix getMisalignedOverlapFromGeoManager(PndLmdOverlapInfo &info) {

	TGeoHMatrix sen0to1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix sen0to1ICPcorr = getOverlapMatrixLikeICP(info);
	// this order is important!
	return sen0to1ICPcorr * sen0to1;
}

void buildCyclic() {

	TString misalignedMatrices = "misalignMatrices-SensorsOnly-100u.root";
	TString idealMatrices = "idealMatrices.root";
	std::string path =
	    "/home/arbeit/RedPro3TB/simulationData/2018-05-07-misalign-100u/LMDmatrices-inSensorOne";

	matricesMisaligned = readRootMatrices(misalignedMatrices);
	matricesIdeal = readRootMatrices(idealMatrices);

	for (int iHalf = 0; iHalf < 2; iHalf++) {
		for (int iPlane = 0; iPlane < 4; iPlane++) {
			for (int iModule = 0; iModule < 5; iModule++) {

				// the order in the vector is not defined, use getSmallOverlapInfo() for that
				std::vector<PndLmdOverlapInfo> overlaps = helper->getOverlapInfos(iHalf, iPlane, iModule);
				cout << "got " << overlaps.size() << " overlaps for module\n";

				// load all 9 overlap matrices

				TGeoHMatrix mat0 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 0));
				TGeoHMatrix mat1 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 1));
				TGeoHMatrix mat2 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 2));
				TGeoHMatrix mat3 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 3));
				TGeoHMatrix mat4 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 4));
				TGeoHMatrix mat5 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 5));
				TGeoHMatrix mat6 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 6));
				TGeoHMatrix mat7 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 7));
				TGeoHMatrix mat8 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 8));

				// paths:
				/*
				 0->1: fixed externally
				 5->6: fixed externally

				 0->5: 0->5

				 1->2: 1->8->2
				 1->3: 1->8->3
				 1->4: 1->8->2->9->4
				 1->4: 1->8->3->7->4

				 1->6: 1->8->3->6
				 1->7: 1->8->3->7
				 1->8: 1->8
				 1->9: 1->8->2->9
				 1->9: 1->8->3->7->4->9

				 */

				// get 0->1 and 5->6
				TGeoHMatrix sen0to1 = getMatrixSensorToSensor(0, 1);
				TGeoHMatrix sen5to6 = getMatrixSensorToSensor(5, 6);

				cout << "first step:\n";

				// 1->2
				TGeoHMatrix sen1to2 = mat4 * mat5.Inverse();
				// 1->3
				TGeoHMatrix sen1to3 = mat4 * mat1.Inverse();
				// 1->4a
				TGeoHMatrix sen1to4a = mat4 * mat5.Inverse() * mat6 * mat2.Inverse();
				// 1->4b
				TGeoHMatrix sen1to4b = mat4 * mat1.Inverse() * mat7 * mat8.Inverse();

				// 1->5
				TGeoHMatrix sen1to5 = mat0;		// well, kind of...
				// 1->6
				TGeoHMatrix sen1to6 = mat4 * mat5.Inverse() * mat3;
				// 1->7
				TGeoHMatrix sen1to7 = mat4 * mat5.Inverse() * mat7;
				// 1->8
				TGeoHMatrix sen1to8 = mat4;
				// 1->9a
				TGeoHMatrix sen1to9a = mat4 * mat5.Inverse() * mat6;
				// 1->9b
				TGeoHMatrix sen1to9b = mat4 * mat1.Inverse() * mat7 * mat8.Inverse() * mat2;

				cout << "1to2:\n";
				sen1to2.Print();
				cout << "1to3:\n";
				sen1to3.Print();
				cout << "1to4a:\n";
				sen1to4a.Print();
				cout << "1to4b:\n";
				sen1to4b.Print();
				cout << "1to5:\n";
				sen1to5.Print();
				cout << "1to6:\n";
				sen1to6.Print();
				cout << "1to7:\n";
				sen1to7.Print();
				cout << "1to8:\n";
				sen1to8.Print();
				cout << "1to9a:\n";
				sen1to9a.Print();
				cout << "1to9b:\n";
				sen1to9b.Print();

			}
		}
	}

	std::vector<PndLmdOverlapInfo> thisOverlaps;

}

void compareCyclic() {

	// get all overlap infos

	// sort them by module

	// for every module, make the matrices

}

void testInCM() {

	/*
	 * REMEMBER! These matrices live in differnet reference frames! misalignment matrices
	 * are in sensor frames, ICP matrices are in Panda global (or wherever the recos were
	 * decoded in). I need the ideal matrix cave1 -> sensor1 and cave1 -> sensor2
	 */

	cout << " ===== Testing in CM =====\n";

	bool testing = false;

	TString misalignedMatrices = "misalignMatrices-SensorsOnly-100u.root";
	TString idealMatrices = "idealMatrices.root";

	std::map<std::string, TGeoHMatrix> *matricesMisaligned = readRootMatrices(misalignedMatrices);
	std::map<std::string, TGeoHMatrix> *matricesIdeal = readRootMatrices(idealMatrices);

	if (!matricesMisaligned || !matricesIdeal) {
		cout << "files not read, aborting.\n";
		exit(1);
	}

	//prepare matrices
	TGeoHMatrix toLMD = (*matricesIdeal)["/cave_1/lmd_root_0"];
	TGeoHMatrix toHalf = (*matricesIdeal)["/cave_1/lmd_root_0/half_0"];
	TGeoHMatrix toPlane = (*matricesIdeal)["/cave_1/lmd_root_0/half_0/plane_0"];
	TGeoHMatrix toModule = (*matricesIdeal)["/cave_1/lmd_root_0/half_0/plane_0/module_4"];
	TGeoHMatrix toSensor0 = (*matricesIdeal)["/cave_1/lmd_root_0/half_0/plane_0/module_4/sensor_0"];
	TGeoHMatrix toSensor5 = (*matricesIdeal)["/cave_1/lmd_root_0/half_0/plane_0/module_4/sensor_5"];

	gGeoManager->cd("/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_0/LumActivePixelRect_0");
	TGeoHMatrix toActive0 = *(gGeoManager->GetCurrentNode()->GetMatrix());

	cout << "\n\n=====================================\n\n";

	//TGeoHMatrix toActive0 = (*matricesIdeal)["/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_0/LumActivePixelRect"];

	//prepare misalignment matrix
	TGeoHMatrix misalignmentToSensor0 =
	    (*matricesMisaligned)["/cave_1/lmd_root_0/half_0/plane_0/module_4/sensor_0"];
	TGeoHMatrix misalignmentToSensor5 =
	    (*matricesMisaligned)["/cave_1/lmd_root_0/half_0/plane_0/module_4/sensor_5"];

	cout << "\n\n=====================================\n\n";
	cout << "LMD to sen 0 :\n";
	TGeoHMatrix PNDtoSen0 = toLMD * toHalf * toPlane * toModule * toSensor0;
	PNDtoSen0.Print();

	cout << "\n\n=====================================\n\n";
	cout << "LMD to sen 5 :\n";
	TGeoHMatrix PNDtoSen5 = toLMD * toHalf * toPlane * toModule * toSensor5;
	PNDtoSen5.Print();

	cout << "\n\n=====================================\n\n";
	cout << "mis 0 :\n";
	misalignmentToSensor0.Print();

	cout << "\n\n=====================================\n\n";
	cout << "mis 5 :\n";
	misalignmentToSensor5.Print();

	cout << "\n\n=====================================\n\n";
	cout << "mis 0 complete:\n";
	(toLMD * toHalf * toPlane * toModule * toSensor0 * misalignmentToSensor0).Print();

	cout << "\n\n=====================================\n\n";
	cout << "mis 5 complete:\n";
	(toLMD * toHalf * toPlane * toModule * toSensor5 * misalignmentToSensor5).Print();

	cout << "\n\n=====================================\n\n";
	cout << "naiive 0to5 misalign:\n";
	(misalignmentToSensor0.Inverse() * misalignmentToSensor5).Print();

	TGeoHMatrix lmdLocalToSensor0 = toHalf * toPlane * toModule * toSensor0 * misalignmentToSensor0
	    * toActive0;
	TGeoHMatrix lmdLocalToSensor5 = toHalf * toPlane * toModule * toSensor5 * misalignmentToSensor5
	    * toActive0;

	cout << "\n\n=====================================\n\n";
	cout << "sen0mis to sen5mis IN SENSOR 0:\n";
	TGeoHMatrix Sen0ToSen5 = PNDtoSen0.Inverse() * PNDtoSen5;
	TGeoHMatrix misSen5inSen0 = Sen0ToSen5 * misalignmentToSensor5 * Sen0ToSen5.Inverse();
	TGeoHMatrix totalmisinsensor0 = misalignmentToSensor0.Inverse() * misSen5inSen0;
	totalmisinsensor0.Print();
	cout << "\n\n=====================================\n\n";

	// transform each misalignment into lmd local:
	TGeoHMatrix mis0inLmd = lmdLocalToSensor0.Inverse() * misalignmentToSensor0 * lmdLocalToSensor0;
	TGeoHMatrix mis5inLmd = lmdLocalToSensor5.Inverse() * misalignmentToSensor5 * lmdLocalToSensor5;

	// --------------- 0 to 5 MIS * 0 to 5 in LMD (should just be misalignment) ---------------
	cout << "\n sen0to5mis * sen0to5Inv in LMD:\n";
	(mis0inLmd.Inverse() * mis5inLmd).Print();

	// --------------- 0 to 5 MIS * 0 to 5 in LMD (should just be misalignment) ---------------
	cout << "\n complete sen0 to sen5:\n";
	TGeoHMatrix toSen0 = toLMD * toHalf * toPlane * toModule * toSensor0 * toActive0;
	TGeoHMatrix toSen5 = toLMD * toHalf * toPlane * toModule * toSensor0 * toActive0;
	(toSen0.Inverse() * toSen5.Inverse()).Print();

}

// WARNING. the geometry inside gGeomanager must be aligned for this to work!
TGeoHMatrix getMisalignedOverlapFromGeoManager(PndLmdOverlapInfo &info) {

	TGeoHMatrix sen0to1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix sen0to1ICPcorr = getOverlapMatrixLikeICP(info);
	// this order is important!
	return sen0to1ICPcorr * sen0to1;
}

// WARNING. the geometry inside gGeomanager must be aligned for this to work!
TGeoHMatrix getMisalignedOverlapFromICP(PndLmdOverlapInfo &info, std::string ICPmatrix) {

	TGeoHMatrix sen0to1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix sen0to1ICPcorr = readMatrixFromDisk(ICPmatrix);
	// this order is important!
	return sen0to1ICPcorr * sen0to1;
}

void testCyclic() {

	TString misalignedMatrices = "misalignMatrices-SensorsOnly-100u.root";
	TString idealMatrices = "idealMatrices.root";
	std::string path =
	    "/home/arbeit/RedPro3TB/simulationData/2018-05-07-misalign-100u/LMDmatrices-inSensorOne";

	matricesMisaligned = readRootMatrices(misalignedMatrices);
	matricesIdeal = readRootMatrices(idealMatrices);

	/*
	 * REMEMBER! These matrices live in differnet reference frames! misalignment matrices
	 * are in sensor frames, ICP matrices are in Panda global (or wherever the recos were
	 * decoded in). I need the ideal matrix cave1 -> sensor1 and cave1 -> sensor2
	 */

	auto overlapInfos = helper->getAllOverlapInfos();

	// -----------------------------------------------------------------------------------

	PndLmdOverlapInfo overlap1;
	overlap1.id1 = 1;
	overlap1.id2 = 8;
	overlap1.path1 = "/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_1";
	overlap1.path2 = "/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_8";
	overlap1.overlapID = helper->getOverlapIdFromSensorIDs(1, 8);
	//TGeoHMatrix sen1to8mis = getMisalignedOverlapFromGeoManager(overlap1);
	string icpMat1 =
	    "/home/arbeit/RedPro3TB/simulationData/2018-04-19-slight-misalignment/himster/m4cm.mat";
	TGeoHMatrix sen1to8mis = getMisalignedOverlapFromICP(overlap1, icpMat1);

	PndLmdOverlapInfo overlap2;
	overlap2.id1 = 2;
	overlap2.id2 = 8;
	overlap2.path1 = "/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_2";
	overlap2.path2 = "/cave_1/lmd_root_0/half_0/plane_0/module_0/sensor_8";
	overlap2.overlapID = helper->getOverlapIdFromSensorIDs(2, 8);
	//TGeoHMatrix sen2to8mis = getMisalignedOverlapFromGeoManager(overlap2);
	string icpMat2 =
	    "/home/arbeit/RedPro3TB/simulationData/2018-04-19-slight-misalignment/himster/m5cm.mat";
	TGeoHMatrix sen2to8mis = getMisalignedOverlapFromICP(overlap2, icpMat2);

	cout << "\n by function 1 to 2 by hand\n\n";
	TGeoHMatrix sen1to2mis = sen1to8mis * sen2to8mis.Inverse();
	sen1to2mis.Print();

	// -----------------------------------------------------------------------------------

	applyMisalignmentToGGeoManager();
	cout << "\n sen 1 to 2 after misalignment\n\n";
	TGeoHMatrix sen1to2GeoManager = getMatrixSensorToSensor(1, 2);
	sen1to2GeoManager.Print();
	// bloody fuck, this finally works

	printMatrixDiff(sen1to2GeoManager, sen1to2mis);

	cout << "okay, this works and delivers the matrix that transforms a point of sensor 2 ";
	cout << "into the system of sensor 1 with misaligned geometry.\n";
	cout << "this is our baseline for what the ICP must find.\n";

	cout << "sen0 to sen5 misaligned:\n";
	sen0to5misaligned.Print();

	// we need the misaligned version because the geometry is misaligned
	TGeoHMatrix PndToSensor0 = toSensor0completeMisaligned * toSensor0 * toModule * toPlane * toHalf
	    * toPnd;

	//transform calculated from sensor0 to pnd system
	TGeoHMatrix sen0to5misalignedPND = sen0to5misaligned;
	//TGeoHMatrix sen0to5misalignedPND = PndToSensor0 * sen0to5misaligned * PndToSensor0.Inverse();

	cout << "sen0 to sen5 misaligned in PND:\n";
	sen0to5misalignedPND.Print();

	//then compare
	cout << "ICP found overlap matrix:\n";
	TGeoHMatrix mat1to2ICP = readMatrixFromDisk(
	    "/home/arbeit/RedPro3TB/simulationData/2018-04-24-slight-misalignment/himster/m0px.mat");
	mat1to2ICP.Print();

	const double *translationDesign = sen0to5misalignedPND.GetTranslation();
	const double *translationICP = mat1to2ICP.GetTranslation();
	const double *rotationDesign = sen0to5misalignedPND.GetRotationMatrix();
	const double *rotationICP = mat1to2ICP.GetRotationMatrix();

	double dx = translationDesign[0] - translationICP[0];
	double dy = translationDesign[1] - translationICP[1];
	double da = rotationDesign[1] - rotationICP[1];

	cout << "=====================\n";
	cout << "dx: " << dx * 1e4 << " um\n";
	cout << "dy: " << dy * 1e4 << " um\n";
	cout << "da: " << da * 1e3 << " mrad\n";
	cout << "=====================\n";
}

void histICPmatrices() {

	vector<int> availableOverlapIDs;

	PndLmdGeometryHelper &helper = PndLmdGeometryHelper::getInstance();
	availableOverlapIDs = helper.getAvailableOverlapIDs();

	string path = "/home/arbeit/RedPro3TB/simulationData/newGeometry-aligned/LMDmatrices/m";
	string ext = "cm.mat";
	string filename;

	double dalpha, dx, dy, dxTarget, dyTarget, daTarget, dxICP, dyICP, daICP;
	TH1D histA("h1", "h1", 30, -1, -1);
	TH1D histX("h2", "h2", 30, -1, -1);
	TH1D histY("h3", "h3", 30, -1, -1);

	for (auto &id : availableOverlapIDs) {

		// make file name string
		filename = path + to_string(id) + ext;

		// read matrix
		TGeoHMatrix matrix = readMatrixFromDisk(filename);

		// calc shift and rot
		const double *translation = matrix.GetTranslation();
		const double *rotation = matrix.GetRotationMatrix();

		double dx = translation[0] * 1e4;
		double dy = translation[1] * 1e4;
		double da = rotation[1] * 1e6;

		// hist das shizzle
		histA.Fill(da);
		histX.Fill(dx);
		histY.Fill(dy);

	}

	//name histograms, clean up later
	histA.GetXaxis()->SetTitle("d#alpha [#murad]");
	histX.GetXaxis()->SetTitle("d#x [#mum]");
	histY.GetXaxis()->SetTitle("d#y [#mum]");
	histA.GetYaxis()->SetTitle("Entries");
	histX.GetYaxis()->SetTitle("Entries");
	histY.GetYaxis()->SetTitle("Entries");
	histA.SetTitle("d#alpha");
	histX.SetTitle("dx");
	histY.SetTitle("dy");

	TCanvas canvas("c1", "c1", 800, 600);
	canvas.cd();
	histA.Draw();
	canvas.Print("dAlpha.pdf");

	histX.Draw();
	canvas.Print("dx.pdf");

	histY.Draw();
	canvas.Print("dy.pdf");

}

void histDairXYZdistances() {

	double distz, distx, disty;
	TH1D histZ("h1", "h1", 30, -1, -1);
	TH1D histX("h2", "h2", 30, -1, -1);
	TH1D histY("h3", "h3", 30, -1, -1);

	vector<string> fileNames;

	//fileNames.push_back("/home/arbeit/RedPro3TB/simulationData/2018-04-19-slight-misalignment/Lumi_Pairs_1000000.root");
	fileNames.push_back(
	    "/home/arbeit/RedPro3TB/simulationData/newGeometry-aligned/LMDPairs/Lumi_Pairs_1000000.root");
	//fileNames.push_back("/home/arbeit/RedPro3TB/simulationData/2018-05-07-misalign-100u/LMDPairs/Lumi_Pairs_1000000.root");

	TChain* chainPairs = new TChain("pndsim");
	for (size_t i = 0; i < fileNames.size(); i++) {
		//cout << files[i] << endl;
		if (fileNames[i].find("Lumi_Pairs") != std::string::npos) {
			chainPairs->Add(fileNames[i].c_str());
		}
	}

	//pairs of sensors in LMD coordinates
	TClonesArray* hitPairs = new TClonesArray("PndLmdHitPair");
	chainPairs->SetBranchAddress("PndLmdHitPair", &hitPairs);
	int nEntries = chainPairs->GetEntries();
	cout << "HitPairs no of entries: " << nEntries << "\n";

	cout << "Sorting Pairs to Manager...\n";
	int totalPairs = 0;
	for (int i_event = 0; i_event < nEntries; i_event++) {

		chainPairs->GetEntry(i_event);
		int nPairs = hitPairs->GetEntries();

		//loop over hitPairs per Event
		for (int i_Pair = 0; i_Pair < nPairs; i_Pair++) {
			PndLmdHitPair *currentPair = (PndLmdHitPair*) (hitPairs->At(i_Pair));

			distx = currentPair->getHit1().x() - currentPair->getHit2().x();
			disty = currentPair->getHit1().y() - currentPair->getHit2().y();
			distz = currentPair->getHit1().z() - currentPair->getHit2().z();

			histX.Fill(distx * 1e4);
			histY.Fill(disty * 1e4);
			histZ.Fill(distz * 1e4);

		}
	}

	//name histograms, clean up later
	histZ.GetXaxis()->SetTitle("dz [#mum]");
	histX.GetXaxis()->SetTitle("dx [#mum]");
	histY.GetXaxis()->SetTitle("dy [#mum]");
	histZ.GetYaxis()->SetTitle("Entries");
	histX.GetYaxis()->SetTitle("Entries");
	histY.GetYaxis()->SetTitle("Entries");
	histZ.SetTitle("dz");
	histX.SetTitle("dx");
	histY.SetTitle("dy");

	TCanvas canvas("c1", "c1", 800, 600);
	canvas.cd();
	histZ.Draw();
	canvas.Print("distZ.pdf");

	histX.Draw();
	canvas.Print("distX.pdf");

	histY.Draw();
	canvas.Print("distY.pdf");

}

int testAlignMatrices() {

	/*
	 * some example values:
	 *
	 * "/home/arbeit/RedPro3TB/simulationData/2018-04-19-slight-misalignment/himster/m0cm.mat"
	 * "misalignMatrices-SensorsOnly-100u.root"
	 * "/cave_1/lmd_root_0/half_0/plane_0/module_1/sensor_0"
	 */

	cout << "starting.\n";

	initDummySimulation();
	helper = &(PndLmdGeometryHelper::getInstance());

	//histICPmatrices();
	//histDairXYZdistances();

	//testCyclic();
	buildCyclic();

	cleanup();

	return 0;
}

