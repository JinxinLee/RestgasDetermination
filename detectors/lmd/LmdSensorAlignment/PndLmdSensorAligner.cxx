/*
 * PndLmdSensorAligner.cxx
 *
 *  Created on: May 6, 2015
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include "PndLmdSensorAligner.h"
#include "PndLmdGeometryHelper.h"

#include <cmath>		// for std::abs() that handles doubles
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include <icpPointToPoint.h>
#include <matrix.h>
#include <PndLmdAlignManager.h>

//do NOT delete this, or else abs() comes from cstdlib and casts everything to ints
using std::abs;

using std::cerr;
using std::cout;
using std::make_pair;
using std::string;

void PndLmdSensorAligner::init() {
	maxNoOfPairs = 0;
	numberOfPairs = 0;
	forceInstant = true;
	_moduleID = -1;
	nonSanePairs = 0;
	skippedPairs = 0;
	swappedPairs = 0;
	verbose = 0;
	overlapID = -1;
	inCentimeters = true;
	success = false;
	zIsTimestamp = true;
	debug = false;
	dim = 2;
}

PndLmdSensorAligner::PndLmdSensorAligner() {
	init();
}

PndLmdSensorAligner::~PndLmdSensorAligner() {
	//destroy everything. leave nothing standing.
}

PndLmdSensorAligner::PndLmdSensorAligner(const PndLmdSensorAligner&) {
	std::cerr
	    << "PndLmdSensorAligner::Warning! Unnecessary copy-construction. These aligners should never be copied.\n";
	init();
}

bool PndLmdSensorAligner::zeroValCheck() {

	int zeroVals = 0;
	int modxinv = 0, modyinv = 0, modzinv = 0;
	int temxinv = 0, temyinv = 0, temzinv = 0;

	unsigned int nPairs = simplePairs.size();

	if (verbose == 3) cout << "checking for zero values...\n";

	for (auto &pair : simplePairs) {
		std::abs(pair[0]) < 1e-15 ? modxinv++ : modxinv;
		std::abs(pair[1]) < 1e-15 ? modyinv++ : modxinv;
		std::abs(pair[2]) < 1e-15 ? modzinv++ : modxinv;
		std::abs(pair[3]) < 1e-15 ? temxinv++ : modxinv;
		std::abs(pair[4]) < 1e-15 ? temyinv++ : modxinv;
		std::abs(pair[5]) < 1e-15 ? temzinv++ : modxinv;
	}

	zeroVals = modxinv + modyinv + modzinv + temxinv + temyinv + temzinv;

	// 3 dimension and 2 arrays = 6
	double zeroFactor = zeroVals / ((double) nPairs * 6.0);
	if (zeroFactor > 0.1 && zeroFactor < 0.3) {
		cout << "WARNING. More than 10 % of your entries is zero. That must be a mistake. \n";
		cout << "Also, the kdtree creation could crash. Keep an eye out for that...\n";
		cout << "Zero factor: " << zeroFactor << "\n";
		cout << "model x vals invalid: " << modxinv / (double) nPairs << "\n";
		cout << "model y vals invalid: " << modyinv / (double) nPairs << "\n";
		cout << "model z vals invalid: " << modzinv / (double) nPairs << "\n";
		cout << "templ x vals invalid: " << temxinv / (double) nPairs << "\n";
		cout << "templ y vals invalid: " << temyinv / (double) nPairs << "\n";
		cout << "templ z vals invalid: " << temzinv / (double) nPairs << "\n";

	}
	if (zeroFactor > 0.3) {
		cout << "ERROR. More than 30 % of your entries is zero. That must be a mistake. \n";
		cout << "Also, the kdtree creation will probably crash. Exiting.\n";
		cout << "Zero factor: " << zeroFactor << "\n";
		cout << "model x vals invalid: " << modxinv / (double) nPairs << "\n";
		cout << "model y vals invalid: " << modyinv / (double) nPairs << "\n";
		cout << "model z vals invalid: " << modzinv / (double) nPairs << "\n";
		cout << "templ x vals invalid: " << temxinv / (double) nPairs << "\n";
		cout << "templ y vals invalid: " << temyinv / (double) nPairs << "\n";
		cout << "templ z vals invalid: " << temzinv / (double) nPairs << "\n";
		cout << "=== additional data ===\n";
		cout << "overlap id: " << overlapID << "\n";
		cout << "no of Pairs: " << nPairs << "\n";
		return false;
	}
	return true;
}

void PndLmdSensorAligner::applyDynamicCut() {

	if (simplePairs[0].size() < 7) {
		cerr << "WARNING. Can't apply dynamic cut because distance information is not stored.\n";
		cerr << "Did you read an old version of the binary pair files?\n";
		return;
	}

	//sort pairs by distance, which is the 7th entry of the inner vector
	std::sort(simplePairs.begin(), simplePairs.end(),
	    [](const std::vector< double >& a, const std::vector< double >& b) {return a[6] < b[6];});

	int quantileMargin = simplePairs.size() * 0.10;  // shave 10% from front and back

	vector<vector<double> >::const_iterator first = simplePairs.begin() + quantileMargin;
	vector<vector<double> >::const_iterator last = simplePairs.end() - quantileMargin;
	vector<vector<double> > newSimplePairs(first, last);		// this creates a copy

	//overwrite simple pairs vector with new, reduced vector
	simplePairs = newSimplePairs;

	//remove bias from sorting
	std::random_shuffle(simplePairs.begin(), simplePairs.end());
}

void PndLmdSensorAligner::calculateMatrix() {

	unsigned int nPairs;

	nPairs = simplePairs.size();

	if (skippedPairs > 0) {
		cout << "=====================================================\n";
		cout << "WARNING! Invalid pairs in pair file, check your data!\n";
		cout << "=====================================================\n";
	}

	//TODO: set from Manager or parameter file!
	bool eventTimeCheck = true;
	double minDelta = 1e-6;
	zIsTimestamp = true;

	// only allow max Pairs!
	if (nPairs > maxNoOfPairs && maxNoOfPairs > 0) {
		nPairs = maxNoOfPairs;
	}

	//number of pairs used in this run can not change anymore
	lastNoOfPairs = nPairs;

	//check if maxPairs > 0
	if (nPairs < 50) {
		cerr
		    << "PndLmdSensrAligner::Error: Trying to use less than 50 pairs! (And that's not going to work.) Aborting.\n";
		success = false;
		return;
	}

	//check for zer values, largely not needed anymore, but keep it for now
	zeroValCheck();

	//transform pairs to local module frame of reference
	bool transformToSensorBool = false;
	Matrix toSensor;
	if (transformToSensorBool && inCentimeters) {
		toSensor = transformToSensorOne();
	}

	double* Model = new double[dim * nPairs];
	double* Template = new double[dim * nPairs];

	if (verbose == 3) {
		cout << "arranging pairs...\n";
		cout << "num pairs from bin: " << numberOfPairs << "\n";
		cout << "num pairs from vec: " << simplePairs.size() << "\n";
		cout << "num pairs from dec: " << nPairs << "\n";
	}

	if (dim == 2) {
		for (unsigned int ipair = 0; ipair < nPairs; ipair++) {
			Model[ipair * dim + 0] = simplePairs[ipair][0];
			Model[ipair * dim + 1] = simplePairs[ipair][1];
			Template[ipair * dim + 0] = simplePairs[ipair][3];
			Template[ipair * dim + 1] = simplePairs[ipair][4];
		}
	}

	else if (dim == 3) {
		for (unsigned int ipair = 0; ipair < nPairs; ipair++) {
			Model[ipair * dim + 0] = simplePairs[ipair][0];
			Model[ipair * dim + 1] = simplePairs[ipair][1];
			Model[ipair * dim + 2] = simplePairs[ipair][2];
			Template[ipair * dim + 0] = simplePairs[ipair][3];
			Template[ipair * dim + 1] = simplePairs[ipair][4];
			Template[ipair * dim + 2] = simplePairs[ipair][5];
		}
	}

	if (verbose == 3) {
		cout << "creating ICP...\n";
	}

	// start with identity as initial transformation
	// in practice you might want to use some kind of prediction here
	Matrix Rotation;
	Matrix translation;

	//perform ICP and store quality parameters
	//attention! dim * nPairs must equal size of model!
	IcpPointToPoint icp(Model, nPairs, dim);

	if (verbose == 3) cout << "ICP and model created...\n";

	//prepare Matrices
	if (dim == 2) {
		Rotation = Matrix::eye(2);
		translation = Matrix(2, 1);
	}
	else if (dim == 3) {
		Rotation = Matrix::eye(3);
		translation = Matrix(3, 1);
	}

	icp.forceInstantResult(forceInstant);
	icp.fit(Template, nPairs, Rotation, translation, -1);

	if (verbose == 3) cout << "ICP fit step done.\n";

	if (dim == 2) {

		//make 4x4 matrix
		double* tempR = new double[4];
		double* tempT = new double[2];

		Rotation.getData(tempR);
		translation.getData(tempT);

		double* finalMatrix = new double[16];

		//okay, this is the version that FIRST rotates, THEN translates.
		finalMatrix[0] = tempR[0];
		finalMatrix[1] = tempR[1];
		finalMatrix[2] = 0;
		finalMatrix[3] = tempT[0];
		finalMatrix[4] = tempR[2];
		finalMatrix[5] = tempR[3];
		finalMatrix[6] = 0;
		finalMatrix[7] = tempT[1];
		finalMatrix[8] = 0;
		finalMatrix[9] = 0;
		finalMatrix[10] = 1.0;
		finalMatrix[11] = 0;
		finalMatrix[12] = 0;
		finalMatrix[13] = 0;
		finalMatrix[14] = 0;
		finalMatrix[15] = 1.0;
		resultMatrix = Matrix(4, 4);

		//save matrix!
		resultMatrix.setVal(4, 4, finalMatrix);

		delete tempR;
		delete tempT;
		delete finalMatrix;

	}
	else if (dim == 3) {

		//make 4x4 matrix
		double* tempR = new double[9];
		double* tempT = new double[3];

		Rotation.getData(tempR);
		translation.getData(tempT);

		double* finalMatrix = new double[16];

		//TODO: check if this still works when we return TGeoHMatrix objects
		//okay, this is the version that FIRST rotates, THEN translates.
		finalMatrix[0] = tempR[0];
		finalMatrix[1] = tempR[1];
		finalMatrix[2] = tempR[2];
		finalMatrix[3] = tempT[0];
		finalMatrix[4] = tempR[3];
		finalMatrix[5] = tempR[4];
		finalMatrix[6] = tempR[5];
		finalMatrix[7] = tempT[1];
		finalMatrix[8] = tempR[6];
		finalMatrix[9] = tempR[7];
		finalMatrix[10] = tempR[8];
		finalMatrix[11] = tempT[2];
		finalMatrix[12] = 0;
		finalMatrix[13] = 0;
		finalMatrix[14] = 0;
		finalMatrix[15] = 1;

		resultMatrix = Matrix(4, 4);

		//save matrix!
		resultMatrix.setVal(4, 4, finalMatrix);

		delete tempR;
		delete tempT;
		delete finalMatrix;
	}

	std::stringstream alignlog;

	if (icp.hasConverged()) {
		if (verbose == 3) cout << "ICP convergence ok.\n";
		success = true;

		//and say a few words for the log
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "icp converged for overlapID " << overlapID << " in " << icp.getInterations()
		    << " iterations.\n";

		alignlog << "pairs available: " << nPairs;
		if (nPairs < 100000) {
			alignlog << " (WARNING! This is not enough for accurate alignment!)\n";
		}
		else {
			alignlog << "\n";
		}
		alignlog << "minDelta: " << minDelta << "\n";

		alignlog << "EventTimeCheck: ";
		eventTimeCheck ? alignlog << "on (and passed)\n" : alignlog << "off\n";

		alignlog << "Force Instant: ";
		forceInstant ? alignlog << "on\n" : alignlog << "off\n";

		alignlog << "====================================================\n";
	}
	else {
		alignlog << "\n";
		alignlog << "====================================================\n";
		alignlog << "CRITICAL ERROR:\n";
		alignlog << "no convergence for overlapID " << overlapID << "." << "\n";
		alignlog << "====================================================\n";
		alignlog << "\n";
		if (verbose == 3) cout << "ICP did not converge!\n";
		success = false;
	}
	if (verbose == 3) cout << alignlog.str();

	delete[] Model;
	delete[] Template;

	return;
}

bool PndLmdSensorAligner::addSimplePair(const PndLmdHitPair &pair) {

	if (simplePairs.size() >= maxNoOfPairs && maxNoOfPairs > 0) {
		return false;
	}

	/*
	 * add pair as follows:
	 * vector<double> pair;
	 *
	 * [0]: oneX
	 * [1]: oneY
	 * [2]: oneZ
	 * [3]: twoX
	 * [4]: twoY
	 * [5]: twoZ
	 * [6]: distance
	 */

	if (inCentimeters) {
		vector<double> tempPair;

		tempPair.push_back(pair.getHit1().x());
		tempPair.push_back(pair.getHit1().y());
		tempPair.push_back(pair.getHit1().z());

		tempPair.push_back(pair.getHit2().x());
		tempPair.push_back(pair.getHit2().y());
		tempPair.push_back(pair.getHit2().z());

		tempPair.push_back(pair.getDistance());

		simplePairs.push_back(tempPair);
	}
	else {
		vector<double> tempPair;

		tempPair.push_back(pair.getCol1());
		tempPair.push_back(pair.getRow1());
		tempPair.push_back(simplePairs.size());

		tempPair.push_back(pair.getCol2());
		tempPair.push_back(pair.getRow2());
		tempPair.push_back(simplePairs.size());

		tempPair.push_back(pair.getDistance());

		simplePairs.push_back(tempPair);
	}
	return true;
}

bool PndLmdSensorAligner::writePairsToBinary(const std::string directory) {

	string filename;					//target directory
	double* pdata;						//array with pairs

	unsigned int nPairs = simplePairs.size();
	if (nPairs == 0) {
		cout << "warning: attempting to write empty binary pair file! (no pairs in buffer for overlapID "
		    << overlapID << ")\n";
		return false;
	}

	int doublesPerPair = simplePairs[0].size();		//well, doubles per Pair, this is determined by addPair()

	size_t length = nPairs * doublesPerPair + 6;	//number of raw doubles (including header), remember pairs have 6 doubles

	filename = directory;
	filename += PndLmdAlignManager::makeBinaryPairFileName(overlapID, inCentimeters);

	//construct header
	unsigned int headersize = 6;						//header size in number of doubles

	double* header = new double[headersize];
	header[0] = headersize;								//header size in double fields
	header[1] = doublesPerPair;		//doubles per pair
	header[2] = nPairs;						//nPairs, read the correct vector!
	header[3] = sizeof(double);  //sizeof(double), check when reading files! might be different on other OSes.
	header[4] = overlapID;				//overlapID
	header[5] = 2.1;								//version, cant read older versions

	pdata = new double[length];

	//save header
	for (unsigned int i = 0; i < headersize; i++) {
		pdata[i] = header[i];
	}
	//save data
	unsigned int currentIndex = headersize;				//data starts here

	//iterate over all pairs
	for (unsigned int iPair = 0; iPair < nPairs; iPair++) {

		//iterate over all entries of the pair
		for (unsigned int jPairIndex = 0; jPairIndex < simplePairs[0].size(); jPairIndex++) {
			pdata[currentIndex + jPairIndex] = simplePairs[iPair][jPairIndex];
		}
		currentIndex += doublesPerPair;
	}

	/*
	 * the write part is easy, just dump everything. read part is more difficult,
	 * need to check file first
	 */

	//TODO: abstract this to Manager!
	//create directory if not already present
	PndLmdAlignManager::mkdir(directory);
	std::ofstream os(filename.c_str(), std::ios::binary | std::ios::out);
	if (!os.is_open()) {
		cout << "ERROR! Could not write to " << filename << "!\n";
		return false;
	}

	os.write(reinterpret_cast<const char*>(pdata), std::streamsize(length * sizeof(double)));
	os.close();
	delete[] pdata;
	delete[] header;
	return true;
}

bool PndLmdSensorAligner::readPairsFromBinary(const std::string directory) {

	string filename;				//binary pair file
	//size_t length;				//number of raw doubles, remember pairs have 6 doubles
	double* pdata;					//array with pairs
	size_t filesize;
	size_t doublesize = sizeof(double);
	unsigned int nPairs;
	unsigned int doublesPerPair;
	unsigned int noOfDoubles = 0;

	/*
	 * read goes as follows:
	 *
	 * first, check file size.
	 * file size is larger than one double (headersize is stored there)? -> read header only! else return false;
	 * read header and interpret data from header.
	 * is nPairs*doublesPerPair*sizeof(double) + headersize*sizeof(double) (header) == filezise?
	 * is nPairs in header == headersize * sizeof(double) (filesize - header)
	 * if so, file seems okay, read header + file. else return false;
	 * when entire file is read, copy data without header to arrays for ICP	 *
	 */

	filename = directory;
	filename += PndLmdAlignManager::makeBinaryPairFileName(overlapID, inCentimeters);

	//check if file exists and file size
	std::fstream inStream(filename.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
	if (inStream) {
		std::fstream::pos_type size = inStream.tellg();
		filesize = size;
	}
	else {
		cout << filename.c_str() << " could not be read!\n";
		return false;
	}

	double* headersizeD = new double[1];
	//read header size
	if (filesize >= sizeof(double)) {
		//read header
		std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
		if (!is.is_open()) return false;
		is.read(reinterpret_cast<char*>(headersizeD), std::streamsize(sizeof(double)));
		is.close();
	}

	int headersize;
	if (headersizeD[0] < 6) {
		//cout << "headersize is " << headersizeD[0] << ", seems to be old format. using 6 for now.";
		headersize = 6;
	}
	else {
		headersize = headersizeD[0];
	}

	//read header
	if (filesize >= headersize * sizeof(double)) {
		double* header = new double[headersize];

		//read header
		std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
		if (!is.is_open()) return false;
		is.read(reinterpret_cast<char*>(header), std::streamsize(headersize * sizeof(double)));
		is.close();

		//check header
		if (verbose == 3) {
			cout << "header size: " << header[0] << "\n";
			cout << "doubles / pair: " << header[1] << "\n";
			cout << "no of pairs: " << header[2] << "\n";
		}

		//check header
		doublesPerPair = header[1];
		nPairs = std::round(header[2]);
		numberOfPairs = nPairs;
		noOfDoubles = nPairs * doublesPerPair + headersize;
		size_t filesizeMust = sizeof(double) * (noOfDoubles);

		if (doublesize != header[3]) {
			cout
			    << "warning! sizeof(double) on this system is different than on the system that made this binary!\n";
			//TODO: decide what to do in this case
			exit(1);
			doublesize = header[3];
			return false;
		}

		if (overlapID != header[4]) {
			cout << "error! file name and overlapID do not match! did you rename the file?\n";

			//FIXME: allow this, for now...
			//return false;
		}

		if (filesizeMust != filesize) {
			cout << "file is corrupt!\n";
			return false;
		}

		//free allocated space!
		delete[] header;

	}
	else {
		cout << filename.c_str() << " is too small, file corrupt!\n";
		return false;
	}

	//allocate double array for entire file
	pdata = new double[noOfDoubles];

	//actually read file
	std::ifstream is(filename.c_str(), std::ios::binary | std::ios::in);
	if (!is.is_open()) return false;
	is.read(reinterpret_cast<char*>(pdata), std::streamsize(noOfDoubles * sizeof(double)));
	is.close();
	// the entire file is now in memory. it's only about 30 MB, so this is okay

	//save data
	unsigned int currentIndex = headersize;				//data starts here
	//bool error = false;

	for (unsigned int iPair = 0; iPair < nPairs; iPair++) {

		vector<double> tempPair;
		//assign pair data
		for (unsigned int jPairIndex = 0; jPairIndex < doublesPerPair; jPairIndex++) {
			double currentVal = pdata[currentIndex + jPairIndex];
			//cout << "current val is: " << currentVal << "\n";
			tempPair.push_back(currentVal);
		}
		simplePairs.push_back(tempPair);

		//get next index, every pair has doublesPerPair doubles
		currentIndex += doublesPerPair;
	}

	//check size one last time

	if (nPairs != simplePairs.size()) {
		cerr << "Warning! Error while reading binary pair file!\n";
		return false;
	}

	//delete array!
	delete[] pdata;

	return true;
}

void PndLmdSensorAligner::clearPairs() {

	lastNoOfPairs = simplePairs.size();

	//call destructors of the member objects (well, they're doubles, so... yeah.)
	simplePairs.clear();

	//force release of allocated memory by vectors
	vector<vector<double> >().swap(simplePairs);

}

bool PndLmdSensorAligner::check() {

	//check for number of pairs
	if (simplePairs.size() < 50) {
		cerr << "PndLmdSensorAligner::check: ERROR! Aligner " << overlapID
		    << "doesn't have enough pairs! Aborting.\n";
		return false;
	}

	//check if pair array is strictly rectangular
	unsigned int dimy = simplePairs[0].size();
	for(auto &pair : simplePairs){
		if(pair.size() != dimy){
			return false;
		}
	}

	//check... I don't know, something.
	return true;

}

std::vector<double> PndLmdSensorAligner::getPairSpread(int what) {

	double xcom, ycom, zcom, xspread, yspread, zspread, xmin, xmax, ymin, ymax, zmin, zmax, avgDist;
	unsigned int nPairs = 0;
	nPairs = simplePairs.size();

	xcom = ycom = zcom = xspread = yspread = zspread = xmin = xmax = ymin = ymax = zmin = zmax = avgDist =
	    0.0;

	// set all values to first entry
	if (what == 0 || what == 1) {
		xcom += simplePairs[0][0];
		ycom += simplePairs[0][1];
		zcom += simplePairs[0][2];

		xmin = simplePairs[0][0];
		xmax = simplePairs[0][0];
		ymin = simplePairs[0][1];
		ymax = simplePairs[0][1];
		zmin = simplePairs[0][2];
		zmax = simplePairs[0][2];
	}

	if (what == 0 || what == 2) {
		xcom += simplePairs[0][3];
		ycom += simplePairs[0][4];
		zcom += simplePairs[0][5];

		xmin = simplePairs[0][3];
		xmax = simplePairs[0][3];
		ymin = simplePairs[0][4];
		ymax = simplePairs[0][4];
		zmin = simplePairs[0][5];
		zmax = simplePairs[0][5];
	}

	avgDist = simplePairs[0][6];

	// collect center of mass and spread in all coordinates
	for (unsigned int i = 1; i < simplePairs.size(); i++) {
		auto &pair = simplePairs[i];

		if (what == 0 || what == 1) {
			xcom += pair[0];
			ycom += pair[1];
			zcom += pair[2];

			xmin = std::min(xmin, pair[0]);
			xmax = std::max(xmax, pair[0]);
			ymin = std::min(ymin, pair[1]);
			ymax = std::max(ymax, pair[1]);
			zmin = std::min(zmin, pair[2]);
			zmax = std::max(zmax, pair[2]);
		}

		if (what == 0 || what == 2) {
			xcom += pair[3];
			ycom += pair[4];
			zcom += pair[5];

			xmin = std::min(xmin, pair[3]);
			xmax = std::max(xmax, pair[3]);
			ymin = std::min(ymin, pair[4]);
			ymax = std::max(ymax, pair[4]);
			zmin = std::min(zmin, pair[5]);
			zmax = std::max(zmax, pair[5]);
		}

		avgDist += pair[6];
	}

	// calculate center of mass and spread
	if (what == 1 || what == 2) {
		xcom /= nPairs;
		ycom /= nPairs;
		zcom /= nPairs;

		avgDist /= nPairs;
	}
	else if (what == 0) {
		xcom /= 2.0 * nPairs;
		ycom /= 2.0 * nPairs;
		zcom /= 2.0 * nPairs;

		avgDist /= 2.0 * nPairs;
	}

	xspread = std::abs(xmax - xmin);
	yspread = std::abs(ymax - ymin);
	zspread = std::abs(zmax - zmin);

	std::vector<double> spread;

	spread.push_back(xcom);
	spread.push_back(ycom);
	spread.push_back(zcom);

	spread.push_back(xspread);
	spread.push_back(yspread);
	spread.push_back(zspread);

	spread.push_back(avgDist);

	return spread;

}

void PndLmdSensorAligner::printPairSpread(int what) {

	auto spread = getPairSpread(what);

	cout << "x com: " << spread[0] << "\n";
	cout << "y com: " << spread[1] << "\n";
	cout << "z com: " << spread[2] << "\n";

	cout << "x spread: " << spread[3] << "\n";
	cout << "y spread: " << spread[4] << "\n";
	cout << "z spread: " << spread[5] << "\n";

	cout << "avg Dist: " << spread[6] << "\n";
}

Matrix PndLmdSensorAligner::transformToSensorOne() {

	Matrix toLMD = Matrix::eye(4);

	PndLmdGeometryHelper *helper = &PndLmdGeometryHelper::getInstance();
	int id1 = helper->getSensorOneFromOverlapID(overlapID);
	TGeoHMatrix matrix = helper->getMatrixPndGlobalToSensor(id1);
	toLMD = superManager->castTGeoHMatrixToMatrix(matrix);

	// ======== for testing only
	// it appears we have to make an actrive trafo from a passive one
	toLMD.inv();
	// ======== for testing only

	for (auto &pair : simplePairs) {
		transformPair(toLMD, pair);
	}

	return toLMD;
}

Matrix PndLmdSensorAligner::transformToLmdLocal() {

	Matrix toLMD = Matrix::eye(4);

	PndLmdGeometryHelper *helper = &PndLmdGeometryHelper::getInstance();
	TGeoHMatrix matrix = helper->getMatrixPndGlobalToLmdLocal();
	toLMD = superManager->castTGeoHMatrixToMatrix(matrix);

	// ======== for testing only
	// it appears we have to make an actrive trafo from a passive one
	toLMD.inv();
	// ======== for testing only

	for (auto &pair : simplePairs) {
		transformPair(toLMD, pair);
	}
	return toLMD;
}

void PndLmdSensorAligner::transformPair(Matrix& trafoMatrix, std::vector<double>& pair) {

	if (trafoMatrix.m != 4 || trafoMatrix.n != 4 || pair.size() < 6) {
		std::cerr << "ERROR. Matrix is not 4x4 or pair has less than 6 entries\n";
		return;
	}

	Matrix pairVal1(4, 1);
	Matrix pairVal2(4, 1);

	pairVal1.val[0][0] = pair[0];
	pairVal1.val[1][0] = pair[1];
	pairVal1.val[2][0] = pair[2];
	pairVal1.val[3][0] = 1.0;

	pairVal2.val[0][0] = pair[3];
	pairVal2.val[1][0] = pair[4];
	pairVal2.val[2][0] = pair[5];
	pairVal2.val[3][0] = 1.0;

	pairVal1 = trafoMatrix * pairVal1;
	pairVal2 = trafoMatrix * pairVal2;

	// don't forget do de-homogenize
	pair[0] = pairVal1.val[0][0] / pairVal1.val[3][0];
	pair[1] = pairVal1.val[1][0] / pairVal1.val[3][0];
	pair[2] = pairVal1.val[2][0] / pairVal1.val[3][0];

	pair[3] = pairVal2.val[0][0] / pairVal2.val[3][0];
	pair[4] = pairVal2.val[1][0] / pairVal2.val[3][0];
	pair[5] = pairVal2.val[2][0] / pairVal2.val[3][0];

}
