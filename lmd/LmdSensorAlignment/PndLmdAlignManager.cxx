/*
 * PndLmdAlignManager.cpp
 *
 *  Created on: May 26, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include <PndLmdAlignManager.h>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/ref.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <PndLmdSensorAligner.h>
#include <PndLmdHitPair.h>
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

boost::mutex incrementMutex;

boost::thread_group alignerThreadGroup;

void PndLmdAlignManager::resetMTLB(int n, int r, int w) {
	_i = 0;
	_n = n;
	_r = r;
	_w = w;
}

void PndLmdAlignManager::incrementMTLB() {

	incrementMutex.lock();
	_i++;

	// Only update r times.
	if (_n == 0)
		return;
	if (_n == 1)
		return;

	if (_r > _n) {
		_r = _n;
	}

	//calculate ev / sec every 100000 iterations
	if (_i % (_n / _r) != 0) {
		return;
	}

	flush(cout);

	// Calculate the ratio of complete-to-incomplete.
	float ratio = _i / (float) _n;
	int c = ratio * _w;

	// Show the percentage complete.
	printf("%3d%% [", (int) (ratio * 100));

	// Show the load bar.
	for (int x = 0; x < c; x++)
		printf("=");

	for (int x = c; x < _w; x++)
		printf(" ");

	// ANSI Control codes to go back to the
	// previous line and clear it.
	printf("] %d of %d \n\033[F\033[J", _i, _n);
	incrementMutex.unlock();
}

PndLmdAlignManager::PndLmdAlignManager() {
	init();
}

void PndLmdAlignManager::init() {

	_info << "info for aligned areas\n";

	_zIsTimestamp = true;
	_allFilesAdded = false;
	useSimpleStorage = true;
	_singleAligner = true;
	_inCentimeters = false;
	_enableHelperMatrix = false;
	_multithreaded = true;

	//int overlapId=-1;
	fileNames.clear();
	aligners.clear();

	helper = &PndLmdGeometryHelper::getInstance();

	vector<int> overlapIDs = helper->getAvailableOverlapIDs();
	for (size_t i = 0; i < overlapIDs.size(); i++) {
		int overlapId = overlapIDs[i];

		PndLmdSensorAligner tempAligner;
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

bool PndLmdAlignManager::addPair(PndLmdHitPair& pair) {

	bool success = false;
	// check if the aligner for that pair is full. if yes, skip this pair.
	// do this even before checking that pair, saves on cpu time.
	if (alignersFull[pair.getOverlapId()]) {
		return false;
	}

	pair.check();
	if (pair.isSane()) {
		if (useSimpleStorage) {
			success = aligners[pair.getOverlapId()].addSimplePair(pair);//returns true if addPair succeeded
			alignersFull[pair.getOverlapId()] = !success;		//if addPair failed, the aligner is full
		} else {
			//_aligners[pair.getOverlapId()].addPair(pair);
			cout << "WARNING! Legacy storage mode is no longer supported.";
		}
	} else {
		cout << "pair is not sane. processing failed.\n";
		success = false;
	}

	return success;
}

bool PndLmdAlignManager::addPairAndStartAligner(PndLmdHitPair &pair) {

	bool success = false;

	// check if the aligner for that pair is full. if yes, skip this pair.
	// do this even before checking that pair, saves on cpu time.
	if (alignersFull[pair.getOverlapId()]) {
		return false;
	}

	pair.check();
	if (pair.isSane()) {
		if (useSimpleStorage) {
			success = aligners[pair.getOverlapId()].addSimplePair(pair);//returns true if addPair succeeded
			alignersFull[pair.getOverlapId()] = !success;		//if addPair failed, the aligner is full
		} else {
			//_aligners[pair.getOverlapId()].addPair(pair);
			cout << "WARNING! Legacy storage mode is no longer supported.";
		}
	} else {
		cout << "pair is not sane. processing failed.\n";
	}

	//if pair could not be added, aligner is full. start thread directly.
	if (!success) {
		alignerThreadGroup.create_thread(
		        boost::bind(&PndLmdAlignManager::alignOne, this,
		                boost::ref(aligners[pair.getOverlapId()])));
	}

	//check if all aligners are done
	allAlignersDone = true;
	for (map<int, bool>::iterator it = alignersFull.begin(); it != alignersFull.end(); it++) {
		if (!(it->second)) {
			allAlignersDone = false;
			break;
		}
	}
	if (allAlignersDone) {
		cout << "all aligners are already full. no further files will be read.\n";
	}

	return success;
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
	} else {
		fileNames.push_back(filename);
		return true;
	}
}

int PndLmdAlignManager::addFilesFromDirectory(std::string directory, int maxFiles) {

	if (_allFilesAdded) {
		return fileNames.size();
	} else {
		std::vector<string> list;
		searchFiles(directory, list, ".root", false);
		for (size_t i = 0; i < list.size(); i++) {
			fileNames.push_back(list[i]);
			if ((int) i == maxFiles - 1) {//we use == instead of >= so that maxFiles=0 always chooses all files
				break;
			}
		}
		_allFilesAdded = true;
		cout << "looking for files in " << directory << ". choose " << fileNames.size()
		        << " files of maximum of " << maxFiles << ".\n";
		return fileNames.size();
	}

}

void PndLmdAlignManager::readFiles() {

	_allFilesAdded = true;

	int noOfFiles = fileNames.size();
	if (noOfFiles > 0) {
		cout << "found " << noOfFiles << " file(s). reading...\n";
	} else {
		cout << "no files found. exiting.\n";
		exit(0);
	}

	TChain* chainPairs = new TChain("cbmsim");
	for (size_t i = 0; i < fileNames.size(); i++) {
		//cout << files[i] << "\n";
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
			addPair(*currentPair);
			totalPairs++;
		}
	}

	cout << "================================ \n";
	cout << "total Pairs: " << totalPairs << "\n";
	cout << "All done. Running Align Manager. \n";
	cout << "================================ \n";

	delete chainPairs;
	delete hitPairs;
}

void PndLmdAlignManager::readFilesAndAlign() {

	_allFilesAdded = true;

	int noOfFiles = fileNames.size();
	if (noOfFiles > 0) {
		cout << "found " << noOfFiles << " file(s). reading...\n";
	} else {
		cout << "no files found. exiting.\n";
		exit(0);
	}

	TChain* chainPairs = new TChain("cbmsim");
	for (size_t i = 0; i < fileNames.size(); i++) {
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
			addPairAndStartAligner(*currentPair);
			totalPairs++;
			if (allAlignersDone) {
				return;
			}
		}
	}

	cout << "================================ \n";
	cout << "total Pairs: " << totalPairs << "\n";
	cout << "All done. Running Align Manager. \n";
	cout << "================================ \n";

	delete chainPairs;
	delete hitPairs;
}

void PndLmdAlignManager::alignOne(PndLmdSensorAligner &aligner) {

	//start aligner, this can be done concurrently
	aligner.calculateMatrix();

	//write binary file only if not already present
	if (!checkForBinaryFiles()) {
		aligner.writePairsToBinary(_binaryPairFileDirectory);
	}
	//if binary files already present, then they have been read earliert and we can display a progress bar for the aliners
	else {
		incrementMTLB();
	}

	//free memory
	aligner.clearPairs();
}

void WorkerThread(boost::shared_ptr<boost::asio::io_service> io_service) {
	io_service->run();
}

void PndLmdAlignManager::alignST() {

	int cur, tot;
	cur = 0;
	tot = aligners.size();

	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		loadBar(cur++, tot, 1000, 60);
		it->second.calculateMatrix();
		if (it->second.successful()) {
			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir
			        + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters);
			writeMatrix(result, matrixFilename);

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";

		} else {
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
}

void PndLmdAlignManager::alignMT() {

	//new version, multi threaded using thread pool model and boost::asio implementation

	//shared pointer, since io_services can't be copied
	boost::shared_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
	boost::shared_ptr<boost::asio::io_service::work> work(
	        new boost::asio::io_service::work(*io_service));

	//thread group
	boost::thread_group worker_threads;

	//make threads, n is number of threads:
	int nThreads;
	nThreads = boost::thread::hardware_concurrency();

	//sometimes hardware_concurrency returns 0 if it can't detect.
	if (nThreads < 1) {
		cout << "INFO:: could not detect number of cores. assuming 4.\n";
		nThreads = 4;
	}

	//create worker threads
	for (int i = 0; i < nThreads; i++) {
		worker_threads.create_thread(boost::bind(&WorkerThread, io_service));
	}

	resetMTLB(aligners.size(), aligners.size(), 60);

	//post work for threads
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {

		/*
		 * when binding member classes, boost::bind needs the namespace AND the pointer to an object
		 * of that class (here: this-pointer). Also, when using references, use boost::ref()
		 */
		io_service->post(boost::bind(&PndLmdAlignManager::alignOne, this, boost::ref(it->second)));
	}

	//wait for all threads to complete, then and only then write matrices to disk
	work.reset();
	worker_threads.join_all();

	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		if (it->second.successful()) {

			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir
			        + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters);

			if (!writeMatrix(result, matrixFilename)) {
				cout << "ERROR: could not write matrix " << matrixFilename << "\n";
			}

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";
		} else {
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
}

void PndLmdAlignManager::alignAllSensors() {

	if (!_inCentimeters && _enableHelperMatrix) {
		_enableHelperMatrix = false;
		cerr << "WARNING! You can't use rotation correction in pixel ICP mode!\n";
	}

	checkIOpaths();
	cout << "starting aligners...\n";

	if (_multithreaded) {
		alignMT();
	} else {
		alignST();
	}

	//write matrix info
	ofstream of;
	if (_inCentimeters) {
		of.open((_matrixOutDir + "/info-cm.txt").c_str());
	} else {
		of.open((_matrixOutDir + "/info-px.txt").c_str());
	}

	of << _info.str();
	of.close();
	cout << "all aligners done.\n";
}

//these parameters depend on sensor geometry. Use with caution!
//Matrix PndLmdAlignManager::transformMatrixFromPixelsToCm(const Matrix &input) {
//
//	/*
//	 * in principle, there should also be a matrix that transforms from sensor active to sensor,
//	 * but inactive area is already accounted for in pixelsX and pixelsY!
//	 */
//
//	//very important, because I still don't know how pixel number corresponds to active area
//	double pixelsX = 247.5;			//guard ring removes part of active area
//	double pixelsY = 242.5;			//guard ring removes part of active area
//	double pixelSize = 80e-4;		//in cm!
//
//	//center of pixel correction matrices
//	Matrix matrixPixelEdgeToCenter;
//	Matrix matrixActiveEdgeToCenter;
//	Matrix matrixScaleActiveToSensor;
//	Matrix newPixelsToSensorInCm, newSensorInCmToPixels;
//
//	//measure pixels in their center, not their corner
//	matrixPixelEdgeToCenter = Matrix::eye(4);
//	matrixPixelEdgeToCenter.val[0][3] += 0.5;
//	matrixPixelEdgeToCenter.val[1][3] += 0.5;
//
//	//move from sensor edge (pixel [0,0] to center of sensor)
//	//KEEP IN MIND! This matrix also takes inactive area into account!!
//	matrixActiveEdgeToCenter = Matrix::eye(4);
//	matrixActiveEdgeToCenter.val[0][3] -= pixelsX / 2;
//	matrixActiveEdgeToCenter.val[1][3] -= pixelsY / 2;
//
//	//scaling must be applied after translations
//	matrixScaleActiveToSensor = Matrix::eye(4);
//	matrixScaleActiveToSensor.val[0][0] = pixelSize;
//	matrixScaleActiveToSensor.val[1][1] = pixelSize;
//
//	newPixelsToSensorInCm = matrixScaleActiveToSensor * matrixActiveEdgeToCenter
//	        * matrixPixelEdgeToCenter;
//	newSensorInCmToPixels = Matrix(newPixelsToSensorInCm);
//	newSensorInCmToPixels.inv();
//
//	Matrix result;
//	result = newPixelsToSensorInCm * input * newSensorInCmToPixels;
//	return result;
//}

/*
 * get transformation matrix of fromSensor -> toSensor (PANDA global reference frame)
 * this is in cm and IN PANDA GLOBAL.
 * ATTENTION! set aligned flag true for perfect geometry, else use false!
 */
Matrix PndLmdAlignManager::getMatrixSensorToSensor(int fromSensor, int toSensor) {

	auto matrixSen1ToLmd = helper->getMatrixSensorToPndGlobal(fromSensor);
	auto matrixLmdToSen2 = helper->getMatrixPndGlobalToSensor(toSensor);

	Matrix matSen1ToLmd = castTGeoHMatrixToMatrix(matrixSen1ToLmd);
	Matrix matLmdToSen2 = castTGeoHMatrixToMatrix(matrixLmdToSen2);
	return matLmdToSen2 * matSen1ToLmd;
}

void PndLmdAlignManager::loadBar(int i, int n, int r, int w, std::string message) {

	// Only update r times.
	if (n == 0)
		return;
	if (n == 1)
		return;

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

	if (!(_matrixOutDir == "")) {
		if (!boost::filesystem::exists(_matrixOutDir)) {
			boost::filesystem::create_directories(_matrixOutDir);
		}
	}
}

//FIXME: give reference, not pointer
//read contents of specified file, appends to stringstream provided, returns no of lines
std::stringstream* PndLmdAlignManager::readFile(std::string filename) {

	int lines = 0;
	stringstream *valueStream = new stringstream();

	ifstream ifs;
	ifs.open(filename.c_str());
	if (!ifs.is_open()) {
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << "\n";
		return NULL;
	}
	string linebuffer;	// = new string();
	while (std::getline(ifs, linebuffer)) {
		*valueStream << linebuffer << "\n";
		lines++;
	}
	ifs.close();

	return valueStream;
}

Matrix PndLmdAlignManager::readMatrix(std::string filename) {

	vector<vector<double> > temp = readFromCSVFile(filename);

	if (temp.size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		exit(1);
	}
	if (temp[0].size() < 1) {
		cout << "warning! can't read matrix from file " << filename << "\n";
		exit(1);
	}

	int rows, columns;
	rows = temp.size();
	columns = temp[0].size();
	Matrix result(rows, columns);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			result.val[i][j] = temp[i][j];
		}
	}

	return result;
}

//TODO: Error reporting to log and handling
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
	stringstream *ss;
	stringstream iss;
	int i = 0;
	ss = readFile(filename);
	string line, token;
	std::vector<std::vector<double> > data;
	while (getline(*ss, line)) {
		iss << line;
		std::vector<double> tempvec;
		while (getline(iss, token, ',')) {
			tempvec.push_back(boost::lexical_cast<double>(token));
		}
		data.push_back(tempvec);
		++i;
		iss.clear();
	}
	ss->str("");
	delete ss;
	iss.str("");
	return data;
}

int PndLmdAlignManager::searchFiles(std::string path, std::vector<std::string> &list, std::string detail,
        bool includeSubDirs) {

	if (!boost::filesystem::exists(path)) {
		return 0;
	}
	boost::filesystem::directory_iterator iterator(path);
	for (; iterator != boost::filesystem::directory_iterator(); ++iterator) {
		if (boost::filesystem::is_directory(iterator->path()) && includeSubDirs) {
			list.push_back(iterator->path().string());
			searchFiles(iterator->path().string(), list, detail, includeSubDirs);
		} else if (boost::filesystem::is_regular_file(iterator->path())) {
			if (iterator->path().string().find(detail) != std::string::npos) {
				list.push_back(iterator->path().string());
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

//TODO: replcae with c++11 regex instead of boost
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
		if (boost::filesystem::is_directory(iterator->path())) {
			list.push_back(iterator->path().string());

			//recursively call for sub directories
			if (includeSubDirs) {
				searchDirectories(iterator->path().string(), list, includeSubDirs);
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
	//finalMatrix[15] = homogenousMatrix[15];
	finalMatrix[15] = 1.0;

	//create matrix and clean up
	Matrix result(4, 4, finalMatrix);
	delete homogenousMatrix;
	delete finalMatrix;
	return result;

}

bool PndLmdAlignManager::checkForBinaryFiles() {

	//list all IDs that SHOULD be there
	vector<int> availableIds = helper->getAvailableOverlapIDs();

	vector<string> files;
	searchFiles(_binaryPairFileDirectory, files, "bin", false);
	int foundFiles = 0;

	//no binary files at all!
	if (files.size() == 0) {
		return false;
	}

	string matrixName;
	bool tempfilefound = false;

	//check for every ID that should be there if there is a corresponding file
	for (size_t i = 0; i < availableIds.size(); i++) {

		tempfilefound = false;
		matrixName = makeBinaryPairFileName(availableIds[i], _inCentimeters);
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

	//list all IDs that SHOULD be there
	vector<int> availableIds = helper->getAvailableOverlapIDs();

	vector<string> files;
	searchFiles(_binaryPairFileDirectory, files, "mat", false);
	int foundFiles = 0;

	//no binary files at all!
	if (files.size() == 0) {
		return false;
	}

	string matrixName1, matrixName2;
	bool tempfilefound = false;

	//check for every ID that should be there if there is a corresponding file
	for (size_t i = 0; i < availableIds.size(); i++) {

		//reset counter
		tempfilefound = false;
		matrixName1 = makeMatrixFileName(availableIds[i], true);
		matrixName1 = makeMatrixFileName(availableIds[i], false);

		for (size_t j = 0; j < files.size(); j++) {
			if (files[j].find(matrixName1) != string::npos) {
				//file is present
				//cout << "file: " << files[j] << ", id: " << availableIds[i] << "\n";
				tempfilefound = true;
				foundFiles++;
			}
		}

		//file not found? then at least one is missing, return false
		if (!tempfilefound) {
			return tempfilefound;
		}
	}
	//cout << "found " << foundFiles << "\n";
	return true;
}

std::string PndLmdAlignManager::makeBinaryPairFileName(int overlapId, bool incentimeters) {
	std::stringstream filename;
	filename << "/pairs-";
	filename << overlapId;
	if (incentimeters) {
		filename << "-cm";
	} else {
		filename << "-px";
	}
	filename << ".bin";
	return filename.str();
}

//std::string PndLmdAlignManager::makeBinaryPairFileName(int sensorOne, int sensorTwo, bool incentimeters) {
//	int overlapId = helper->getOverlapIdFromSensorIDs(sensorOne, sensorTwo);
//	return makeBinaryPairFileName(overlapId, incentimeters);
//}

std::string PndLmdAlignManager::makeMatrixFileName(int overlapId, bool incentimeters) {
	stringstream matrixName;
	matrixName << "/m";
	if (incentimeters) {
		matrixName << overlapId << "cm.mat";
	} else {
		matrixName << overlapId << "px.mat";
	}
	return matrixName.str();
}

Matrix PndLmdAlignManager::combineCyclicMatrix(int id) {

	Matrix result = Matrix::eye(4);

	// get id of first sensor on module
	int id1 = (std::floor(id / 10.0)) * 10;
	int id2 = id % 10;

	//FIXME: assign matrices, this is shuddy atm. source this out to pndlmddim.
	string m05f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 0, id1 + 5), _inCentimeters);
	string m18f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 1, id1 + 8), _inCentimeters);
	string m28f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 2, id1 + 8), _inCentimeters);
	string m29f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 2, id1 + 9), _inCentimeters);
	string m36f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 6), _inCentimeters);
	string m37f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 7), _inCentimeters);
	string m38f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 8), _inCentimeters);
	string m47f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 4, id1 + 7), _inCentimeters);
	string m49f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 4, id1 + 9), _inCentimeters);

	// we have to know these matrices from external measurements, so it's okay to use misaligned matrices here
	Matrix m01 = getMatrixSensorToSensor(id1, id1 + 1);
	Matrix m56 = getMatrixSensorToSensor(id1 + 5, id1 + 6);

	// remember, CM matrices are in LMD local, px matrices are in sensor local!
	// and since we know those from external measurements, we have those
	//FIXME: these functions are missing, so the code does not work right now!
//	if (!_inCentimeters) {
//		transformFromLmdLocalToSensor(m01, id1 + 0, aligned);
//		transformFromLmdLocalToSensor(m56, id1 + 5, aligned);
//	}

	Matrix m05 = readMatrix(m05f);
	Matrix m18 = readMatrix(m18f);
	Matrix m28 = readMatrix(m28f);
	Matrix m29 = readMatrix(m29f);
	Matrix m36 = readMatrix(m36f);
	Matrix m37 = readMatrix(m37f);
	Matrix m38 = readMatrix(m38f);
	Matrix m47 = readMatrix(m47f);
	Matrix m49 = readMatrix(m49f);

	// I think all px matrices are inverted
	if (!_inCentimeters) {
		m01.inv();
		m56.inv();
		m05.inv();
		m18.inv();
		m28.inv();
		m29.inv();
		m36.inv();
		m37.inv();
		m38.inv();
		m47.inv();
		m49.inv();
	}

	// since we read only correction matrices in cm, we must multiply them
	// with the ideal senToSen to get the complete misaligned senToSen.
	// this isn't cheating as we are only using ideal matrices, which we should
	// always have.
	// but remember, they still live on separate modules.
	if (_inCentimeters) {

		Matrix m05ideal = getMatrixSensorToSensor(id1 + 0, id1 + 5);
		Matrix m18ideal = getMatrixSensorToSensor(id1 + 1, id1 + 8);
		Matrix m28ideal = getMatrixSensorToSensor(id1 + 2, id1 + 8);
		Matrix m29ideal = getMatrixSensorToSensor(id1 + 2, id1 + 9);
		Matrix m36ideal = getMatrixSensorToSensor(id1 + 3, id1 + 6);
		Matrix m37ideal = getMatrixSensorToSensor(id1 + 3, id1 + 7);
		Matrix m38ideal = getMatrixSensorToSensor(id1 + 3, id1 + 8);
		Matrix m47ideal = getMatrixSensorToSensor(id1 + 4, id1 + 7);
		Matrix m49ideal = getMatrixSensorToSensor(id1 + 4, id1 + 9);

		//FIXME: this is a work around since I can't yet construct the correct misaligned matrices
//		realignMatrixInLmd(m05ideal, id1 + 0);
//		realignMatrixInLmd(m18ideal, id1 + 1);
//		realignMatrixInLmd(m28ideal, id1 + 2);
//		realignMatrixInLmd(m29ideal, id1 + 2);
//		realignMatrixInLmd(m36ideal, id1 + 3);
//		realignMatrixInLmd(m37ideal, id1 + 3);
//		realignMatrixInLmd(m38ideal, id1 + 3);
//		realignMatrixInLmd(m47ideal, id1 + 4);
//		realignMatrixInLmd(m49ideal, id1 + 4);

		m05 = m05 * m05ideal;
		m18 = m18 * m18ideal;
		m28 = m28 * m28ideal;
		m29 = m29 * m29ideal;
		m36 = m36 * m36ideal;
		m37 = m37 * m37ideal;
		m38 = m38 * m38ideal;
		m47 = m47 * m47ideal;
		m49 = m49 * m49ideal;
	}

	//prepare inverted matrices
	Matrix m10 = m01.inv(m01);
	Matrix m50 = m05.inv(m05);
	Matrix m65 = m56.inv(m56);
	Matrix m81 = m18.inv(m18);
	Matrix m82 = m28.inv(m28);
	Matrix m92 = m29.inv(m29);
	Matrix m63 = m36.inv(m36);
	Matrix m73 = m37.inv(m37);
	Matrix m83 = m38.inv(m38);
	Matrix m74 = m47.inv(m47);
	Matrix m94 = m49.inv(m49);

	// case switch, conctruct all cyclic matrices
	switch (id2) {
	case 0:
		result = m10 * m81 * m38 * m63 * m56 * m05;		//uses hand-measured matrices m01 and m56
		break;
	case 1:
		result = m81 * m38 * m73 * m47 * m94 * m29 * m82 * m18;	//FIXME: change to other path with self inverse
		break;
	case 2:
		result = m82 * m38 * m73 * m47 * m94 * m29;
		break;
	case 3:
		result = m83 * m28 * m92 * m49 * m74 * m37;
		break;
	case 4:
		result = m94 * m29 * m82 * m38 * m73 * m47;
		break;
	case 5:
		result = m05 * m10 * m81 * m38 * m63 * m56;		//uses hand-measured matrices m01 and m56
		break;
	case 6:
		result = m36 * m83 * m28 * m92 * m49 * m74 * m37 * m63;	//FIXME: change to other path with self inverse
		break;
	case 7:
		result = m47 * m94 * m29 * m82 * m38 * m73;
		break;
	case 8:
		result = m38 * m73 * m47 * m94 * m29 * m82;
		break;
	case 9:
		result = m29 * m82 * m38 * m73 * m47 * m94;
		break;
	}

	return result;
}

Matrix PndLmdAlignManager::combineMatrix(int id1, int id2) {

	bool success = false;

	//FIXME: this is not the correct way to do this. better use the real int values below
	if (id1 > id2) {
		std::swap(id1, id2);
	}

	//what module are we on? are id1 and id2 on same module?
	int fhalf, fplane, fmodule, fside;
	int bhalf, bplane, bmodule, bside;

	//dimension->Get_sensor_by_id(id1, fhalf, fplane, fmodule, fside, fdie, fsensor);
	//dimension->Get_sensor_by_id(id2, bhalf, bplane, bmodule, bside, bdie, bsensor);

	auto infoOne = helper->getHitLocationInfo(id1);
	auto infoTwo = helper->getHitLocationInfo(id2);

	fhalf = infoOne.detector_half;
	bhalf = infoTwo.detector_half;

	fplane = infoOne.plane;
	bplane = infoTwo.plane;

	fmodule = infoOne.module;
	bmodule = infoTwo.module;

	if (fhalf != bhalf) {
		cout << "error! id1 and id2 are not on same half!\n";
		success = false;
	}
	if (fplane != bplane) {
		cout << "error! id1 and id2 are not on same plane!\n";
		success = false;
	}
	if (fmodule != bmodule) {
		cout << "error! id1 and id2 are not on same module!\n";
		success = false;
	}

	Matrix result;

	//at this point, id0 should end in 0, so we can just add numbers
	//FIXME: obviously, this is shuddy and needs fixing

	success = true;

	//last time, just for safety
	if ((id1 % 10) != 0) {
		cout << "error: id1 is not first sensor on module, something went wrong!\n";
	}

	//FIXME: assign matrices, this is shuddy atm. source this out to pndlmddim.
	string m05f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 0, id1 + 5), _inCentimeters);
	string m18f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 1, id1 + 8), _inCentimeters);
	string m28f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 2, id1 + 8), _inCentimeters);
	string m29f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 2, id1 + 9), _inCentimeters);
	string m36f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 6), _inCentimeters);
	string m37f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 7), _inCentimeters);
	string m38f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 3, id1 + 8), _inCentimeters);
	string m47f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 4, id1 + 7), _inCentimeters);
	string m49f = _matrixOutDir
	        + makeMatrixFileName(helper->getOverlapIdFromSensorIDs(id1 + 4, id1 + 9), _inCentimeters);

	// we have to know these matrices from external measurements, so it's okay to use misaligned matrices here
	Matrix m01 = getMatrixSensorToSensor(id1, id1 + 1);
	Matrix m56 = getMatrixSensorToSensor(id1 + 5, id1 + 6);

	// remember, CM matrices are in LMD local, px matrices are in sensor local!
	// and since we know those from external measurements, we have those
	//FIXME: these functions are missing, so the code does not work right now!
//	if (!_inCentimeters) {
//		transformFromLmdLocalToSensor(m01, id1 + 0, aligned);
//		transformFromLmdLocalToSensor(m56, id1 + 5, aligned);
//	}

	Matrix m05 = readMatrix(m05f);
	Matrix m18 = readMatrix(m18f);
	Matrix m28 = readMatrix(m28f);
	Matrix m29 = readMatrix(m29f);
	Matrix m36 = readMatrix(m36f);
	Matrix m37 = readMatrix(m37f);
	Matrix m38 = readMatrix(m38f);
	Matrix m47 = readMatrix(m47f);
	Matrix m49 = readMatrix(m49f);

	// I think all px matrices are inverted
	if (!_inCentimeters) {
		m01.inv();
		m56.inv();
		m05.inv();
		m18.inv();
		m28.inv();
		m29.inv();
		m36.inv();
		m37.inv();
		m38.inv();
		m47.inv();
		m49.inv();
	}

	// since we read only correction matrices in cm, we must multiply them
	// with the ideal senToSen to get the complete misaligned senToSen.
	// this isn't cheating as we are only using ideal matrices, which we should
	// always have.
	// but remember, they still live on separate modules.
	if (_inCentimeters) {

		Matrix m05ideal = getMatrixSensorToSensor(id1 + 0, id1 + 5);
		Matrix m18ideal = getMatrixSensorToSensor(id1 + 1, id1 + 8);
		Matrix m28ideal = getMatrixSensorToSensor(id1 + 2, id1 + 8);
		Matrix m29ideal = getMatrixSensorToSensor(id1 + 2, id1 + 9);
		Matrix m36ideal = getMatrixSensorToSensor(id1 + 3, id1 + 6);
		Matrix m37ideal = getMatrixSensorToSensor(id1 + 3, id1 + 7);
		Matrix m38ideal = getMatrixSensorToSensor(id1 + 3, id1 + 8);
		Matrix m47ideal = getMatrixSensorToSensor(id1 + 4, id1 + 7);
		Matrix m49ideal = getMatrixSensorToSensor(id1 + 4, id1 + 9);

		//FIXME: this is a work around since I can't yet construct the correct misaligned matrices
//		realignMatrixInLmd(m05ideal, id1 + 0);
//		realignMatrixInLmd(m18ideal, id1 + 1);
//		realignMatrixInLmd(m28ideal, id1 + 2);
//		realignMatrixInLmd(m29ideal, id1 + 2);
//		realignMatrixInLmd(m36ideal, id1 + 3);
//		realignMatrixInLmd(m37ideal, id1 + 3);
//		realignMatrixInLmd(m38ideal, id1 + 3);
//		realignMatrixInLmd(m47ideal, id1 + 4);
//		realignMatrixInLmd(m49ideal, id1 + 4);

		m05 = m05 * m05ideal;
		m18 = m18 * m18ideal;
		m28 = m28 * m28ideal;
		m29 = m29 * m29ideal;
		m36 = m36 * m36ideal;
		m37 = m37 * m37ideal;
		m38 = m38 * m38ideal;
		m47 = m47 * m47ideal;
		m49 = m49 * m49ideal;
	}

	//prepare inverted matrices
	Matrix m10 = m01.inv(m01);
	Matrix m50 = m05.inv(m05);
	Matrix m65 = m56.inv(m56);
	Matrix m81 = m18.inv(m18);
	Matrix m82 = m28.inv(m28);
	Matrix m92 = m29.inv(m29);
	Matrix m63 = m36.inv(m36);
	Matrix m73 = m37.inv(m37);
	Matrix m83 = m38.inv(m38);
	Matrix m74 = m47.inv(m47);
	Matrix m94 = m49.inv(m49);

	id2 %= 10;

	//finally, if cascade:
	switch (id2) {
	case 0:
		result = m10 * m81 * m38 * m63 * m56 * m05;
		break;
	case 1:
		result = m01;
		break;
	case 2:
		result = m82 * m18 * m01;
		break;
	case 3:
		result = m83 * m18 * m01;
		break;
	case 4:
		result = m94 * m29 * m82 * m18 * m01;
		break;
	case 5:
		result = m05;
		break;
	case 6:
		result = m56 * m05;
		break;
	case 7:
		result = m37 * m63 * m56 * m05;
		break;
	case 8:
		result = m38 * m63 * m56 * m05;
		break;
	case 9:
		result = m29 * m82 * m38 * m63 * m56 * m05;
		break;
	default:
		result = Matrix::eye(4);
		success = false;
	}

	//return successfully combined matrix
	if (success) {
		if (!_inCentimeters) {
			result.inv();		//FIXME: this is from a bug where all PX matrices are inverted
		}
		return result;
	}
	//else return unity matrix
	cerr << "warning! could not return matrix " << id1 << " to " << id2
	        << "! returning identity matrix!\n";
	return Matrix::eye(4);
}

void PndLmdAlignManager::setMaxPairs(int maxPairs) {

	if (maxPairs > 0) {
		for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
			it->second.setMaximumNumberOfHitPairs(maxPairs);
		}
		return;
	} else {
		cout << "warning. max pairs must be larger than 0!\n";
		return;
	}

}

void PndLmdAlignManager::clearScreen() {
	cout << "\x1B[2J\x1B[H";
}

void PndLmdAlignManager::waitForCompletion() {

	//start all alignsers that have not already started (i.e. don't have required no of Pairs)
	int notStarted = 0;

	cout << "starting remaining aligners.\n";
	for (map<int, bool>::iterator it = alignersFull.begin(); it != alignersFull.end(); it++) {
		if (!(it->second)) {

			//cout << "starting aligner " << it->first << "\n";
			//if pair could not be added, aligner is full. start thread directly.
			alignerThreadGroup.create_thread(
			        boost::bind(&PndLmdAlignManager::alignOne, this, boost::ref(aligners[it->first])));
			notStarted++;
		}
	}
	cout << notStarted << " aligners remained.\n";
	cout << "jobs queue size : " << alignerThreadGroup.size() << "/360\n";
	cout << "waiting for all aligners to finish...";

	//wait for all threads to complete
	alignerThreadGroup.join_all();
	cout << "done!\n";

	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		if (it->second.successful()) {

			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir
			        + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters);

			if (!writeMatrix(result, matrixFilename)) {
				cout << "ERROR: could not write matrix " << matrixFilename << "\n";
			}

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";
		} else {
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}

	//write matrix info
	ofstream of;
	if (_inCentimeters) {
		of.open((_matrixOutDir + "/info-cm.txt").c_str());
	} else {
		of.open((_matrixOutDir + "/info-px.txt").c_str());
	}

	of << _info.str();
	of.close();
	cout << "all aligners done.\n";
}

Matrix PndLmdAlignManager::getPixelToCentimeterTransformation() {

	Matrix result = Matrix::eye(4);
	Matrix shift = Matrix::eye(4);
	Matrix scale = Matrix::eye(4);

	//correct for pixel corner to center
	shift.val[0][3] += 0.5;
	shift.val[1][3] += 0.5;

	//shift from corner to center (this considers inactive area!)
	shift.val[0][3] -= (247.5 / 2.0);
	shift.val[1][3] -= (242.5 / 2.0);

	//scale for pixel size
	scale.val[0][0] *= 80e-4;
	scale.val[1][1] *= 80e-4;

	result = scale * shift;
	return result;
}

bool PndLmdAlignManager::writePairsToBinaryFiles() {

	if (_binaryPairFileDirectory == "") {
		cout << "error: binary pair file directory not set.\n";
		cout << "use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur = 0;
	tot = aligners.size();

	cout << "writing all pairs to binary files\n";
	mkdir(_binaryPairFileDirectory);

	//maybe do this multithreaded?
	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		loadBar(cur++, tot, 1000, 60);
		if (!it->second.writePairsToBinary(_binaryPairFileDirectory)) {
			return false;
		}

	}
	return true;
}

bool PndLmdAlignManager::readPairsFromBinaryFiles() {

	bool success = false;

	if (_binaryPairFileDirectory == "") {
		cout << "error: binary pair file directory not set.\n";
		cout << "use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur = 0;
	tot = aligners.size();

	cout << "reading all pairs from binary files\n";

	for (mapIt it = aligners.begin(); it != aligners.end(); it++) {
		loadBar(cur++, tot, 1000, 60);
		if (!it->second.readPairsFromBinary(_binaryPairFileDirectory)) {
			success = false;
		} else {
			success = true;
		}
	}
	return success;
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
	} catch (std::exception &e) {
		cerr << "PndLmdAlignManager::readConfig: ERROR! Can't parse json file " << filename << ".\n";
	}
	return root;
}

bool PndLmdAlignManager::writeConfigFile(boost::property_tree::ptree configTree, std::string filename,
        bool replaceExisting) {

	//replace logic

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
