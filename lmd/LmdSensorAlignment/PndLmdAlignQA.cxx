/*
 * PndLmdAlignQA.cxx
 *
 *  Created on: Jul 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
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
		for(size_t i=0; i<idPairs.size(); i++){
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

	}

	//plot by module
	//TODO: implement
	else if(param == kPlotByModule){

		for(size_t i=0; i<idPairs.size(); i++){

			//check cases
			int sensorID, half, plane, module, side, die, sensor;
			sensorID = idPairs[i].first;
			dimension->Get_sensor_by_id(sensorID, half, plane, module, side, die, sensor);

			//for(int j=0; j<10; j++){
			//	createThreeHistsVeryDirty(idPairs[i].first, idPairs[i].second, j, data, param);
			//}

		}
	}

	//TODO: handle path and filenames correctly
	else if(param == kPlotCMMatrixResiduals){

		string pdfdir = pdfOutPath;
		_inCentimeters = true;

		for(size_t i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < pairsRequired){
				continue;
			}

			//prepare
			string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
			string path = LMDMatPath;
			matrixNameCM = path + matrixNameCM;

			//read matrices from disk
			Matrix matrixCM = manager.readMatrix(matrixNameCM);

			Matrix senToSenCorrTarget;
			if(alignOptionBool){
				senToSenCorrTarget = Matrix::eye(4);
			}
			else{
				senToSenCorrTarget = manager.getCorrectionMatrix(id1, id2);
			}

			//transform both correction matrices to full sensor to sensor matrices
			Matrix senToSenIdeal = manager.getMatrixOfficialGeometry(id1, id2, true);

			Matrix matrixDif = (matrixCM*senToSenIdeal) - (senToSenCorrTarget*senToSenIdeal);

			//store this residual tuple to data
			std::vector<double> result;
			result.push_back(id1);
			result.push_back(id2);
			result.push_back(matrixDif.val[0][1]);		// sin(alpha)
			result.push_back(matrixDif.val[0][3]);		// tx
			result.push_back(matrixDif.val[1][3]);		// ty
			result.push_back(matrixCM.val[2][3]);		// tz
			data.push_back(result);
		}

		histParams parameters;
		//parameters.bins=25;

		//for DX
		parameters.path = pdfdir + "/residuals/";
		parameters.title = "matrixCM - correctionTarget, #DeltaX";
		parameters.xtitle = "dX [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dx.pdf";
		parameters.vectorIndex = 3;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);

		//for DY
		//parameters.path = pdfdir;
		parameters.title = "matrixCM - correctionTarget, #DeltaY";
		parameters.xtitle = "dY [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dy.pdf";
		parameters.vectorIndex = 4;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);

		if(true){
			//for DZ
			//parameters.path = pdfdir;
			parameters.title = "#DeltaZ of HitPairs";
			parameters.xtitle = "dZ [pair Steps]";
			parameters.ytitle = "entries";
			parameters.scaleFactor = 1.0;
			parameters.fileName = "dz.pdf";
			parameters.vectorIndex = 5;
			parameters.xMin=-1;
			parameters.xMax=-1;
			parameters.printCMPXinPathName = true;
			createHist(data, parameters);
		}

		//for DAlpha
		//parameters.path = pdfdir;
		parameters.title = "matrixCM - correctionTarget, #Delta#alpha";
		parameters.xtitle = "d#alpha [#murad]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e6;
		parameters.fileName = "dalpha.pdf";
		parameters.vectorIndex = 2;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);
	}

	//TODO: handle path and filenames correctly
	else if(param == kPlotPXMatrixResiduals){

		string pdfdir = pdfOutPath;
		_inCentimeters = false;

		for(size_t i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < pairsRequired){
				continue;
			}

			//prepare
			Matrix target = manager.getMatrixOfficialGeometry(id1, id2,alignOptionBool); //true = aligned, false = misaligned
			string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
			string path = LMDMatPath;
			matrixNamePX = path + matrixNamePX;

			//read matrices from disk
			Matrix matrixPX = manager.readMatrix(matrixNamePX);
			manager.transformFromSensorToLmdLocal(matrixPX, id1, alignOptionBool); // false = misaligned geometry
			Matrix matrixDif = matrixPX - target;

			//store this residual tuple to data
			std::vector<double> result;
			result.push_back(id1);
			result.push_back(id2);
			result.push_back(matrixDif.val[0][1]);		// sin(alpha)
			result.push_back(matrixDif.val[0][3]);		// tx
			result.push_back(matrixDif.val[1][3]);		// ty
			result.push_back(matrixPX.val[2][3]);		// tz
			data.push_back(result);
		}

		histParams parameters;
		//parameters.bins=25;

		//for DX
		parameters.path = pdfdir + "/residuals/";
		parameters.title = "matrixPX(transformed) - senToSenTarget, #DeltaX";
		parameters.xtitle = "dX [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dx.pdf";
		parameters.vectorIndex = 3;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);

		//for DY
		//parameters.path = pdfdir;
		parameters.title = "matrixPX(transformed) - senToSenTarget, #DeltaY";
		parameters.xtitle = "dY [#mum]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4;
		parameters.fileName = "dy.pdf";
		parameters.vectorIndex = 4;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);

		if(true){
			//for DZ
			//parameters.path = pdfdir;
			parameters.title = "#DeltaZ of HitPairs";
			parameters.xtitle = "dZ [pair Steps]";
			parameters.ytitle = "entries";
			parameters.scaleFactor = 1.0;
			parameters.fileName = "dz.pdf";
			parameters.vectorIndex = 5;
			parameters.xMin=-1;
			parameters.xMax=-1;
			parameters.printCMPXinPathName = true;
			createHist(data, parameters);
		}

		//for DAlpha
		//parameters.path = pdfdir;
		parameters.title = "matrixPX(transformed) - senToSenTarget, #Delta#alpha";
		parameters.xtitle = "d#alpha [#murad]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e6;
		parameters.fileName = "dalpha.pdf";
		parameters.vectorIndex = 2;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = true;
		createHist(data, parameters);
	}

	//TODO: handle path and filenames correctly
	else if(param==kPlotCMvsPX){

		string pdfdir = pdfOutPath;

		for(size_t i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < pairsRequired){
				continue;
			}

			//prepare
			string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
			string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
			string path = LMDMatPath;
			matrixNameCM = path + matrixNameCM;
			matrixNamePX = path + matrixNamePX;

			//read matrices from disk
			Matrix matrixCM = manager.readMatrix(matrixNameCM);
			Matrix matrixPX = manager.readMatrix(matrixNamePX);

			//transform matrixPX to lmd local
			manager.transformFromSensorToLmdLocal(matrixPX, id1);

			Matrix senToSen = manager.getMatrixOfficialGeometry(id1,id2,true);
			Matrix senToSenWithCorr = matrixCM * senToSen;  //this is now the total matrix from sen1 to sen2
			Matrix matrixDif = senToSenWithCorr - matrixPX;

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
		parameters.scaleFactor = 1e4*1e3;
		parameters.fileName = "dx.pdf";
		parameters.vectorIndex = 3;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = false;
		createHist(data, parameters);

		//for DY
		//parameters.path = pdfdir;
		parameters.title = "matrixCM*matrixTarget - matrixPX(transformed), #DeltaY)";
		parameters.xtitle = "dY [nm]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e4*1e3;
		parameters.fileName = "dy.pdf";
		parameters.vectorIndex = 4;
		parameters.xMin=-1;
		parameters.xMax=-1;
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
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName = false;
		createHist(data, parameters);
	}

	else if(param==kPlotPXvsCMResiduals){

		//prepare some stuff for aligned/misaligned cases
		string pdfdir = pdfOutPath;

		//remember, we want the difference between two residuals
		// (matrixPX-matrixTarget) - (matrixCM-matrixtarget)

		// prepare data sets
		std::vector<std::vector<double> > dataCM;
		std::vector<std::vector<double> > dataPX;

		//so, gather all CMresiduals
		for(size_t i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < pairsRequired){
				continue;
			}

			//prepare
			string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
			string path = LMDMatPath;
			matrixNameCM = path + matrixNameCM;

			//read matrices from disk
			Matrix matrixCM = manager.readMatrix(matrixNameCM);

			//the aligned case is special because the correction matrix is the identity
			Matrix senToSenCorrTarget;
			if(alignOptionBool){
				senToSenCorrTarget = Matrix::eye(4);
			}
			else{
				senToSenCorrTarget = manager.getCorrectionMatrix(id1, id2);
			}

			//transform both correction matrices to full sensor to sensor matrices
			Matrix senToSenIdeal = manager.getMatrixOfficialGeometry(id1, id2, alignOptionBool);

			Matrix matrixDif = (matrixCM*senToSenIdeal) - (senToSenCorrTarget*senToSenIdeal);

			//store this residual tuple to data
			std::vector<double> resultCM;
			resultCM.push_back(id1);
			resultCM.push_back(id2);
			resultCM.push_back(matrixDif.val[0][1]);		// sin(alpha)
			resultCM.push_back(matrixDif.val[0][3]);		// tx
			resultCM.push_back(matrixDif.val[1][3]);		// ty
			dataCM.push_back(resultCM);
		}

		//gather all PX residuals
		for(size_t i=0; i<idPairs.size(); i++){

			int id1 = idPairs[i].first;
			int id2 = idPairs[i].second;

			//only select overlap areas with more than 2e5 pairs
			int overlapID = dimension->makeOverlapID(id1, id2);
			if( matrixInfo[overlapID] < 250e3){
				continue;
			}

			//prepare
			Matrix target = manager.getMatrixOfficialGeometry(id1, id2,alignOptionBool); //true = aligned, false = misaligned
			string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
			string path = LMDMatPath;
			matrixNamePX = path + matrixNamePX;

			//read matrices from disk
			Matrix matrixPX = manager.readMatrix(matrixNamePX);
			manager.transformFromSensorToLmdLocal(matrixPX, id1, alignOptionBool); // false = misaligned geometry
			Matrix matrixDif = matrixPX - target;

			//store this residual tuple to data
			std::vector<double> resultPX;
			resultPX.push_back(id1);
			resultPX.push_back(id2);
			resultPX.push_back(matrixDif.val[0][1]);		// sin(alpha)
			resultPX.push_back(matrixDif.val[0][3]);		// tx
			resultPX.push_back(matrixDif.val[1][3]);		// ty
			dataPX.push_back(resultPX);
		}

		//we now have two data sets, dataCM and dataPX. combine!

		if(dataCM.size() != dataPX.size()){
			cout << "ERROR. the two data sets are not equally large! exiting!\n";
			exit(1);
		}

		data.reserve(dataCM.size());

		for(size_t i=0; i<dataCM.size(); i++){

			//check if ids match
			if( (dataCM[i][0] != dataPX[i][0]) || (dataCM[i][1] != dataPX[i][1]) ){
				cout << "ERROR. vectors are not in the same order!\n";
			}

			std::vector<double> interimData;
			interimData.push_back(dataCM[i][0]);
			interimData.push_back(dataCM[i][1]);
			interimData.push_back(dataPX[i][2] - dataCM[i][2]);		// sin(alpha)
			interimData.push_back(dataPX[i][3] - dataCM[i][3]);		// tx
			interimData.push_back(dataPX[i][4] - dataCM[i][4]);		// ty

			if(abs(interimData[2]) > 2e-6 || abs(interimData[3]) > 0.2e-4 || abs(interimData[4]) > 0.2e-4 ){
				cout << "offender: id: " << interimData[0] << "to" << interimData[1] << "\n";
			}

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
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName=false;
		createHist(data, parameters);

		//for DY
		//parameters.path = pdfOutPath;
		parameters.title = "PXresiduals - CMresiduals, #DeltaY (0u)";
		parameters.xtitle = "dY [nm]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e7;
		parameters.fileName = "dy.pdf";
		parameters.vectorIndex = 4;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName=false;
		createHist(data, parameters);

		//for DAlpha
		//parameters.path = pdfOutPath;
		parameters.title = "PXresiduals - CMresiduals, #Delta#alpha (0u)";
		parameters.xtitle = "d#alpha [#murad]";
		parameters.ytitle = "entries";
		parameters.scaleFactor = 1e6;
		parameters.fileName = "dalpha.pdf";
		parameters.vectorIndex = 2;
		parameters.xMin=-1;
		parameters.xMax=-1;
		parameters.printCMPXinPathName=false;
		createHist(data, parameters);

	}
	else if(param==kHistPixelDistances){
		histPixelDistances(0,5);
		histPixelDistances(1,8);
		histPixelDistances(2,8);
		histPixelDistances(2,9);
		histPixelDistances(3,6);
		histPixelDistances(3,7);
		histPixelDistances(3,8);
		histPixelDistances(4,7);
		histPixelDistances(4,9);
	}

	/*
	 * test functions and sandbox
	 */
	else if(param == 99){

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

		int id1 = 0;
		int id2 = 5;

		//prepare
		string matrixNameCM = manager.makeMatrixFileName(id1,id2,true,false);
		string matrixNamePX = manager.makeMatrixFileName(id1,id2,false,false);
		string path = "/home/arbeit/simulationData/boxtest-misaligned-10u-1.5/binaryPairs/LMDmatrices/";
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
		Matrix real = manager.getMatrixOfficialGeometry(id1, id2,true);
		cout << "official matrix:\n" << real << "\n";
		Matrix icpMatrixNewOne = manager.readMatrix(matrixNamePX);
		manager.transformFromSensorToLmdLocal(icpMatrixNewOne, id1, false);		//remember, icp matrix is local to sensor!
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
}

void PndLmdAlignQA::histPixelDistances(int sensor1, int sensor2){

	int sensorID1=sensor1, sensorID2=sensor2;

	Matrix PXtoCM = manager.getPixelToCentimeterTransformation();
	Matrix CMtoPX = Matrix::inv(PXtoCM);
	Matrix sen1ToSen2 = manager.getMatrixOfficialGeometry(sensorID1, sensorID2, true);
	manager.transformFromLmdLocalToSensor(sen1ToSen2,sensorID1,true);
	Matrix matSensorOneToSensorTwo = CMtoPX * sen1ToSen2 * PXtoCM;

	//histogram those distances
	double colTest2=0, rowTest2=0;

	TH1D hist("Pixel Distances", "Pixel Distances",300,-1,-1);

	int valid=0;

	//for every sensor pair on ONE module (all modules are equal, none are more equal than others)


	//for every pixel (250*250) find all 4 overlap pixels (cutoff at 100 microns, pixels can't be that far away on perfect geometry)
	for(int colTest1=0; colTest1<250; colTest1++){
		for(int rowTest1=0; rowTest1 < 250; rowTest1++){

			//create hit on sensor1 and compute corresponding hit on sensor2
			Matrix hit1 = manager.makeFourVector(colTest1, rowTest1, 0);
			Matrix hit2 = matSensorOneToSensorTwo * hit1;
			colTest2 = hit2.val[0][0];
			rowTest2 = hit2.val[1][0];

			//also, check if overlap pixel even exists. must be row elem [0,250], col elem [0,250]
			//are we still overlapping area?
			if(colTest2 < 0 || colTest2 > 247
			){
				continue;
			}
			if(rowTest2 < 0 || rowTest2 > 242){
				continue;
			}

			//count for statistics
			valid++;

			//four possible neighbors
			double colTest2Floor = floor(colTest2);
			double colTest2Ceil = ceil(colTest2);
			double rowTest2Floor = floor(rowTest2);
			double rowTest2Ceil = ceil(rowTest2);

			//only in verbose mode
			if(false){
				cout << "=== h1: ===\n" << hit1 << "\n";
				cout << "=== h2: ===\n" << hit2 << "\n";
				cout << "c1: " << colTest1 << "\n";
				cout << "r1: " << rowTest1 << "\n";
				cout << "c2: " << colTest2 << ", floor: " << colTest2Floor << ", ceil: " << colTest2Ceil << "\n";
				cout << "r2: " << rowTest2 << ", floor: " << rowTest2Floor << ", ceil: " << rowTest2Ceil << "\n";
			}

			//make all possible hit2's:
			vector<Matrix> hit2Candidates;
			hit2Candidates.push_back(manager.makeFourVector(colTest2Floor, rowTest2Floor, -0.025));
			hit2Candidates.push_back(manager.makeFourVector(colTest2Ceil, rowTest2Floor, -0.025));
			hit2Candidates.push_back(manager.makeFourVector(colTest2Floor, rowTest2Ceil, -0.025));
			hit2Candidates.push_back(manager.makeFourVector(colTest2Ceil, rowTest2Ceil, -0.025));

			//OR use matrices directly. actually, maybe use matrix directly

			//now, transform everything to cm in sensor coordinate system
			hit1 = PXtoCM * hit1;

			//cout << "=== start ===\n";
			for(size_t i=0; i<hit2Candidates.size(); i++){
				double distance;
				hit2Candidates[i] = PXtoCM * Matrix::inv(matSensorOneToSensorTwo) * hit2Candidates[i];
				distance = (hit1 - hit2Candidates[i]).l2norm()*1e4;		//convert to microns

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
	double coverage = (double)valid/(250.0*250.0)*100;
	cout << "overlap: " << coverage << "\n";

	stringstream ss;
	ss << "Pixel Distances Area " << sensorID1 << " to " << sensorID2 << ", " << coverage << "% coverage." ;
	hist.SetTitle(ss.str().c_str());
	ss.str("");
	ss << "d [#mum]";
	hist.GetXaxis()->SetTitle(ss.str().c_str());
	ss.str("");
	ss << "Entries";
	hist.GetYaxis()->SetTitle(ss.str().c_str());




	stringstream filename;
	filename << pdfOutPath  << "/AreaPixelDistances" << sensor1 << "to" << sensor2 << ".pdf";
	TCanvas canvas;
	canvas.cd();
	hist.Draw();
	canvas.Print(filename.str().c_str());
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
	string matrixName = LMDMatPath + PndLmdAlignManager::makeMatrixFileName(id1, id2, _inCentimeters);

	//FIXME: remember the changes. correct those methods.
	if(_inCentimeters){
		Matrix icpMatrix = manager.readMatrix(matrixName);
		Matrix corrSensorToSensor = manager.getCorrectionMatrix(id1, id2);
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

	string filename;
	if(_inCentimeters){
		filename = LMDMatPath + "/info-cm.txt";
	}
	else{
		filename = LMDMatPath + "/info-px.txt";
	}

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
	manager.searchFiles(LMDMatPath, files, "mat", false);
	int foundFiles=0;

	//no matrix files at all!
	if(files.size()==0){
		return false;
	}

	string matrixName;
	bool tempfilefound=false;

	//check for every ID that should be there if there is a corresponding file
	for(size_t i=0; i<availableIds.size(); i++){

		//reset counter
		tempfilefound=false;
		matrixName = manager.makeMatrixFileName(availableIds[i], _inCentimeters, false);

		for(size_t j=0; j<files.size(); j++){
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

	TH1D histogram(parameters.title.c_str(), parameters.title.c_str(), parameters.bins, parameters.xMin, parameters.xMax);

	histogram.GetXaxis()->SetTitle(parameters.xtitle.c_str());
	histogram.GetYaxis()->SetTitle(parameters.ytitle.c_str());

	if(vec.size()==0){
		cout << "Error: nothing read, data empty! (maybe not enough pairs?) \n";
		exit(1);
	}

	vector< std::pair<double, std::string> > offenders;

	//have all data now
	double dataPoint=0.0;
	for(size_t iArea=0; iArea<vec.size();iArea++){
		dataPoint = vec[iArea][parameters.vectorIndex] * parameters.scaleFactor;
		histogram.Fill(dataPoint);

		if(abs(dataPoint) > 0.2){
			offenders.push_back(make_pair(dataPoint, "ARGH"));
			//cout << "offender at " << dataPoint << "\n";
		}

	}

	// iterate over all offenders
	// find bin for every offender
	// set bin label for every offender
	// FUCK ignore bins that have two offenders

	//for(int i=0; i<offenders.size(); i++){
	//	Int_t binNo = histogram.GetXaxis()->FindFixBin(offenders[i].first );
	//	histogram.GetXaxis()->SetBinLabel(binNo, offenders[i].second.c_str());
	//}

	stringstream pathname;
	//pathname << _outputPath;
	pathname << parameters.path;

	if(parameters.printCMPXinPathName){
		_inCentimeters ? pathname << "/inCm/" : pathname << "/inPx/";
	}

	_enableHelperMatrix ? pathname << "corrFull-" : pathname << "";

	PndLmdAlignManager::mkdir(pathname.str());

	TCanvas canvas("canvas", "canvas", 800,600);
	canvas.cd();
	histogram.Draw();
	histogram.Draw("HIST TEXT0 SAME");
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

	manager.setMatrixOutDir(LMDMatPath);
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
