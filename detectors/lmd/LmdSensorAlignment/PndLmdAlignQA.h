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
#include <PndLmdGeometryHelper.h>

#include <TGeoMatrix.h>

#include <matrix.h>

struct histParams{
	bool printCMPXinPathName=true;
	double scaleFactor=1.0;
	std::string fileName, title, xtitle, ytitle, path;
	int bins=30;
	double xMin=-1;
	double xMax=-1;
	int vectorIndex=0;
};

class PndLmdAlignQA {

private:

	std::vector<std::string> _inputFiles;
	std::string outputPath, pdfOutPath, binaryMatPath, LMDMatPath;
	double infoMomentum;
	bool infoAbsolute, infoRelative, byPlane, _inCentimeters;
	bool alignOptionBool;
	int pairsRequired;
	PndLmdAlignManager manager;
	PndLmdGeometryHelper *helper;

	//contains number of pairs on overlap area
	std::map<int, int> matrixInfo;

	//set of misaligned matrices read from disk
	std::map<std::string, TGeoHMatrix> *matricesMisaligned;

	void createHist(std::vector<std::vector<double> > &vec, histParams &parameters);

	double calculateOverlappingArea(int id1, int id2);

	int noOfPairs(int overlapID);

public:

	PndLmdAlignQA();
	virtual ~PndLmdAlignQA();
	void init();

	// ========== QA functions
	void checkCombined();
	//TODO: reimplement these with new geometry
	void checkCyclicMatrices(bool inCentimeters=true);
	void checkCombinedMatrices(bool inCentimeters=true);

	// ========== helper functions
	std::map<std::string, TGeoHMatrix> * readRootMatrices(TString &filename);
	TGeoHMatrix baseTransformation(TGeoHMatrix &input, TGeoHMatrix &toBaseMatrix);
	TGeoHMatrix getMatrixSensorToSensor(int sensorOne, int sensorTwo);
	TGeoHMatrix getOverlapMatrixLikeICP(PndLmdOverlapInfo &info);
	TGeoHMatrix getMisalignedOverlapFromGeoManager(PndLmdOverlapInfo &info);
	TGeoHMatrix getMisalignedOverlapFromICP(PndLmdOverlapInfo &info, std::string ICPmatrix);
	std::vector<double> getMatrixDiffCM(PndLmdOverlapInfo &info, std::string &icpFile);
	PndLmdOverlapInfo& getSmallOverlapInfo(std::vector<PndLmdOverlapInfo> &infos, int smallOverlap);

	void readMatrixInfo();
	void checkIOpaths();
	bool checkForMatrixFiles();
	void calculateOverlapingAreas();


	// ========== settings for this QA task
	void setInCentimeters(bool inCentimeters) {	this->_inCentimeters = inCentimeters; }
	void setLmdMatPath(const std::string& path) { LMDMatPath = path; }
	void setBinaryMatPath(const std::string& path) { binaryMatPath = path; }
	void setPdfOutPath(const std::string& path) { pdfOutPath = path; }
	void setAlignedGeometry(bool aligned) {	alignOptionBool = aligned; }
	void setPairsRequired(int number) {	pairsRequired = number;	}
	void setOutputPath(std::string path){ outputPath = path; }
	void setInfoAbsolute(bool info) { infoAbsolute = info; }
	void setInfoMomentum(double info) { infoMomentum = info; }
	void setInfoRelative(bool info) { infoRelative = info; }
};

#endif /* LMD_LMDSENSORALIGNMENT_PNDLMDALIGNQA_H_ */
