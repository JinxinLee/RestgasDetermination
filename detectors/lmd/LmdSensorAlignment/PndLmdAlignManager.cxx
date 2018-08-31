/*
 * PndLmdAlignManager.cpp
 *
 *  Created on: May 26, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include <PndLmdAlignManager.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/ref.hpp>
#include <boost/regex.hpp>

#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>

#include "PndLmdSensorAligner.h"
#include "PndLmdHitPair.h"

#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGeoMatrix.h>

using std::cerr;
using std::cout;
using std::ifstream;
using std::map;
using std::ofstream;
using std::string;
using std::stringstream;
using std::vector;

PndLmdAlignManager::PndLmdAlignManager() {
	init();
}

void PndLmdAlignManager::init() {

	_info << "info for aligned areas\n";

	_zIsTimestamp = true;
	_allFilesAdded = false;
	_singleAligner = true;
	_inCentimeters = false;
	enableHelperMatrix = false;
	_multithreaded = false;
	_verboseLevel = 0;

	//int overlapId=-1;
	fileNames.clear();
	aligners.clear();

	PndLmdGeometryHelper *helper = &PndLmdGeometryHelper::getInstance();
	overlapIDs = helper->getAvailableOverlapIDs();
	for (size_t i = 0; i < overlapIDs.size(); i++) {
		int overlapId = overlapIDs[i];

		PndLmdSensorAligner tempAligner;
		tempAligner.setManager(this);
		tempAligner.setOverlapId(overlapId);
		tempAligner.setZasTimetamp(_zIsTimestamp);
		tempAligner.setInCentimeters(_inCentimeters);
		aligners[overlapId] = tempAligner;
	}

	// we just started, all aligners are empty
	for (size_t i = 0; i < overlapIDs.size(); i++) {
		alignersFull[overlapIDs[i]] = false;
	}

	outFilename = "";
	_firstInitDone = true;
	std::cout << "PndLmdAlignManager::Init(): Initialization successful." << "\n";
}

PndLmdAlignManager::~PndLmdAlignManager() {
}

void PndLmdAlignManager::validate() {
	std::cout << "using " << aligners.size() << " aligners, which have:\n";
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		cout << "id: " << it->second.getModuleID() << " has " << it->second.getNoOfPairs() << " pairs."
		    << "\n";
	}
}

bool PndLmdAlignManager::addFile(std::string filename) {

	if (_allFilesAdded) {
		return false;
	}
	else {
		fileNames.push_back(filename);
		return true;
	}
}

int PndLmdAlignManager::addFilesFromDirectory(std::string directory, int maxFiles) {

	if (_allFilesAdded) {
		return fileNames.size();
	}
	else {
		std::vector<string> list;
		searchFiles(directory, list, ".root", false);
		for (size_t i = 0; i < list.size(); i++) {
			fileNames.push_back(list[i]);
			if ((int) i == maxFiles - 1) {	//we use == instead of >= so that maxFiles=0 always chooses all files
				break;
			}
		}
		_allFilesAdded = true;
		cout << "looking for files in " << directory << ". choose " << fileNames.size()
		    << " files of maximum of " << maxFiles << ".\n";
		return fileNames.size();
	}

}

void PndLmdAlignManager::readFilesAndAlign() {

	_allFilesAdded = true;

	PndLmdThreadPool threadPool;

	int noOfFiles = fileNames.size();
	if (noOfFiles > 0) {
		cout << "found " << noOfFiles << " file(s). reading...\n";
	}
	else {
		cout << "no files found. exiting.\n";
		exit(0);
	}

	TChain* chainPairs = new TChain("pndsim");
	for (size_t i = 0; i < fileNames.size(); i++) {
		//cout << files[i] << endl;
		if (fileNames[i].find("Lumi_Pairs") != std::string::npos) {
			chainPairs->Add(fileNames[i].c_str());
		}
	}

	//pairs of sensors in LMD coordinates
	TClonesArray* hitPairs = new TClonesArray("PndLmdHitPair");
	chainPairs->SetBranchAddress("PndLmdHitPair", &hitPairs);
	int nEntries = chainPairs->GetEntries();
	cout << "HitPairs no of entries: " << nEntries << "\n";

	cout << "Sorting Pairs to Manager...\n";
	int totalPairs = 0;
	for (int i_event = 0; i_event < nEntries; i_event++) {

		loadBar(i_event, nEntries, 1000, 60);
		chainPairs->GetEntry(i_event);
		int nPairs = hitPairs->GetEntries();

		//loop over hitPairs per Event
		for (int i_Pair = 0; i_Pair < nPairs; i_Pair++) {
			PndLmdHitPair* currentPair = (PndLmdHitPair*) hitPairs->At(i_Pair);
			addPairAndStartAligner(*currentPair, threadPool);
			totalPairs++;
		}

		//should this be done at the end or the start of the for loop?
		if (allAlignersFull()) {
			break;
		}
	}

	//save matrices to disk
	waitForCompletion(threadPool);

	cout << "================================ \n";
	cout << "total Pairs: " << totalPairs << "\n";
	cout << "All done. Running Align Manager. \n";
	cout << "================================ \n";

	delete chainPairs;
	delete hitPairs;
}

void PndLmdAlignManager::runSensorAligner(PndLmdSensorAligner &aligner) {

	//perform first checks
	if (!aligner.check()) {
		return;
	}

	//write binary file only if not already present
	if (!checkForBinaryFiles()) {
		aligner.writePairsToBinary(binaryPairFileDirectory);
	}

	// apply dynamic cut. this changes the amount of pairs the aligner has,
	// so don't re-save the pairs after that!
	//aligner.applyDynamicCut();

	//calculate the matrix
	aligner.calculateMatrix();

	//free memory
	aligner.clearPairs();
}

void PndLmdAlignManager::loadBar(int i, int n, int r, int w, std::string message) {

	std::lock_guard<std::mutex> lock(MTLBmutex);

	// Only update r times.
	if (n == 0) return;
	if (n == 1) return;

	if (r > n) {
		r = n;
	}

	//calculate ev / sec every 100000 iterations
	if (i % (n / r) != 0) {
		return;
	}

	flush(cout);

	// Calculate the ratio of complete-to-incomplete.
	float ratio = i / (float) n;
	int c = ratio * w;

	// Show the percentage complete.
	printf("%3d%% [", (int) (ratio * 100));

	// Show the load bar.
	for (int x = 0; x < c; x++)
		printf("=");

	for (int x = c; x < w; x++)
		printf(" ");

	// ANSI Control codes to go back to the
	// previous line and clear it.
	printf("] %s %d of %d \n\033[F\033[J", message.c_str(), i, n);

}

void PndLmdAlignManager::checkIOpaths() {

	if (fileNames.size() == 0) {
		//cout << "no pair files specified, are we using binary data?\n";
	}
	for (size_t iFile = 0; iFile < fileNames.size(); iFile++) {
		if (!boost::filesystem::exists(fileNames[iFile])) {
			cout << "error opening file:";
			cout << fileNames[iFile] << "\n";
			exit(1);
		}
	}

	if (!(outFilename == "")) {
		boost::filesystem::path outPath(outFilename);
		if (!boost::filesystem::exists(outPath.parent_path())) {
			boost::filesystem::create_directories(outPath.parent_path());
		}
	}

	if (!(matrixOutDir == "")) {
		if (!boost::filesystem::exists(matrixOutDir)) {
			boost::filesystem::create_directories(matrixOutDir);
		}
	}
}

Matrix PndLmdAlignManager::readMatrix(std::string filename) {
	// avoid code duplication
	return castTGeoHMatrixToMatrix(readTGeoHMatrix(filename));
}

TGeoHMatrix PndLmdAlignManager::readTGeoHMatrix(std::string filename) {

	vector<vector<double> > temp = readFromCSVFile(filename);

	if (temp.size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		return TGeoHMatrix();
	}
	if (temp[0].size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		return TGeoHMatrix();
	}

	int rows, columns;
	rows = temp.size();
	columns = temp[0].size();
	Matrix resultMat(rows, columns);

	double rotation[9];
	double translation[3];
	TGeoHMatrix result;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			resultMat.val[i][j] = temp[i][j];
		}
	}

	// have we even read a real matrix?
	if (resultMat.m != 4 || resultMat.n != 4) {
		cerr << "Error! Invalid matrix!\n";
		return TGeoHMatrix();
	}

	// fuck it, we do it by hand
	rotation[0] = resultMat.val[0][0];
	rotation[1] = resultMat.val[0][1];
	rotation[2] = resultMat.val[0][2];
	rotation[3] = resultMat.val[1][0];
	rotation[4] = resultMat.val[1][1];
	rotation[5] = resultMat.val[1][2];
	rotation[6] = resultMat.val[2][0];
	rotation[7] = resultMat.val[2][1];
	rotation[8] = resultMat.val[2][2];

	translation[0] = resultMat.val[0][3];
	translation[1] = resultMat.val[1][3];
	translation[2] = resultMat.val[2][3];

	result.SetRotation(rotation);
	result.SetTranslation(translation);

	return result;
}

bool PndLmdAlignManager::writeMatrix(Matrix &mat, std::string filename) {

	if (!(filename == "")) {
		boost::filesystem::path outPath(filename);
		if (!boost::filesystem::exists(outPath.parent_path())) {
			boost::filesystem::create_directories(outPath.parent_path());
		}
	}

	//cout << "writing matrix " << filename << "\n";
	std::ofstream outFileStream;
	outFileStream.open(filename.c_str());

	if (outFileStream.fail()) {
		return false;
	}

	outFileStream << std::setprecision(16);
	outFileStream << mat;
	outFileStream.close();
	return true;
}

vector<vector<double> > PndLmdAlignManager::readFromCSVFile(std::string filename) {

	std::vector<std::vector<double> > data;
	ifstream ifs;
	ifs.open(filename.c_str());
	if (ifs.is_open()) {
		stringstream iss;
		string line, token;

		while (getline(ifs, line)) {
			iss << line;
			std::vector<double> tempvec;
			while (getline(iss, token, ',')) {
				tempvec.push_back(boost::lexical_cast<double>(token));
			}
			data.push_back(tempvec);
			iss.clear();
		}
		ifs.close();
	}
	else {
		throw std::runtime_error("Unable to open file " + filename);
	}

	return data;
}

int PndLmdAlignManager::searchFiles(std::string path, std::vector<std::string> &list, std::string detail,
    bool includeSubDirs) {

	if (!boost::filesystem::exists(path)) {
		return 0;
	}
	boost::filesystem::directory_iterator iterator(path);
	for (; iterator != boost::filesystem::directory_iterator(); ++iterator) {
		boost::filesystem::path thisPath = iterator->path();
		if (boost::filesystem::is_directory(thisPath) && includeSubDirs) {
			list.push_back(thisPath.string());
			searchFiles(thisPath.string(), list, detail, includeSubDirs);
		}
		else if (boost::filesystem::is_regular_file(thisPath)) {
			if (thisPath.string().find(detail) != std::string::npos) {
				list.push_back(thisPath.string());
			}
		}
	}
	sort(list.begin(), list.end());
	return list.size();
}

bool PndLmdAlignManager::mkdir(std::string path) {
	boost::filesystem::path bpath(path);
	if (boost::filesystem::exists(bpath)) {
		return true;
	}
	return boost::filesystem::create_directories(bpath);
}

bool PndLmdAlignManager::exists(std::string path) {
	boost::filesystem::path bpath(path);
	if (boost::filesystem::exists(bpath)) {
		return true;
	}
	return false;
}

vector<string> PndLmdAlignManager::findRegex(std::string source, std::string regex) {

	boost::regex expression(regex);
	std::string::const_iterator start, end;
	start = source.begin();
	end = source.end();
	boost::match_results<std::string::const_iterator> result;
	boost::match_flag_type flags = boost::match_default;

	vector<string> resultStrings;

	while (regex_search(start, end, result, expression, flags)) {
		// update search position:
		start = result[0].second;

		for (size_t j = 0; j < result.size(); j++) {
			resultStrings.push_back(boost::lexical_cast<string>(result[j]));
		}

		// update flags:
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}

	return resultStrings;
}

int PndLmdAlignManager::searchDirectories(std::string curr_directory, std::vector<std::string> &list,
    bool includeSubDirs) {

	if (!boost::filesystem::exists(curr_directory)) {
		return 0;
	}
	boost::filesystem::directory_iterator iterator(curr_directory);

	for (; iterator != boost::filesystem::directory_iterator(); ++iterator) {
		boost::filesystem::path thisPath = iterator->path();
		if (boost::filesystem::is_directory(thisPath)) {
			list.push_back(thisPath.string());

			//recursively call for sub directories
			if (includeSubDirs) {
				searchDirectories(thisPath.string(), list, includeSubDirs);
			}
		}
	}
	sort(list.begin(), list.end());
	return list.size();

}

void PndLmdAlignManager::setInCentimeters(bool inCentimeters) {
	_inCentimeters = inCentimeters;
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		it->second.setInCentimeters(_inCentimeters);
	}
}

void PndLmdAlignManager::setZasTimestamp(bool timestamp) {
	_zIsTimestamp = timestamp;
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		it->second.setZasTimetamp(_zIsTimestamp);
	}
}

Matrix PndLmdAlignManager::castTGeoHMatrixToMatrix(const TGeoHMatrix& matrix) {

	//allocate memory for matrix elements
	double* homogenousMatrix = new double[16];
	double* finalMatrix = new double[16];
	matrix.GetHomogenousMatrix(homogenousMatrix);

	/*
	 * this code was tested on 2016-08-29 and works. The root implementation is still not
	 * according to ROOT documentation, in that the translation parts of a matrix are not
	 * where they should be and sometimes the homogenous coordinate is not set to 1.
	 * This fixes that.
	 *
	 * DO NOT REMOVE THESE COMMENTS!
	 *
	 * When in doubt, refer to the
	 * following code example, that shows where the translations (wrongfully) is:
	 */
	/*

	 //test tgeohmatrices
	 TGeoHMatrix *h1 = new TGeoHMatrix(TGeoTranslation(1,2,3));
	 TGeoHMatrix *h2 = new TGeoHMatrix();
	 TGeoHMatrix *h3 = new TGeoHMatrix();
	 TGeoHMatrix *h4 = new TGeoHMatrix();

	 h2->SetDx(1.); h2->SetDy(2.); h2->SetDz(3.);
	 double tr[3] = {1.,2.,3.};
	 h3->SetTranslation(tr);
	 const double rot[9] = {0,1,0,-1,0,0,0,0,0};
	 h4->SetRotation(rot);

	 Matrix m1 = castTGeoHMatrixToMatrix(*h1);
	 Matrix m2 = castTGeoHMatrixToMatrix(*h2);
	 Matrix m3 = castTGeoHMatrixToMatrix(*h3);
	 Matrix m4 = castTGeoHMatrixToMatrix(*h4);

	 cout << "m1:\n" << m1 << "\n";
	 cout << "m2:\n" << m2 << "\n";
	 cout << "m3:\n" << m3 << "\n";
	 cout << "m4:\n" << m4 << "\n";
	 */

	//copy values from the wrong to the correct positions
	finalMatrix[0] = homogenousMatrix[0];
	finalMatrix[1] = homogenousMatrix[1];
	finalMatrix[2] = homogenousMatrix[2];
	finalMatrix[3] = homogenousMatrix[12];
	finalMatrix[4] = homogenousMatrix[4];
	finalMatrix[5] = homogenousMatrix[5];
	finalMatrix[6] = homogenousMatrix[6];
	finalMatrix[7] = homogenousMatrix[13];
	finalMatrix[8] = homogenousMatrix[8];
	finalMatrix[9] = homogenousMatrix[9];
	finalMatrix[10] = homogenousMatrix[10];
	finalMatrix[11] = homogenousMatrix[14];
	finalMatrix[12] = homogenousMatrix[3];
	finalMatrix[13] = homogenousMatrix[7];
	finalMatrix[14] = homogenousMatrix[11];
	finalMatrix[15] = 1.0;

	//create matrix and clean up
	Matrix result(4, 4, finalMatrix);
	delete homogenousMatrix;
	delete finalMatrix;
	return result;

}

bool PndLmdAlignManager::checkForBinaryFiles() {

	vector<string> files;
	searchFiles(binaryPairFileDirectory, files, "bin", false);
	int foundFiles = 0;

	//no binary files at all!
	if (files.size() == 0) {
		return false;
	}

	string matrixName;
	bool tempfilefound = false;

	//check for every ID that should be there if there is a corresponding file
	for (size_t i = 0; i < overlapIDs.size(); i++) {

		tempfilefound = false;
		matrixName = makeBinaryPairFileName(overlapIDs[i], _inCentimeters);
		for (size_t j = 0; j < files.size(); j++) {
			if (files[j].find(matrixName) != string::npos) {
				tempfilefound = true;
				foundFiles++;
			}
		}

		//file not found? then at least one is missing, return false
		if (!tempfilefound) {
			return tempfilefound;
		}
	}
	return true;
}

bool PndLmdAlignManager::checkForLmdMatrixFiles() {

	vector<string> files;
	searchFiles(binaryPairFileDirectory, files, "mat", false);
	int foundFiles = 0;

	//no binary files at all!
	if (files.size() == 0) {
		return false;
	}

	string matrixName1, matrixName2;
	bool tempfilefound = false;

	//check for every ID that should be there if there is a corresponding file
	for (size_t i = 0; i < overlapIDs.size(); i++) {

		//reset counter
		tempfilefound = false;
		matrixName1 = makeMatrixFileName(overlapIDs[i], true);
		matrixName1 = makeMatrixFileName(overlapIDs[i], false);

		for (size_t j = 0; j < files.size(); j++) {
			if (files[j].find(matrixName1) != string::npos) {
				//file is present
				tempfilefound = true;
				foundFiles++;
			}
		}

		//file not found? then at least one is missing, return false
		if (!tempfilefound) {
			return tempfilefound;
		}
	}
	return true;
}

std::string PndLmdAlignManager::makeBinaryPairFileName(int overlapId, bool incentimeters) {
	std::stringstream filename;
	filename << "/pairs-" << overlapId;
	incentimeters ? filename << "-cm.bin" : filename << "-px.bin";
	return filename.str();
}

std::string PndLmdAlignManager::makeMatrixFileName(int overlapId, bool incentimeters) {
	stringstream matrixName;
	matrixName << "/m";
	incentimeters ? matrixName << overlapId << "cm.mat" : matrixName << overlapId << "px.mat";
	return matrixName.str();
}

void PndLmdAlignManager::setMaxPairs(int maxPairs) {

	if (maxPairs > 0) {
		for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
			it->second.setMaximumNumberOfHitPairs(maxPairs);
		}
		return;
	}
	else {
		cout << "warning. max pairs must be larger than 0!\n";
		return;
	}

}

void PndLmdAlignManager::clearScreen() {
	cout << "\x1B[2J\x1B[H";
}

bool PndLmdAlignManager::addPairAndStartAligner(PndLmdHitPair &pair, PndLmdThreadPool &threadPool) {

	bool pairAdded = false;

	int thisID = pair.getOverlapId();

	// check if the aligner for that pair is full. if yes, skip this pair.
	// do this even before checking that pair, saves on cpu time.
	if (alignersFull[thisID]) {
		return false;
	}

	pair.check();
	if (pair.isSane()) {
		pairAdded = aligners[thisID].addSimplePair(pair);  //returns true if addPair succeeded
		alignersFull[thisID] = !pairAdded;		//if addPair failed, the aligner is full
	}
	else {
		verbosePrint("pair is not sane. processing failed.\n", 1);
	}

	//if aligner is full, start thread directly.
	if (alignersFull[thisID]) {
		//alignerThreadGroup.create_thread(
		threadPool.enqueue(
		    boost::bind(&PndLmdAlignManager::runSensorAligner, this, boost::ref(aligners[thisID])));
	}
	return true;
}

bool PndLmdAlignManager::allAlignersFull() {
	//check if all aligners are done
	for (auto &id : overlapIDs) {
		if (!alignersFull[id]) {
			return false;
		}
	}
	verbosePrint("all aligners are already full. no further files will be read.\n", 1);
	return true;
}

void PndLmdAlignManager::waitForCompletion(PndLmdThreadPool &threadPool) {

	//start all alignsers that have not already started (i.e. don't have required no of Pairs)
	int notStarted = 0;

	cout << "starting remaining aligners.\n";
	for (auto &id : overlapIDs) {

		//onlt start aligners that have not already started
		if (!(alignersFull[id])) {

			//if pair could not be added, aligner is full. start thread directly.
			//alignerThreadGroup.create_thread(
			threadPool.enqueue(
			    boost::bind(&PndLmdAlignManager::runSensorAligner, this, boost::ref(aligners[id])));
			notStarted++;
		}
	}
	cout << notStarted << " aligners remained.\n";
	//cout << "jobs queue size : " << alignerThreadGroup.size() << "/360\n";
	cout << "waiting for all aligners to finish...";

	//wait for all threads to complete
	//alignerThreadGroup.join_all();
	flush(cout);
	threadPool.wait();

	cout << "done!\n";

	for (auto &id : overlapIDs) {
		if (aligners[id].successful()) {

			Matrix result = aligners[id].getResultMatrix();
			string matrixFilename = matrixOutDir + makeMatrixFileName(id, _inCentimeters);

			if (!writeMatrix(result, matrixFilename)) {
				cout << "ERROR: could not write matrix " << matrixFilename << "\n";
			}

			_info << "aligner " << id << ":\n";
			_info << "no of pairs: " << aligners[id].getNoOfPairs() << "\n";
			_info << "\n";
		}
		else {
			cout << "Error: aligner for " << id << " failed.\n";
		}
	}

	//write matrix info
	ofstream of;
	if (_inCentimeters) {
		of.open((matrixOutDir + "/info-cm.txt").c_str());
	}
	else {
		of.open((matrixOutDir + "/info-px.txt").c_str());
	}

	of << _info.str();
	of.close();
	cout << "all aligners done.\n";
}

bool PndLmdAlignManager::writePairsToBinaryFiles() {

	if (binaryPairFileDirectory == "") {
		cout << "error: binary pair file directory not set.\n";
		cout << "use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur = 0;
	tot = aligners.size();

	cout << "writing all pairs to binary files\n";
	mkdir(binaryPairFileDirectory);

	//maybe do this multithreaded?
	for (auto &aligner : aligners) {
		loadBar(cur++, tot, 1000, 60);
		if (!aligner.second.writePairsToBinary(binaryPairFileDirectory)) {
			return false;
		}
	}
	return true;
}

bool PndLmdAlignManager::readPairsFromBinaryFilesAndAlign() {

	bool success = true;

	if (binaryPairFileDirectory == "") {
		cout << "error: binary pair file directory not set.\n";
		cout << "use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur = 0;
	tot = aligners.size();

	cout << "reading all pairs from binary files and starting aligners...\n";

	// use single thread as long as gGeoManger / GeometryHelper are not thread safe!
	bool mt = true;
	PndLmdThreadPool threadPool(maxThreads);

	for (auto &id : overlapIDs) {

		if (!mt) {
			// single threaded, low memory consumption version
			loadBar(cur++, tot, 1000, 60);
			PndLmdSensorAligner &thisAligner = aligners[id];
			if (thisAligner.readPairsFromBinary(binaryPairFileDirectory)) {
				runSensorAligner(thisAligner);
			}
		}
		else {
			loadBar(cur++, tot, 1000, 60);
			PndLmdSensorAligner &thisAligner = aligners[id];

			if (thisAligner.readPairsFromBinary(binaryPairFileDirectory)) {
				alignersFull[id] = true;
				threadPool.enqueue(
				    boost::bind(&PndLmdAlignManager::runSensorAligner, this, boost::ref(thisAligner)));
			}
			else {
				success = false;
			}
		}
	}

	// write matrices to disk etc
	waitForCompletion(threadPool);
	return success;
}

void PndLmdAlignManager::verbosePrint(std::string input, int level) {
	if (_verboseLevel >= level) {
		cout << input;
	}
}

boost::property_tree::ptree PndLmdAlignManager::readConfigFile(std::string filename) {

	//check if file exists
	if (!boost::filesystem::exists(filename)) {
		cerr << "PndLmdAlignManager::readConfig: ERROR! File " << filename << " not found!\n";
	}

	std::ifstream is(filename);
	boost::property_tree::ptree root;
	try {
		boost::property_tree::read_json(is, root);
	}
	catch (std::exception &e) {
		cerr << "PndLmdAlignManager::readConfig: ERROR! Can't parse json file " << filename << ".\n";
	}
	return root;
}

bool PndLmdAlignManager::writeConfigFile(boost::property_tree::ptree configTree, std::string filename,
    bool replaceExisting) {

	if (boost::filesystem::exists(filename) && !replaceExisting) {
		cerr << "PndLmdAlignManager::writeConfig: Config already exists, will not be replaced.\n";
		return false;
	}

	boost::filesystem::path outPath(filename);
	if (!boost::filesystem::exists(outPath.parent_path())) {
		boost::filesystem::create_directories(outPath.parent_path());
	}

	std::ofstream os(filename);
	boost::property_tree::write_json(os, configTree);
	return true;
}

void PndLmdAlignManager::clearPairs() {
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		it->second.clearPairs();
	}
}
