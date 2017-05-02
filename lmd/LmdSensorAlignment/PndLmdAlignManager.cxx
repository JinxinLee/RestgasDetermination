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

#include <PndLmdDim.h>
#include <PndLmdSensorAligner.h>
#include <PndLmdHitPair.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TGeoMatrix.h>

using std::vector;
using std::string;

boost::mutex incrementMutex;

boost::thread_group alignerThreadGroup;

void PndLmdAlignManager::resetMTLB(int n, int r, int w){
	_i=0;
	_n=n;
	_r=r;
	_w=w;
}

void PndLmdAlignManager::incrementMTLB(){

	incrementMutex.lock();
	_i++;

	// Only update r times.
	if(_n==0) return;
	if(_n==1) return;

	if(_r>_n){
		_r = _n;
	}

	//calculate ev / sec every 100000 iterations
	if ( _i % (_n/_r) != 0 ){
		return;
	}

	flush(cout);

	// Calculate the ratio of complete-to-incomplete.
	float ratio = _i/(float)_n;
	int   c     = ratio * _w;

	// Show the percentage complete.
	printf("%3d%% [", (int)(ratio*100) );

	// Show the load bar.
	for (int x=0; x<c; x++)
		printf("=");

	for (int x=c; x<_w; x++)
		printf(" ");

	// ANSI Control codes to go back to the
	// previous line and clear it.
	printf("] %d of %d \n\033[F\033[J", _i, _n);
	incrementMutex.unlock();
}

PndLmdAlignManager::PndLmdAlignManager() {
	_firstInitDone=false;
	init();
}

void PndLmdAlignManager::init(){

	dimension = PndLmdDim::Instance();

	_info << "info for aligned areas\n";

	_zIsTimestamp=true;
	_allFilesAdded=false;
	useSimpleStorage=true;
	_singleAligner=true;
	_pretend=false;
	_inCentimeters=false;
	_enableHelperMatrix=false;
	_multithreaded=true;

	//int overlapId=-1;
	fileNames.clear();
	aligners.clear();

	vector<int> overlapIDs = dimension->getAvailableOverlapIDs();
	for(size_t i=0; i<overlapIDs.size(); i++){
		int overlapId = overlapIDs[i];

		PndLmdSensorAligner tempAligner;
		tempAligner.setOverlapId(overlapId);
		tempAligner.setModuleID(dimension->makeModuleID(overlapId));
		tempAligner.setId1(dimension->getID1fromOverlapID(overlapId));
		tempAligner.setId2(dimension->getID2fromOverlapID(overlapId));
		tempAligner.setHelperMatrix(helperMatrix);
		tempAligner.setZasTimetamp(_zIsTimestamp);
		tempAligner.setNumericCorrection(_enableHelperMatrix);
		tempAligner.setInCentimeters(_inCentimeters);
		aligners[overlapId]=tempAligner;
	}

	// we just started, all aligners are empty
	for(size_t i=0; i<overlapIDs.size(); i++){
		alignersFull[overlapIDs[i]] = false;
	}

	outFilename="";
	_firstInitDone=true;
	std::cout << "PndLmdAlignManager::Init(): Initialization successful." << std::endl;
}

PndLmdAlignManager::~PndLmdAlignManager(){
}

bool PndLmdAlignManager::addPair(PndLmdHitPair& pair) {

	bool success = false;
	// check if the aligner for that pair is full. if yes, skip this pair.
	// do this even before checking that pair, saves on cpu time.
	if(alignersFull[pair.getOverlapId()]){
		return false;
	}

	pair.check();
	if(pair.isSane() ){
		if(useSimpleStorage){
			success = aligners[pair.getOverlapId()].addSimplePair(pair);	//returns true if addPair succeeded
			alignersFull[pair.getOverlapId()] = !success;		//if addPair failed, the aligner is full
		}
		else{
			//_aligners[pair.getOverlapId()].addPair(pair);
			cout << "WARNING! Legacy storage mode is no longer supported.";
		}
	}
	else{
		cout << "pair is not sane. processing failed.\n";
		success = false;
	}

	return success;
}

bool PndLmdAlignManager::addPairAndStartAligner(PndLmdHitPair &pair){

	bool success = false;

	// check if the aligner for that pair is full. if yes, skip this pair.
	// do this even before checking that pair, saves on cpu time.
	if(alignersFull[pair.getOverlapId()]){
		return false;
	}

	pair.check();
	if(pair.isSane() ){
		if(useSimpleStorage){
			success = aligners[pair.getOverlapId()].addSimplePair(pair);	//returns true if addPair succeeded
			alignersFull[pair.getOverlapId()] = !success;		//if addPair failed, the aligner is full
		}
		else{
			//_aligners[pair.getOverlapId()].addPair(pair);
			cout << "WARNING! Legacy storage mode is no longer supported.";
		}
	}
	else{
		cout << "pair is not sane. processing failed.\n";
	}

	//if pair could not be added, aligner is full. start thread directly.
	if(!success){
		//cout << "Aligner " << pair.getOverlapId() << " full. starting!\n";
		alignerThreadGroup.create_thread(
				boost::bind(
						&PndLmdAlignManager::alignOne, this, boost::ref(
								aligners[pair.getOverlapId()]
						)
				)
		);
	}

	//check if all aligners are done
	allAlignersDone=true;
	for(map<int, bool>::iterator it = alignersFull.begin(); it != alignersFull.end(); it++){
		if(!(it->second)){
			allAlignersDone=false;
			break;
		}
	}
	if(allAlignersDone){
		cout << "all aligners are already full. no further files will be read.\n";
	}

	return success;
}

void PndLmdAlignManager::validate() {
	std::cout << "using " << aligners.size() << " aligners, which have:\n";
	for(mapIt it = aligners.begin();  it != aligners.end(); it++){
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
		fileNames.push_back(filename);
		return true;
	}
}

int PndLmdAlignManager::addFilesFromDirectory(std::string directory, int maxFiles) {

	if(_pretend){
		return -1;
	}

	if(_allFilesAdded){
		return fileNames.size();
	}
	else{
		std::vector<string> list;
		searchFiles(directory, list, ".root", false);
		for(size_t i=0;i<list.size();i++){
			fileNames.push_back(list[i]);
			if((int)i==maxFiles-1){			//we use == instead of >= so that maxFiles=0 always chooses all files
				break;
			}
		}
		_allFilesAdded=true;
		cout << "looking for files in " << directory << ". choose " << fileNames.size() << " files of maximum of " << maxFiles <<".\n";
		return fileNames.size();
	}

}

void PndLmdAlignManager::readFiles(){

	if(!_firstInitDone){
		init();
	}

	if(_pretend){
		cout << "pretending to read files...\n";
		return;
	}

	_allFilesAdded=true;

	int noOfFiles = fileNames.size();
	if(noOfFiles > 0){
		cout << "found " << noOfFiles << " file(s). reading...\n";
	}
	else{
		cout << "no files found. exiting.\n";
		exit(0);
	}

	TChain* chainPairs = new TChain("cbmsim");
	for(size_t i=0; i<fileNames.size(); i++){
		//cout << files[i] << endl;
		if( fileNames[i].find("Lumi_Pairs") != std::string::npos ){
			chainPairs->Add(fileNames[i].c_str());
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

		loadBar(i_event, nEntries, 1000,60);
		chainPairs->GetEntry(i_event);
		int nPairs = hitPairs->GetEntries();

		//loop over hitPairs per Event
		for(int i_Pair=0; i_Pair<nPairs;i_Pair++){
			PndLmdHitPair* currentPair = (PndLmdHitPair*)hitPairs->At(i_Pair);
			addPair(*currentPair);
			totalPairs++;
		}
	}

	cout << "================================\n";
	cout << "total Pairs: " << totalPairs << endl;
	cout << "All done. Running Align Manager.\n";
	cout << "================================\n";

	delete chainPairs;
	delete hitPairs;
}


void PndLmdAlignManager::readFilesAndAlign(){

	if(!_firstInitDone){
		init();
	}

	if(_pretend){
		cout << "pretending to read files...\n";
		return;
	}

	_allFilesAdded=true;

	int noOfFiles = fileNames.size();
	if(noOfFiles > 0){
		cout << "found " << noOfFiles << " file(s). reading...\n";
	}
	else{
		cout << "no files found. exiting.\n";
		exit(0);
	}

	TChain* chainPairs = new TChain("cbmsim");
	for(size_t i=0; i<fileNames.size(); i++){
		//cout << files[i] << endl;
		if( fileNames[i].find("Lumi_Pairs") != std::string::npos ){
			chainPairs->Add(fileNames[i].c_str());
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

		loadBar(i_event, nEntries, 1000,60);
		chainPairs->GetEntry(i_event);
		int nPairs = hitPairs->GetEntries();

		//loop over hitPairs per Event
		for(int i_Pair=0; i_Pair<nPairs;i_Pair++){
			PndLmdHitPair* currentPair = (PndLmdHitPair*)hitPairs->At(i_Pair);

			/*
			if(currentPair->getOverlapId() == 0){
				cout << "testpair\n";
				cout << "col1: " << currentPair->getCol1() << "\n";
				cout << "row1: " << currentPair->getRow1() << "\n";
				cout << "col2: " << currentPair->getCol2() << "\n";
				cout << "row2: " << currentPair->getRow2() << "\n";
			}
			 */


			addPairAndStartAligner(*currentPair);
			totalPairs++;

			if(allAlignersDone){
				return;
			}
		}
	}

	cout << "================================\n";
	cout << "total Pairs: " << totalPairs << endl;
	cout << "All done. Running Align Manager.\n";
	cout << "================================\n";

	delete chainPairs;
	delete hitPairs;
}

void PndLmdAlignManager::alignOne(PndLmdSensorAligner &aligner){

	//cout << "Aligner " << aligner.getOverlapId() << " starting.\n";
	//cout << "Saving binaries to: " << _binaryPairFileDirectory << "!\n";

	//start aligner, this can be done concurrently
	aligner.calculateMatrix();

	//write binary file only if not already present
	if(!checkForBinaryFiles()){
		aligner.writePairsToBinary(_binaryPairFileDirectory);
	}
	//if binary files already present, then they have been read earliert and we can display a progress bar for the aliners
	else{
		incrementMTLB();
	}

	//free memory
	aligner.clearPairs();
}

void WorkerThread( boost::shared_ptr< boost::asio::io_service > io_service ){
	io_service->run();
}

void PndLmdAlignManager::alignST() {

	int cur, tot;
	cur=0;
	tot=aligners.size();

	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		loadBar(cur++, tot, 1000, 60);
		it->second.calculateMatrix();
		if(it->second.successful()){
			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters, _enableHelperMatrix);
			writeMatrix(result, matrixFilename);

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "area " << it->second.getId1() << " to " << it->second.getId2() << "\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";

		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
}

void PndLmdAlignManager::alignMT() {

	//new version, multi threaded using thread pool model and boost::asio implementation

	//shared pointer, since io_services can't be copied
	boost::shared_ptr< boost::asio::io_service > io_service(new boost::asio::io_service);
	boost::shared_ptr< boost::asio::io_service::work > work(new boost::asio::io_service::work( *io_service ));

	//thread group
	boost::thread_group worker_threads;

	//make threads, n is number of threads:
	int nThreads;
	nThreads = boost::thread::hardware_concurrency();

	//FIXME: remove after debug!
	nThreads =1;

	//sometimes hardware_concurrency returns 0 if it can't detect.
	if(nThreads < 1){
		cout << "INFO:: could not detect number of cores. assuming 4.\n";
		nThreads = 4;
	}

	//create worker threads
	for(int i=0; i<nThreads; i++){
		worker_threads.create_thread( boost::bind( &WorkerThread, io_service ) );
	}

	resetMTLB(aligners.size(), aligners.size(), 60);

	//post work for threads
	for(mapIt it=aligners.begin(); it != aligners.end(); it++){

		/*
		 * when binding member classes, boost::bind needs the namespace AND the pointer to an object
		 * of that class (here: this-pointer). Also, when using references, use boost::ref()
		 */
		io_service->post( boost::bind( &PndLmdAlignManager::alignOne, this, boost::ref( it->second ) ) );
	}

	//wait for all threads to complete, then and only then write matrices to disk
	work.reset();
	worker_threads.join_all();

	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		if(it->second.successful()){

			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters, _enableHelperMatrix);

			if(!writeMatrix(result, matrixFilename)){
				cout << "ERROR: could not write matrix " << matrixFilename << "\n";
			}

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "area " << it->second.getId1() << " to " << it->second.getId2() << "\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";
		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
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
	cout << "starting aligners...\n";

	if(_multithreaded){
		alignMT();
	}
	else{
		alignST();
	}

	//write matrix info
	ofstream of;
	if(_inCentimeters){
		of.open(( _matrixOutDir + "/info-cm.txt").c_str());
	}
	else{
		of.open(( _matrixOutDir + "/info-px.txt").c_str());
	}

	of << _info.str();
	of.close();
	cout << "all aligners done.\n";
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
	tot=aligners.size();

	//maybe do this multithreaded?
	for(mapIt it=aligners.begin(); it != aligners.end(); it++){

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

			int overlapID, nPairs;
			double sinAlphaAbs, sinAlphaRel, xAbs, xRel, yAbs, yRel;

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

			of << overlapID << "," << "," << id1 << "," << id2 << "," << nPairs;
			of << "," << sinAlphaAbs << "," << xAbs << "," << yAbs << ",";
			of << sinAlphaRel << "," << xRel << "," << yRel << endl;
		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}
	of.close();
}

//these parameters depend on sensor geometry. Use with caution!
Matrix PndLmdAlignManager::transformMatrixFromPixelsToCm(const Matrix &input) {

	/*
	 * in principle, there should also be a matrix that transforms from sensor active to sensor,
	 * but inactive area is already accounted for in pixelsX and pixelsY!
	 */

	//very important, because I still don't know how pixel number corresponds to active area
	double pixelsX = 247.5;			//guard ring removes part of active area
	double pixelsY = 242.5;			//guard ring removes part of active area
	double pixelSize = 80e-4;		//in cm!

	//center of pixel correction matrices
	Matrix matrixPixelEdgeToCenter;
	Matrix matrixActiveEdgeToCenter;
	Matrix matrixScaleActiveToSensor;
	Matrix newPixelsToSensorInCm, newSensorInCmToPixels;

	//measure pixels in their center, not their corner
	matrixPixelEdgeToCenter = Matrix::eye(4);
	matrixPixelEdgeToCenter.val[0][3] += 0.5;
	matrixPixelEdgeToCenter.val[1][3] += 0.5;

	//move from sensor edge (pixel [0,0] to center of sensor)
	//KEEP IN MIND! This matrix also takes inactive area into account!!
	matrixActiveEdgeToCenter = Matrix::eye(4);
	matrixActiveEdgeToCenter.val[0][3] -= pixelsX/2;
	matrixActiveEdgeToCenter.val[1][3] -= pixelsY/2;

	//scaling must be applied after translations
	matrixScaleActiveToSensor = Matrix::eye(4);
	matrixScaleActiveToSensor.val[0][0]=pixelSize;
	matrixScaleActiveToSensor.val[1][1]=pixelSize;

	newPixelsToSensorInCm = matrixScaleActiveToSensor * matrixActiveEdgeToCenter * matrixPixelEdgeToCenter;
	newSensorInCmToPixels = Matrix(newPixelsToSensorInCm);
	newSensorInCmToPixels.inv();

	Matrix result;
	result = newPixelsToSensorInCm * input * newSensorInCmToPixels;
	return result;
}

/*
 * get transformation matrix of fromSensor -> toSensor (LMD local reference frame)
 * ATTENTION! set aligned flag true for perfect geometry, else use false!
 */
Matrix PndLmdAlignManager::getMatrixOfficialGeometry(int fromSensor, int toSensor, bool aligned){

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(fromSensor, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(toSensor, bhalf, bplane, bmodule, bside, bdie, bsensor);

	const TGeoHMatrix& matrixSen1ToLmd = dimension->Get_transformation_sensor_to_lmd_local(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	const TGeoHMatrix& matrixLmdToSen2   = dimension->Get_transformation_lmd_local_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);

	Matrix matSen1ToLmd = castTGeoHMatrixToMatrix(matrixSen1ToLmd);
	Matrix matLmdToSen2 = castTGeoHMatrixToMatrix(matrixLmdToSen2);
	return matLmdToSen2 * matSen1ToLmd;

}

/*
 * get transformation matrix of fromSensor -> toSensor (PANDA global reference frame)
 * this is in cm and IN PANDA GLOBAL.
 * ATTENTION! set aligned flag true for perfect geometry, else use false!
 */
Matrix PndLmdAlignManager::getMatrixOfficialGeometryGlobal(int fromSensor, int toSensor, bool aligned){

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(fromSensor, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(toSensor, bhalf, bplane, bmodule, bside, bdie, bsensor);

	const TGeoHMatrix& matrixSen1ToLmd = dimension->Get_transformation_sensor_to_global(fhalf, fplane, fmodule, fside, fdie, fsensor, aligned);
	const TGeoHMatrix& matrixLmdToSen2   = dimension->Get_transformation_global_to_sensor(bhalf, bplane, bmodule, bside, bdie, bsensor, aligned);

	Matrix matSen1ToLmd = castTGeoHMatrixToMatrix(matrixSen1ToLmd);
	Matrix matLmdToSen2 = castTGeoHMatrixToMatrix(matrixLmdToSen2);
	return matLmdToSen2 * matSen1ToLmd;
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

void PndLmdAlignManager::checkIOpaths() {

	if(fileNames.size()==0 ){
		//cout << "no pair files specified, are we using binary data?\n";
	}
	for(size_t iFile=0; iFile<fileNames.size(); iFile++){
		if(!boost::filesystem::exists(fileNames[iFile])){
			cout << "error opening file:";
			cout << fileNames[iFile] << "\n";
			exit(1);
		}
	}

	if(!(outFilename=="")){
		boost::filesystem::path outPath(outFilename);
		if(!boost::filesystem::exists(outPath.parent_path())){
			boost::filesystem::create_directories(outPath.parent_path());
		}
	}

	if(!(_matrixOutDir=="")){
		if(!boost::filesystem::exists(_matrixOutDir)){
			boost::filesystem::create_directories(_matrixOutDir);
		}
	}
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
		return NULL;
	}
	string linebuffer;// = new string();
	while(std::getline(ifs,linebuffer)){
		*valueStream << linebuffer << endl;
		lines++;
	}
	ifs.close();

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
	return result;
}

//TODO: Error reporting to log and handling
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

		for(size_t j=0; j<result.size(); j++){
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

			//recursively call for sub directories
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
	for(mapIt it = aligners.begin();  it != aligners.end(); it++){
		it->second.setNumericCorrection(_enableHelperMatrix);
	}
	if(_enableHelperMatrix){
		writeMatrix(helperMatrix, _matrixOutDir + "cmHelperMatrix.mat");
	}
}

void PndLmdAlignManager::setInCentimeters(bool inCentimeters) {
	_inCentimeters = inCentimeters;
	for(mapIt it = aligners.begin();  it != aligners.end(); it++){
		it->second.setInCentimeters(_inCentimeters);
	}
}

void PndLmdAlignManager::setZasTimestamp(bool timestamp) {
	_zIsTimestamp = timestamp;
	for(mapIt it = aligners.begin();  it != aligners.end(); it++){
		it->second.setZasTimetamp(_zIsTimestamp);
	}
}

void PndLmdAlignManager::transformGlobalToLmd(Matrix& matrix) {

	cerr << "WARNING. You are using transformGlobalToLmd. This should not be needed anymore!\n";

	//make temp copy
	Matrix tempmatrix = Matrix(matrix);

	//get appropriate matrices
	TGeoHMatrix lmdToGlobal=dimension->Get_transformation_lmd_local_to_global(true);
	TGeoHMatrix globalToLmd=dimension->Get_transformation_global_to_lmd_local(true);

	//use existing casting function
	Matrix lToG = castTGeoHMatrixToMatrix(lmdToGlobal);
	Matrix gToL = castTGeoHMatrixToMatrix(globalToLmd);

	//return result
	matrix = lToG * tempmatrix * gToL;
}

void PndLmdAlignManager::transformFromSensorToLmdLocal(Matrix& matrix, int sensorId, bool aligned) {
	//create local copy
	Matrix tempmatrix = Matrix(matrix);

	int half, plane, module, side, die, sensor;
	dimension->Get_sensor_by_id(sensorId, half, plane, module, side, die, sensor);

	TGeoHMatrix sensorToLmd = dimension->Get_transformation_sensor_to_lmd_local(half, plane, module, side, die, sensor, aligned);
	Matrix matSensorToLmd = castTGeoHMatrixToMatrix(sensorToLmd);
	Matrix matLmdToSensor = Matrix::inv(matSensorToLmd);

	matrix = matLmdToSensor * matrix * matSensorToLmd;
}

void PndLmdAlignManager::transformFromLmdLocalToSensor(Matrix& matrix, int sensorId, bool aligned) {
	//create local copy
	Matrix tempmatrix = Matrix(matrix);

	int half, plane, module, side, die, sensor;
	dimension->Get_sensor_by_id(sensorId, half, plane, module, side, die, sensor);

	TGeoHMatrix sensorToLmd = dimension->Get_transformation_sensor_to_lmd_local(half, plane, module, side, die, sensor, aligned);
	Matrix matSensorToLmd = castTGeoHMatrixToMatrix(sensorToLmd);
	Matrix matLmdToSensor = Matrix::inv(matSensorToLmd);

	matrix = matSensorToLmd *  matrix * matLmdToSensor;
}

/*
 * returns the matrix needed to transform the position measured by the MISALIGNED lumi (aka measured position)
 * to the real position of a hit (as seen by a perfectly aligned lumi). It is therefore dubbed "correction matrix"
 * (see interim reports).
 * This is in lmd local frame of reference
 */
Matrix PndLmdAlignManager::getCorrectionMatrix(int id) {

	int half, plane, module, side, die, sensor;
	dimension->Get_sensor_by_id(id, half, plane, module, side, die, sensor);

	TGeoHMatrix sensorToLmd = dimension->Get_transformation_sensor_to_lmd_local(half, plane, module, side, die, sensor, true);
	TGeoHMatrix lmdToSensor = dimension->Get_transformation_lmd_local_to_sensor(half, plane, module, side, die, sensor, false);

	Matrix sensorToLmdMat = castTGeoHMatrixToMatrix(sensorToLmd);
	Matrix lmdToSensorMat = castTGeoHMatrixToMatrix(lmdToSensor);

	return lmdToSensorMat * sensorToLmdMat;
}

Matrix PndLmdAlignManager::getCorrectionMatrix(int id1, int id2) {
	Matrix corrId1 = getCorrectionMatrix(id1);
	Matrix corrId2 = getCorrectionMatrix(id2);
	return corrId2 * corrId1.inv(corrId1);
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
	//finalMatrix[15] = homogenousMatrix[15];	//don't really need to do this anymore
	finalMatrix[15] = 1.0;

	//create matrix and clean up
	Matrix result(4,4,finalMatrix);
	delete homogenousMatrix; delete finalMatrix;
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

TVector3 PndLmdAlignManager::castMatrixToTVector3(const Matrix& vec) {

	TVector3 result;
	if(vec.val[3][0] < 1e-11){
		cout << "ERROR: castMatrixToTVector3: matrix can not be dehomogenized.\n";

	}
	else{
		result.SetXYZ(vec.val[0][0]/vec.val[3][0], vec.val[1][0]/vec.val[3][0], vec.val[2][0]/vec.val[3][0]);
	}
	return result;
}

bool PndLmdAlignManager::checkForBinaryFiles() {

	//list all IDs that SHOULD be there
	vector<int> availableIds = dimension->getAvailableOverlapIDs();

	vector<string> files;
	searchFiles(_binaryPairFileDirectory, files, "bin", false);
	int foundFiles=0;

	//cout << "looking for binary files in " << _binaryPairFileDirectory << "\n";

	//no binary files at all!
	if(files.size()==0){
		return false;
	}

	string matrixName;
	bool tempfilefound=false;

	//check for every ID that should be there if there is a corresponding file
	for(size_t i=0; i<availableIds.size(); i++){

		//reset counter
		tempfilefound=false;
		matrixName = makeBinaryPairFileName(availableIds[i], _inCentimeters);

		for(size_t j=0; j<files.size(); j++){
			if(files[j].find(matrixName)!=string::npos){
				//file is present
				//cout << "file: " << files[j] << ", id: " << availableIds[i] << "\n";
				tempfilefound=true;
				foundFiles++;
			}
		}

		//file not found? then at least one is missing, return false
		if(!tempfilefound){
			return tempfilefound;
		}
	}
	//cout << "found " << foundFiles << "\n";
	return true;
}

std::string PndLmdAlignManager::makeBinaryPairFileName(int overlapId, bool incentimeters, bool ) {
	std::stringstream filename;
	filename << "/pairs-";
	filename << overlapId;
	if(incentimeters){
		filename << "-cm";
	}
	else{
		filename << "-px";
	}
	filename << ".bin";
	return filename.str();
}

std::string PndLmdAlignManager::makeBinaryPairFileName(int sensorOne, int sensorTwo, bool incentimeters, bool ) {
	int overlapId;
	PndLmdDim *dimension = PndLmdDim::Instance();
	overlapId = dimension->makeOverlapID(sensorOne, sensorTwo);
	return makeBinaryPairFileName(overlapId, incentimeters);
}

std::string PndLmdAlignManager::makeMatrixFileName(int overlapId, bool incentimeters, bool ) {
	stringstream matrixName;
	matrixName << "/m";
	if(incentimeters){
		matrixName << overlapId << "cm.mat";
	}
	else{
		matrixName << overlapId << "px.mat";
	}
	return matrixName.str();
}

std::string PndLmdAlignManager::makeMatrixFileName(int sensorOne, int sensorTwo, bool incentimeters, bool ) {
	int overlapId;
	PndLmdDim *dimension = PndLmdDim::Instance();
	overlapId = dimension->makeOverlapID(sensorOne, sensorTwo);
	return makeMatrixFileName(overlapId, incentimeters);
}

Matrix PndLmdAlignManager::combineMatrix(int id1, int id2) {

	bool success = false;

	//FIXME: this is not the correct way to do this. better use the real int values below
	if(id1 > id2){
		swap(id1,id2);
	}

	//what module are we on? are id1 and id2 on same module?
	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(id1, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(id2, bhalf, bplane, bmodule, bside, bdie, bsensor);

	if(fhalf != bhalf){
		cout << "error! id1 and id2 are not on same half!\n";
		success = false;
	}
	if(fplane != bplane){
		cout << "error! id1 and id2 are not on same plane!\n";
		success = false;
	}
	if(fmodule != bmodule){
		cout << "error! id1 and id2 are not on same module!\n";
		success = false;
	}

	Matrix result;

	//check if either sensor is 0 on that module
	if(!(fside==0 && fdie == 0 && fsensor ==0)){
		cout << "can only do matrices from 0 to i for now. sorry!\n";
		success=false;
		return result;
	}

	//at this point, id0 should end in 0, so we can just add numbers
	//FIXME: obviously, this is shuddy and needs fixing

	success=true;

	//last time, just for safety
	if( (id1 % 10) != 0 ){
		cout << "error: id1 is not first sensor on module, something went wrong!\n";
	}

	//FIXME: assign matrices, this is shuddy atm. source this out to pndlmddim.
	string m05f = _matrixOutDir + makeMatrixFileName(id1+0, id1+5, _inCentimeters);
	string m18f = _matrixOutDir + makeMatrixFileName(id1+1, id1+8, _inCentimeters);
	string m28f = _matrixOutDir + makeMatrixFileName(id1+2, id1+8, _inCentimeters);
	string m29f = _matrixOutDir + makeMatrixFileName(id1+2, id1+9, _inCentimeters);
	string m36f = _matrixOutDir + makeMatrixFileName(id1+3, id1+6, _inCentimeters);
	string m37f = _matrixOutDir + makeMatrixFileName(id1+3, id1+7, _inCentimeters);
	string m38f = _matrixOutDir + makeMatrixFileName(id1+3, id1+8, _inCentimeters);
	string m47f = _matrixOutDir + makeMatrixFileName(id1+4, id1+7, _inCentimeters);
	string m49f = _matrixOutDir + makeMatrixFileName(id1+4, id1+9, _inCentimeters);

	//god gave us this matrix:
	Matrix m01 = getMatrixOfficialGeometry(id1, id1+1, false);

	//cout << "m01: \n" << m01 << "\n";

	// TODO: the following code can be optimized, as it currently computes the inverse of 9 matrices.
	// also, only read matrices from disk that are required here.
	// but that's not really a priority right now

	Matrix m05 = readMatrix(m05f);
	Matrix m18 = readMatrix(m18f);
	Matrix m28 = readMatrix(m28f);
	Matrix m29 = readMatrix(m29f);
	Matrix m36 = readMatrix(m36f);
	Matrix m37 = readMatrix(m37f);
	Matrix m38 = readMatrix(m38f);
	Matrix m47 = readMatrix(m47f);
	Matrix m49 = readMatrix(m49f);

	Matrix m50 = m05.inv(m05);
	Matrix m81 = m18.inv(m18);
	Matrix m82 = m28.inv(m28);
	Matrix m92 = m29.inv(m29);
	Matrix m63 = m36.inv(m36);
	Matrix m73 = m37.inv(m37);
	Matrix m83 = m38.inv(m38);
	Matrix m74 = m47.inv(m47);
	Matrix m94 = m49.inv(m49);

	//TODO: check here for m01 or m56 presence, we only need one.

	Matrix m56 = m50*m01*m18*m83*m36;		//TODO: check if this is correct!
	//Matrix m56s = getMatrixOfficialGeometry(5,6, false);
	//cout << "drum roll:\n " << m56-m56s << "\n end of drum roll \n"; //seems to work

	id2 %= 10;

	//TODO: check again, something doesn't work


	//finally, if cascade:
	//TODO: check if this is not backwards again.  multiplication goes from right to left!
	switch(id2){
	case 2:
		result = m01*m18*m82;
		break;
	case 3:
		result = m01*m18*m83;
		break;
	case 4:
		result = m01*m18*m82*m29*m94;		//TODO: case 4 can have two differnt paths, check both!
		break;
	case 5:
		result = m05;
		break;
	case 6:
		result = m05*m56;
		break;
	case 7:
		result = m05*m56*m63*m37;
		break;
	case 8:
		result = m05*m56*m63*m38;
		break;
	case 9:
		result = m05*m56*m63*m38*m82*m29;	//TODO: case 9 can have two differnt paths, check both!
		break;
	default:
		result = Matrix::eye(4);
		success = false;
	}

	//return successfully combined matrix
	if(success){
		return result;
	}
	//else return unity matrix
	cerr << "warning! could not return matrix " << id1 << " to " << id2 << "! returning identity matrix!\n";
	return Matrix::eye(4);
}

void PndLmdAlignManager::setMaxPairs(int maxPairs) {

	if(maxPairs > 0){
		for(mapIt it=aligners.begin(); it != aligners.end(); it++){
			it->second.setMaximumNumberOfHitPairs(maxPairs);
		}
		return;
	}
	else{
		cout << "warning. max pairs must be larger than 0!\n";
		return;
	}

}

void PndLmdAlignManager::clearScreen() {
	cout << "\x1B[2J\x1B[H";
}

void PndLmdAlignManager::compareCombinedMatrices() {

	// for every half
	// for every plane
	// for every module

	//double avgX=0, avgY=0, avgA=0; //[R.K. 01/2017] unused variable

	for(int iHalf=0; iHalf < 2; iHalf++){
		for(int iPlane=0; iPlane < 4; iPlane++){
			for(int iModule=0; iModule < 5; iModule++){
				// calculate all combined matrices
				// compare with target matrices
				// histogram dat shit on a per-module basis
				// if results are good and consistent, on a corridor and/or half plane basis

				int firstID=0;
				firstID = dimension->Get_sensor_id(iHalf, iPlane, iModule, 0, 0, 0);

				if(firstID % 10 !=0 ){
					cout << "WARNING. something went wrong. first sensor of module should be mod 10";
					cout << ", but is actually " << firstID << "\n";
				}

				for(int iSecondSensor=1; iSecondSensor<9; iSecondSensor++){
					cout << "-------------------------------\n";
					Matrix thisCombined = combineMatrix(firstID, iSecondSensor);
					Matrix targetCombined = getMatrixOfficialGeometry(firstID, iSecondSensor, true);
					cout << "thismat:\n" << thisCombined << "\ntargetmat:\n" << targetCombined << "\n";
				}
			}
		}
	}
}

void PndLmdAlignManager::computeCombinedMatrices() {
	cout << "PndLmdAlignManager::computeCombinedMatrices(): feature not implemented yet.\n";

}

void PndLmdAlignManager::waitForCompletion() {

	//start all alignsers that have not already started (i.e. don't have required no of Pairs)

	int notStarted=0;

	cout << "starting remaining aligners.\n";
	for(map<int, bool>::iterator it=alignersFull.begin(); it != alignersFull.end(); it++){
		if(!(it->second)){

			//cout << "starting aligner " << it->first << "\n";
			//if pair could not be added, aligner is full. start thread directly.
			alignerThreadGroup.create_thread(
					boost::bind(
							&PndLmdAlignManager::alignOne, this, boost::ref(
									aligners[it->first]
							)
					)
			);
			notStarted++;
		}

	}
	cout << notStarted << " aligners remained.\n";

	cout << "jobs queue size : " << alignerThreadGroup.size() << "/360\n";
	cout << "waiting for all aligners to finish...";
	//wait for all threads to complete
	alignerThreadGroup.join_all();
	cout << "done!\n";

	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		if(it->second.successful()){

			Matrix result = it->second.getResultMatrix();
			string matrixFilename = _matrixOutDir + makeMatrixFileName(it->second.getOverlapId(), _inCentimeters, _enableHelperMatrix);

			if(!writeMatrix(result, matrixFilename)){
				cout << "ERROR: could not write matrix " << matrixFilename << "\n";
			}

			_info << "aligner " << it->second.getOverlapId() << ":\n";
			_info << "area " << it->second.getId1() << " to " << it->second.getId2() << "\n";
			_info << "no of pairs: " << it->second.getNoOfPairs() << "\n";
			_info << "\n";
		}
		else{
			cout << "Error: aligner for " << it->second.getOverlapId() << " failed.\n";
		}
	}

	//write matrix info
	ofstream of;
	if(_inCentimeters){
		of.open(( _matrixOutDir + "/info-cm.txt").c_str());
	}
	else{
		of.open(( _matrixOutDir + "/info-px.txt").c_str());
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
	scale.val[0][0]  *= 80e-4;
	scale.val[1][1]  *= 80e-4;

	result = scale * shift;

	return result;
}

Matrix PndLmdAlignManager::makeFourVector(double x, double y, double z) {
	Matrix result(4,1);
	result.val[0][0] = x;
	result.val[1][0] = y;
	result.val[2][0] = z;
	result.val[3][0] = 1.0;
	return result;
}

void PndLmdAlignManager::xOption(int option) {

	if(option==15){
		cout << "checking if cm and x are about the same data.\n";

		//load pairs.root, it has (row,col) info and (x,y) info

		//check if (x,y)... well, no. this won't work...



	}
	else{
		cout << "invalid selection\n";
		exit(0);
	}

}

bool PndLmdAlignManager::writePairsToBinaryFiles() {

	if(_binaryPairFileDirectory==""){
		cout << "error: binary pair file directory not set. use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur=0;
	tot=aligners.size();
	bool success = true;

	cout << "writing all pairs to binary files\n";
	mkdir(_binaryPairFileDirectory);

	//maybe do this multithreaded?
	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		loadBar(cur++, tot, 1000, 60);
		if(!it->second.writePairsToBinary(_binaryPairFileDirectory)){
			success=false;
		}

	}
	return success;
}

bool PndLmdAlignManager::readPairsFromBinaryFiles() {

	if(_binaryPairFileDirectory==""){
		cout << "error: binary pair file directory not set. use PndLmdAlignManager::setBinaryPairFileDirectory()\n";
		return false;
	}

	int cur, tot;
	cur=0;
	tot=aligners.size();
	bool success = true;

	cout << "reading all pairs from binary files\n";

	//maybe do this multithreaded?
	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		loadBar(cur++, tot, 1000, 60);
		if(!it->second.readPairsFromBinary(_binaryPairFileDirectory)){
			success=false;
		}
		else{
			//generate ID1 and ID2 from overlapID;
			it->second.setId1(dimension->getID1fromOverlapID(it->second.getOverlapId()));
			it->second.setId2(dimension->getID2fromOverlapID(it->second.getOverlapId()));

			//FIXME: remove after debugging!
			return true;
		}
	}
	return success;
}

void PndLmdAlignManager::clearPairs() {
	for(mapIt it=aligners.begin(); it != aligners.end(); it++){
		it->second.clearPairs();
	}
}
