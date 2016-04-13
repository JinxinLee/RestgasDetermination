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

	//contiains number of pairs on overlap area
	std::map<int, int> matrixInfo;

	void histDeltaCorrection(int id1, int id2, std::vector<std::vector<double> > &vec);
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
	bool checkForMatrixFiles();

	void compareMatrices();
	void compareCombinedMatrices();

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
