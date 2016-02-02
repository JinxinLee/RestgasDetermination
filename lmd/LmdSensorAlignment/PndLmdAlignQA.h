/*
 * PndLmdAlignQA.h
 *
 *  Created on: Jul 6, 2015
 *      Author: roman
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

class PndLmdAlignQA {

private:

	std::vector<std::pair<double, double> > shitIds;
	std::vector<std::string> _inputFiles;
	std::string _outputPath, _matrixDir;
	double infoMomentum;
	bool infoAbsolute, infoRelative, byPlane, _inCentimeters, _enableHelperMatrix;
	int curPlane;
	PndLmdAlignManager manager;
	std::map<int, int> matrixInfo;

	void histDeltaCorrection(int id1, int id2, std::vector<std::vector<double> > &vec);
	int noOfPairs(int id1, int id2);

	Matrix getdeltaCorrectionMatrix(int id1, int id2);
	int _signerrors;

public:

	void readMatrixInfo();

	PndLmdAlignQA();
	virtual ~PndLmdAlignQA();
	void init();

	//tales absolute matrix, in perfect alignment means identity matrix
	void plotErrorsAbsolute(std::string filename, std::string outputPath);

	//takes matrix in pixels and compares to ideal matrix
	void plotErrorsRelative(int overlapID=-1);

	//plot by overlapID
	void plotByOverlapID();

	//plot by overlapID and plane
	void plotByPlane();

	void addInputfile(std::string input){
		_inputFiles.push_back(input);
	}
	void setOutputPath(std::string path){
		_outputPath = path;
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

	void clear();

	int getPlane(double moduleID);

	void compareMatrices();

	void compareMatricesNew();

	void setInCentimeters(bool inCentimeters) {
		this->_inCentimeters = inCentimeters;
	}

	void setCorrection(bool correction) {
		_enableHelperMatrix = correction;
	}

	void setMatrixDir(const std::string& matrixDir) {
		_matrixDir = matrixDir;
	}
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNQA_H_ */
