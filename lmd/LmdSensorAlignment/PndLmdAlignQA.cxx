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

	manager.init();

	//FIXME: maybe don't do this hard-coded
	manager.readTrafoMatrix("/geometry/trafo_matrices_lmd.dat", true);
	manager.readTrafoMatrix("/geometry/trafo_matrices_lmd_misaligned.dat", false);
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
/*
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
*/

void PndLmdAlignQA::plotByOverlapID() {


	checkIOpaths();
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

	if(noOfPairs(id1, id2) < 200e3){
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
		matrixName << _matrixDir << "m";
		int overlapID = dimension->makeOverlapID(id1, id2);
		matrixName << overlapID << "cm";
		if(_enableHelperMatrix){
			matrixName << "C";
		}
		matrixName << ".mat";
		Matrix icpMatrix = manager.readMatrix(matrixName.str());
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
		stringstream matrixName;
		//FIXME: this won't work forever
		matrixName << _matrixDir << "m";
		int overlapID = dimension->makeOverlapID(id1, id2);
		matrixName << overlapID << "px.mat";
		Matrix icpMatrix = manager.readMatrix(matrixName.str());
		return icpMatrix - real;
	}

	//default action, if all else fails.
	return Matrix::eye(4);
}

int PndLmdAlignQA::noOfPairs(int id1, int id2) {
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
