/*
 * PndLmdAlignQA.h
 *
 *  Created on: Jul 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 *
 *      helper class that plots stuff, use one instance for desired plot, then delete and create new.
 */

#ifndef LMD_LMDSENSORALIGNMENT_PNDLMDALIGNQA_H_
#define LMD_LMDSENSORALIGNMENT_PNDLMDALIGNQA_H_

#include <sstream>
#include <string>
#include <vector>

#include <PndLmdAlignManager.h>
#include <matrix.h>

enum runParameter{kNormal, kPlotByOverlapID, kPlotByModule, kPlotCMMatrixResiduals, kPlotPXMatrixResiduals, kPlotCMvsPX, kPlotPXvsCMResiduals, kHistPixelDistances, kCalcOverlap};

struct histParams{
	bool printCMPXinPathName=true;
	double scaleFactor=1.0;
	std::string fileName, title, xtitle, ytitle, path;
	int bins=50;
	double xMin=-1;
	double xMax=-1;
	int vectorIndex=0;
	runParameter runParam = kNormal;
};

class PndLmdAlignQA {

private:

	std::vector<std::pair<double, double> > shitIds;
	std::vector<std::string> _inputFiles;
	std::string outputPath, pdfOutPath, binaryMatPath, LMDMatPath;
	double infoMomentum;
	bool infoAbsolute, infoRelative, byPlane, _inCentimeters, _enableHelperMatrix;
	bool alignOptionBool;
	int curPlane;
	int alignOption;	// 0: aligned, others in um: 10, 50, 100, 200
	int pairsRequired;
	PndLmdAlignManager manager;

	//contains number of pairs on overlap area
	std::map<int, int> matrixInfo;

	void histDeltaCorrection(int id1, int id2, std::vector<std::vector<double> > &vec);
	void createThreeHistsVeryDirty(int id1, int id2, int module, std::vector<std::vector<double> > &vec, runParameter param);
	void createHist(std::vector<std::vector<double> > &vec, histParams &parameters);

	void histPixelDistances(int sen1, int sen2, bool aligned=true);
	double calculateOverlappingArea(int id1, int id2, bool aligned=true);

	int noOfPairs(int id1, int id2);

	//get matrix residuals for matrix id1->id2 from icp and target matrix
	Matrix getMatrixResiduals(int id1, int id2);
	int _signerrors;

	PndLmdDim *dimension;

public:

	void readMatrixInfo();

	PndLmdAlignQA();
	virtual ~PndLmdAlignQA();
	void init();

	void setOutputPath(std::string path){
		outputPath = path;
	}
	void setInfoAbsolute(bool info) {
		infoAbsolute = info;
	}

	void setInfoMomentum(double info) {
		infoMomentum = info;
	}

	void setInfoRelative(bool info) {
		infoRelative = info;
	}

	void checkIOpaths();
	bool checkForMatrixFiles();

	void compareMatrices(runParameter param=kNormal);
	void compareCombinedMatrices();

	void setInCentimeters(bool inCentimeters) {
		this->_inCentimeters = inCentimeters;
	}

	void setCorrection(bool correction) {
		_enableHelperMatrix = correction;
	}

	void setLmdMatPath(const std::string& path) {
		LMDMatPath = path;
	}

	void setBinaryMatPath(const std::string& path) {
		binaryMatPath = path;
	}

	void setPdfOutPath(const std::string& path) {
		pdfOutPath = path;
	}

	void setAlignOption(int align) {
		if(align == 0){
			alignOptionBool = true;
		}
		else{
			alignOptionBool = false;
		}
		alignOption = align;
	}

	void setPairsRequired(int number) {
		pairsRequired = number;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNQA_H_ */
