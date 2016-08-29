/*
 * PndLmdAlignQA.cxx
 *
 *  Created on: Jul 6, 2015
 *      Author: roman
 */

#include <PndLmdAlignQA.h>

#include <PndLmdDim.h>
#include <TGeoManager.h>

#include <boost/filesystem.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include <PndLmdAlignManager.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGeoMatrix.h>

using std::string;
using std::vector;


PndLmdAlignQA::PndLmdAlignQA(){
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
	_enableHelperMatrix = false;
	curPlane = -1;
	_signerrors = -1;
	dimension = PndLmdDim::Instance();

	//FIXME: maybe don't do this hard-coded
	manager.readTrafoMatrix("/geometry/trafo_matrices_lmd.dat", true);
	manager.readTrafoMatrix("/geometry/trafo_matrices_lmd_misaligned.dat", false);
}

void PndLmdAlignQA::compareMatrices(runParameter param){

	//data holds sets of entries. order is (id1, id2, alpha, dx, dy
	std::vector<std::vector<double> > data;
	readMatrixInfo();

	//iterate over all available id pairs and store them to vector of std::pairs
	vector<std::pair<int, int>> idPairs;
	for(int i = 0; i<400; i+=10){
		idPairs.push_back(make_pair(0+i,5+i));
		idPairs.push_back(make_pair(1+i,8+i));
		idPairs.push_back(make_pair(2+i,8+i));
		idPairs.push_back(make_pair(2+i,9+i));
		idPairs.push_back(make_pair(3+i,6+i));
		idPairs.push_back(make_pair(3+i,7+i));
		idPairs.push_back(make_pair(3+i,8+i));
		idPairs.push_back(make_pair(4+i,7+i));
		idPairs.push_back(make_pair(4+i,9+i));
	}
	//should now contain all available id pairs

	//plot all
	if(param==kNormal){

		//gather data
		cout << "gathering data...\n";
		for(int i=0; i<idPairs.size(); i++){
			histDeltaCorrection(idPairs[i].first, idPairs[i].second, data);
		}

		//now do the case distinction by parameters
		histParams parameters;

		parameters.runParam = param;

		//for DX
		parameters.title = "DeltaX";
		parameters.xtitle = "dX [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dx.pdf";
		parameters.vectorIndex = 3;
		parameters.xMin=-30;
		parameters.xMax=30;
		createHist(data, parameters);


		/*
		//for DY
		parameters.title = "DeltaY";
		parameters.xtitle = "dY [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dyNEWMETHOD.pdf";
		parameters.vectorIndex = 4;
		createHist(data, parameters);

		//for Dalpha
		parameters.title = "DeltaAlpha";
		parameters.xtitle = "d#alpha [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dalphaNEWMETHOD.pdf";
		parameters.vectorIndex = 2;
		createHist(data, parameters);
		 */
	}

	//plot by module
	else if(param == kPlotByModule){

		for(int i=0; i<idPairs.size(); i++){

			//check cases
			int sensorID, half, plane, module, side, die, sensor;
			sensorID = idPairs[i].first;
			dimension->Get_sensor_by_id(sensorID, half, plane, module, side, die, sensor);

			//for(int j=0; j<10; j++){
			//	createThreeHistsVeryDirty(idPairs[i].first, idPairs[i].second, j, data, param);
			//}

		}
		/*
		tDeltaCorrection(0 +iMult*10, 5 +iMult*10, data);
		histDeltaCorrection(1 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(2 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(2 +iMult*10, 9 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 6 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 7 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(4 +iMult*10, 7 +iMult*10, data);
		histDeltaCorrection(4 +iMult*10, 9 +iMult*10, data);

		 */
	}

	else if(param == kPlotMatrixResiduals){

		string pdfdir = "/home/roman/arbeit/fairsoft_mar15/pandaroot/macro/lmd/test/newTest/residualsPlots/";

		for(int i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < 200e3){
				continue;
			}

			//prepare
			string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
			string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
			string path = "/home/arbeit/simulationData/boxtest-misaligned-50u-1.5/binaryPairs/LMDmatrices";
			matrixNameCM = path + matrixNameCM;
			matrixNamePX = path + matrixNamePX;

			//read matrices from disk
			Matrix matrixCM = manager.readMatrix(matrixNameCM);
			Matrix matrixPX = manager.readMatrix(matrixNamePX);

			//invert both
			//matrixCM = Matrix::inv(matrixCM);
			//matrixPX = Matrix::inv(matrixPX);

			Matrix senToSen = manager.getMatrixOfficialGeometry(id1,id2,true);
			matrixCM = matrixCM * senToSen;  //this is now the total matrix from sen1 to sen2

			//matrixCM = Matrix::inv(matrixCM);

			Matrix matrixDif = matrixCM - matrixPX;

			/*
			cout << "after transformation:\n";
			cout << "matrixCM:\n" << matrixCM << "\n\n";
			cout << "matrixPX:\n" << matrixPX << "\n\n";
			cout << "matrix residual:\n" << matrixDif << "\n\n";
			 */

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
		parameters.path = pdfdir;
		parameters.title = "DeltaX";
		parameters.xtitle = "dX [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dx.pdf";
		parameters.vectorIndex = 3;
		parameters.xMin=-1;
		parameters.xMax=-1;
		createHist(data, parameters);

		//for DY
		parameters.path = pdfdir;
		parameters.title = "DeltaY";
		parameters.xtitle = "dY [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dy.pdf";
		parameters.vectorIndex = 4;
		parameters.xMin=-1;
		parameters.xMax=-1;
		createHist(data, parameters);

		//for DAlpha
		parameters.path = pdfdir;
		parameters.title = "Delta#alpha";
		parameters.xtitle = "d#alpha [#murad]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e6;
		parameters.fileName = "dalpha.pdf";
		parameters.vectorIndex = 2;
		parameters.xMin=-1;
		parameters.xMax=-1;
		createHist(data, parameters);
	}

	else if(param == 4){

		//clear console
		manager.clearScreen();

		/*
		cout << "-- debug test --\n";
		cout << "m0to1:\n" << manager.getMatrixOfficialGeometry(0,1, true) << "\n";
		cout << "m0to2:\n" << manager.getMatrixOfficialGeometry(0,2, true) << "\n";
		cout << "m0to3:\n" << manager.getMatrixOfficialGeometry(0,3, true) << "\n";
		cout << "m0to4:\n" << manager.getMatrixOfficialGeometry(0,4, true) << "\n";
		cout << "m5to6:\n" << manager.getMatrixOfficialGeometry(5,6, true) << "\n";
		cout << "m5to7:\n" << manager.getMatrixOfficialGeometry(5,7, true) << "\n";
		cout << "m5to8:\n" << manager.getMatrixOfficialGeometry(5,8, true) << "\n";
		cout << "m5to9:\n" << manager.getMatrixOfficialGeometry(5,9, true) << "\n";
		cout << "-- debug end --\n";
		 */

		int id1 = 20;
		int id2 = 25;

		//prepare
		string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
		string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
		string path = "/home/arbeit/simulationData/boxtest-misaligned-50u-1.5/binaryPairs/LMDmatrices/";
		matrixNameCM = path + matrixNameCM;
		matrixNamePX = path + matrixNamePX;

		//read matrices from disk
		Matrix matrixCM = manager.readMatrix(matrixNameCM);
		Matrix matrixPX = manager.readMatrix(matrixNamePX);

		cout << "before transformation:\n";
		cout << "matrixCM:\n" << matrixCM << "\n\n";
		cout << "matrixPX:\n" << matrixPX << "\n\n";

		//transform matrices
		//matrixPX = manager.transformMatrixFromPixelsToCm(matrixPX);
		Matrix senToSen = manager.getMatrixOfficialGeometry(id1,id2,true);
		matrixCM = matrixCM * senToSen;

		cout << "after transformation:\n";
		cout << "matrixCM:\n" << matrixCM << "\n\n";
		cout << "matrixPX:\n" << matrixPX << "\n\n";
		cout << "matrix residual:\n" << matrixCM - matrixPX << "\n\n";

		/*
		 * Okay the following finally works. Now all work!
		 */
		cout << "\n--------------------------------\n\n";
		cout << "different approach: px - target:\n";
		Matrix real = manager.getMatrixOfficialGeometry(id1, id2,false);
		cout << "official matrix:\n" << real << "\n";
		Matrix icpMatrixNewOne = manager.readMatrix(matrixNamePX);
		manager.transformFromSensorToLmdLocal(icpMatrixNewOne, id1);		//remember, icp matrix is local to sensor!
		cout << "residual:\n" << icpMatrixNewOne - real << "\n";			//return matrix residuals

		/*
		 * Okay the following finally works. Back to the previous ones.
		 */
		cout << "\n--------------------------------\n\n";
		cout << "different approach: cm - target:\n";
		Matrix icpMatrixNewTwo = manager.readMatrix(matrixNameCM);
		Matrix corrSensorToSensor = manager.getCorrectionMatrix(id1, id2);
		cout << icpMatrixNewTwo - corrSensorToSensor << "\n";	//return matrix residuals

		/*
		 * Okay the following finally works. Back to the previous ones.
		 * EDIT: okay I changed the getMatrixOfficial back to the previous version
		 * and it doesn't work again. I am stumped.
		 */
		cout << "\n--------------------------------\n\n";
		cout << "last try, px - cm FROM PNDLMDDIM ONLY:\n";

		Matrix senToSenLast = manager.getMatrixOfficialGeometry(id1,id2,true);
		Matrix corrSenOne = manager.getCorrectionMatrix(id1);
		Matrix corrSenTwo = manager.getCorrectionMatrix(id2);

		Matrix senToSenWithCorrLast = corrSenTwo * senToSenLast * Matrix::inv(corrSenOne);

		Matrix senToSenWithCorrDirectLast = manager.getMatrixOfficialGeometry(id1, id2,false);
		cout << senToSenWithCorrDirectLast - senToSenWithCorrLast << "\n";

		return;
	}

	//The vector now contains ALL overlap matrix parameters

	/*


	if(param==kNormal){
		TH1D DeltaX("DeltaX", "DeltaX", 50,-1,-1);
		TH1D DeltaY("DeltaY", "DeltaY", 50,-1,-1);
		TH1D DeltaAlpha("DeltaAlpha", "DeltaAlpha", 50,-1,-1);

		DeltaX.GetXaxis()->SetTitle("dX [#mum]");
		DeltaX.GetYaxis()->SetTitle("entries");
		DeltaY.GetXaxis()->SetTitle("dY [#mum]");
		DeltaY.GetYaxis()->SetTitle("entries");
		DeltaAlpha.GetXaxis()->SetTitle("d#alpha [#murad]");
		DeltaAlpha.GetYaxis()->SetTitle("entries");

		if(data.size()==0){
			cout << "Error: nothing read! (maybe not enough pairs?) \n";
			exit(1);
		}

		//have all data now
		for(int iArea=0; iArea<data.size();iArea++){
			DeltaAlpha.Fill(data[iArea][2] * 1e6);
			DeltaX.Fill(data[iArea][3] * 1e4);
			DeltaY.Fill(data[iArea][4] * 1e4);
		}

		stringstream pathname;
		pathname << _outputPath;

		_inCentimeters ? pathname << "/inCm/" : pathname << "/inPx/";
		_enableHelperMatrix ? pathname << "corrFull-" : pathname << "";

		PndLmdAlignManager::mkdir(pathname.str());

		TCanvas canvas("canvas", "canvas", 800,600);
		canvas.cd();
		DeltaX.Draw();
		canvas.Print((pathname.str()+"dx.pdf").c_str());
		DeltaY.Draw();
		canvas.Print((pathname.str()+"dy.pdf").c_str());
		DeltaAlpha.Draw();
		canvas.Print((pathname.str()+"dalpha.pdf").c_str());

		return;
	}
	else if(param==kPlotByModule){

	 */



	//}
}

void PndLmdAlignQA::histDeltaCorrection(int id1, int id2, std::vector<std::vector<double> > &vec) {

	if(noOfPairs(id1, id2) < 200e3){
		return;
	}

	//int overlapId = dimension->makeOverlapID(id1, id2);
	//if(overlapId % 10 != 8){
	//	return;
	//}

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	dimension->Get_sensor_by_id(id1, fhalf, fplane, fmodule, fside, fdie, fsensor);

	if(fhalf!=1){
		return;
	}

	if(fmodule != 4){
		return;
	}

	std::vector<double> result;
	Matrix thisMat = getMatrixResiduals(id1, id2);

	result.push_back(id1);
	result.push_back(id2);
	result.push_back(thisMat.val[0][1]);
	result.push_back(thisMat.val[0][3]);
	result.push_back(thisMat.val[1][3]);
	vec.push_back(result);

}

Matrix PndLmdAlignQA::getMatrixResiduals(int id1, int id2) {

	//TODO: works only with ICP matrices, not arbitrary matrices (like combined matrices). but those are interesting too!

	//get matrix file name from PndLmdAlignManager
	string matrixName = _matrixDir + PndLmdAlignManager::makeMatrixFileName(id1, id2, _inCentimeters);

	if(_inCentimeters){
		Matrix icpMatrix = manager.readMatrix(matrixName);
		Matrix corrSensorToSensor = manager.getCorrectionMatrix(id1, id2);

		/*
		 * remember, all pairs were in lmd local, transform matrix from PndLmdDim
		 * (which came in panda global) to lmd local, since ICP matrix will be in
		 * lmd local
		 */
		manager.transformGlobalToLmd(corrSensorToSensor);
		return icpMatrix - corrSensorToSensor;	//return matrix residuals
	}

	// in pixels
	else{
		Matrix real = manager.getMatrixOfficialGeometry(id1, id2,false);
		Matrix icpMatrix = manager.readMatrix(matrixName);
		return icpMatrix - real;				//return matrix residuals
	}

	//default action, if all else fails.
	return Matrix::eye(4);
}

void PndLmdAlignQA::readMatrixInfo() {

	string filename = _matrixDir + "/info.txt";
	stringstream *info = manager.readFile(filename);

	//parser: first, find line aligenr n (n is overlap id)
	// then find no of pairs: x
	// save to map: n->x

	string line;
	std::vector<string> values;
	int overlapid,noPairs;
	bool alignerComplete=true;

	while(std::getline(*info, line)){
		if(line.find("aligner") != std::string::npos && alignerComplete){
			//cout << "found aligner line\n";
			values = manager.findRegex(line, "aligner (\\d{1,4})");
			if(values.size()>1){
				overlapid = boost::lexical_cast<int>(values[1]);
				//cout << "aligner: " << overlapid << endl;
				alignerComplete=false;
			}
		}
		else if(line.find("no of pairs") != std::string::npos){
			//cout << "found pairs line\n";
			values = manager.findRegex(line, "no of pairs. (\\d{1,6})");
			if(values.size()>1){
				noPairs = boost::lexical_cast<int>(values[1]);
				matrixInfo[overlapid]=noPairs;
				alignerComplete=true;
			}
		}
	}
}

int PndLmdAlignQA::noOfPairs(int id1, int id2) {
	int overlapId = dimension->makeOverlapID(id1, id2);
	return matrixInfo[overlapId];
}

bool PndLmdAlignQA::checkForMatrixFiles(){

	//list all IDs that SHOULD be there
	vector<int> availableIds = dimension->getAvailableOverlapIDs();

	vector<string> files;
	manager.searchFiles(_matrixDir, files, "mat", false);
	int foundFiles=0;

	//no matrix files at all!
	if(files.size()==0){
		return false;
	}

	string matrixName;
	bool tempfilefound=false;

	//check for every ID that should be there if there is a corresponding file
	for(int i=0; i<availableIds.size(); i++){

		//reset counter
		tempfilefound=false;
		matrixName = manager.makeMatrixFileName(availableIds[i], _inCentimeters, false);

		for(int j=0; j<files.size(); j++){
			if(files[j].find(matrixName)!=string::npos){
				tempfilefound=true;
				foundFiles++;
			}
		}
		//file not found? return false
		if(!tempfilefound){
			return tempfilefound;
		}
	}
	// if no file could not be found, everything is okay
	return true;
}

void PndLmdAlignQA::createHist(std::vector<std::vector<double> >& vec, histParams &parameters) {

	TH1D histogram(parameters.title.c_str(), parameters.title.c_str(), 50, parameters.xMin, parameters.xMax);

	histogram.GetXaxis()->SetTitle(parameters.xtitle.c_str());
	histogram.GetYaxis()->SetTitle(parameters.ytitle.c_str());

	if(vec.size()==0){
		cout << "Error: nothing read! (maybe not enough pairs?) \n";
		exit(1);
	}

	//have all data now
	for(int iArea=0; iArea<vec.size();iArea++){
		histogram.Fill(vec[iArea][parameters.vectorIndex] * parameters.scaleFactor);
	}

	stringstream pathname;
	//pathname << _outputPath;
	pathname << parameters.path;

	_inCentimeters ? pathname << "/inCm/" : pathname << "/inPx/";
	_enableHelperMatrix ? pathname << "corrFull-" : pathname << "";

	PndLmdAlignManager::mkdir(pathname.str());

	TCanvas canvas("canvas", "canvas", 800,600);
	canvas.cd();
	histogram.Draw();
	canvas.Print((pathname.str()+parameters.fileName).c_str());
}

void PndLmdAlignQA::createThreeHistsVeryDirty(int id1, int id2, int module,	std::vector<std::vector<double> >& data, runParameter param) {

	//check cases
	int fSensorID, fhalf, fplane, fmodule, side, die, sensor;
	fSensorID = id1;
	dimension->Get_sensor_by_id(fSensorID, fhalf, fplane, fmodule, side, die, sensor);
	if(module == fmodule){
		histDeltaCorrection(id1, id2, data);
	}

	//now do the case distinction by parameters
	histParams parameters;

	parameters.runParam = param;

	//for DX
	parameters.title = "DeltaX";
	parameters.xtitle = "dX [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dx" + std::to_string(module) +  ".pdf";
	parameters.vectorIndex = 3;
	createHist(data, parameters);

	//for DY
	parameters.title = "DeltaY";
	parameters.xtitle = "dY [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dy" + std::to_string(module) +  ".pdf";
	parameters.vectorIndex = 4;
	createHist(data, parameters);

	//for Dalpha
	parameters.title = "DeltaAlpha";
	parameters.xtitle = "d#alpha [#mum]";
	parameters.ytitle = "entries";
	parameters.scaleFactor = 1e4;
	parameters.fileName = "dalpha" + std::to_string(module) +  ".pdf";
	parameters.vectorIndex = 2;
	createHist(data, parameters);

}

void PndLmdAlignQA::compareCombinedMatrices() {

	cout << "quick and dirty!\n";
	bool geometryAligned = false;

	manager.setMatrixOutDir(_matrixDir);
	manager.setInCentimeters(false);

	cout << "================= m02\n";

	Matrix m02t = manager.getMatrixOfficialGeometry(0,2,geometryAligned);
	cout << "target: \n" << m02t << "\n";
	Matrix m02i = manager.combineMatrix(0,2);
	cout << "icp: \n" << m02i << "\n";
	cout << "diff: \n" << (m02t - m02i) << "\n";
	cout << "================= m03\n";

	Matrix m03t = manager.getMatrixOfficialGeometry(0,3,geometryAligned);
	Matrix m03i = manager.combineMatrix(0,3);
	cout << (m03t - m03i) << "\n";

	cout << "================= m04\n";

	Matrix m04t = manager.getMatrixOfficialGeometry(0,4,geometryAligned);
	Matrix m04i = manager.combineMatrix(0,4);
	cout << (m04t - m04i) << "\n";

	cout << "================= m05\n";

	Matrix m05t = manager.getMatrixOfficialGeometry(0,5,geometryAligned);
	Matrix m05i = manager.combineMatrix(0,5);
	cout << (m05t - m05i) << "\n";

	cout << "================= m06\n";

	Matrix m06t = manager.getMatrixOfficialGeometry(0,6,geometryAligned);
	Matrix m06i = manager.combineMatrix(0,6);
	cout << (m06t - m06i) << "\n";

	cout << "================= m07\n";

	Matrix m07t = manager.getMatrixOfficialGeometry(0,7,geometryAligned);
	Matrix m07i = manager.combineMatrix(0,7);
	cout << (m07t - m07i) << "\n";

	cout << "================= m08\n";

	Matrix m08t = manager.getMatrixOfficialGeometry(0,8,geometryAligned);
	Matrix m08i = manager.combineMatrix(0,8);
	cout << (m08t - m08i) << "\n";

	cout << "================= m09\n";

	Matrix m09t = manager.getMatrixOfficialGeometry(0,9,geometryAligned);
	Matrix m09i = manager.combineMatrix(0,9);
	cout << (m09t - m09i) << "\n";

}
