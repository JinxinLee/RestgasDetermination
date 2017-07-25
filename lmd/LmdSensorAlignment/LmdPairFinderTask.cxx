/*
 * PairFinderTask.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
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

/*
 * actually I don't need empty constructors, but fkn root crashes if no empty constructor is present
 */
LmdPairFinderTask::LmdPairFinderTask() : PndSdsTask("pairfinder") {
	mcPixels = NULL;
	unsuitable=0;
	_useDynamicCut = false;
	_findDynamicCutParameters = false;
	_cutParameterFile = "";
	_ignoreClusters = false;
}

LmdPairFinderTask::LmdPairFinderTask(const char* name)  : PndSdsTask("pairfinder with name") {
	mcPixels = NULL;
	if(!strcmp(name,"")) SetName(name);
	_useDynamicCut = false;
	_findDynamicCutParameters = false;
	_cutParameterFile = "";
	_ignoreClusters = false;
}

LmdPairFinderTask::~LmdPairFinderTask() {
	std::cout << "PairFinderTask destructor called." << std::endl;
}

InitStatus LmdPairFinderTask::Init() {

	noOfGoodPairs=0;
	noOfEvents=noOfCombos=0;
	hitsClustered=distanceTooHigh=0;
	hitsSinglePixel=0;hitsSinglePixel=0;
	unsuitable=sumOfPixelHits=0;
	eventMissedAllPlanes=noOverlap=0;
	plane0=plane1=plane2=plane3=0;
	fInBranchName = "LMDPixelDigis";
	fOutBranchName = "LMDPixelPairs";
	fFolderName = "cbmsim";

	SetBranchNames();

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

	if(!_findDynamicCutParameters && _useDynamicCut){
		// TODO: read dynamic cut parameters from disk
		cout << "PndLmdSensorAligner: reading dynamic cut Parameters from file... ";
		std::vector<int> overlapIDs = dimension->getAvailableOverlapIDs();

		if(!PndLmdAlignManager::exists(_cutParameterFile)){
			cout << "cut parameter file does not exist! using static cut instead.\n";
			_useDynamicCut = false;
		}
		else{
			config = PndLmdAlignManager::readConfigFile(_cutParameterFile);

			for(int i=0; i<overlapIDs.size(); i++){
				int overlapID = overlapIDs[i];
				dynamicCutHandler &handler = cutHandlers[overlapID];
				handler._overlapID = overlapIDs[i];

				std::stringstream configput("");
				configput << "dynamicCut.aligners." << handler._overlapID <<".";

				try{
					handler._minDist = config.get<double>(configput.str() + "minDist");
					handler._maxDist = config.get<double>(configput.str() + "maxDist");
				}
				catch(exception e){
					cerr << "PndLmdSensorAligner: ERROR! Parameter not found in config file!\n";
				}
				handler._ready = true;

				//cout << "reading for aligner " << handler._overlapID << ": min: " << handler._minDist << ", max: " << handler._maxDist << "\n";
			}
			cout << "done.\n";
		}
	}

	if(_findDynamicCutParameters){
		cout << "PndLmdSensorAligner: trying to find dynamic cut Parameters.\n";
	}

	hitPairArray = new TClonesArray("PndLmdHitPair");
	ioman->Register("PndLmdHitPair", "PndLmd", hitPairArray, kTRUE);

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
 * The HitPair contains BOTH original row and col hits as well as LMD xyz Coordinates (as TVector3).
 * This consumes a lot of storage, but storage is cheap and for now we want the info.
 */
void LmdPairFinderTask::Exec(Option_t*) {

	//clear temporary array for next event
	hitPairArray->Clear();

	Int_t nPixels = mcPixels->GetEntriesFast();
	noOfEvents++;

	//no pixel hits? no wörk!
	if(nPixels <1){
		eventMissedAllPlanes++;
		return;
	}
	sumOfPixelHits +=nPixels;

	int hitSensorId, col, row;

	//display some kind of progress
	if((noOfEvents%10000)==0){
		cout << "processed " << noOfEvents << "\n";
	}

	//make firing pixels to clusters
	vector<pixelCluster> clusters;
	//read pixel hits from root file
	for(int i_Pixel=0; i_Pixel<nPixels;i_Pixel++){

		PndSdsDigiPixel* mcPixel = (PndSdsDigiPixel*)mcPixels->At(i_Pixel);
		hitSensorId = mcPixel->GetSensorID();
		col = mcPixel->GetPixelColumn();
		col = mcPixel->GetPixelColumn();
		row = mcPixel->GetPixelRow();

		//skip decoding errors
		if(col < 0 || row < 0){
			continue;
		}
		else{
			clusters.push_back(pixelCluster(pixelHit(hitSensorId, col, row)));
		}
	}

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
		for(size_t i=0; i<clusters.size(); i++){
			//clusters are interchangeable, check every pair only once
			for(size_t j=i+1; j<clusters.size(); j++){

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
	for(size_t i=0; i<clusters.size();i++){
		clusters[i].calculateCenter();
		//FIXME: read from parameter file!
		if(clusters[i].clusterSize > 1){
			hitsClustered++;

			//if ignoreClustres is set, skip clustered events
			//FIXME: get maximum cluster size from config file
			//TODO: deleting from vector is expensive, maybe do this another way?
			if(_ignoreClusters || clusters[i].clusterSize > 3){
				clusters.erase(clusters.begin()+i);
				/*
				 * this is important! when you erase cluster i, cluster i+1 becomes cluster i, but the first i
				 * becomes i+1 itself.
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
	 * ============ assign hitPairs ============
	 * algorithm: make all possible combinations of two clusters
	 * and check if candidates are realistic. if not, discard,
	 * otherwise save candidate to disk!
	 *
	 * I don't know how many tracks a single event will have. up until now,
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

	//cout << "find cut: " << _findDynamicCutParameters << ", use cut: " << _useDynamicCut << "\n";

	//try every cluster combination and check
	for(size_t i=0; i<clusters.size(); i++){
		for(size_t j=i+1; j<clusters.size(); j++){

			col1=clusters[i].centerCol;
			col2=clusters[j].centerCol;
			row1=clusters[i].centerRow;
			row2=clusters[j].centerRow;
			id1=clusters[i]._sensorId;
			id2=clusters[j]._sensorId;
			noOfCombos++;

			//make PndLmdHitPair and check for data sanity, then store to vector
			PndLmdHitPair pairCanditate(col1, row1, id1, col2, row2, id2);

			//is the candidate even on an overlapping area?
			if(!candHitsOverlappingArea(pairCanditate)){
				noOverlap++;
				continue;
			}

			/*
			 * choose coordinate system and store moduleID. This must be done
			 * prior to suitability check, because that relies on the TVector3s
			 * in the HitPair in LMD Coordinates. This is using the perfect geometry,
			 * since we don't know the misalignment at this point. This also sets
			 * overlapID, moduleID and the TVector3 for hit1 and hit2
			 */
			transformToLMDlocal(pairCanditate);
			pairCanditate.check();

			if(!pairCanditate.isSane()){
				pairCanditate.PrintPair();
				cerr << "====              WARNING:                 ====" << endl;
				cerr << "pair seems valid but did not pass sanity check!" << endl;
				cerr << "===============================================" << endl;
				continue;
			}

			//pair must now be sane, in LMD local and has overlapID et al.

			//are we still looking for the dynamic cut values?
			if(_findDynamicCutParameters){
				dynamicCutHandler &handler = cutHandlers[pairCanditate.getOverlapId()];
				handler.addToSamples(pairCanditate);
				continue;
			}

			//choose whether to apply dynamic cut or simple cut.
			if(!_useDynamicCut){
				//cout << "using static cut.\n";
				if(!applyStaticDistanceCut(pairCanditate)){
					unsuitable++;
					continue;
				}
				//pair survived distance cut? great, store!
			}

			if(_useDynamicCut){
				//is the cutHandler ready for this overlapID? if not, something went wrong.
				dynamicCutHandler &handler = cutHandlers[pairCanditate.getOverlapId()];
				if(!handler.ready()){
					//cout << "handler not ready.\n";
					continue;
				}
				//cout << "applying cut.\n";
				//the cutHandler is ready, apply distance cut
				if(!applyDynamicDistanceCut(pairCanditate)){

					distanceTooHigh++;
					continue;
				}
				//pair survived distance cut? great, store!
				//cout << "pair survived.\n";
			}

			/*
			 * if the pair survived to this point, it's valid. store!
			 */

			getStatistics(pairCanditate);
			new((*hitPairArray)[storedPairsPerEvent]) PndLmdHitPair(pairCanditate);
			storedPairsPerEvent++;
		}
	}
	return;
}

void LmdPairFinderTask::FinishEvent() {
}

void LmdPairFinderTask::FinishTask() {

	int notReady=0;

	//were we looking for dynamic cut parameters? write them to disk
	if(_findDynamicCutParameters){
		//TODO: write all cutHandlers Data to disk
		cout << "PndLmdSensorAligner: writing cut parameters to disk...\n";

		for (auto &handlerIt : cutHandlers){

			dynamicCutHandler &handler = handlerIt.second;
			if(!handler._ready){
				notReady++;
				cout << "Warning! handler " << handler._overlapID << " only has  "<< handler.samples.size() << " pairs!\n";
				continue;
			}
			handler.calcMinAndMax();

			std::stringstream configput("");
			configput << "dynamicCut.aligners." << handler._overlapID <<".";
			config.put(configput.str() + "minDist", handler._minDist);
			config.put(configput.str() + "maxDist", handler._maxDist);

		}
		if(notReady > 0){
			cout << "PndLmdSensorAligner: Attention! " << notReady << " handlers don't have enough pairs.\n";

		}

		if(PndLmdAlignManager::writeConfigFile(config, _cutParameterFile, false)){
			cout << "PndLmdSensorAligner: Successfully written cutParameters to " << _cutParameterFile << "\n";
			return;
		}
		cout << "PndLmdSensorAligner: could not write cut parameters to disk!\n";

		//exit after parameters are found and stored
		return;
	}

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
	return;
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

	//store in lmd local
	const TVector3 frontInLMD = dimension->Transform_global_to_lmd_local(frontHit, false, true);
	const TVector3 backInLMD = dimension->Transform_global_to_lmd_local(backHit, false, true);
	pair.setHit1(frontInLMD);
	pair.setHit2(backInLMD);

	pair.setModuleId(dimension->makeModuleID(dimension->makeOverlapID(fid, bid)));
	pair.setOverlapId(dimension->makeOverlapID(fid, bid));
	return;
}


bool LmdPairFinderTask::applyDynamicDistanceCut(PndLmdHitPair &candidate) {

	int overlapID = candidate.getOverlapId();

	double distance = candidate.getDistance();
	dynamicCutHandler &handler = cutHandlers[overlapID];

	//cout << "dist: " << distance << ", min: " << handler.getMinDist() << " max: " << handler.getMaxDist() << "\n";

	if(distance < handler.getMinDist() || distance > handler.getMaxDist()){
		return false;
	}
	noOfGoodPairs++;
	return true;
}

bool LmdPairFinderTask::applyStaticDistanceCut(PndLmdHitPair &candidate) {

	//check distance squared
	double distance = candidate.getDistance();
	if(distance > _maxDistance){
		distanceTooHigh++;
		return false;
	}
	return true;
}

void LmdPairFinderTask::getStatistics(PndLmdHitPair &candidate) {

	//check for overlap
	int fhalf, fplane, fmodule, fside, fdie, fsensor;
	int bhalf, bplane, bmodule, bside, bdie, bsensor;

	dimension->Get_sensor_by_id(candidate.getId1(), fhalf, fplane, fmodule, fside, fdie, fsensor);
	dimension->Get_sensor_by_id(candidate.getId2(), bhalf, bplane, bmodule, bside, bdie, bsensor);

	//count events per plane
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
	noOfGoodPairs++;
}

//depends only on PndLmdDim, so should work
bool LmdPairFinderTask::candHitsOverlappingArea(PndLmdHitPair &candidate) {
	int firstSensorId, secondSensorId;
	firstSensorId=candidate.getId1();
	secondSensorId=candidate.getId2();

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


	//sort them that hit0 is always upstream
	if(bside < fside){
		candidate.swapHits();
		dimension->Get_sensor_by_id(firstSensorId, fhalf, fplane, fmodule, fside, fdie, fsensor);
		dimension->Get_sensor_by_id(secondSensorId, bhalf, bplane, bmodule, bside, bdie, bsensor);
	}

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

void LmdPairFinderTask::Reset() {
}

