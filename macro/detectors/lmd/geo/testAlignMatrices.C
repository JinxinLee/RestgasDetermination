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

void applyMisalignmentToGGeoManager(TString filename) {
	bool misaligned = true;
	if (misaligned) {
		//load matrices
		TFile *misalignmentMatrixRootfile = new TFile(filename, "READ");

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

TGeoHMatrix baseTransformation(TGeoHMatrix &input, TGeoHMatrix &toBaseMatrix) {
	return TGeoHMatrix(toBaseMatrix * input * toBaseMatrix.Inverse());
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

std::vector<double> getMatrixDiff(TGeoHMatrix &mat1, TGeoHMatrix &mat2) {
	const double *translationDesign = mat1.GetTranslation();
	const double *translationICP = mat2.GetTranslation();
	const double *rotationDesign = mat1.GetRotationMatrix();
	const double *rotationICP = mat2.GetRotationMatrix();

	double dx = translationDesign[0] - translationICP[0];
	double dy = translationDesign[1] - translationICP[1];
	double da = rotationDesign[1] - rotationICP[1];

	std::vector<double> result;
	result.push_back(dx);
	result.push_back(dy);
	result.push_back(da);
	return result;
}

void buildCyclic(int alignParam) {

	TString idealMatrices = "idealMatrices.root";

	TString misalignedMatrices = "";
	std::string pathPrefix = "/home/arbeit/RedPro3TB/simulationData/";
	std::string path;
	std::string pdfPath;


	switch(alignParam){
	case 0:
		break;
	case 10:
		misalignedMatrices = "misalignMatrices-SensorsOnly-10.root";
		path = "2018-08-himster2-misalign-10u/LMDmatrices/";
		pdfPath = "misalign-10u/";
		break;
	case 50:
		misalignedMatrices = "misalignMatrices-SensorsOnly-50.root";
		path = "2018-08-himster2-misalign-50u/LMDmatrices/";
		pdfPath = "misalign-50u/";
		break;
	case 100:
		misalignedMatrices = "misalignMatrices-SensorsOnly-100.root";
		path = "2018-08-himster2-misalign-100u/LMDmatrices/";
		pdfPath = "misalign-100u/";
		break;
	case 150:
		misalignedMatrices = "misalignMatrices-SensorsOnly-150.root";
		path = "2018-08-himster2-misalign-150u/LMDmatrices/";
		pdfPath = "misalign-150u/";
		break;
	case 200:
		misalignedMatrices = "misalignMatrices-SensorsOnly-200.root";
		path = "2018-08-himster2-misalign-200u/LMDmatrices/";
		pdfPath = "misalign-200u/";
		break;
	case 250:
		misalignedMatrices = "misalignMatrices-SensorsOnly-250.root";
		path = "2018-08-himster2-misalign-250u/LMDmatrices/";
		pdfPath = "misalign-250u/";
		break;
	}

	matricesMisaligned = readRootMatrices(misalignedMatrices);
	matricesIdeal = readRootMatrices(idealMatrices);

	std::map<int, TGeoHMatrix> matricesByHand;
	std::map<int, TGeoHMatrix> matricesByGeoManager;

	bool compareWithIdeal = false;
	string ext = "cm.mat";

	if (compareWithIdeal) {
		cout << "getting matrices by hand | ICP-like matrices from gGeoManager\n";
	}
	else {
		cout << "getting matrices by hand | real ICP matrices\n";
	}

	path = pathPrefix + path;

	gSystem->Exec(("mkdir "+pdfPath).c_str());

	for (int iHalf = 0; iHalf < 2; iHalf++) {
		for (int iPlane = 0; iPlane < 4; iPlane++) {
			for (int iModule = 0; iModule < 5; iModule++) {

				// the order in the vector is not defined, use getSmallOverlapInfo() for that
				std::vector<PndLmdOverlapInfo> overlaps = helper->getOverlapInfos(iHalf, iPlane, iModule);

				// load all 9 overlap matrices
				TGeoHMatrix mat0;
				TGeoHMatrix mat1;
				TGeoHMatrix mat2;
				TGeoHMatrix mat3;
				TGeoHMatrix mat4;
				TGeoHMatrix mat5;
				TGeoHMatrix mat6;
				TGeoHMatrix mat7;
				TGeoHMatrix mat8;

				if (compareWithIdeal) {
					mat0 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 0));
					mat1 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 1));
					mat2 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 2));
					mat3 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 3));
					mat4 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 4));
					mat5 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 5));
					mat6 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 6));
					mat7 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 7));
					mat8 = getMisalignedOverlapFromGeoManager(getSmallOverlapInfo(overlaps, 8));
				}
				else {
					string icp0 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 0).overlapID) + ext;
					mat0 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 0), icp0);
					string icp1 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 1).overlapID) + ext;
					mat1 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 1), icp1);
					string icp2 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 2).overlapID) + ext;
					mat2 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 2), icp2);
					string icp3 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 3).overlapID) + ext;
					mat3 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 3), icp3);
					string icp4 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 4).overlapID) + ext;
					mat4 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 4), icp4);
					string icp5 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 5).overlapID) + ext;
					mat5 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 5), icp5);
					string icp6 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 6).overlapID) + ext;
					mat6 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 6), icp6);
					string icp7 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 7).overlapID) + ext;
					mat7 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 7), icp7);
					string icp8 = path + "/m" + std::to_string(getSmallOverlapInfo(overlaps, 8).overlapID) + ext;
					mat8 = getMisalignedOverlapFromICP(getSmallOverlapInfo(overlaps, 8), icp8);
				}

				// get 0->1 and 5->6
				TGeoHMatrix sen0to1 = getMatrixSensorToSensor(0, 1);
				TGeoHMatrix sen5to6 = getMatrixSensorToSensor(5, 6);

				TGeoHMatrix sen1to2 = mat4 * mat5.Inverse();
				TGeoHMatrix sen1to3 = mat4 * mat1.Inverse();
				TGeoHMatrix sen1to4a = mat4 * mat5.Inverse() * mat6 * mat2.Inverse();
				TGeoHMatrix sen1to4b = mat4 * mat1.Inverse() * mat7 * mat8.Inverse();

				TGeoHMatrix sen1to5 = mat0;		// well, kind of...
				TGeoHMatrix sen1to6 = mat4 * mat1.Inverse() * mat3;
				TGeoHMatrix sen1to7 = mat4 * mat1.Inverse() * mat7;
				TGeoHMatrix sen1to8 = mat4;
				TGeoHMatrix sen1to9a = mat4 * mat5.Inverse() * mat6;
				TGeoHMatrix sen1to9b = mat4 * mat1.Inverse() * mat7 * mat8.Inverse() * mat2;

				int matrixId = 1000 * iHalf + 100 * iPlane + 10 * iModule;

				matricesByHand[matrixId + 2] = sen1to2;
				matricesByHand[matrixId + 3] = sen1to3;
				matricesByHand[matrixId + 4] = sen1to4a;
				matricesByHand[matrixId + 5] = sen1to5;		// this is not yet correct
				matricesByHand[matrixId + 6] = sen1to6;
				matricesByHand[matrixId + 7] = sen1to7;
				matricesByHand[matrixId + 8] = sen1to8;
				matricesByHand[matrixId + 9] = sen1to9a;
			}
		}
	}

	applyMisalignmentToGGeoManager(misalignedMatrices);
	cout << "getting matrices by geoManager after misalignment\n";
	for (int iHalf = 0; iHalf < 2; iHalf++) {
		for (int iPlane = 0; iPlane < 4; iPlane++) {
			for (int iModule = 0; iModule < 5; iModule++) {

				// the order in the vector is not defined, use getSmallOverlapInfo() for that
				std::vector<PndLmdOverlapInfo> overlaps = helper->getOverlapInfos(iHalf, iPlane, iModule);
				// get everything from gGeoManager
				int matrixId = 1000 * iHalf + 100 * iPlane + 10 * iModule;

				PndLmdOverlapInfo info0 = getSmallOverlapInfo(overlaps, 0);
				PndLmdOverlapInfo info1 = getSmallOverlapInfo(overlaps, 1);
				PndLmdOverlapInfo info2 = getSmallOverlapInfo(overlaps, 2);
				PndLmdOverlapInfo info3 = getSmallOverlapInfo(overlaps, 3);
				PndLmdOverlapInfo info4 = getSmallOverlapInfo(overlaps, 4);
				PndLmdOverlapInfo info5 = getSmallOverlapInfo(overlaps, 5);
				PndLmdOverlapInfo info6 = getSmallOverlapInfo(overlaps, 6);
				PndLmdOverlapInfo info7 = getSmallOverlapInfo(overlaps, 7);
				PndLmdOverlapInfo info8 = getSmallOverlapInfo(overlaps, 8);

				// get sensors 1 and 2 from the overlapInfos
				int sensor0 = info0.id1;
				int sensor1 = info4.id1;
				int sensor2 = info5.id1;
				int sensor3 = info1.id1;
				int sensor4 = info2.id1;
				int sensor5 = info0.id2;
				int sensor6 = info3.id2;
				int sensor7 = info7.id2;
				int sensor8 = info4.id2;
				int sensor9 = info6.id2;

				TGeoHMatrix sen1to2 = getMatrixSensorToSensor(sensor1, sensor2);
				TGeoHMatrix sen1to3 = getMatrixSensorToSensor(sensor1, sensor3);
				TGeoHMatrix sen1to4a = getMatrixSensorToSensor(sensor1, sensor4);
				TGeoHMatrix sen1to5 = getMatrixSensorToSensor(sensor0, sensor5);
				TGeoHMatrix sen1to6 = getMatrixSensorToSensor(sensor1, sensor6);
				TGeoHMatrix sen1to7 = getMatrixSensorToSensor(sensor1, sensor7);
				TGeoHMatrix sen1to8 = getMatrixSensorToSensor(sensor1, sensor8);
				TGeoHMatrix sen1to9a = getMatrixSensorToSensor(sensor1, sensor9);

				matricesByGeoManager[matrixId + 2] = sen1to2;
				matricesByGeoManager[matrixId + 3] = sen1to3;
				matricesByGeoManager[matrixId + 4] = sen1to4a;
				matricesByGeoManager[matrixId + 5] = sen1to5;		// this is not yet correct
				matricesByGeoManager[matrixId + 6] = sen1to6;
				matricesByGeoManager[matrixId + 7] = sen1to7;
				matricesByGeoManager[matrixId + 8] = sen1to8;
				matricesByGeoManager[matrixId + 9] = sen1to9a;
			}
		}
	}

	cout << "making histograms\n";
	double dalpha, dx, dy, dxTarget, dyTarget, daTarget, dxICP, dyICP, daICP;
	TH1D histA("h1", "h1", 30, -1, -1);
	TH1D histX("h2", "h2", 30, -1, -1);
	TH1D histY("h3", "h3", 30, -1, -1);

	for (auto &i : matricesByHand) {
		int key = i.first;
		auto thisDiff = getMatrixDiff(matricesByHand[key], matricesByGeoManager[key]);

		// hist das shizzle
		histA.Fill(thisDiff[2] * 1e3);
		histX.Fill(thisDiff[0] * 1e4);
		histY.Fill(thisDiff[1] * 1e4);
	}

	//name histograms, clean up later
	histA.GetXaxis()->SetTitle("d#alpha [#mrad]");
	histX.GetXaxis()->SetTitle("dx [#mum]");
	histY.GetXaxis()->SetTitle("dy [#mum]");
	histA.GetYaxis()->SetTitle("Entries");
	histX.GetYaxis()->SetTitle("Entries");
	histY.GetYaxis()->SetTitle("Entries");
	histA.SetTitle("d#alpha-combined");
	histX.SetTitle("dx-combined");
	histY.SetTitle("dy-combined");

	TCanvas canvas("c1", "c1", 800, 600);
	canvas.cd();
	histA.Draw();
	canvas.Print((pdfPath + "dAlpha-combined.pdf").c_str());

	histX.Draw();
	canvas.Print((pdfPath + "dx-combined.pdf").c_str());

	histY.Draw();
	canvas.Print((pdfPath + "dy-combined.pdf").c_str());

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

	buildCyclic(10);
	buildCyclic(50);
	buildCyclic(100);
	buildCyclic(150);
	buildCyclic(200);
	buildCyclic(250);


	// compareMatrices(matrices, "/LMDMatrices/");
	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

	return 0;
}
