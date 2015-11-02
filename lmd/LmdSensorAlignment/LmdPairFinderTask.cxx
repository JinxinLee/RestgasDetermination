/*
 * PairFinderTask.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, klasen@kph.uni-mainz.de
 */

#include "LmdPairFinderTask.h"
#include <FairRun.h>
#include <FairRootManager.h>
#include <TClonesArray.h>
#include <PndSdsDigiPixel.h>
#include <PndLmdDim.h>
#include <PndLmdHitPair.h>

#include <vector>
#include <algorithm>
#include <sstream>

ClassImp(LmdPairFinderTask);

//simple pixel hit, maybe not even necessary
struct pixelHit{
	int _sensorId;
	double _col;
	double _row;

	double x() const{
		return _col;
	}

	pixelHit(int idVal, double colVal, double rowVal){
		_sensorId = idVal;
		_col=colVal;
		_row=rowVal;
	}

	pixelHit(){
		_col=-1;
		_row=-1;
		_sensorId=-1;
	}
};

/*
 * contains multiple pixelHits that form a cluster. most routines are for checking,
 * if two separate pixelHits belong to the same cluster
 */
struct pixelCluster{
	int _sensorId;
	double centerCol, centerRow;//,centerZ;
	double clusterSize;
	vector<pixelHit> pixelHits;
	bool clusterReady;

	pixelCluster(){
		_sensorId=-1;
		centerCol=-1; centerRow=-1;//centerZ=-1;
		clusterSize=-1;
		clusterReady=false;
	}

	pixelCluster(const pixelHit &hit){
		_sensorId=hit._sensorId;
		pixelHits.push_back(hit);

		centerCol=-1; centerRow=-1;//centerZ=-1;
		clusterSize=-1;
		clusterReady=false;
	}

	pixelCluster(const pixelCluster& copy){
		_sensorId=copy._sensorId;
		for(int i=0; i<copy.pixelHits.size(); i++){
			pixelHits.push_back(copy.pixelHits[i]);
		}

		centerCol=-1; centerRow=-1;//centerZ=-1;
		clusterSize=-1;
		clusterReady=false;
	}

	//checks, if two clusters lie DIRECTLY next to each other, that means any two pixels
	//must be directly next to each other
	//TODO: inefficient code, may be improved
	bool isNeighbour(pixelCluster &other){
		//first, they must be on same sensor
		if(_sensorId!=other._sensorId){
			return false;
		}
		double _col1,_col2,_row1,_row2;
		for(int i=0; i<this->pixelHits.size(); i++){
			_col1 = this->pixelHits[i]._col;
			_row1 = this->pixelHits[i]._row;
			for(int j=0; j<other.pixelHits.size(); j++){
				_col2 = other.pixelHits[j]._col;
				_row2 = other.pixelHits[j]._row;
				//check if neighboring, that means distance of pixels is smaller than 1.5 pixels
				if( (_col2-_col1)*(_col2-_col1)+(_row2-_row1)*(_row2-_row1) < 2.25){
					return true;
				}
			}
		}
		return false;
	}

	//merges other to this one
	void merge(pixelCluster &other){
		for(int i=0; i<other.pixelHits.size(); i++){
			pixelHits.push_back(other.pixelHits[i]);
		}
	}

	void calculateCenter(){
		centerCol=0;
		centerRow=0;
		for(int i=0; i<pixelHits.size(); i++){
			centerCol+=pixelHits[i]._col;
			centerRow+=pixelHits[i]._row;
		}
		centerCol /= pixelHits.size();
		centerRow /= pixelHits.size();
		double tempDistance;
		//calaculate size, go from corner to corner for clusters larger than 2 pixels
		if(pixelHits.size()==1){
			clusterSize=1;
		}
		else{
			for(int i=0; i<pixelHits.size(); i++){
				for(int j=i+1; j<pixelHits.size(); j++){
					double deltax=(pixelHits[i]._col-pixelHits[j]._col);
					if(deltax>0){
						deltax=deltax+1;
					}
					if(deltax<0){
						deltax=deltax-1;
					}
					double deltay=(pixelHits[i]._row-pixelHits[j]._row);
					if(deltay>0){
						deltay=deltay+1;
					}
					if(deltay<0){
						deltay=deltay-1;
					}
					tempDistance = sqrt(deltax*deltax + deltay*deltay);
					clusterSize=max(clusterSize, tempDistance);
				}
			}
		}
		clusterReady=true;
	}

	void printPixels(){
		for(int i=0; i<pixelHits.size(); i++){
			cout << "pixelHit x:" << pixelHits[i]._col << ", y:" << pixelHits[i]._row << " on sensor " << pixelHits[i]._sensorId << endl;
		}
	}
	void printCenter(){
		cout << "clusterCenter x:" << centerCol << ", y:" << centerRow << " on sensor " << _sensorId << ", contains " << pixelHits.size() << " pixels and is " << clusterSize << " pixels in diameter."<< endl;
	}

};

// -----   Default constructor   -------------------------------------------
//PairFinderTask::PairFinderTask() : PndSdsTask("SDS Hybrid Hit Producer") {
//	std::cout << "CONSTRUCTED IN FIRE!!" << std::endl;
//}

/*
 * actually I don't need empty constructors, but fkn root crashes if no empty constructor is present
 */
LmdPairFinderTask::LmdPairFinderTask() : PndSdsTask("pairfinder") {
	mcPixels = NULL;
	unsuitable=0;
}

LmdPairFinderTask::LmdPairFinderTask(const char* name)  : PndSdsTask("pairfinder with name") {
	mcPixels = NULL;
}

LmdPairFinderTask::~LmdPairFinderTask() {
	std::cout << "PairFinderTask destructor called." << std::endl;
}

InitStatus LmdPairFinderTask::Init() {

	//for now, disregard sorting option, always store unsorted (data will be supplied in the future sorted anyway)
	sortByModule=false;

	noOfGoodPairs=0;
	noOfEvents=noOfCombos=0;
	hitsClustered=distanceTooHigh=0;
	hitsSinglePixel=hitsSinglePixel=0;
	unsuitable=sumOfPixelHits=0;
	eventMissedAllPlanes=noOverlap=0;
	plane0=plane1=plane2=plane3=0;
	fInBranchName = "LMDPixelDigis";
	fOutBranchName = "LMDPixelPairs";
	fFolderName = "cbmsim";
	SetBranchNames();

	FairRun* ana = FairRun::Instance();
	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman)	{
		std::cout << "-E- LmdPairFinder::Init: " << "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	mcPixels = (TClonesArray*) ioman->GetObject(fInBranchName);
	if (!mcPixels)	{
		std::cout << "-W- LmdPairFinder::Init: "<< "ERROR, branch name not found!" << std::endl;
		return kERROR;
	}

	dimension = PndLmdDim::Instance();
	dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd.dat",true);
	dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd_misaligned.dat",false);

	if(sortByModule){
		cout << "====== STORING SORTED =========" << endl;
		/*
		 * initialize multiple TClonesArrays and register them
		 */
		int moduleID;
		const char *moduleIDchar;
		//std::stringstream moduleIDstrm;
		// half can be 0 or 1
		for(unsigned int iHalf=0; iHalf < 2; iHalf++){
			// plane can be 0, 1, 2, 3
			for(unsigned int iPlane=0; iPlane < 4; iPlane++){
				// module can be 0, 1, 2, 3, 4
				for(unsigned int iModule=0; iModule<5; iModule++){

					/*
					 * TODO: maybe outsource these two ModuleID generators to PndLmdDim.h
					 */
					//moduleIDstrm << iHalf << iPlane << iModule;
					//moduleID = makeModuleID(iHalf, iPlane, iModule);

					moduleID = dimension->makeModuleID(iHalf, iPlane, iModule);
					moduleIDchar = dimension->makeModuleIDchar(iHalf, iPlane, iModule);

					hitPairMap[moduleID] = new TClonesArray("PndLmdHitPair");
					ioman->Register(moduleIDchar, "PndLmd", hitPairMap[moduleID], kTRUE);

					cerr << "HOLA! I maed dis: " << moduleIDchar << endl;
					//delete moduleIDchar;

					//ioman->Register(moduleIDstrm.str().c_str(), "PndLmd", hitPairMap[moduleID], kTRUE);
					//moduleIDstrm.str("");
				}
			}
		}
	}
	else{
		cout << "====== STORING UNSORTED =========" << endl;
		hitPairArray = new TClonesArray("PndLmdHitPair");
		ioman->Register("PndLmdHitPair", "PndLmd", hitPairArray, kTRUE);
	}

	//maxDistance=160e-4;

	std::cout << "LmdPairFinder::Init(): Initialization successful." << std::endl;
	return kSUCCESS;
}

void LmdPairFinderTask::SetBranchNames() {
	std::cout << "branch names set to " << fInBranchName << std::endl;
}

InitStatus LmdPairFinderTask::ReInit() {
	return InitStatus();
}

/*
 * Main SensorHit filter. It stores the sensor row and column info to a PndLmdHitPair object,
 * filters for valid hit pairs and stores the decoded hit in LMD coordinate system to the HitPair.
 * The HitPair contains BOTH original row and col hits as well as LMD xyz Coordinates (as TVector3)
 */
void LmdPairFinderTask::Exec(Option_t* opt) {

	//clear temporary array for next event

	if(sortByModule){
		//clear hit count map for next event
		typedef std::map<int, int>::iterator it_type1;
		typedef std::map<int, TClonesArray*>::iterator it_type2;

		for(it_type1 iterator = hitCountMap.begin(); iterator != hitCountMap.end(); iterator++) {
			iterator->second = 0;
		}
		for(it_type2 iterator = hitPairMap.begin(); iterator != hitPairMap.end(); iterator++) {
			iterator->second->Clear();
		}
	}
	else{
		hitPairArray->Clear();
	}

	Int_t nPixels = mcPixels->GetEntriesFast();
	noOfEvents++;

	//no pixel hits? no wörk!
	if(nPixels <1){
		eventMissedAllPlanes++;
		return;
	}
	sumOfPixelHits +=nPixels;

	int hitSensorId, col, row;
	int sortedHits=0;

	//display some kind of progress
	if((noOfEvents%10000)==0){
		cout << "processed " << noOfEvents << endl;
	}

	//make firing pixels to clusters
	vector<pixelCluster> clusters;
	//read pixel hits from root file
	for(int i_Pixel=0; i_Pixel<nPixels;i_Pixel++){

		PndSdsDigiPixel* mcPixel = (PndSdsDigiPixel*)mcPixels->At(i_Pixel);
		hitSensorId = mcPixel->GetSensorID();
		col = mcPixel->GetPixelColumn();
		row = mcPixel->GetPixelRow();

		//skip decoding errors
		if(col < 0 | row < 0){
			continue;
		}
		else{
			clusters.push_back(pixelCluster(pixelHit(hitSensorId, col, row)));
		}
	}

	//DEBUG ONLY
	/*
	for(int iCluster=0; iCluster<clusters.size(); iCluster++){
		clusters[iCluster].printPixels();
	}
	for(int iCluster=0; iCluster<clusters.size(); iCluster++){
		clusters[iCluster].printCenter();
	}
	for(int iCluster=0; iCluster<clusters.size(); iCluster++){
		clusters[iCluster].calculateCenter();
	}
	for(int iCluster=0; iCluster<clusters.size(); iCluster++){
		clusters[iCluster].printCenter();
	}
	 */

	//all hits are present in clusters

	/*
	 * ============ find clusters ============
	 * input: vector<pixelCluster>
	 * output vector<pixelCluster>
	 *
	 * algorithm: Hierarchical Clustering Algorithm, see https://en.wikipedia.org/wiki/Hierarchical_clustering
	 * start by putting every pixel hit in a separate cluster (done above).
	 * then merge every two clusters that are close enough (1-2 pixels, may be open to adjustment).
	 * terminate if no more clusters can be merged or after N iterations for N pixel hits.
	 * all remaining clusters contain every pixel hit.
	 */
	bool actionDone;
	for(int iRounds=0; iRounds<nPixels; iRounds++){
		actionDone=false;
		//for every cluster, check every other cluster
		for(int i=0; i<clusters.size(); i++){
			//clusters are interchangeable, check every pair only once
			for(int j=i+1; j<clusters.size(); j++){

				//clusters must be on same sensor
				if(clusters[i]._sensorId != clusters[j]._sensorId){
					continue;
				}

				if(clusters[i].isNeighbour(clusters[j])){
					clusters[i].merge(clusters[j]);
					clusters.erase(clusters.begin() + j);
					j--;
					actionDone=true;
				}
			}
		}
		//nothing done this iteration? then nothing can be merged and we are done
		if(!actionDone){
			break;
		}
	}
	//calculate cluster centers and discard large clusters
	//for statistis: count cluster ratio
	for(int i=0; i<clusters.size();i++){
		clusters[i].calculateCenter();
		//FIXME: read from parameter file!
		if(clusters[i].clusterSize > 1){
			hitsClustered++;

			//FIXME: get from config file
			if(clusters[i].clusterSize > 3){
				clusters.erase(clusters.begin()+i);
				/*
				 * this is important! when you erase cluster i, cluster i+1 becomes cluster i, but i becomes i+1.
				 * that means, cluster i (former i+1) never gets checked in the first line of the outer for loop!
				 */
				i--;
			}
		}
		else{
			hitsSinglePixel++;
		}
	}

	/*
	for(int i=0; i<clusters.size(); i++){
		clusters[i].printPixels();
	}
	for(int i=0; i<clusters.size(); i++){
		if(clusters[i]._sensorId==95){
			cout << "===== suspect A ======" << endl;
			clusters[i].printPixels();
			clusters[i].printCenter();
			cout << "===== suspect B ======" << endl;
			clusters[i].calculateCenter();
			clusters[i].printCenter();
			cout << "===== suspect C ======" << endl;
		}
		else{
			clusters[i].printCenter();
		}
	}
	 */

	/*
	 * ============ assign hitPairs ============
	 * algorithm: make all possible combinations of two clusters
	 * and check if candidates are realistic. if not, discard,
	 * otherwise save candidate to disk!
	 *
	 * TODO: I don't know how many tracks a single event will have. up until now,
	 * there are about 8 clusters from 1 track which make 28 possible combinations. if we were to have
	 * 80 clusters, that would make 3160 combinations. this may be slow.
	 *
	 * I made a small improvement, cluster pairs are checked by ID before they are made to HitPair candidate.
	 * That should save a lot of time since useless candidates are not produced. Depends on sensor ID,
	 * so PndLmdDim must work correctly.
	 */
	Double_t col1, col2, row1, row2;
	Int_t id1, id2;
	Int_t storedPairsPerEvent=0;
	Int_t moduleId;
	Int_t storedForBranch;

	//try every cluster combination and check
	for(int i=0; i<clusters.size(); i++){
		for(int j=i+1; j<clusters.size(); j++){

			col1=clusters[i].centerCol;
			col2=clusters[j].centerCol;
			row1=clusters[i].centerRow;
			row2=clusters[j].centerRow;
			id1=clusters[i]._sensorId;
			id2=clusters[j]._sensorId;
			noOfCombos++;

			//check if the two sensor IDs can be overlapping
			if(candHitsOverlappingArea(id1, id2)){
				// make PndLmdHitPair and check for data sanity, then store to vector
				PndLmdHitPair pairCanditate(col1, row1, id1, col2, row2, id2);

				/*
				 * choose coordinate system and store moduleID. This must be done
				 * prior to suitability check, because that relies on the TVector3s
				 * in the HitPair in LMD Coordinates
				 */
				transformToLMDlocal(pairCanditate);

				/*
				 * ============ apply filters ============
				 */
				pairCanditate.check();
				if(pairCanditate.isSane()){

					if(isSuitable(pairCanditate)){
						/*
						 * ============ store pairs ============
						 */
						if(sortByModule){
							moduleId = pairCanditate.getModuleId();
							TClonesArray* targetBranch = hitPairMap[moduleId];
							storedForBranch = hitCountMap[moduleId];
							/*
							if(pairCanditate.getCol1()<0 | pairCanditate.getCol2()<0 | pairCanditate.getRow1()<0 | pairCanditate.getRow2()<0){
								cout << "got it!" << endl;
								pairCanditate.Print();
								exit(1);
							}
							 */
							new( (*targetBranch)[storedForBranch]) PndLmdHitPair(pairCanditate);
							hitCountMap[moduleId]++;
						}
						else{
							new((*hitPairArray)[storedPairsPerEvent]) PndLmdHitPair(pairCanditate);
							storedPairsPerEvent++;
						}
					}
					else{
						unsuitable++;
					}
				}
				else{
					pairCanditate.PrintPair();
					cerr << "===               WARNING:                 ====" << endl;
					cerr << "pair seems valid but did not pass sanity check!" << endl;
					cerr << "===============================================" << endl;
					exit(1);
				}
			}
			else{
				noOverlap++;
			}
		}
	}
	return;
}

void LmdPairFinderTask::FinishEvent() {
}

void LmdPairFinderTask::FinishTask() {

	Int_t sumOfEvents;
	Int_t sumOfAllPlanes=plane0+plane1+plane2+plane3;
	double plane0Percent = ((double)plane0/noOfGoodPairs)*100;
	double plane1Percent = ((double)plane1/noOfGoodPairs)*100;
	double plane2Percent = ((double)plane2/noOfGoodPairs)*100;
	double plane3Percent = ((double)plane3/noOfGoodPairs)*100;
	double allPlanesPercent = ((double)sumOfAllPlanes/noOfGoodPairs)*100;
	double clusterRatio = ((double)hitsClustered/(double)(hitsSinglePixel+hitsClustered))*100;
	double pixelsPerEvent = (double)sumOfPixelHits/(double)noOfEvents;
	double goodPairsPerEvent = (double)noOfGoodPairs/(double)noOfEvents;

	cout << endl;
	cout << "*************************************************************" << endl;
	cout << "                      pair finder done                       " << endl;
	cout << "*************************************************************" << endl;
	cout << endl;
	cout << "                     counting statistics:" << endl;
	cout << endl;
	printf("total events: %d \n", noOfEvents);
	printf("events that missed all sensors: %d \n", eventMissedAllPlanes);
	printf("total pixel hits: %d \n", sumOfPixelHits);
	printf("cluster ratio: %.2f %% \n", clusterRatio);
	printf("pixel hits per event: %.2f \n", pixelsPerEvent);
	printf("----------------------------\n");
	printf("possible hit pair combinations: %d \n", noOfCombos);
	printf("no overlap: %d \n", noOverlap);
	printf("distance too high: %d \n", distanceTooHigh);
	printf("----------------------------\n");
	printf("good pairs: %d \n", noOfGoodPairs);
	printf("good pairs per event: %.2f \n", goodPairsPerEvent);
	//printf("good pairs per event and plane (indicator for track multiplicity): %.2f \n", goodPairsPerEvent/4);
	printf("----------------------------\n");
	printf("hits on plane 0: %.2f %% \n", plane0Percent);
	printf("hits on plane 1: %.2f %%\n", plane1Percent);
	printf("hits on plane 2: %.2f %%\n", plane2Percent);
	printf("hits on plane 3: %.2f %%\n", plane3Percent);
	printf("hits on all planes: %.2f %% \n", allPlanesPercent);
	cout << endl;
	cout << "*************************************************************" << endl;
}

void LmdPairFinderTask::Register() {
}

void LmdPairFinderTask::transformToLMDlocal(PndLmdHitPair &pair) {

	const int fid = pair.getId1();
	const int bid = pair.getId2();

	const double col1 = pair.getCol1();
	const double row1 = pair.getRow1();
	const double row2 = pair.getRow2();
	const double col2 = pair.getCol2();

	// the pair finder must assume the geometry is aligned. It does not know the matrices
	// for the misaligned case
	const TVector3 frontHit = dimension->Decode_hit(fid, col1,row1, true);
	const TVector3 backHit = dimension->Decode_hit(bid, col2, row2, true);

	//dont't store in lmd local
	const TVector3 frontInLMD = dimension->Transform_global_to_lmd_local(frontHit, false, true);
	const TVector3 backInLMD = dimension->Transform_global_to_lmd_local(backHit, false, true);

	pair.setHit1(frontHit);
	pair.setHit2(backHit);

	pair.setModuleId(dimension->makeModuleID(fid, bid));
	pair.setOverlapId(dimension->makeOverlapID(fid, bid));
}

bool LmdPairFinderTask::isSuitable(PndLmdHitPair &candidate) {

	if(!candDistanceIsGood(candidate)){
		distanceTooHigh++;
		return false;
	}

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(candidate.getId1(), fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(candidate.getId2(), bhalf, bplane, bmodule, bside, bdie, bsensor);

	if(fplane!=bplane){
		return false;
	}

	switch(fplane){
	case 0:
		plane0++;
		break;
	case 1:
		plane1++;
		break;
	case 2:
		plane2++;
		break;
	case 3:
		plane3++;
		break;
	default:
		//should never happen, can only indicate decoding error
		cerr << "WARNING: hit was deemed suitable but plane number is " << fplane << endl;
		cerr << "This should not happen!" << endl;
	}

	if(bside < fside){
		candidate.swapHits();
	}

	noOfGoodPairs++;
	return true;
}

//depends only on PndLmdDim, so should work
bool LmdPairFinderTask::candHitsOverlappingArea(PndLmdHitPair &candidate) {
	int firstSensorId, secondSensorId;
	firstSensorId=candidate.getId1();
	secondSensorId=candidate.getId2();
	return candHitsOverlappingArea(firstSensorId, secondSensorId);
}

//depends only on PndLmdDim, so should work
bool LmdPairFinderTask::candHitsOverlappingArea(Int_t firstSensorId, Int_t secondSensorId) {

	//same sensor hit?
	if(firstSensorId==secondSensorId){
		return false;
	}

	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	/*
	 * TODO: the new PndLmdDim.h also supports checks for overlapping-ness.
	 * Maybe use this in the future.
	 */

	dimension->Get_sensor_by_id(firstSensorId, fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(secondSensorId, bhalf, bplane, bmodule, bside, bdie, bsensor);

	//the necessities for overlapping, must be on same half, plane, module and other side
	if(bhalf != fhalf){
		return false;
	}
	if(bplane != fplane){
		return false;
	}
	if(bmodule != fmodule){
		return false;
	}
	if(bside == fside){
		return false;
	}

	//0to5
	if(fdie == 0 && fsensor == 0 && bdie == 0 && bsensor == 0){
		return true;
	}
	//3to8
	if(fdie == 1 && fsensor == 1 && bdie == 1 && bsensor == 1){
		return true;
	}
	//4to9
	if(fdie == 1 && fsensor == 2 && bdie == 1 && bsensor == 2){
		return true;
	}
	//3to6
	if(fdie == 1 && fsensor == 1 && bdie == 0 && bsensor == 1){
		return true;
	}
	//1to8
	if(fdie == 0 && fsensor == 1 && bdie == 1 && bsensor == 1){
		return true;
	}
	//2to8
	if(fdie == 0 && fsensor == 2 && bdie == 1 && bsensor == 1){
		return true;
	}
	//2to9
	if(fdie == 0 && fsensor == 2 && bdie == 1 && bsensor == 2){
		return true;
	}
	//3to7
	if(fdie == 1 && fsensor == 1 && bdie == 0 && bsensor == 2){
		return true;
	}
	//4to7
	if(fdie == 1 && fsensor == 2 && bdie == 0 && bsensor == 2){
		return true;
	}
	//all other checks are negative? then the sensors don't overlap!
	return false;
}

bool LmdPairFinderTask::candDistanceIsGood(PndLmdHitPair &candidate) {

	//FIXME: this distance must be set in parameter file! currently is 2 pixels
	//use distance squared


	double distance = candidate.getDistance();
	if(distance > maxDistance){
		return false;
	}
	return true;
}

void LmdPairFinderTask::Reset() {
}

