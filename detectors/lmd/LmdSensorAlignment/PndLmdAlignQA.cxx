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

void PndLmdAlignQA::calculateOverlapingAreas() {

	//get all available overlapping areas

	cout << "------------ TESTING --------------\n";
	cout << "\\AtoB{4}{9} & " << calculateOverlappingArea(4, 9, false) << "\n";
	cout << "\\AtoB{0}{1} & " << calculateOverlappingArea(0, 1, false) << "\n";
	cout << "\\AtoB{0}{2} & " << calculateOverlappingArea(0, 2, false) << "\n";
	cout << "\\AtoB{0}{9} & " << calculateOverlappingArea(0, 9, false) << "\n";
	cout << "------------  DONE --------------\n";

	vector<int> overlapIDs = helper->getAvailableOverlapIDs();
	int id1, id2;
	double areaPercent = 0;

	//get id1 and id2 from them and calc
	for (auto i = 0; i < overlapIDs.size(); i++) {
		//id1 = helper->getID1fromOverlapID(overlapIDs[i]);
		//id2 = helper->getID2fromOverlapID(overlapIDs[i]);

		areaPercent = calculateOverlappingArea(id1, id2, false);
		cout << "\\AtoB{" << id1 << "}{" << id2 << "} & " << areaPercent << "\n";
	}
	exit(0);

}

void PndLmdAlignQA::plotCMvsPXmatrices() {

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();

	//iterate over all available id pairs and store them to vector of std::pairs
	vector<std::pair<int, int>> idPairs;
	for (int i = 0; i < 400; i += 10) {
		idPairs.push_back(make_pair(0 + i, 5 + i));
		idPairs.push_back(make_pair(1 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 9 + i));
		idPairs.push_back(make_pair(3 + i, 6 + i));
		idPairs.push_back(make_pair(3 + i, 7 + i));
		idPairs.push_back(make_pair(3 + i, 8 + i));
		idPairs.push_back(make_pair(4 + i, 7 + i));
		idPairs.push_back(make_pair(4 + i, 9 + i));
	}
	//should now contain all available id pairs

	string pdfdir = pdfOutPath;

	for (size_t i = 0; i < idPairs.size(); i++) {

		int id1 = idPairs[i].first;
		int id2 = idPairs[i].second;

		//only select overlap areas with more than 2e5 pairs
		int overlapID = helper->getOverlapIdFromSensorIDs(id1, id2);
		if (matrixInfo[overlapID] < pairsRequired) {
			continue;
		}

		//prepare
		string matrixNameCM = manager.makeMatrixFileName(overlapID, true);
		string matrixNamePX = manager.makeMatrixFileName(overlapID, false);
		string path = LMDMatPath;
		matrixNameCM = path + matrixNameCM;
		matrixNamePX = path + matrixNamePX;

		//read matrices from disk
		Matrix matrixCM = manager.readMatrix(matrixNameCM);
		Matrix matrixPX = manager.readMatrix(matrixNamePX);
		Matrix matrixDif;

		if (false) { //transform both to lmd local and compare there
			//transform matrixPX to lmd local
			//manager.transformFromSensorToLmdLocal(matrixPX, id1);
			Matrix senToSen = manager.getMatrixSensorToSensor(id1, id2);
			Matrix senToSenWithCorr = matrixCM * senToSen; //this is now the total matrix from sen1 to sen2
			matrixDif = senToSenWithCorr - matrixPX;
		} else { //transform noth to sensor and compare there
		         //transform matrixPX to lmd local
		         //manager.transformFromSensorToLmdLocal(matrixPX, id1);
			//Matrix senToSen = manager.getMatrixOfficialGeometry(overlapID, true);
			//Matrix senToSenWithCorr = matrixCM * senToSen; //this is now the total matrix from sen1 to sen2

			//manager.transformFromLmdLocalToSensor(senToSenWithCorr, id1);

			//matrixDif = senToSenWithCorr - matrixPX;
		}

		//store this residual tuple to data
		std::vector<double> result;
		result.push_back(id1);
		result.push_back(id2);
		result.push_back(matrixDif.val[0][1]);		// sin(alpha)
		result.push_back(matrixDif.val[0][3]);		// tx
		result.push_back(matrixDif.val[1][3]);		// ty
		data.push_back(result);
	}

	histParams parameters;

	//for DX
	parameters.path = pdfdir + "/PXvsCM/";
	parameters.title = "matrixCM*matrixTarget - matrixPX(transformed), #DeltaX)";
	parameters.xtitle = "dX [nm]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4 * 1e3;
	parameters.fileName = "dx.pdf";
	parameters.vectorIndex = 3;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

	//for DY
	//parameters.path = pdfdir;
	parameters.title = "matrixCM*matrixTarget - matrixPX(transformed), #DeltaY)";
	parameters.xtitle = "dY [nm]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4 * 1e3;
	parameters.fileName = "dy.pdf";
	parameters.vectorIndex = 4;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

	//for DAlpha
	//parameters.path = pdfdir;
	parameters.title = "matrixCM*matrixTarget - matrixPX(transformed), #Delta#alpha";
	parameters.xtitle = "d#alpha [nrad]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e9;
	parameters.fileName = "dalpha.pdf";
	parameters.vectorIndex = 2;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

}

void PndLmdAlignQA::calculatePixelDistancesFrontToBack() {
	histPixelDistances(0, 5);
	histPixelDistances(1, 8);
	histPixelDistances(2, 8);
	histPixelDistances(2, 9);
	histPixelDistances(3, 6);
	histPixelDistances(3, 7);
	histPixelDistances(3, 8);
	histPixelDistances(4, 7);
	histPixelDistances(4, 9);
}

void PndLmdAlignQA::checkCombinedMatrices(bool inCentimeters) {

	if (inCentimeters) {
		_inCentimeters = true;
		manager.setInCentimeters(_inCentimeters);
	} else {
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

	for (auto id1 = 0; id1 < 400; id1 += 10) {

		for (auto id2 = id1 + 1; id2 < id1 + 10; id2++) {

			Matrix matrixDif;
			if (inCentimeters) {
				Matrix mCombined = manager.combineMatrix(id1, id2);
				//Matrix senToSenOneStep = manager.getMatrixOfficialGeometry(id1, id2);
				//matrixDif = mCombined - senToSenOneStep;
			} else {
				Matrix mCombined = manager.combineMatrix(id1, id2);
				//manager.transformFromSensorToLmdLocal(mCombined, id1);
				//Matrix senToSenOneStep = manager.getMatrixOfficialGeometry(id1, id2);
				//matrixDif = mCombined - senToSenOneStep;
			}

			//store this residual tuple to data
			std::vector<double> result;
			result.push_back(id1);
			result.push_back(id2);
			result.push_back(matrixDif.val[0][1]); // sin(alpha)
			result.push_back(matrixDif.val[0][3]); // tx
			result.push_back(matrixDif.val[1][3]); // ty
			//result.push_back(matrixPX.val[2][3]);  // tz
			data.push_back(result);
		}
	}

	histParams parameters;
	parameters.bins = 20;

	//for DX
	parameters.path = pdfdir + "/residualsCombined/";
	inCentimeters ? parameters.title = "matrix combined 0-9 CM - Target, #DeltaX" : parameters.title =
	                        "matrix combined 0-9 PX - Target, #DeltaX";
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
	inCentimeters ? parameters.title = "matrix combined 0-9 CM - Target, #DeltaY" : parameters.title =
	                        "matrix combined 0-9 PX - Target, #DeltaY";
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
	inCentimeters ?
	        parameters.title = "matrix combined 0-9 CM - Target, #Delta#alpha" : parameters.title =
	                "matrix combined 0-9 PX - Target, #Delta#alpha";
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

void PndLmdAlignQA::checkCyclicMatrices(bool inCentimeters) {

	if (inCentimeters) {
		_inCentimeters = true;
		manager.setInCentimeters(_inCentimeters);
	} else {
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
		cycle = manager.combineCyclicMatrix(i);

		//store this residual tuple to data
		std::vector<double> result;
		result.push_back(0);
		result.push_back(0);
		result.push_back(cycle.val[0][1]); // sin(alpha)
		result.push_back(cycle.val[0][3]); // tx
		result.push_back(cycle.val[1][3]); // ty
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

void PndLmdAlignQA::histPixelDistances(int sensor1, int sensor2, bool aligned) {

	int sensorID1 = sensor1, sensorID2 = sensor2;

	Matrix PXtoCM = manager.getPixelToCentimeterTransformation();
	Matrix CMtoPX = Matrix::inv(PXtoCM);
	//Matrix sen1ToSen2 = manager.getMatrixOfficialGeometry(sensorID1, sensorID2, aligned);
	//manager.transformFromLmdLocalToSensor(sen1ToSen2, sensorID1, aligned);
	//Matrix matSensorOneToSensorTwo = CMtoPX * sen1ToSen2 * PXtoCM;

	//histogram those distances
	double colTest2 = 0, rowTest2 = 0;

	TH1D hist("Pixel Distances", "Pixel Distances", 300, -1, -1);

	int valid = 0;

	//for every sensor pair on ONE module (all modules are equal, none are more equal than others)

	//for every pixel (250*250) find all 4 overlap pixels (cutoff at 100 microns, pixels can't be that far away on perfect geometry)
	for (int colTest1 = 0; colTest1 < 250; colTest1++) {
		for (int rowTest1 = 0; rowTest1 < 250; rowTest1++) {

			//create hit on sensor1 and compute corresponding hit on sensor2
			//Matrix hit1 = manager.makeFourVector(colTest1, rowTest1, 0);
			//Matrix hit2 = matSensorOneToSensorTwo * hit1;
//			colTest2 = hit2.val[0][0];
//			rowTest2 = hit2.val[1][0];

			//also, check if overlap pixel even exists. must be row elem [0,250], col elem [0,250]
			//are we still overlapping area?
			if (colTest2 < 0 || colTest2 > 247) {
				continue;
			}
			if (rowTest2 < 0 || rowTest2 > 242) {
				continue;
			}

			//count for statistics
			valid++;

			//four possible neighbors
			double colTest2Floor = floor(colTest2);
			double colTest2Ceil = ceil(colTest2);
			double rowTest2Floor = floor(rowTest2);
			double rowTest2Ceil = ceil(rowTest2);

			//make all possible hit2's:
			vector<Matrix> hit2Candidates;
//			hit2Candidates.push_back(manager.makeFourVector(colTest2Floor, rowTest2Floor, -0.025));
//			hit2Candidates.push_back(manager.makeFourVector(colTest2Ceil, rowTest2Floor, -0.025));
//			hit2Candidates.push_back(manager.makeFourVector(colTest2Floor, rowTest2Ceil, -0.025));
//			hit2Candidates.push_back(manager.makeFourVector(colTest2Ceil, rowTest2Ceil, -0.025));

			//OR use matrices directly. actually, maybe use matrix directly

			//now, transform everything to cm in sensor coordinate system
			//hit1 = PXtoCM * hit1;

			//cout << "=== start ===\n";
			for (size_t i = 0; i < hit2Candidates.size(); i++) {
				double distance;
				//hit2Candidates[i] = PXtoCM * Matrix::inv(matSensorOneToSensorTwo) * hit2Candidates[i];
				//distance = (hit1 - hit2Candidates[i]).l2norm() * 1e4;		//convert to microns

				//if(distance > 100){
				//continue;
				//}
				//else{
				//fill already
				//cout << "distance: " << distance << "\n";
				hist.Fill(distance);
				//}
			}
		}
	}
	double coverage = (double) valid / (250.0 * 250.0) * 100;
	cout << "overlap: " << coverage << "\n";

	std::stringstream ss;
	ss << "Pixel Distances Area " << sensorID1 << " to " << sensorID2 << ", " << coverage
	        << "% coverage.";
	hist.SetTitle(ss.str().c_str());
	ss.str("");
	ss << "d [#mum]";
	hist.GetXaxis()->SetTitle(ss.str().c_str());
	ss.str("");
	ss << "Entries";
	hist.GetYaxis()->SetTitle(ss.str().c_str());

	std::stringstream filename;
	filename << pdfOutPath << "/AreaPixelDistances" << sensor1 << "to" << sensor2 << ".pdf";
	TCanvas canvas;
	canvas.cd();
	hist.Draw();
	canvas.Print(filename.str().c_str());
}

double PndLmdAlignQA::calculateOverlappingArea(int sensor1, int sensor2, bool aligned) {

	int sensorID1 = sensor1, sensorID2 = sensor2;

	Matrix PXtoCM = manager.getPixelToCentimeterTransformation();
	Matrix CMtoPX = Matrix::inv(PXtoCM);
	//Matrix sen1ToSen2 = manager.getMatrixOfficialGeometry(sensorID1, sensorID2, aligned);
	//manager.transformFromLmdLocalToSensor(sen1ToSen2, sensorID1, aligned);
	//Matrix matSensorOneToSensorTwo = CMtoPX * sen1ToSen2 * PXtoCM;

	//Matrix sen2toSen1 = PXtoCM * Matrix::inv(matSensorOneToSensorTwo);

	//histogram those distances
	double colTest2 = 0, rowTest2 = 0;
	int valid = 0;

	//for every pixel (250*250) find all 4 overlap pixels (cutoff at 100 microns, pixels can't be that far away on perfect geometry)
	for (int colTest1 = 0; colTest1 < 247; colTest1++) {
		for (int rowTest1 = 0; rowTest1 < 242; rowTest1++) {

			//create hit on sensor1 and compute corresponding hit on sensor2
			//Matrix hit1 = manager.makeFourVector(colTest1, rowTest1, 0);
			//Matrix hit2 = matSensorOneToSensorTwo * hit1;
			//colTest2 = hit2.val[0][0];
			//rowTest2 = hit2.val[1][0];

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

void PndLmdAlignQA::readMatrixInfo() {

	string filename;
	if (_inCentimeters) {
		filename = LMDMatPath + "/info-cm.txt";
	} else {
		filename = LMDMatPath + "/info-px.txt";
	}

	std::stringstream *info = manager.readFile(filename);

	string line;
	std::vector<string> values;
	int overlapid, noPairs;
	bool alignerComplete = true;

	while (std::getline(*info, line)) {
		if (line.find("aligner") != std::string::npos && alignerComplete) {
			//cout << "found aligner line\n";
			values = manager.findRegex(line, "aligner (\\d{1,4})");
			if (values.size() > 1) {
				overlapid = std::stoi(values[1]);
				//cout << "aligner: " << overlapid << endl;
				alignerComplete = false;
			}
		} else if (line.find("no of pairs") != std::string::npos) {
			//cout << "found pairs line\n";
			values = manager.findRegex(line, "no of pairs. (\\d{1,6})");
			if (values.size() > 1) {
				noPairs = std::stoi(values[1]);
				matrixInfo[overlapid] = noPairs;
				alignerComplete = true;
			}
		}
	}
	delete info;
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

void PndLmdAlignQA::createHist(std::vector<std::vector<double> >& vec, histParams &parameters) {

	TH1D histogram(parameters.title.c_str(), parameters.title.c_str(), parameters.bins, parameters.xMin,
	        parameters.xMax);

	histogram.GetXaxis()->SetTitle(parameters.xtitle.c_str());
	histogram.GetYaxis()->SetTitle(parameters.ytitle.c_str());

	if (vec.size() == 0) {
		cout << "Error: nothing read, data empty! (maybe not enough pairs?) \n";
		exit(1);
	}

	//have all data now
	double dataPoint = 0.0;
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

void PndLmdAlignQA::plotPXvsCMmatricesResiduals() {

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();

	//iterate over all available id pairs and store them to vector of std::pairs
	vector<std::pair<int, int>> idPairs;
	for (int i = 0; i < 400; i += 10) {
		idPairs.push_back(make_pair(0 + i, 5 + i));
		idPairs.push_back(make_pair(1 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 9 + i));
		idPairs.push_back(make_pair(3 + i, 6 + i));
		idPairs.push_back(make_pair(3 + i, 7 + i));
		idPairs.push_back(make_pair(3 + i, 8 + i));
		idPairs.push_back(make_pair(4 + i, 7 + i));
		idPairs.push_back(make_pair(4 + i, 9 + i));
	}
	//should now contain all available id pairs

	//prepare some stuff for aligned/misaligned cases
	string pdfdir = pdfOutPath;

	//remember, we want the difference between two residuals
	// (matrixPX-matrixTarget) - (matrixCM-matrixtarget)

	// prepare data sets
	std::vector<std::vector<double> > dataCM;
	std::vector<std::vector<double> > dataPX;

	//so, gather all CMresiduals
	for (size_t i = 0; i < idPairs.size(); i++) {

		int id1 = idPairs[i].first;
		int id2 = idPairs[i].second;

		//only select overlap areas with more than 2e5 pairs
		int overlapID = helper->getOverlapIdFromSensorIDs(id1, id2);
		if (matrixInfo[overlapID] < pairsRequired) {
			continue;
		}

		//prepare
		string matrixNameCM = manager.makeMatrixFileName(overlapID, true);
		string path = LMDMatPath;
		matrixNameCM = path + matrixNameCM;

		//read matrices from disk
		Matrix matrixCM = manager.readMatrix(matrixNameCM);

		//the aligned case is special because the correction matrix is the identity
		Matrix senToSenCorrTarget;
		if (alignOptionBool) {
			senToSenCorrTarget = Matrix::eye(4);
		} else {
			//senToSenCorrTarget = manager.getCorrectionMatrix(overlapID);
		}

		//transform both correction matrices to full sensor to sensor matrices
		//Matrix senToSenIdeal = manager.getMatrixSensorToSensor(overlapID)

		//Matrix matrixDif = (matrixCM * senToSenIdeal) - (senToSenCorrTarget * senToSenIdeal);

		//store this residual tuple to data
		std::vector<double> resultCM;
		resultCM.push_back(id1);
		resultCM.push_back(id2);
//		resultCM.push_back(matrixDif.val[0][1]);		// sin(alpha)
//		resultCM.push_back(matrixDif.val[0][3]);		// tx
//		resultCM.push_back(matrixDif.val[1][3]);		// ty
		dataCM.push_back(resultCM);
	}

	//gather all PX residuals
	for (size_t i = 0; i < idPairs.size(); i++) {

		int id1 = idPairs[i].first;
		int id2 = idPairs[i].second;

		//only select overlap areas with more than 2e5 pairs
		int overlapID = helper->getOverlapIdFromSensorIDs(id1, id2);
		if (matrixInfo[overlapID] < pairsRequired) {
			continue;
		}

		//prepare
		//Matrix target = manager.getMatrixSensorToSensor(overlapID) //true = aligned, false = misaligned
		string matrixNamePX = manager.makeMatrixFileName(overlapID, false);
		string path = LMDMatPath;
		matrixNamePX = path + matrixNamePX;

		//read matrices from disk
		Matrix matrixPX = manager.readMatrix(matrixNamePX);
		//manager.transformFromSensorToLmdLocal(matrixPX, id1, alignOptionBool); // false = misaligned geometry
		//Matrix matrixDif = matrixPX - target;

		//store this residual tuple to data
		std::vector<double> resultPX;
//		resultPX.push_back(id1);
//		resultPX.push_back(id2);
//		resultPX.push_back(matrixDif.val[0][1]);		// sin(alpha)
//		resultPX.push_back(matrixDif.val[0][3]);		// tx
//		resultPX.push_back(matrixDif.val[1][3]);		// ty
		dataPX.push_back(resultPX);
	}

	//we now have two data sets, dataCM and dataPX. combine!
	if (dataCM.size() != dataPX.size()) {
		cout << "ERROR. the two data sets are not equally large! exiting!\n";
		exit(1);
	}

	data.reserve(dataCM.size());

	for (size_t i = 0; i < dataCM.size(); i++) {

		//check if ids match
		if ((dataCM[i][0] != dataPX[i][0]) || (dataCM[i][1] != dataPX[i][1])) {
			cout << "ERROR. vectors are not in the same order!\n";
		}

		std::vector<double> interimData;
		interimData.push_back(dataCM[i][0]);
		interimData.push_back(dataCM[i][1]);
		interimData.push_back(dataPX[i][2] - dataCM[i][2]);		// sin(alpha)
		interimData.push_back(dataPX[i][3] - dataCM[i][3]);		// tx
		interimData.push_back(dataPX[i][4] - dataCM[i][4]);		// ty

		data.push_back(interimData);
	}

	//plot difference
	histParams parameters;
	//parameters.bins=25;

	//for DX
	parameters.path = pdfOutPath + "/PXvsCMresiduals/";
	parameters.title = "PXresiduals - CMresiduals, #DeltaX (0u)";
	parameters.xtitle = "dX [nm]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e7;
	parameters.fileName = "dx.pdf";
	parameters.vectorIndex = 3;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

	//for DY
	//parameters.path = pdfOutPath;
	parameters.title = "PXresiduals - CMresiduals, #DeltaY (0u)";
	parameters.xtitle = "dY [nm]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e7;
	parameters.fileName = "dy.pdf";
	parameters.vectorIndex = 4;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

	//for DAlpha
	//parameters.path = pdfOutPath;
	parameters.title = "PXresiduals - CMresiduals, #Delta#alpha (0u)";
	parameters.xtitle = "d#alpha [#murad]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e6;
	parameters.fileName = "dalpha.pdf";
	parameters.vectorIndex = 2;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = false;
	createHist(data, parameters);

}

void PndLmdAlignQA::plotMatrixresiduals(bool inCentimeters) {

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();

	//iterate over all available id pairs and store them to vector of std::pairs
	vector<std::pair<int, int>> idPairs;
	for (int i = 0; i < 400; i += 10) {
		idPairs.push_back(make_pair(0 + i, 5 + i));
		idPairs.push_back(make_pair(1 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 8 + i));
		idPairs.push_back(make_pair(2 + i, 9 + i));
		idPairs.push_back(make_pair(3 + i, 6 + i));
		idPairs.push_back(make_pair(3 + i, 7 + i));
		idPairs.push_back(make_pair(3 + i, 8 + i));
		idPairs.push_back(make_pair(4 + i, 7 + i));
		idPairs.push_back(make_pair(4 + i, 9 + i));
	}
	//should now contain all available id pairs

	//prepare global settings
	string pdfdir = pdfOutPath;
	_inCentimeters = inCentimeters;
	manager.setInCentimeters(inCentimeters);
	Matrix matrixDif;

	for (size_t i = 0; i < idPairs.size(); i++) {

		int id1 = idPairs[i].first;
		int id2 = idPairs[i].second;

		//only select overlap areas with more than 2e5 pairs
		int overlapID = helper->getOverlapIdFromSensorIDs(id1, id2);
		if (matrixInfo[overlapID] < pairsRequired) {
			continue;
		}

		//in CM
		if (inCentimeters) {
			//prepare
			string matrixNameCM = manager.makeMatrixFileName(overlapID, true);
			string path = LMDMatPath;
			matrixNameCM = path + matrixNameCM;

			//read matrices from disk
			Matrix matrixCM = manager.readMatrix(matrixNameCM);

			//transform both correction matrices to full sensor to sensor matrices
			Matrix senToSenIdeal = manager.getMatrixSensorToSensor(id1, id2);

			if (false) {	//this one uses get Correction Matrix, tested and works

				Matrix senToSenCorrTarget;
				if (alignOptionBool) {
					senToSenCorrTarget = Matrix::eye(4);
				} else {
					//senToSenCorrTarget = manager.getCorrectionMatrix(id1, id2);
				}
				matrixDif = (matrixCM * senToSenIdeal) - (senToSenCorrTarget * senToSenIdeal);
			}

			else {		//this one uses the complete matrix and works as well

				Matrix senToSen = manager.getMatrixSensorToSensor(id1, id2);

				//next try, I think I'm onto it...
				//manager.transformFromLmdLocalToSensor(senToSenIdeal, id1, true);// THIS IS THE MAGIC BEAN
				//manager.transformFromSensorToLmdLocal(senToSenIdeal, id1, alignOptionBool);

				Matrix ICPcomplete = matrixCM * senToSenIdeal;

				matrixDif = ICPcomplete - senToSen;
			}
		}
		// in PX
		else {
			//prepare
			//Matrix target = manager.getMatrixSensorToSensor(overlapID) //true = aligned, false = misaligned

			string matrixNamePX = manager.makeMatrixFileName(overlapID, false);
			string path = LMDMatPath;
			matrixNamePX = path + matrixNamePX;

			//read matrices from disk
			Matrix matrixPX = manager.readMatrix(matrixNamePX);
			//manager.transformFromSensorToLmdLocal(matrixPX, id1, alignOptionBool); // false = misaligned geometry

			//matrixDif = matrixPX - target;
		}
		//store this residual tuple to data
		std::vector<double> result;
		result.push_back(id1);
		result.push_back(id2);
		result.push_back(matrixDif.val[0][1]);		// sin(alpha)
		result.push_back(matrixDif.val[0][3]);		// tx
		result.push_back(matrixDif.val[1][3]);		// ty
		result.push_back(matrixDif.val[2][3]);		// tz
		data.push_back(result);
	}

	histParams parameters;

	//for DX
	parameters.path = pdfdir + "/residuals/";
	if (inCentimeters) {
		parameters.title = "matrixCM - correctionTarget, #DeltaX";
	} else {
		parameters.title = "matrixPX(transformed) - senToSenTarget, #DeltaX";
	}
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
	if (inCentimeters) {
		parameters.title = "matrixCM - correctionTarget, #DeltaY";
	} else {
		parameters.title = "matrixPX(transformed) - senToSenTarget, #DeltaY";
	}
	parameters.xtitle = "dY [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dy.pdf";
	parameters.vectorIndex = 4;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = true;
	createHist(data, parameters);

	//for DZ
	//parameters.path = pdfdir;
	if (inCentimeters) {
		parameters.title = "#DeltaZ of HitPairs";
	} else {
		parameters.title = "#DeltaZ of HitPairs";
	}
	parameters.xtitle = "dZ [pair Steps]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1.0;
	parameters.fileName = "dz.pdf";
	parameters.vectorIndex = 5;
	parameters.xMin = -1;
	parameters.xMax = -1;
	parameters.printCMPXinPathName = true;
	createHist(data, parameters);

	//for DAlpha
	//parameters.path = pdfdir;
	if (inCentimeters) {
		parameters.title = "matrixCM - correctionTarget, #Delta#alpha";
	} else {
		parameters.title = "matrixPX(transformed) - senToSenTarget, #Delta#alpha";
	}
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

void PndLmdAlignQA::histogramPairDistances() {

	/*FIXME: this is bullshit. it reads all 360 binary pair files, each 28 MB and consumes
	 * 10 GB of RAM while doing mostly nothing.
	 * Change this to o the following:
	 *
	 * create one aligner
	 * have aligner read binary pair file
	 * read pairs and create histogram
	 * save histogram
	 *
	 * and this function can me multi-threaded to 4-8 threads.
	 */

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();
	string pdfdir = pdfOutPath;

	//reset the manager
	manager.init();

	// make manager read binary pair files
	manager.setInCentimeters(true);
	manager.setBinaryPairFileDirectory(binaryMatPath);

	bool allpresent = manager.checkForBinaryFiles();

	if (!allpresent) {
		cout << "Warning: Not all binary pair files are present. Aborting this run.\n";
		return;
	}

	// for every pair file, read all pairs (CM should suffice)
	manager.readPairsFromBinaryFiles();

	double x1, y1, x2, y2, distance;

	// the PairFinders now have all the binary pair loaded, ask them! (they are friends)
	for (auto &aligner : manager.aligners) {
		PndLmdSensorAligner &thisAligner = aligner.second;
		int overlapId = thisAligner.overlapID;
		int noOfPairs = thisAligner.numberOfPairs;

		for (int i = 0; i < noOfPairs; i++) {
			x1 = thisAligner.simpleSensorOneX[i];
			y1 = thisAligner.simpleSensorOneY[i];
			x2 = thisAligner.simpleSensorTwoX[i];
			y2 = thisAligner.simpleSensorTwoY[i];

			distance = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
			vector<double> datum;
			datum.push_back(distance);
			data.push_back(datum);
		}

		histParams parameters;

		//for distance
		parameters.path = pdfdir + "/distHistograms/";
		std::stringstream titless;
		titless << "pair distance on area " << overlapId;
		parameters.title = titless.str();
		parameters.xtitle = "d [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		std::stringstream filenamess;
		filenamess << overlapId << ".pdf";
		parameters.fileName = filenamess.str();
		parameters.vectorIndex = 0;
		parameters.xMin = -1;
		parameters.xMax = -1;
		parameters.printCMPXinPathName = false;
		createHist(data, parameters);

		// clean up for next round
		data.clear();
	}
}

