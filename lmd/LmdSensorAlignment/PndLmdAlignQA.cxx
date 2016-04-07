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
	// TODO Auto-generated destructor stub
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

void PndLmdAlignQA::compareMatrices(){

	std::vector<std::vector<double> > data;
	readMatrixInfo();

	for(int iMult=0; iMult < 40; iMult++){
		histDeltaCorrection(0 +iMult*10, 5 +iMult*10, data);
		histDeltaCorrection(1 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(2 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(2 +iMult*10, 9 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 6 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 7 +iMult*10, data);
		histDeltaCorrection(3 +iMult*10, 8 +iMult*10, data);
		histDeltaCorrection(4 +iMult*10, 7 +iMult*10, data);
		histDeltaCorrection(4 +iMult*10, 9 +iMult*10, data);
	}

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

		//TODO: delete
		//if(data[iArea][3]*1e4 < 1e2){
		//	cout << "data is: " << data[iArea][3]*1e4 << " on area " << data[iArea][0] << " to " << data[iArea][1] << "\n";
		//}
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

void PndLmdAlignQA::histDeltaCorrection(int id1, int id2, std::vector<std::vector<double> > &vec) {

	if(noOfPairs(id1, id2) < 200e3){
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

	//get matrix file name from PndLmdAlignManager
	string matrixName = _matrixDir + PndLmdAlignManager::makeMatrixFileName(id1, id2, _inCentimeters);

	if(_inCentimeters){
		Matrix icpMatrix = manager.readMatrix(matrixName);
		Matrix corrSensorToSensor = manager.getCorrectionMatrix(id1, id2);

		/*
		 * remember, all pairs were in lmd local, transform matrix from PndLmdDim
		 * (which came in panda gloabal) to lmd local, since ICP matrix will be in
		 * lmd local
		 */

		manager.transformGlobalToLmd(corrSensorToSensor);
		//residual matrix
		Matrix result = icpMatrix - corrSensorToSensor;
		return result;
	}

	// in pixels
	else{
		Matrix real = manager.getMatrixOfficialGeometry(id1, id2,false);
		Matrix icpMatrix = manager.readMatrix(matrixName);
		return icpMatrix - real;
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
