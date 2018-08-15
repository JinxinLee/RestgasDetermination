/*
 * PndLmdAlignQA.cxx
 *
 *  Created on: Jul 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include <PndLmdAlignQA.h>

#include <boost/filesystem.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGeoManager.h>

#include "PndLmdAlignManager.h"

using std::cout;
using std::cerr;
using std::make_pair;
using std::string;
using std::vector;

PndLmdAlignQA::PndLmdAlignQA() {
	init();
}

PndLmdAlignQA::~PndLmdAlignQA() {

}

void PndLmdAlignQA::init() {
	infoMomentum = -1;
	infoAbsolute = false;
	infoRelative = false;
	byPlane = false;
	_inCentimeters = false;
	helper = &PndLmdGeometryHelper::getInstance();
}

// -------------------------------- new functions --------------------------------

// ========== helper functions

void PndLmdAlignQA::createHist(std::vector<std::vector<double> >& vec, histParams &parameters) {

	TH1D histogram(parameters.title.c_str(), parameters.title.c_str(), parameters.bins, parameters.xMin,
	    parameters.xMax);

	histogram.GetXaxis()->SetTitle(parameters.xtitle.c_str());
	histogram.GetYaxis()->SetTitle(parameters.ytitle.c_str());
	if (vec.size() == 0) {
		cout << "Error: nothing read, data empty! (maybe not enough pairs?) \n";
		exit(1);
	}

	double dataPoint;
	for (size_t iArea = 0; iArea < vec.size(); iArea++) {
		dataPoint = vec[iArea][parameters.vectorIndex] * parameters.scaleFactor;
		histogram.Fill(dataPoint);
	}
	std::stringstream pathname;
	pathname << parameters.path;

	if (parameters.printCMPXinPathName) {
		_inCentimeters ? pathname << "/inCm/" : pathname << "/inPx/";
	}
	PndLmdAlignManager::mkdir(pathname.str());
	TCanvas canvas("canvas", "canvas", 800, 600);
	canvas.cd();
	histogram.Draw();
	histogram.Draw("HIST TEXT0 SAME");
	canvas.Print((pathname.str() + parameters.fileName).c_str());
}

void PndLmdAlignQA::readMatrixInfo() {

	string filename;
	if (_inCentimeters) {
		filename = LMDMatPath + "/info-cm.txt";
	}
	else {
		filename = LMDMatPath + "/info-px.txt";
	}

	std::ifstream info;
	info.open(filename.c_str());
	if (info.is_open()) {

		string line;
		std::vector<string> values;
		int overlapid, noPairs;
		bool alignerComplete = true;

		while (std::getline(info, line)) {
			if (line.find("aligner") != std::string::npos && alignerComplete) {
				//cout << "found aligner line\n";
				values = manager.findRegex(line, "aligner (\\d{1,4})");
				if (values.size() > 1) {
					overlapid = std::stoi(values[1]);
					//cout << "aligner: " << overlapid << endl;
					alignerComplete = false;
				}
			}
			else if (line.find("no of pairs") != std::string::npos) {
				//cout << "found pairs line\n";
				values = manager.findRegex(line, "no of pairs. (\\d{1,6})");
				if (values.size() > 1) {
					noPairs = std::stoi(values[1]);
					matrixInfo[overlapid] = noPairs;
					alignerComplete = true;
				}
			}
		}
	}
	else {
		throw std::runtime_error("Unable to open file " + filename);
	}
}

std::map<std::string, TGeoHMatrix> * PndLmdAlignQA::readRootMatrices(TString &filename) {
	cout << "reading matrices from file: " << filename << "\n";

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

TGeoHMatrix PndLmdAlignQA::baseTransformation(TGeoHMatrix &input, TGeoHMatrix &toBaseMatrix) {
	return TGeoHMatrix(toBaseMatrix * input * toBaseMatrix.Inverse());
}

// WARNING. the geometry inside gGeomanager must be aligned for this to work!
TGeoHMatrix PndLmdAlignQA::getMisalignedOverlapFromGeoManager(PndLmdOverlapInfo &info) {

	TGeoHMatrix sen0to1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix sen0to1ICPcorr = getOverlapMatrixLikeICP(info);
	// this order is important!
	return sen0to1ICPcorr * sen0to1;
}

// WARNING. the geometry inside gGeomanager must be aligned for this to work!
TGeoHMatrix PndLmdAlignQA::getMisalignedOverlapFromICP(PndLmdOverlapInfo &info, std::string ICPmatrix) {

	TGeoHMatrix sen0to1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix sen0to1ICPcorr = manager.readTGeoHMatrix(ICPmatrix);
	// this order is important!
	return sen0to1ICPcorr * sen0to1;
}

// this depends on the geometry currently loaded to gGeoManager!
TGeoHMatrix PndLmdAlignQA::getMatrixSensorToSensor(int sensorOne, int sensorTwo) {

	// the passive transformation that base-transforms a vector in Panda Global to the system of Sensor x
	TGeoHMatrix toSen0 = helper->getMatrixPndGlobalToSensor(sensorOne);
	TGeoHMatrix toSen1 = helper->getMatrixPndGlobalToSensor(sensorTwo);

	return toSen0.Inverse() * toSen1;

}

TGeoHMatrix PndLmdAlignQA::getOverlapMatrixLikeICP(PndLmdOverlapInfo &info) {

	if (!matricesMisaligned) {
		cerr << "Error! No matrices in memory, please load them first.\n";
		return TGeoHMatrix();
	}

	//prepare misalignment matrices
	TGeoHMatrix misalignmentToSensor0 = (*matricesMisaligned)[info.path1];
	TGeoHMatrix misalignmentToSensor1 = (*matricesMisaligned)[info.path2];
	// transform second misaignment Matrix to new base
	TGeoHMatrix Sen0ToSen1 = getMatrixSensorToSensor(info.id1, info.id2);
	TGeoHMatrix misSen1inSen0 = baseTransformation(misalignmentToSensor1, Sen0ToSen1);

	return misalignmentToSensor0.Inverse() * misSen1inSen0;
}

double PndLmdAlignQA::calculateOverlappingArea(int sensor1, int sensor2) {

	TGeoHMatrix sen1to2 = getMatrixSensorToSensor(sensor1, sensor2);

	//histogram those distances
	double colTest2 = 0, rowTest2 = 0;
	int valid = 0;

	TVector3 testVector;
	TVector3 testVectorTransformed;

	//for every pixel (250*250) find all 4 overlap pixels (cutoff at 100 microns, pixels can't be that far away on perfect geometry)
	for (int colTest1 = 0; colTest1 < 247; colTest1++) {
		for (int rowTest1 = 0; rowTest1 < 242; rowTest1++) {

			testVector.SetXYZ(colTest1, rowTest1, 0.0);

			//FIXME: this line is not correct
			//sen1to2.MasterToLocal(testVector., testVectorTransformed);

			colTest2 = testVectorTransformed.x();
			rowTest2 = testVectorTransformed.y();

			//also, check if overlap pixel even exists. must be row elem [0,250], col elem [0,250]
			//are we still overlapping area?
			if (colTest2 < 0 || colTest2 > 247) {		//to account for inactive area
				continue;
			}
			if (rowTest2 < 0 || rowTest2 > 242) {		//to account for inactive area
				continue;
			}

			//we are still on overlapping area
			valid++;
		}
	}
	double coverage = (double) valid / (250.0 * 250.0) * 100;

	return coverage;
}

void PndLmdAlignQA::calculateOverlapingAreas() {

	//get all available overlapping areas

	cout << "------------ TESTING --------------\n";
	cout << "\\AtoB{4}{9} & " << calculateOverlappingArea(4, 9) << "\n";
	cout << "\\AtoB{0}{1} & " << calculateOverlappingArea(0, 1) << "\n";
	cout << "\\AtoB{0}{2} & " << calculateOverlappingArea(0, 2) << "\n";
	cout << "\\AtoB{0}{9} & " << calculateOverlappingArea(0, 9) << "\n";
	cout << "------------  DONE --------------\n";

	exit(0);

	vector<int> overlapIDs = helper->getAvailableOverlapIDs();
	int id1, id2;
	double areaPercent = 0;

	//get id1 and id2 from them and calc
	for (unsigned int i = 0; i < overlapIDs.size(); i++) {
		id1 = helper->getSensorOneFromOverlapID(overlapIDs[i]);
		id2 = helper->getSensorTwoFromOverlapID(overlapIDs[i]);

		areaPercent = calculateOverlappingArea(id1, id2);
		cout << "\\AtoB{" << id1 << "}{" << id2 << "} & " << areaPercent << "\n";
	}
	exit(0);

}

int PndLmdAlignQA::noOfPairs(int overlapID) {
	return matrixInfo[overlapID];
}

bool PndLmdAlignQA::checkForMatrixFiles() {

	//list all IDs that SHOULD be there
	vector<int> availableIds = helper->getAvailableOverlapIDs();

	vector<string> files;
	manager.searchFiles(LMDMatPath, files, "mat", false);
	int foundFiles = 0;

	//no matrix files at all!
	if (files.size() == 0) {
		return false;
	}

	string matrixName;
	bool tempfilefound = false;

	//check for every ID that should be there if there is a corresponding file
	for (size_t i = 0; i < availableIds.size(); i++) {

		//reset counter
		tempfilefound = false;
		matrixName = manager.makeMatrixFileName(availableIds[i], _inCentimeters);

		for (size_t j = 0; j < files.size(); j++) {
			if (files[j].find(matrixName) != string::npos) {
				tempfilefound = true;
				foundFiles++;
			}
		}
		//file not found? at least one is missing, return false
		if (!tempfilefound) {
			return tempfilefound;
		}
	}
	// if no file could not be found, everything is okay
	return true;
}

std::vector<double> PndLmdAlignQA::getMatrixDiffCM(PndLmdOverlapInfo &info, std::string &icpFile) {
	std::vector<double> result;

	TGeoHMatrix ICP = manager.readTGeoHMatrix(icpFile);
	TGeoHMatrix Mtarget = getOverlapMatrixLikeICP(info);

	const double *translationDesign = Mtarget.GetTranslation();
	const double *translationICP = ICP.GetTranslation();
	const double *rotationDesign = Mtarget.GetRotationMatrix();
	const double *rotationICP = ICP.GetRotationMatrix();

	double dx = translationDesign[0] - translationICP[0];
	double dy = translationDesign[1] - translationICP[1];
	double da = rotationDesign[1] - rotationICP[1];

	result.push_back(dx);
	result.push_back(dy);
	result.push_back(da);

	return result;
}

PndLmdOverlapInfo& PndLmdAlignQA::getSmallOverlapInfo(std::vector<PndLmdOverlapInfo> &infos,
    int smallOverlap) {
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

// ========== QA functions

void PndLmdAlignQA::checkCombined() {

	//TODO: make more general, set paths during setup
	TString misalignedMatrices = "misalignMatrices-SensorsOnly-100u.root";
	TString idealMatrices = "idealMatrices.root";
	std::string path =
	    "/home/arbeit/RedPro3TB/simulationData/2018-05-07-misalign-100u/LMDmatrices-inSensorOne";

	matricesMisaligned = readRootMatrices(misalignedMatrices);

	std::map<int, TGeoHMatrix> matricesByHand;
	std::map<int, TGeoHMatrix> matricesByGeoManager;

	bool compareWithIdeal = true;
	string ext = "cm.mat";

	cout << "getting matrices by hand/ICP-like\n";
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

	// TODO: this must be implemented
	//applyMisalignmentToGGeoManager();
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

	//data holds sets of entries. order is (matrixID, alpha, dx, dy
	std::vector<std::vector<double> > data;

	for (auto &i : matricesByHand) {
		int key = i.first;
		auto thisDiff = getMatrixDiff(matricesByHand[key], matricesByGeoManager[key]);

		std::vector<double> result;
		result.push_back(key);
		result.push_back(thisDiff[2]);  // sin(alpha)
		result.push_back(thisDiff[0]);  // tx
		result.push_back(thisDiff[1]);  // ty
		data.push_back(result);
	}

	//prepare
	string pdfdir = pdfOutPath;
	manager.setMatrixOutDir(path);

	histParams parameters;
	parameters.bins = 20;
	parameters.path = pdfdir + "/residualsCombined/";
	parameters.ytitle = "entries";

	//for DX
	parameters.title = "matrix combined 0-9 CM - Target, #DeltaX";
	parameters.xtitle = "dX [#mum]";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dx.pdf";
	parameters.vectorIndex = 2;
	createHist(data, parameters);

	//for DY
	parameters.title = "matrix combined 0-9 CM - Target, #DeltaY";
	parameters.xtitle = "dY [#mum]";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dy.pdf";
	parameters.vectorIndex = 3;
	createHist(data, parameters);

	//for DAlpha
	parameters.title = "matrix combined 0-9 CM - Target, #Delta#alpha";
	parameters.xtitle = "d#alpha [#murad]";
	parameters.scaleFactor = 1e6;
	parameters.fileName = "dalpha.pdf";
	parameters.vectorIndex = 1;
	createHist(data, parameters);

}

// -------------------------------- old functions --------------------------------

void PndLmdAlignQA::checkCyclicMatrices(bool inCentimeters) {

	if (inCentimeters) {
		_inCentimeters = true;
		manager.setInCentimeters(_inCentimeters);
	}
	else {
		_inCentimeters = false;
		manager.setInCentimeters(_inCentimeters);
	}

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();

	//prepare
	string path = LMDMatPath;
	string pdfdir = pdfOutPath;
	manager.setMatrixOutDir(path);

	Matrix cycle;
	for (int i = 0; i < 400; i++) {

		// does not matter if in LMC local or sensor local, should always be identity matrix!
		//cycle = manager.combineCyclicMatrix(i);

		//store this residual tuple to data
		std::vector<double> result;
		result.push_back(0);
		result.push_back(0);
		result.push_back(cycle.val[0][1]);  // sin(alpha)
		result.push_back(cycle.val[0][3]);  // tx
		result.push_back(cycle.val[1][3]);  // ty
		data.push_back(result);
	}

	histParams parameters;

	parameters.bins = 20;

	//for DX
	parameters.path = pdfdir + "/cyclicChecks/";
	inCentimeters ? parameters.title = "matrixCM cycle check, #DeltaX" : parameters.title =
	                    "matrixPX cycle check, #DeltaX";
	parameters.xtitle = "dX [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dx.pdf";
	parameters.vectorIndex = 3;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = true;
	createHist(data, parameters);

	//for DY
	//parameters.path = pdfdir;
	inCentimeters ? parameters.title = "matrixCM cycle check, #DeltaY" : parameters.title =
	                    "matrixPX cycle check, #DeltaY";
	parameters.xtitle = "dY [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dy.pdf";
	parameters.vectorIndex = 4;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = true;
	createHist(data, parameters);

	//for DAlpha
	//parameters.path = pdfdir;
	inCentimeters ? parameters.title = "matrixCM cycle check, #Delta#alpha" : parameters.title =
	                    "matrixPX cycle check, #Delta#alpha";
	parameters.xtitle = "d#alpha [#murad]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e6;
	parameters.fileName = "dalpha.pdf";
	parameters.vectorIndex = 2;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = true;
	createHist(data, parameters);
}
