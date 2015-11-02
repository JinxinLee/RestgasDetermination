/*
 * PndLmdAlignManager.cpp
 *
 *  Created on: May 26, 2015
 *      Author: roman
 */

#include <PndLmdAlignManager.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <PndLmdDim.h>

#include <PndLmdSensorAligner.h>
#include <PndLmdHitPair.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGeoMatrix.h>

using std::vector;
using std::string;

PndLmdAlignManager::PndLmdAlignManager() {
	reinitialize();
}

PndLmdAlignManager::~PndLmdAlignManager() {
}

bool PndLmdAlignManager::addPair(PndLmdHitPair& pair) {

	//TODO: implement use of single aligner, to conserve memory

	//TODO: checking is done elsewhere, but const permissions don't allow to change the pair
	pair.check();
	if(pair.isSane() ){
		if(_useSimpleStorage){
			_aligners[pair.getOverlapId()].addSimplePair(pair);
		}
		else{
			_aligners[pair.getOverlapId()].addPair(pair);
		}
	}
	else{
		cout << "pair failed.\n";
	}
	return true;
}

void PndLmdAlignManager::validate() {
	std::cout << "using " << _aligners.size() << " aligners, which have:\n";
	for(mapIt it = _aligners.begin();  it != _aligners.end(); it++){
		cout << "id: " << it->second.getModuleID() << " has " << it->second.getNoOfPairs() << " pairs." << std::endl;
	}
}

bool PndLmdAlignManager::addFile(std::string filename) {

	if(_pretend){
		//cout << "pretending to add file...\n";
		return true;
	}

	if(_allFilesAdded){
		return false;
	}
	else{
		_fileNames.push_back(filename);
		return true;
	}
}

void PndLmdAlignManager::readFiles(){

	if(_pretend){
		cout << "pretending to read files...\n";
		return;
	}

	_allFilesAdded=true;

	TChain* chainPairs = new TChain("cbmsim");
	for(int i=0; i<_fileNames.size(); i++){
		//cout << files[i] << endl;
		if( _fileNames[i].find("Lumi_Pairs") != std::string::npos ){
			chainPairs->Add(_fileNames[i].c_str());
		}
	}

	//pairs of sensors in LMD coordinates
	TClonesArray* hitPairs = new TClonesArray("PndLmdHitPair");
	chainPairs->SetBranchAddress("PndLmdHitPair", &hitPairs);
	int nEntries = chainPairs->GetEntries();
	cout << "HitPairs no of entries: " << nEntries << endl;

	cout << "Sorting Pairs to Manager...\n";
	int totalPairs=0;
	for(int i_event=0; i_event<nEntries; i_event++ ){

		loadBar(i_event, nEntries, 1000,30);
		chainPairs->GetEntry(i_event);
		int nPairs = hitPairs->GetEntries();
		//loop over hitPairs per Event
		for(int i_Pair=0; i_Pair<nPairs;i_Pair++){
			PndLmdHitPair* currentPair = (PndLmdHitPair*)hitPairs->At(i_Pair);
			addPair(*currentPair);
			totalPairs++;
		}
	}

	//TODO: remove
	cout << "=========================\n";
	cout << "total Pairs: " << totalPairs << endl;
	cout << "All done. Running Align Manager.\n";
	cout << "=========================\n";

	delete chainPairs;
	delete hitPairs;
}

void PndLmdAlignManager::alignAllSensors() {

	if(_pretend){
		cout << "merely pretending\n";
		return;
	}

	if(!_inCentimeters && _enableHelperMatrix){
		_enableHelperMatrix=false;
		cerr << "WARNING! You can't use rotation correction in pixel ICP mode!\n";
	}

	checkIOpaths();

	int cur, tot;
	cur=0;
	tot=_aligners.size();

	stringstream info;
	info << "info for aligned areas\n";

	//maybe do this multithreaded?
	for(mapIt it=_aligners.begin(); it != _aligners.end(); it++){

		loadBar(cur++, tot, 1000, 30);
		it->second.calculateMatrix();
		if(it->second.successful()){
			int id1, id2;
			id1=it->second.getId1();
			id2=it->second.getId2();
			Matrix result = it->second.getResultMatrix();

			//FIXME: matrix only needs to be converted from px to cm when using real hit pairs

			stringstream matrixfilename;
			matrixfilename << _matrixOutDir << "m"<<id1<<"to"<<id2;
			if(_inCentimeters){
				matrixfilename << "cm";
			}
			else{
				matrixfilename << "px";
			}
			if(_enableHelperMatrix){
				matrixfilename << "C";
			}
			matrixfilename<<".mat";
			writeMatrix(result, matrixfilename.str());

			info << "aligner " << it->second.getOverlapId() << ":\n";
			info << "area " << it->second.getId1() << " to " << it->second.getId2() << "\n";
			info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			info << "\n";

		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}

	ofstream of;
	of.open(( _matrixOutDir + "info.txt").c_str());
	of << info.str();
	of.close();

}

void PndLmdAlignManager::writeDebugInfoOnAllSensors() {

	if(_pretend){
		cout << "pretending to align all sensors...\n";
		ofstream of;
		of.open(outFilename.c_str());
		if(of.fail()){
			cout << "can't write to file \n";
			return;
		}

		of << "merely pretending.";
		of.close();
		return;
	}

	//some checks still

	ofstream of;
	of.open(outFilename.c_str());
	if(of.fail()){
		cout << "can't write to file \n";
		return;
	}
	of << std::setprecision(16);

	// csv types: 6 Fields
	// moduleID, overlapID, nP, alpha, x, y

	int cur, tot;
	cur=0;
	tot=_aligners.size();

	//maybe do this multithreaded?
	for(mapIt it=_aligners.begin(); it != _aligners.end(); it++){

		loadBar(cur++, tot, 1000, 30);

		it->second.calculateMatrix();

		if(it->second.successful()){
			int id1, id2;
			id1=it->second.getId1();
			id2=it->second.getId2();

			Matrix result = it->second.getResultMatrix();
			Matrix real = getMatrixOfficialGeometry(id1,id2, true);
			Matrix dif = real - transformMatrixFromPixelsToCm(result);

			stringstream matrixfilename;
			matrixfilename << _matrixOutDir << "/m"<<id1<<"to"<<id2<<".mat";
			writeMatrix(result, matrixfilename.str());

			/*
			cout << "============================\n";
			cout << "========  this is it  ======\n";
			cout << "============================\n";
			cout << transformMatrixFromPixelsToCm(result) << endl;
			cout << "============================\n";
			cout << real << endl;
			cout << "======= =====================\n";
			cout << "=======  difference  =======\n";
			cout << dif << endl;
			cout << "=======  difference  =======\n";
			 */

			int moduleID, overlapID, nPairs;
			double sinAlphaAbs, sinAlphaRel, xAbs, xRel, yAbs, yRel;

			moduleID = dimension->makeModuleID(id1, id2);
			overlapID = it->second.getOverlapId();
			nPairs = it->second.getNoOfPairs();
			sinAlphaRel = -dif.val[0][1]/2.0 + dif.val[1][0]/2.0;
			sinAlphaRel = asin(sinAlphaRel);
			xRel = dif.val[0][3];
			yRel = dif.val[1][3];

			sinAlphaAbs = -result.val[0][1]/2.0 + result.val[1][0]/2.0;
			sinAlphaAbs = asin(sinAlphaAbs);
			xAbs = result.val[0][3];
			yAbs = result.val[1][3];

			/*
			 * scheme is overlapID, moduleID, id1, id2, nPairs, sinAlphaAbs, xAbs, yAbs, sinAlphaRel, xRel, yRel
			 *
			 * that means:
			 *
			 * nPairs = 4
			 * aAbs = 5
			 * xAbs = 6
			 * yAbs = 7
			 * aRel = 8
			 * xRel = 9
			 * yRel = 10
			 */

			of << overlapID << "," << moduleID  << "," << id1 << "," << id2 << "," << nPairs;
			of << "," << sinAlphaAbs << "," << xAbs << "," << yAbs << ",";
			of << sinAlphaRel << "," << xRel << "," << yRel << endl;
		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
	of.close();
}

/*
 * these parameters depend on sensor geometry. Use with caution!
 */
Matrix PndLmdAlignManager::transformMatrixFromPixelsToCm(const Matrix &input) {

	/*
	 * in principle, there should also be a matrix that transforms from sensor active to sensor,
	 * but inactive area is already accounted for in pixelsX and pixelsY!
	 */

	//very important, because I still don't know how pixel number corresponds to active area
	double pixelsX = 247.5;
	double pixelsY = 242.5;

	//center of pixel correction matrices
	Matrix matrixPixelEdgeToCenter;
	Matrix matrixActiveEdgeToCenter;
	Matrix matrixScaleActiveToSensor;
	Matrix newPixelsToSensorInCm, newSensorInCmToPixels;

	matrixPixelEdgeToCenter = Matrix::eye(4);
	matrixPixelEdgeToCenter.val[0][3] += 0.5;
	matrixPixelEdgeToCenter.val[1][3] += 0.5;

	//KEEP IN MIND! This matrix also takes inactive area into account!!
	matrixActiveEdgeToCenter = Matrix::eye(4);
	matrixActiveEdgeToCenter.val[0][3] -= pixelsX/2;
	matrixActiveEdgeToCenter.val[1][3] -= pixelsY/2;

	//scaling must be applied after translations
	matrixScaleActiveToSensor = Matrix::eye(4);
	matrixScaleActiveToSensor.val[0][0]=80e-4;
	matrixScaleActiveToSensor.val[1][1]=80e-4;

	newPixelsToSensorInCm = matrixScaleActiveToSensor * matrixActiveEdgeToCenter * matrixPixelEdgeToCenter;
	newSensorInCmToPixels = Matrix(newPixelsToSensorInCm);
	newSensorInCmToPixels.inv();

	Matrix result;
	result = newPixelsToSensorInCm * input * newSensorInCmToPixels;
	return result;
}

/*
 * get transformation matrix of fromSensor -> toSensor (LMD local reference frame)
 * this is in pixel coordinates and IN LMD LOCAL. but we can use transform functions to correct that.
 * ATTENTION! set aligned flag true for perfect geometry, else use false!
 */
Matrix PndLmdAlignManager::getMatrixOfficialGeometry(int fromSensor, int toSensor, bool aligned){

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(fromSensor, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(toSensor, bhalf, bplane, bmodule, bside, bdie, bsensor);

	//const TGeoHMatrix& matrix_from = dimension->Get_transformation_sensor_to_lmd_local(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	//const TGeoHMatrix& matrix_to   = dimension->Get_transformation_lmd_local_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);

	const TGeoHMatrix& matrix_from = dimension->Get_transformation_sensor_to_global(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	const TGeoHMatrix& matrix_to   = dimension->Get_transformation_global_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);


	TGeoMatrix* matrix = &(matrix_from*matrix_to);

	double* homogenousMatrix = new double[16];
	double* finalMatrix = new double[16];
	matrix->GetHomogenousMatrix(homogenousMatrix);

	/*
	cout << "intermission: teh matrix\n";

	for(int i=0; i<16; i++){
		cout << "entry " << i << ": " << homogenousMatrix[i] << "\n";
	}
	 */

	//root documentation differs from actual implementation
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
	finalMatrix[15] = homogenousMatrix[15];

	// corrections, because numerics suck
	if(false){
		finalMatrix[2] = 0.0;
		finalMatrix[6] = 0.0;
		finalMatrix[8] = 0.0;
		finalMatrix[9] = 0.0;
	}

	finalMatrix[15] = 1.0;

	Matrix result(4,4,finalMatrix);
	delete homogenousMatrix, finalMatrix, matrix;
	return result;
}

/*
 * get transformation matrix of fromSensor -> toSensor (PANDA global reference frame)
 * this is in pixel coordinates and IN PANDA GLOBAL.
 * ATTENTION! set aligned flag true for perfect geometry, else use false!
 */
Matrix PndLmdAlignManager::getMatrixOfficialGeometryGlobal(int fromSensor, int toSensor, bool aligned){

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(fromSensor, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(toSensor, bhalf, bplane, bmodule, bside, bdie, bsensor);

	const TGeoHMatrix& matrix_from = dimension->Get_transformation_sensor_to_global(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	const TGeoHMatrix& matrix_to   = dimension->Get_transformation_global_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);

	TGeoMatrix* matrix = &(matrix_from*matrix_to);

	double* homogenousMatrix = new double[16];
	double* finalMatrix = new double[16];
	matrix->GetHomogenousMatrix(homogenousMatrix);

	/*
	cout << "intermission: teh matrix\n";

	for(int i=0; i<16; i++){
		cout << "entry " << i << ": " << homogenousMatrix[i] << "\n";
	}
	 */

	//root documentation differs from actual implementation
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
	finalMatrix[15] = homogenousMatrix[15];


	// corrections, because numerics suck
	if(false){
		finalMatrix[2] = 0.0;
		finalMatrix[6] = 0.0;
		finalMatrix[8] = 0.0;
		finalMatrix[9] = 0.0;
	}

	finalMatrix[15] = 1.0;

	Matrix result(4,4,finalMatrix);
	delete homogenousMatrix, finalMatrix, matrix;
	return result;
}

void PndLmdAlignManager::loadBar(int i, int n, int r, int w, std::string message) {

	// Only update r times.
	if(n==0) return;
	if(n==1) return;

	if(r>n){
		r = n;
	}

	//calculate ev / sec every 100000 iterations
	if ( i % (n/r) != 0 ){
		return;
	}

	flush(cout);

	// Calculate the ratio of complete-to-incomplete.
	float ratio = i/(float)n;
	int   c     = ratio * w;

	// Show the percentage complete.
	printf("%3d%% [", (int)(ratio*100) );

	// Show the load bar.
	for (int x=0; x<c; x++)
		printf("=");

	for (int x=c; x<w; x++)
		printf(" ");

	// ANSI Control codes to go back to the
	// previous line and clear it.

	printf("] %s %d of %d \n\033[F\033[J", message.c_str(), i, n);

}

void PndLmdAlignManager::reinitialize(){

	dimension = PndLmdDim::Instance();

	//FIXME: do this from calling makro, not hard coded
	//dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd.dat", true);
	//dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd_misaligned.dat", false);

	int overlapId=-1;
	_aligners.clear();
	_fileNames.clear();


	//hackjob, do this right!
	Matrix matrix1 = Matrix::rotMatX(M_PI/16);
	Matrix matrix2 = Matrix::rotMatY(M_PI/16);
	Matrix matrix3 = Matrix::rotMatZ(M_PI/16);
	Matrix hmatrix1 = matrix3 * matrix2 * matrix1;
	//Matrix hmatrix1 = Matrix::eye(4);

	Matrix hmatrix2(3,1);
	hmatrix2.val[0][0] = -26.85;
	hmatrix2.val[1][0] = 0.0;
	hmatrix2.val[2][0] = -1147.0;

	hmatrix1 = homogenizeMatrix(hmatrix1);
	hmatrix2 = homogenizeMatrix(hmatrix2);

	helperMatrix = hmatrix2 * hmatrix1;

	//make map<moduleid, SensorAligner> with 40 entries

	//FIXME: do this in PndLmdDim, so that if something changes, only one place must be corrected
	for(int iHalf=0; iHalf<2; iHalf++){
		for(int iPlane=0; iPlane<4; iPlane++){
			for(int iModule=0; iModule<5; iModule++){
				for(int iOverlap=0; iOverlap<9;iOverlap++){
					overlapId = 10* (dimension->makeModuleID(iHalf, iPlane, iModule))+iOverlap;
					PndLmdSensorAligner tempAligner;
					tempAligner.setOverlapId(overlapId);
					tempAligner.setModuleID(dimension->makeModuleID(iHalf, iPlane, iModule));
					tempAligner.setHelperMatrix(helperMatrix);
					_aligners[overlapId]=tempAligner;
				}
			}
		}
	}
	_allFilesAdded=false;
	_useSimpleStorage=true;
	outFilename="";
	_singleAligner=true;
	_pretend=false;

	std::cout << "PndLmdAlignManager::Init(): Initialization successful." << std::endl;
}

void PndLmdAlignManager::checkIOpaths() {

	//ensure input file exists
	//cout << "checking input files...";

	if(_fileNames.size()==0){
		cout << "no file specified or found!\n";
		exit(1);
	}
	for(int iFile=0; iFile<_fileNames.size(); iFile++){
		if(!boost::filesystem::exists(_fileNames[iFile])){
			cout << "error opening file:";
			cout << _fileNames[iFile] << "\n";
			exit(1);
		}
	}
	//cout << " ok!\n";

	//ensure output path exists
	//cout << "checking output path...";
	//cout << outFilename << endl;

	if(!(outFilename=="")){
		boost::filesystem::path outPath(outFilename);
		if(!boost::filesystem::exists(outPath.parent_path())){
			boost::filesystem::create_directories(outPath.parent_path());
		}
	}

	//cerr << "checking " << _matrixOutDir << endl;

	if(!(_matrixOutDir=="")){
		if(!boost::filesystem::exists(_matrixOutDir)){
			boost::filesystem::create_directories(_matrixOutDir);
		}
	}
	//cout << " ok!\n";
}

//read contents of specified file, appends to stringstream provided, returns no of lines
std::stringstream* PndLmdAlignManager::readFile(std::string filename) {

	int lines=0;
	stringstream *valueStream = new stringstream();

	//get events from text file. later: directly from root file
	//cout << "reading info from file.." << endl;
	//stringstream* valueStream = ss;

	ifstream ifs;
	ifs.open(filename.c_str());
	if(!ifs.is_open()){
		cout << "could not read ";
		cout << filename;
		cout << "! aborting." << endl;
		return 0;
	}
	string linebuffer;// = new string();
	while(std::getline(ifs,linebuffer)){
		*valueStream << linebuffer << endl;
		lines++;
	}
	ifs.close();

	//cout << "file read to ram. "<< lines << " lines read." << endl;
	return valueStream;
}

Matrix PndLmdAlignManager::readMatrix(std::string filename) {

	vector< vector<double> > temp = readFromCSVFile(filename);

	if(temp.size() < 1){
		cout << "warning! can't read matrix from file " << filename << endl;
		exit(1);
	}
	if(temp[0].size() < 1){
		cout << "warning! can't read matrix from file " << filename << endl;
		exit(1);
	}

	int rows, columns;
	rows = temp.size();
	columns = temp[0].size();

	Matrix result(rows, columns);

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			result.val[i][j] = temp[i][j];
		}
	}

	if(false){
		// numerical corrections
		result.val[0][2] = 0.0;
		result.val[1][2] = 0.0;
		result.val[2][0] = 0.0;
		result.val[2][1] = 0.0;
		result.val[2][3] = 0.0;
	}

	return result;
}

//FIXME: Error reporting to log and handling
bool PndLmdAlignManager::writeMatrix(Matrix &mat, std::string filename){

	checkIOpaths();

	//cout << "writing matrix " << filename << "\n";
	std::ofstream outFileStream;
	outFileStream.open(filename.c_str());

	if(outFileStream.fail()){
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
	int i=0;
	ss = readFile(filename);
	string line, token;
	std::vector<std::vector<double> > data;
	while(getline(*ss, line)){
		iss << line;
		std::vector<double> tempvec;
		while(getline(iss, token, ',')) {
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

int PndLmdAlignManager::searchFiles(std::string path, std::vector<std::string> &list, std::string extension, bool includeSubDirs) {

	if(!boost::filesystem::exists(path)){
		return 0;
	}
	boost::filesystem::directory_iterator iterator(path);
	for(; iterator != boost::filesystem::directory_iterator(); ++iterator){
		if(boost::filesystem::is_directory(iterator->path()) && includeSubDirs){
			list.push_back(iterator->path().string());
			searchFiles(iterator->path().string(), list, extension, includeSubDirs);
		}
		else if(boost::filesystem::is_regular_file(iterator->path())){
			if( iterator->path().string().find(extension, (iterator->path().string().length() - extension.length())) != std::string::npos ){
				list.push_back(iterator->path().string());
			}
		}
	}
	sort(list.begin(), list.end());
	return list.size();

}

bool PndLmdAlignManager::mkdir(std::string path) {
	boost::filesystem::path bpath(path);
	if(boost::filesystem::exists(bpath)){
		return true;
	}
	return boost::filesystem::create_directories(bpath);
}

vector<string> PndLmdAlignManager::findRegex(std::string source, std::string regex){

	boost::regex expression(regex);

	std::string::const_iterator start, end;
	start = source.begin();
	end = source.end();
	boost::match_results<std::string::const_iterator> result;
	boost::match_flag_type flags = boost::match_default;

	vector<string> resultStrings;

	while(regex_search(start, end, result, expression, flags))
	{
		// update search position:
		start = result[0].second;

		for(int j=0; j<result.size(); j++){
			resultStrings.push_back( boost::lexical_cast<string>(result[j]));
		}

		// update flags:
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}

	return resultStrings;
}

int PndLmdAlignManager::searchDirectories(std::string curr_directory, std::vector<std::string> &list, bool includeSubDirs) {

	if(!boost::filesystem::exists(curr_directory)){
		return 0;
	}
	boost::filesystem::directory_iterator iterator(curr_directory);
	for(; iterator != boost::filesystem::directory_iterator(); ++iterator){
		if(boost::filesystem::is_directory(iterator->path())){
			list.push_back(iterator->path().string());
			//recursively call
			if(includeSubDirs){
				searchDirectories(iterator->path().string(), list, includeSubDirs);
			}
		}
	}
	sort(list.begin(), list.end());
	return list.size();

}

void PndLmdAlignManager::enableHelperMatrix(bool enable) {
	_enableHelperMatrix = enable;
	for(mapIt it = _aligners.begin();  it != _aligners.end(); it++){
		it->second.setNumericCorrection(_enableHelperMatrix);
	}
	if(_enableHelperMatrix){
		writeMatrix(helperMatrix, _matrixOutDir + "cmHelperMatrix.mat");
	}
}

void PndLmdAlignManager::setInCentimeters(bool inCentimeters) {
	_inCentimeters = inCentimeters;
	for(mapIt it = _aligners.begin();  it != _aligners.end(); it++){
		it->second.setInCentimeters(_inCentimeters);
	}
}

void PndLmdAlignManager::transformGlobalToLmd(Matrix& matrix) {

	//make temp copy
	Matrix tempmatrix = Matrix(matrix);

	double arrayLtoG[16];
	double arrayGtoL[16];

	// trafo matrix changes, too!!!
	TGeoHMatrix lmdToGlobal=dimension->Get_transformation_lmd_local_to_global(true);
	TGeoHMatrix globalToLmd=dimension->Get_transformation_global_to_lmd_local(true);

	lmdToGlobal.GetHomogenousMatrix(arrayLtoG);
	globalToLmd.GetHomogenousMatrix(arrayGtoL);

	double finalArrayLtoG[16];
	double finalArrayGtoL[16];

	//root documentation differs from actual implementation
	finalArrayLtoG[0] = arrayLtoG[0];
	finalArrayLtoG[1] = arrayLtoG[1];
	finalArrayLtoG[2] = arrayLtoG[2];
	finalArrayLtoG[3] = arrayLtoG[12];
	finalArrayLtoG[4] = arrayLtoG[4];
	finalArrayLtoG[5] = arrayLtoG[5];
	finalArrayLtoG[6] = arrayLtoG[6];
	finalArrayLtoG[7] = arrayLtoG[13];
	finalArrayLtoG[8] = arrayLtoG[8];
	finalArrayLtoG[9] = arrayLtoG[9];
	finalArrayLtoG[10] = arrayLtoG[10];
	finalArrayLtoG[11] = arrayLtoG[14];
	finalArrayLtoG[12] = arrayLtoG[3];
	finalArrayLtoG[13] = arrayLtoG[7];
	finalArrayLtoG[14] = arrayLtoG[11];
	finalArrayLtoG[15] = arrayLtoG[15];

	finalArrayGtoL[0] = arrayGtoL[0];
	finalArrayGtoL[1] = arrayGtoL[1];
	finalArrayGtoL[2] = arrayGtoL[2];
	finalArrayGtoL[3] = arrayGtoL[12];
	finalArrayGtoL[4] = arrayGtoL[4];
	finalArrayGtoL[5] = arrayGtoL[5];
	finalArrayGtoL[6] = arrayGtoL[6];
	finalArrayGtoL[7] = arrayGtoL[13];
	finalArrayGtoL[8] = arrayGtoL[8];
	finalArrayGtoL[9] = arrayGtoL[9];
	finalArrayGtoL[10] = arrayGtoL[10];
	finalArrayGtoL[11] = arrayGtoL[14];
	finalArrayGtoL[12] = arrayGtoL[3];
	finalArrayGtoL[13] = arrayGtoL[7];
	finalArrayGtoL[14] = arrayGtoL[11];
	finalArrayGtoL[15] = arrayGtoL[15];

	finalArrayLtoG[15] = 1.0;
	finalArrayGtoL[15] = 1.0;

	Matrix ltoG(4,4,finalArrayLtoG);
	Matrix gToL(4,4,finalArrayGtoL);

	matrix = gToL * tempmatrix * ltoG;
	//matrix = ltoG * tempmatrix * gToL;

}

Matrix PndLmdAlignManager::homogenizeMatrix(const Matrix& input) {

	if(input.m == 4 && input.n == 4){
		return input;
	}

	Matrix result = Matrix::eye(4);

	if(input.m == 3 && input.n == 3){
		result.val[0][0] = input.val[0][0];
		result.val[0][1] = input.val[0][1];
		result.val[0][2] = input.val[0][2];
		result.val[0][3] = 0;
		result.val[1][0] = input.val[1][0];
		result.val[1][1] = input.val[1][1];
		result.val[1][2] = input.val[1][2];
		result.val[1][3] = 0;
		result.val[2][0] = input.val[2][0];
		result.val[2][1] = input.val[2][1];
		result.val[2][2] = input.val[2][2];
		result.val[2][3] = 0;
		result.val[3][0] = 0;
		result.val[3][1] = 0;
		result.val[3][2] = 0;
		result.val[3][3] = 1;
		return result;
	}
	else if(input.m == 3 && input.n == 1){
		result.val[0][0] = 1;
		result.val[0][1] = 0;
		result.val[0][2] = 0;
		result.val[0][3] = input.val[0][0];
		result.val[1][0] = 0;
		result.val[1][1] = 1;
		result.val[1][2] = 0;
		result.val[1][3] = input.val[1][0];
		result.val[2][0] = 0;
		result.val[2][1] = 0;
		result.val[2][2] = 1;
		result.val[2][3] = input.val[2][0];
		result.val[3][0] = 0;
		result.val[3][1] = 0;
		result.val[3][2] = 0;
		result.val[3][3] = 1;
		return result;
	}

	cout << "Can only homogenize 3x3 matrix or 3x1 vector!\n";
	exit(1);

	return result;
}

/*
 * returns the matrix needed to transform the position measured by the MISALIGNED lumi (aka measured position)
 * to the real position of a hit (as seen by a perfectly aligned lumi). It is therefore dubbed "correction matrix"
 * (see interim reports).
 * ATTENTION. THis is in PANDA GLOBAL frame of reference, when working in LMD LOCAL, transform accordingly!
 *  * TODO: which interim report? look up date/number
 */
Matrix PndLmdAlignManager::getCorrectionMatrix(int id) {

	int half, plane, module, side, die, sensor;
	dimension->Get_sensor_by_id(id, half, plane, module, side, die, sensor);

	TGeoHMatrix sensorToGlobal = dimension->Get_transformation_sensor_to_global(half, plane, module, side, die, sensor, true);
	TGeoHMatrix globalToSensor = dimension->Get_transformation_global_to_sensor(half, plane, module, side, die, sensor, false);

	TGeoHMatrix combined = globalToSensor * sensorToGlobal;
	return castTGeoHMatrixToMatrix(combined);
}

//FIXME: warning, probably wrong
Matrix PndLmdAlignManager::getCorrectionMatrix(int id1, int id2) {

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(id1, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(id2, bhalf, bplane, bmodule, bside, bdie, bsensor);

	TGeoHMatrix sensorToLmd = dimension->Get_transformation_sensor_to_global(fhalf, fplane, fmodule, fside, fdie, fsensor, true);
	TGeoHMatrix lmdToSensor = dimension->Get_transformation_global_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, true);

	TGeoHMatrix matrixAligned = lmdToSensor * sensorToLmd;

	sensorToLmd = dimension->Get_transformation_sensor_to_global(fhalf, fplane, fmodule, fside, fdie, fsensor, false);
	lmdToSensor = dimension->Get_transformation_global_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, false);

	TGeoHMatrix matrixMisalignedInverse = (lmdToSensor * sensorToLmd).Inverse();
	TGeoHMatrix combined = matrixMisalignedInverse * matrixAligned;

	return castTGeoHMatrixToMatrix(combined);

}

Matrix PndLmdAlignManager::castTGeoHMatrixToMatrix(const TGeoHMatrix& matrix) {

	double* homogenousMatrix = new double[16];
	double* finalMatrix = new double[16];
	matrix.GetHomogenousMatrix(homogenousMatrix);

	//root documentation differs from actual implementation
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
	finalMatrix[15] = homogenousMatrix[15];
	finalMatrix[15] = 1.0;

	Matrix result(4,4,finalMatrix);
	delete homogenousMatrix, finalMatrix;
	return result;

}


Matrix PndLmdAlignManager::castTVector3toMatrix(const TVector3& vec) {
	Matrix result(4,1);
	double values[3];
	vec.GetXYZ(values);
	result.val[0][0] = values[0];
	result.val[1][0] = values[1];
	result.val[2][0] = values[2];
	result.val[3][0] = 1.0;
	return result;
}

//FIXME: no de-homogenizeation, use only for rigid transformations
TVector3 PndLmdAlignManager::castMatrixToTVector3(const Matrix& vec) {
	TVector3 result;
	result.SetXYZ(vec.val[0][0], vec.val[1][0], vec.val[2][0]);
	return result;
}

TVector3 PndLmdAlignManager::transformMeasuredToTrue(const TVector3& hit1measured, int sensorID) {


}

void PndLmdAlignManager::xOption(int option) {

	if(option==1){
		cout << "sensor to sensor in lmd local:\n";
		cout << getMatrixOfficialGeometry(0,5,true) << "\n";
		cout << "sensor to sensor in panda global:\n";
		cout << getMatrixOfficialGeometryGlobal(0,5,true) << "\n";
		cout << "sensor to sensor artificially transformed to panda global:\n";
		Matrix m1 = getMatrixOfficialGeometry(0,5,true);
		transformGlobalToLmd(m1);
		cout << m1 << "\n";
		cout << "difference:\n";
		cout << getMatrixOfficialGeometry(0,5,true) - getMatrixOfficialGeometryGlobal(0,5,true) << "\n";
	}

	if(option==2){
		int id1=0;
		int id2=5;

		//this is no mistake, I want the inverted matrix of (id1,id2)
		Matrix idealInv = getMatrixOfficialGeometry(id2,id1,true);
		Matrix real = getMatrixOfficialGeometry(id1,id2,false);
		Matrix correction = real * idealInv;
		cout << correction << "\n";

	}

	if(option==3){
		Matrix result1 = castTGeoHMatrixToMatrix(dimension->Get_transformation_lmd_local_to_global(true));
		cout << result1 << "\n";
		Matrix result2 = castTGeoHMatrixToMatrix(dimension->Get_transformation_global_to_lmd_local(true));
		cout << result2 << "\n";
		Matrix corr = getMatrixOfficialGeometry(0,5,true);
		cout << "corr without trafo:\n";
		cout << corr << "\n";
		cout << "corr with trafo:\n";
		cout << result2 * corr * result1 << "\n";
	}

	if(option==5){

		TChain* chainPairs = new TChain("cbmsim");
		for(int i=0; i<_fileNames.size(); i++){
			//cout << files[i] << endl;
			if( _fileNames[i].find("Lumi_Pairs") != std::string::npos ){
				chainPairs->Add(_fileNames[i].c_str());
			}
		}

		//pairs of sensors in LMD coordinates
		TClonesArray* hitPairs = new TClonesArray("PndLmdHitPair");
		chainPairs->SetBranchAddress("PndLmdHitPair", &hitPairs);
		int nEntries = chainPairs->GetEntries();
		cout << "HitPairs no of entries: " << nEntries << endl;

		cout << "Sorting Pairs to Manager...\n";
		int totalPairs=0;
		for(int i_event=0; i_event<nEntries; i_event++ ){

			loadBar(i_event, nEntries, 1000,30);
			chainPairs->GetEntry(i_event);
			int nPairs = hitPairs->GetEntries();
			//loop over hitPairs per Event
			for(int i_Pair=0; i_Pair<nPairs;i_Pair++){
				PndLmdHitPair* currentPair = (PndLmdHitPair*)hitPairs->At(i_Pair);

				//get hits 1 and 2 and transform 2 via matrix. compare hits!

			}
		}

		//TODO: remove
		cout << "=========================\n";
		cout << "total Pairs: " << totalPairs << endl;
		cout << "All done. Running Align Manager.\n";
		cout << "=========================\n";

		delete chainPairs, hitPairs;
	}

	else{
		cout << "invalid selection\n";
		exit(0);
	}

}
