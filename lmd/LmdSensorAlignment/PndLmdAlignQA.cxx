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

//using namespace std;

using std::string;
using std::vector;


PndLmdAlignQA::PndLmdAlignQA(){
	// TODO Auto-generated constructor stub
	//FIXME: path is hard coded and therefore wrong
	manager.readTrafoMatrix("/macro/lmd/testPixel/countTest/FullRun/geometry/trafo_matrices_lmd.dat", true);
	manager.readTrafoMatrix("/macro/lmd/testPixel/countTest/FullRun/geometry/trafo_matrices_lmd_misaligned.dat", false);
}

PndLmdAlignQA::~PndLmdAlignQA() {
	// TODO Auto-generated destructor stub
}

void PndLmdAlignQA::plotErrorsAbsolute(std::string inputFile, string outputPath){

	checkIOpaths();

	TCanvas canvas("canvas", "canvas", 640, 480);

	TH1D deltaX("x", "x", 30, -1, -1);
	TH1D deltaY("y", "y", 30, -1, -1);
	TH1D deltaAlpha("#alpha", "#alpha", 30, -1, -1);

	TGraph deltaXvspairs(360);
	TGraph deltaYvspairs(360);
	TGraph deltaAlphavspairs(360);

	//read one data file
	vector< vector<double> > data = PndLmdAlignManager::readFromCSVFile(inputFile);

	for(int i=0; i<data.size(); i++){
		deltaAlpha.Fill(data[i][5]*1e6);
		deltaX.Fill(data[i][6]*1e4);
		deltaY.Fill(data[i][7]*1e4);

		deltaAlphavspairs.SetPoint(i, data[i][4], abs(data[i][5])*1e6);
		deltaXvspairs.SetPoint(i, data[i][4], abs(data[i][6])*1e4);
		deltaYvspairs.SetPoint(i, data[i][4], abs(data[i][7])*1e4);
	}

	deltaXvspairs.Sort();
	deltaYvspairs.Sort();
	deltaAlphavspairs.Sort();

	deltaX.GetXaxis()->SetTitle("offset / #mum");
	deltaX.GetYaxis()->SetTitle("entries");
	deltaX.SetTitle("[absolute matrix, non converted] X Value (x)");

	deltaY.GetXaxis()->SetTitle("offset / #mum");
	deltaY.GetYaxis()->SetTitle("entries");
	deltaY.SetTitle("[absolute matrix, non converted] Y Value (y)");

	deltaAlpha.GetXaxis()->SetTitle("rotation / #murad");
	deltaAlpha.GetYaxis()->SetTitle("entries");
	deltaAlpha.SetTitle("[absolute matrix, non converted] rotation Value (#alpha)");

	deltaXvspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaXvspairs.GetYaxis()->SetTitle("x / #mum");
	deltaXvspairs.SetTitle("x vs Number of Pairs");

	deltaYvspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaYvspairs.GetYaxis()->SetTitle("y / #mum");
	deltaYvspairs.SetTitle("y vs Number of Pairs");

	deltaAlphavspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaAlphavspairs.GetYaxis()->SetTitle("#alpha / #murad");
	deltaAlphavspairs.SetTitle("#alpha vs Number of Pairs");

	canvas.cd();
	deltaX.Draw();
	canvas.Print((outputPath + "/absoluteX.pdf").c_str());
	deltaY.Draw();
	canvas.Print((outputPath + "/absoluteY.pdf").c_str());
	deltaAlpha.Draw();
	canvas.Print((outputPath + "/absoluteAlpha.pdf").c_str());
	deltaXvspairs.Draw("AL");
	canvas.Print((outputPath + "/absoluteXvsNPairs.pdf").c_str());
	deltaYvspairs.Draw("AL");
	canvas.Print((outputPath + "/absoluteYvsNPairs.pdf").c_str());
	deltaAlphavspairs.Draw("AL");
	canvas.Print((outputPath + "absoluteAlphavsNPairs.pdf").c_str());

}

void PndLmdAlignQA::plotErrorsRelative(int overlapID){

	checkIOpaths();
	cout << "overlapID is " << overlapID << endl;

	TCanvas canvas("canvas", "canvas", 640, 480);

	int bins;
	overlapID == -1 ? bins = 30 : bins = 40;

	double xmin = -1;
	double xmax = -1;

	TH1D deltaX("#Deltax", "#Deltax", bins, xmin, xmax);
	TH1D deltaY("#Deltay", "#Deltay", bins, xmin, xmax);
	TH1D deltaAlpha("#Delta#alpha", "#Delta#alpha", bins, -1, -1);

	//FIXME: this number comes from 360 overlapping areas, but it needs to be calculated in case of multiple geometries
	TGraph deltaXvspairs(360);
	TGraph deltaYvspairs(360);
	TGraph deltaAlphavspairs(360);

	int fillCounter=0;
	double avgNoOfPairs;

	for(int iFile=0; iFile<_inputFiles.size(); iFile++){

		//read one data file
		vector< vector<double> > data = PndLmdAlignManager::readFromCSVFile(_inputFiles[iFile]);
		//cout << "data file contains " << data.size() << " lines.\n";

		if(overlapID==-1){
			for(int i=0; i<data.size(); i++){
				deltaAlpha.Fill(data[i][5]*1e6);
				deltaX.Fill(data[i][6]*1e4);
				deltaY.Fill(data[i][7]*1e4);

				deltaAlphavspairs.SetPoint(i, data[i][4], abs(data[i][5])*1e6);
				deltaXvspairs.SetPoint(i, data[i][4], abs(data[i][6])*1e4);
				deltaYvspairs.SetPoint(i, data[i][4], abs(data[i][7])*1e4);
			}
		}

		else{
			int iterator=0;
			for(int i=0; i<data.size(); i++){
				if( fmod(data[i][0],10) == overlapID){

					if(byPlane){
						if(curPlane == getPlane(data[i][1])){
							deltaAlpha.Fill(data[i][5]*1e6);
							deltaX.Fill(data[i][6]*1e4);
							deltaY.Fill(data[i][7]*1e4);
							avgNoOfPairs+=data[i][4];
							fillCounter++;
						}
					}
					else{
						deltaAlpha.Fill(data[i][5]*1e6);
						deltaX.Fill(data[i][6]*1e4);
						deltaY.Fill(data[i][7]*1e4);

						avgNoOfPairs+=data[i][4];
						fillCounter++;

						deltaAlphavspairs.SetPoint(iterator, data[i][4], abs(data[i][5])*1e6);
						deltaXvspairs.SetPoint(iterator, data[i][4], abs(data[i][6])*1e4);
						deltaYvspairs.SetPoint(iterator++, data[i][4], abs(data[i][7])*1e4);
					}
				}
			}
		}
	}
	cout << "read " << _inputFiles.size() << " files.\n";
	avgNoOfPairs/=fillCounter;

	deltaXvspairs.Sort();
	deltaYvspairs.Sort();
	deltaAlphavspairs.Sort();


	deltaX.GetXaxis()->SetTitle("offset / #mum");
	deltaX.GetYaxis()->SetTitle("entries");
	stringstream titleX;
	titleX << "#Deltax, OverlapArea " << overlapID << ", avg #Pairs=" << avgNoOfPairs;
	if(overlapID==-1){
		deltaX.SetTitle("[real Matrix - found Matrix] X Value (#Deltax)");
	}
	else{
		deltaX.SetTitle(titleX.str().c_str());
	}



	deltaY.GetXaxis()->SetTitle("offset / #mum");
	deltaY.GetYaxis()->SetTitle("entries");
	stringstream titleY;
	titleY << "#Deltay, OverlapArea " << overlapID << ", avg #Pairs=" << avgNoOfPairs;
	if(overlapID==-1){
		deltaY.SetTitle("[real Matrix - found Matrix] Y Value (#Deltax)");
	}
	else{
		deltaY.SetTitle(titleY.str().c_str());
	}

	deltaAlpha.GetXaxis()->SetTitle("rotation / #murad");
	deltaAlpha.GetYaxis()->SetTitle("entries");
	stringstream titleR;
	titleR << "#Delta#alpha, OverlapArea " << overlapID << ", avg #Pairs=" << avgNoOfPairs;

	if(overlapID==-1){
		deltaAlpha.SetTitle("[real Matrix - found Matrix] rotation (#Delta#alpha)");
	}
	else{
		deltaAlpha.SetTitle(titleR.str().c_str());
	}

	deltaXvspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaXvspairs.GetYaxis()->SetTitle("#Deltax / #mum");
	deltaXvspairs.SetTitle("#Deltax vs Number of Pairs");

	deltaYvspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaYvspairs.GetYaxis()->SetTitle("#Deltay / #mum");
	deltaYvspairs.SetTitle("#Deltay vs Number of Pairs");

	deltaAlphavspairs.GetXaxis()->SetTitle("Number of Pairs");
	deltaAlphavspairs.GetYaxis()->SetTitle("#Delta#alpha / #murad");
	deltaAlphavspairs.SetTitle("#Delta#alpha vs Number of Pairs");

	/*
	stringstream deltaFilename;
	if(overlapID==-1){
		deltaFilename << _outputPath << "/delta.pdf";
	}
	else{
		deltaFilename << _outputPath << "/oID-" << overlapID << "-delta.pdf";
	}

	canvas.cd();
	deltaX.Draw();
	canvas.Print((deltaFilename.str() + "(").c_str());
	deltaY.Draw();
	canvas.Print(deltaFilename.str().c_str());
	deltaAlpha.Draw();
	canvas.Print((deltaFilename.str() + ")").c_str());
	deltaXvspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf(").c_str());
	deltaYvspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf").c_str());
	deltaAlphavspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf)").c_str());
	 */

	stringstream deltaFilename;
	if(overlapID==-1){
		deltaFilename << _outputPath << "/delta.pdf";
	}
	else{
		deltaFilename << _outputPath << "/oID-" << overlapID << "-delta";
	}

	canvas.cd();
	deltaX.Draw();
	canvas.Print((deltaFilename.str() + "x.pdf").c_str());
	deltaY.Draw();
	canvas.Print((deltaFilename.str() + "y.pdf").c_str());
	deltaAlpha.Draw();
	canvas.Print((deltaFilename.str() + "a.pdf").c_str());
	deltaXvspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf(").c_str());
	deltaYvspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf").c_str());
	deltaAlphavspairs.Draw("AL");
	canvas.Print((_outputPath + "/deltavsNPairs.pdf)").c_str());
}

void PndLmdAlignQA::plotByOverlapID() {


	checkIOpaths();

	//each overlap needs x, y shift and rot

	//will need a struct to hold them

	//make 9 overlap hists

	/*
	 * one method to generate three hists, or one hist called three times?
	 */

	//name them accordingly

	//fill them

	//draw and save them to SINGLE pdf file (do this by using ( and ) )
	//or don't, I'm not your mother

	curPlane=-1;
	for(int iOverlapID=0; iOverlapID<9; iOverlapID++){
		plotErrorsRelative(iOverlapID);
	}
}

void PndLmdAlignQA::checkIOpaths() {

	//ensure input file exists
	cout << "checking input files...";

	if(_inputFiles.size()==0){
		cout << "no file specified or found!\n";
		exit(1);
	}
	for(int iFile=0; iFile<_inputFiles.size(); iFile++){
		if(!boost::filesystem::exists(_inputFiles[iFile])){
			cout << "error opening file:";
			cout << _inputFiles[iFile] << "\n";
			exit(1);
		}
	}
	cout << " ok!\n";

	//ensure output path exists
	cout << "checking output path...";
	if(!boost::filesystem::exists(_outputPath)){
		boost::filesystem::create_directories(_outputPath);
	}
	cout << " ok!\n";
}

void PndLmdAlignQA::plotByPlane() {

	checkIOpaths();
	byPlane=true;

	string oldOutputPath=_outputPath;
	stringstream pathstream;

	//curplane is a global parameter
	for(curPlane =0; curPlane<4; curPlane++){
		for(int iOverlapID=0; iOverlapID<9; iOverlapID++){
			pathstream.str("");
			pathstream << oldOutputPath << "/plane" << curPlane << "/";
			_outputPath = pathstream.str();
			plotErrorsRelative(iOverlapID);
		}
	}
	_outputPath=oldOutputPath;
}

void PndLmdAlignQA::clear() {

	_inputFiles.clear();
}

int PndLmdAlignQA::getPlane(double moduleID) {

	if(moduleID<0){
		return -1;
	}

	if(moduleID<10){
		return 0;
	}
	if(moduleID<100){
		return floor( moduleID/10 );
	}
	if (moduleID<400){
		double remainder = fmod(moduleID, 100);
		return floor( remainder/10 );
	}
	return -1;

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
	}

	//FIXME: won't work forever, change!
	stringstream pathname;
	pathname << _outputPath;
	//pathname << "/home/roman/pandaroot/macro/lmd/testPixel/countTest/pdf/";

	_inCentimeters ? pathname << "inCm/" : pathname << "inPx/";
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

	if(noOfPairs(id1, id2) < 5e3){
		return;
	}

	std::vector<double> result;
	Matrix thisMat = getdeltaCorrectionMatrix(id1, id2);
	result.push_back(id1);
	result.push_back(id2);
	result.push_back(thisMat.val[0][1]);
	result.push_back(thisMat.val[0][3]);
	result.push_back(thisMat.val[1][3]);
	vec.push_back(result);

}

Matrix PndLmdAlignQA::getdeltaCorrectionMatrix(int id1, int id2) {

	if(_inCentimeters){

		stringstream matrixName;
		//FIXME: this won't work forever
		matrixName << _matrixDir << "/m";
		matrixName << id1 << "to" << id2 << "cm";
		if(_enableHelperMatrix){
			matrixName << "C";
		}
		matrixName << ".mat";
		Matrix icpMatrix = manager.readMatrix(matrixName.str());

		//second new setup
		//Matrix corr = manager.getCorrectionMatrix(id1,id2);
		//return corr-icpMatrix;

		//old setup

		//this is no mistake, I want the inverted matrix of (id1,id2)
		Matrix idealInv = manager.getMatrixOfficialGeometry(id2,id1,true);
		Matrix real = manager.getMatrixOfficialGeometry(id1,id2,false);
		Matrix correction;

		//idealInv.val[2][3]=0;
		//real.val[2][3]=0;

		if(_enableHelperMatrix){
			Matrix helper = PndLmdAlignManager::homogenizeMatrix(manager.getHelperMatrix());
			helper.inv();
			correction = real * idealInv * helper;
		}
		else{
			correction = real * idealInv;
		}

		//transform ICP matrix here
		//Matrix lmdToGlobal = manager.getMatrixLmdToGlobal();
		//Matrix GlobalToLmd = manager.getMatrixGlobalToLmd();
		//correction = lmdToGlobal * correction * GlobalToLmd;
		//correction = GlobalToLmd * correction * lmdToGlobal;
		/*

		cout << "==========================\n";
		cout << "correction before:\n";
		cout << correction << "\n";

		//transform correction matrix here!
		//manager.transformGlobalToLmd(correction);

		cout << "correction after:\n";
		cout << correction << "\n";
		cout << "icp matrix:\n";

		cout << icpMatrix << "\n";
		cout << "==========================\n";

		stringstream newdir;
		newdir << _matrixDir << "/mathematica/";
		 */

		//manager.writeMatrix(correction, "");
		//manager.writeMatrix(icpMatrix, "");

		/*

		cout << "=================== TEST =======================\n";
		cout << "ideal inv from pndlmddim:\n";
		cout << idealInv << endl;
		cout << "real from pndlmddim:\n";
		cout << real << endl;
		cout << "correction:\n";
		cout << real * idealInv << endl;
		cout << "from icp:\n";
		cout << icpMatrix << endl;
		cout << "difference:\n";
		cout << correction - icpMatrix << endl;
		cout << "=================== END =======================\n";

		 */
		//cout << icpMatrix << "\n";
		//cout << "det dif: " << correction.det() - icpMatrix.det() << " (is " << correction.det() << " - " << icpMatrix.det() << ")\n";

		//return correction;
		return correction-icpMatrix;

		//TODO: implement other way for comparison
	}

	// in pixels, lmd local frame of reference
	else{
		Matrix real = manager.getMatrixOfficialGeometry(id1, id2,false);
		stringstream matrixName;
		//FIXME: this won't work forever
		matrixName << _matrixDir << "/m";
		matrixName << id1 << "to" << id2 << "px.mat";
		Matrix icpMatrix = manager.readMatrix(matrixName.str());

		/*
		cout << "=================== TEST =======================\n";
		cout << "real from pndlmddim:\n";
		cout << real << endl;
		cout << "from icp:\n";
		cout << icpMatrix << endl;
		cout << "difference:\n";
		cout << real-icpMatrix << endl;
		cout << "=================== END =======================\n";
		 */
		return icpMatrix - real;
	}

	return Matrix::eye(4);

}

int PndLmdAlignQA::noOfPairs(int id1, int id2) {

	PndLmdDim *dimension = PndLmdDim::Instance();
	int overlapId = dimension->makeOverlapID(id1, id2);
	return matrixInfo[overlapId];
}

void PndLmdAlignQA::readMatrixInfo() {

	string filename = _matrixDir + "info.txt";
	stringstream *info = manager.readFile(filename);

	//parser: first, find line aligenr n (n is overlap id)
	// then find no of pairs: x
	// save to map: n->x

	string line;
	std::vector<string> values;
	int overlapid,noPairs;
	bool alignerComplete=true;

	//cout << "all read and intialized.\n";

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
				//cout << "pairs: " << noPairs << endl;
				matrixInfo[overlapid]=noPairs;
				alignerComplete=true;
			}
		}
	}
}

void PndLmdAlignQA::compareMatricesNew() {

	vector<Matrix> difMatrices;

	vector<string> list;
	string matrixDir="/home/roman/pandaroot/macro/lmd/testPixel/countTest/FullRun/matrices/";
	PndLmdAlignManager::searchFiles(matrixDir, list, "mat", false);
	cout << "found " << list.size() << " files.\n";
	for(int iFile=0; iFile<list.size(); iFile++){

		vector<string> ids = PndLmdAlignManager::findRegex(list[iFile], "m(\\d+)to(\\d+)cm");
		if(ids.size()>2){

			int sensorID1 = boost::lexical_cast<int>(ids[1]);
			int sensorID2 = boost::lexical_cast<int>(ids[2]);

			Matrix icpMatrix = PndLmdAlignManager::readMatrix(list[iFile]);
			Matrix correctionSensor1 = manager.getCorrectionMatrix(sensorID1);
			Matrix correctionSensor2 = manager.getCorrectionMatrix(sensorID2);
			Matrix correction = Matrix::inv(correctionSensor1) * correctionSensor2;

			difMatrices.push_back( correction - icpMatrix);
		}

	}

	cout << "found " << difMatrices.size() << " matrices.\n";

	double shiftBound=100;
	double rotBound=150;

	TH1D DeltaX("DeltaX", "DeltaX", 50,-shiftBound,shiftBound);
	TH1D DeltaY("DeltaY", "DeltaY", 50,-shiftBound,shiftBound);
	TH1D DeltaAlpha("DeltaAlpha", "DeltaAlpha", 50,-rotBound,rotBound);

	DeltaX.GetXaxis()->SetTitle("dX [#mum]");
	DeltaX.GetYaxis()->SetTitle("entries");
	DeltaY.GetXaxis()->SetTitle("dY [#mum]");
	DeltaY.GetYaxis()->SetTitle("entries");
	DeltaAlpha.GetXaxis()->SetTitle("d#alpha [#murad]");
	DeltaAlpha.GetYaxis()->SetTitle("entries");

	//have all data now
	for(int iMatrix; iMatrix < difMatrices.size(); iMatrix++){
		DeltaAlpha.Fill(difMatrices[iMatrix].val[0][1] * 1e6);
		DeltaX.Fill(difMatrices[iMatrix].val[0][3] * 1e4);
		DeltaY.Fill(difMatrices[iMatrix].val[1][3] * 1e4);
	}

	//FIXME: won't work forever, change!
	stringstream pathname;
	pathname << "/home/roman/pandaroot/macro/lmd/testPixel/countTest/FullRun/pdf/matrixDif/";
	PndLmdAlignManager::mkdir(pathname.str());

	TCanvas canvas;
	canvas.cd();
	DeltaX.Draw();
	canvas.Print((pathname.str()+"dx.pdf").c_str());
	DeltaY.Draw();
	canvas.Print((pathname.str()+"dy.pdf").c_str());
	DeltaAlpha.Draw();
	canvas.Print((pathname.str()+"dalpha.pdf").c_str());

	return;


}
