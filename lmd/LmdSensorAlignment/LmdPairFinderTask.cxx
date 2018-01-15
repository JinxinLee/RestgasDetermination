/*
 * PairFinderTask.cpp
 *
 *  Created on: Jul 22, 2014
 *      Author: Roman Klasen, roklasen@uni-mainz.de or klasen@kph.uni-mainz.de
 */

#include "PndLmdAlignManager.h"
#include "LmdPairFinderTask.h"
#include <FairRun.h>

#include <FairRootManager.h>
#include <TClonesArray.h>
#include <PndSdsDigiPixel.h>
#include <PndLmdHitPair.h>
#include <PndLmdGeometryHelper.h>
#include <PndGeoHandling.h>
#include <PndSdsClusterPixel.h>

#include <vector>
#include <algorithm>
#include <sstream>

#include <FairRuntimeDb.h>
#include <PndLmdContFact.h>

using std::cout;
using std::cerr;
using swap;

ClassImp(LmdPairFinderTask);

/*
 * actually I don't need empty constructors, but fkn root crashes if no empty constructor is present
 */
LmdPairFinderTask::LmdPairFinderTask() : // @suppress("Class members should be properly initialized")
		PndSdsTask("pairfinder") {
	digiArray = NULL;
	recoArray = NULL;
	clusterCandidateArray = NULL;

	unsuitable = 0;
	_useDynamicCut = false;
	_findDynamicCutParameters = false;
	_cutParameterFile = "";
	_ignoreClusters = false;

}

LmdPairFinderTask::LmdPairFinderTask(const char* name) : // @suppress("Class members should be properly initialized")
		PndSdsTask("pairfinder with name") {
	digiArray = NULL;
	recoArray = NULL;
	clusterCandidateArray = NULL;

	if (!strcmp(name, ""))
		SetName(name);
	_useDynamicCut = false;
	_findDynamicCutParameters = false;
	_cutParameterFile = "";
	_ignoreClusters = false;
}

LmdPairFinderTask::~LmdPairFinderTask() {
	std::cout << "PairFinderTask destructor called." << "\n";
}

InitStatus LmdPairFinderTask::Init() {

	noOfGoodPairs = 0;
	noOfEvents = noOfCombos = 0;
	hitsClustered = distanceTooHigh = 0;
	hitsSinglePixel = 0;
	hitsSinglePixel = 0;
	unsuitable = sumOfPixelHits = 0;
	eventMissedAllPlanes = noOverlap = 0;
	plane0 = plane1 = plane2 = plane3 = 0;

	fInBranchName = "LMDPixelDigis";
	fInRecoBranchName = "LMDHitsPixel";
	fOutBranchName = "LMDPixelPairs";
	fInClusterCandidates = "LMDPixelClusterCand";

	fFolderName = "cbmsim";

	SetBranchNames();

	helper = &PndLmdGeometryHelper::getInstance();

	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman) {
		std::cout << "-E- LmdPairFinder::Init: " << "RootManager not instantiated!" << "\n";
		return kFATAL;
	}

	digiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
	recoArray = (TClonesArray*) ioman->GetObject(fInRecoBranchName);
	clusterCandidateArray = (TClonesArray*) ioman->GetObject(fInClusterCandidates);

	if (!digiArray) {
		std::cout << "-W- LmdPairFinder::Init: " << "ERROR, branch name " << fInBranchName << " could not found!" << "\n";
		return kERROR;
	}

	if (!recoArray) {
		std::cout << "-W- LmdPairFinder::Init: " << "ERROR, branch name " << fInRecoBranchName << " not found!" << "\n";
		return kERROR;
	}

	if (!clusterCandidateArray) {
		std::cout << "-W- LmdPairFinder::Init: " << "ERROR, branch name " << fInClusterCandidates << " not found!" << "\n";
		return kERROR;
	}

	//dimension = PndLmdDim::Instance();
	//dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd.dat",true);
	//dimension->Read_transformation_matrices("/geometry/trafo_matrices_lmd_misaligned.dat",false);

	if (!_findDynamicCutParameters && _useDynamicCut) {

		//cout << "PndLmdSensorAligner: getting available overlap IDs... ";
		//std::vector<int> overlapIDs = dimension->getAvailableOverlapIDs();
		const std::vector<int> &overlapIDs = helper->getAvailableOverlapIDs();

		cout << "PndLmdSensorAligner: reading dynamic cut Parameters from file... ";
		if (!PndLmdAlignManager::exists(_cutParameterFile)) {
			cout << "cut parameter file does not exist! using static cut instead.\n";
			_useDynamicCut = false;
		} else {
			config = PndLmdAlignManager::readConfigFile(_cutParameterFile);

			for (auto i = 0; i < overlapIDs.size(); i++) {
				int overlapID = overlapIDs[i];
				dynamicCutHandler &handler = cutHandlers[overlapID];
				handler._overlapID = overlapIDs[i];

				std::stringstream configput("");
				configput << "dynamicCut.aligners." << handler._overlapID << ".";

				try {
					handler._minDist = config.get<double>(configput.str() + "minDist");
					handler._maxDist = config.get<double>(configput.str() + "maxDist");
				} catch (std::exception &e) {
					cerr << "PndLmdSensorAligner: ERROR! Parameter not found in config file!\n";
				}
				handler._ready = true;

				//cout << "reading for aligner " << handler._overlapID << ": min: " << handler._minDist << ", max: " << handler._maxDist << "\n";
			}
			cout << "done.\n";
		}
	}

	if (_findDynamicCutParameters) {
		cout << "PndLmdSensorAligner: trying to find dynamic cut Parameters.\n";
	}

	hitPairArray = new TClonesArray("PndLmdHitPair");
	ioman->Register("PndLmdHitPair", "PndLmd", hitPairArray, kTRUE);

	std::cout << "LmdPairFinder::Init(): Initialization successful." << "\n";
	return kSUCCESS;
}

void LmdPairFinderTask::SetBranchNames() {
	std::cout << "branch names set to " << fInBranchName << "\n";
}

InitStatus LmdPairFinderTask::ReInit() {
	return InitStatus();
}

void LmdPairFinderTask::SetParContainers() {

	FairRun* ana;
	FairRuntimeDb* rtdb;

	std::cout << "PndLmdPixelClusterTask::SetParContainers() " << "\n";
	// Get Base Container
	ana = FairRun::Instance();
	rtdb = ana->GetRuntimeDb();
	//fDigiPar = (PndSdsPixelDigiPar*) (rtdb->getContainer("SDSPixelDigiPar"));
	//	rtdb->getContainer("SDSPixelTotDigiPar");

	PndLmdContFact* themvdcontfact = (PndLmdContFact*) rtdb->getContFactory("PndLmdContFact");
	//read params for lumi alignment
	TList* theAlignLMDContNames = themvdcontfact->GetAlignParNames();
	Info("SetParContainers()", "AlignLMD The container names list contains %i entries", theAlignLMDContNames->GetEntries());
	TIter cfAlIter(theAlignLMDContNames);
	while (TObjString* contname = (TObjString*) cfAlIter()) {
		TString parsetname = contname->String();
		Info("SetParContainers()", "%s", parsetname.Data());
	}

	PndGeoHandling::Instance()->SetParContainers();
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

	//Int_t nPixels = digiArray->GetEntriesFast();
	noOfEvents++;

	//make firing pixels to clusters
	//vector<pixelCluster> clusters;

	//display some kind of progress
	if ((noOfEvents % 10000) == 0) {
		cout << "processed " << noOfEvents << "\n";
	}

	// ========== loop over recos in recoArray =========

	Int_t nRecos = recoArray->GetEntriesFast();

	Int_t storedPairsPerEvent = 0;

	//try every cluster combination and check
	for (auto iReco = 0; iReco < nRecos; iReco++) {
		for (auto jReco = iReco + 1; jReco < nRecos; jReco++) {

			PndSdsHit *hitOne = (PndSdsHit*) recoArray->At(iReco);
			PndSdsHit *hitTwo = (PndSdsHit*) recoArray->At(jReco);

			int id1 = hitOne->GetSensorID();
			int id2 = hitTwo->GetSensorID();

			// sort hits so hitOne is ALWAYS upstream
			auto &infoOne = helper->getHitLocationInfo(id1);
			auto &infoTwo = helper->getHitLocationInfo(id2);

			if (infoOne.module_side > infoTwo.module_side) {
				std::swap(hitOne, hitTwo);
				id1 = hitOne->GetSensorID();
				id2 = hitTwo->GetSensorID();
			}

			//from here on, the hits are sorted so hitOne is ALWAYS upstream

			const TVector3 vecOneGlobal = hitOne->GetPosition();
			const TVector3 vecTwoGlobal = hitTwo->GetPosition();

			auto vecOneLocal = helper->transformPndGlobalToLmdLocal(vecOneGlobal);
			auto vecTwoLocal = helper->transformPndGlobalToLmdLocal(vecTwoGlobal);

			//make PndLmdHitPair and check for data sanity, then store to vector
			PndLmdHitPair pairCanditate(vecOneGlobal, vecTwoGlobal, id1, id2);

			//is the candidate even on an overlapping area? this swaps hits if necessary
			if (!candHitsOverlappingArea(pairCanditate)) {
				noOverlap++;
				continue;
			}

			int overlapId = helper->getOverlapIdFromSensorIDs(pairCanditate.getId1(), pairCanditate.getId2());
			pairCanditate.setOverlapId(overlapId);

			pixelHit pixelHitOne = getPixelHitFromSdsHit(hitOne);
			pixelHit pixelHitTwo = getPixelHitFromSdsHit(hitTwo);

			pairCanditate.setPixelHits(pixelHitOne._col, pixelHitOne._row, pixelHitTwo._col, pixelHitTwo._row);

			pairCanditate.calculateDistance();
			pairCanditate.check();

			if (pairCanditate.getDistance() <= 0.025) {

				cout << std::setprecision(12);

				PndLmdHitPair candGlobal = PndLmdHitPair(vecOneGlobal, vecTwoGlobal, id1, id2);
				PndLmdHitPair candlocal = PndLmdHitPair(vecOneLocal, vecTwoLocal, id1, id2);

				candGlobal.setPixelHits(pixelHitOne._col, pixelHitOne._row, pixelHitTwo._col, pixelHitTwo._row);
				candlocal.setPixelHits(pixelHitOne._col, pixelHitOne._row, pixelHitTwo._col, pixelHitTwo._row);

				candGlobal.setOverlapId(overlapId);
				candlocal.setOverlapId(overlapId);

				candGlobal.check();
				candGlobal.calculateDistance();
				candlocal.check();
				candlocal.calculateDistance();

				auto hitOneInSensorOne = helper->transformPndGlobalToSensor(vecOneGlobal, id1);
				auto hitTwoInSensorTwo = helper->transformPndGlobalToSensor(vecTwoGlobal, id2);

				cout << "Suspect Pair:\nGloabal:\n";
				candGlobal.PrintPair();
				cout << "Hit1 in Sensor1:\n";
				hitOneInSensorOne.Print();
				cout << "Hit2 in Sensor2:\n";
				hitTwoInSensorTwo.Print();
				cout << "\bLocal:\n";
				candlocal.PrintPair();
				cout << "-=-=-=-=-=-=-=-=-=-= End of Suspect pair. =-=-=-=-=-=-=-=-=-=-\n";

			}

			if (!pairCanditate.isSane()) {
				pairCanditate.PrintPair();
				cerr << "====              WARNING:                 ====" << "\n";
				cerr << "pair seems valid but did not pass sanity check!" << "\n";
				cerr << "===============================================" << "\n";
				continue;
			}

			//pair must now be sane, in LMD local and has overlapID et al.

			//are we still looking for the dynamic cut values?
			if (_findDynamicCutParameters) {
				dynamicCutHandler &handler = cutHandlers[pairCanditate.getOverlapId()];
				handler.addToSamples(pairCanditate);
				continue;
			}

			//choose whether to apply dynamic cut or simple cut.
			if (!_useDynamicCut) {
				//cout << "using static cut.\n";
				if (!applyStaticDistanceCut(pairCanditate)) {
					unsuitable++;
					continue;
				}
				//pair survived distance cut? great, store!
			}

			if (_useDynamicCut) {
				//is the cutHandler ready for this overlapID? if not, something went wrong.
				dynamicCutHandler &handler = cutHandlers[pairCanditate.getOverlapId()];
				if (!handler.ready()) {
					//cout << "handler not ready.\n";
					continue;
				}
				//cout << "applying cut.\n";
				//the cutHandler is ready, apply distance cut
				if (!applyDynamicDistanceCut(pairCanditate)) {

					distanceTooHigh++;
					continue;
				}
				//pair survived distance cut? great, store!
				//cout << "pair survived.\n";
			}

			// if the pair survived to this point, it's valid. store!
			getStatistics(pairCanditate);
			new ((*hitPairArray)[storedPairsPerEvent]) PndLmdHitPair(pairCanditate);
			storedPairsPerEvent++;

		}
	}

	return;
}

void LmdPairFinderTask::FinishEvent() {
}

void LmdPairFinderTask::FinishTask() {

	int notReady = 0;

	//were we looking for dynamic cut parameters? write them to disk
	if (_findDynamicCutParameters) {
		cout << "PndLmdSensorAligner: writing cut parameters to disk...\n";

		cout << "There are " << cutHandlers.size() << " handlers.\n";

		for (auto &handlerIt : cutHandlers) {
			dynamicCutHandler &handler = handlerIt.second;
			if (!handler._ready) {
				notReady++;
				cout << "Warning! handler " << handler._overlapID << " only has  " << handler.samples.size() << " pairs!\n";
				continue;
			}
			handler.calcMinAndMax();

			std::stringstream configput("");
			configput << "dynamicCut.aligners." << handler._overlapID << ".";
			config.put(configput.str() + "minDist", handler._minDist);
			config.put(configput.str() + "maxDist", handler._maxDist);
			config.put(configput.str() + "mean", handler._mean);
			config.put(configput.str() + "RMS", handler._RMS);

		}
		if (notReady > 0) {
			cout << "PndLmdSensorAligner: Attention! " << notReady << " handlers don't have enough pairs.\n";

		}

		if (PndLmdAlignManager::writeConfigFile(config, _cutParameterFile, true)) {
			cout << "PndLmdSensorAligner: Successfully written cutParameters to " << _cutParameterFile << "\n";
		}
		cout << "PndLmdSensorAligner: could not write cut parameters to disk!\n";
	}

	//also, write statistics
	Int_t sumOfAllPlanes = plane0 + plane1 + plane2 + plane3;
	double plane0Percent = ((double) plane0 / noOfGoodPairs) * 100;
	double plane1Percent = ((double) plane1 / noOfGoodPairs) * 100;
	double plane2Percent = ((double) plane2 / noOfGoodPairs) * 100;
	double plane3Percent = ((double) plane3 / noOfGoodPairs) * 100;
	double allPlanesPercent = ((double) sumOfAllPlanes / noOfGoodPairs) * 100;
	double clusterRatio = ((double) hitsClustered / (double) (hitsSinglePixel + hitsClustered)) * 100;
	double pixelsPerEvent = (double) sumOfPixelHits / (double) noOfEvents;
	double goodPairsPerEvent = (double) noOfGoodPairs / (double) noOfEvents;

	cout << "\n";
	cout << "*************************************************************" << "\n";
	cout << "                      pair finder done                       " << "\n";
	cout << "*************************************************************" << "\n";
	cout << "\n";
	cout << "                     counting statistics:" << "\n";
	cout << "\n";
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
	printf("hits on all planes: %.2f %% (should be 100%!) \n", allPlanesPercent);
	cout << "\n";
	cout << "*************************************************************" << "\n";
	return;
}

void LmdPairFinderTask::Register() {
}

bool LmdPairFinderTask::applyDynamicDistanceCut(PndLmdHitPair &candidate) {

	int overlapID = candidate.getOverlapId();

	double distance = candidate.getDistance();
	dynamicCutHandler &handler = cutHandlers[overlapID];

	//cout << "dist: " << distance << ", min: " << handler.getMinDist() << " max: " << handler.getMaxDist() << "\n";

	if (distance < handler.getMinDist() || distance > handler.getMaxDist()) {
		return false;
	}
	return true;
}

bool LmdPairFinderTask::applyStaticDistanceCut(PndLmdHitPair &candidate) {

	//check distance squared
	double distance = candidate.getDistance();
	if (distance > _maxDistance) {
		distanceTooHigh++;
		return false;
	}
	return true;
}

void LmdPairFinderTask::getStatistics(PndLmdHitPair &candidate) {

	//check for overlap
	int fplane;

	auto &infoOne = helper->getHitLocationInfo(candidate.getId1());
	fplane = infoOne.plane;

	//count events per plane
	switch (fplane) {
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
		cerr << "WARNING: hit was deemed suitable but plane number is " << fplane << "\n";
		cerr << "This should not happen!" << "\n";
	}
	noOfGoodPairs++;
}

pixelHit LmdPairFinderTask::getPixelHitFromSdsHit(PndSdsHit* sdsHit) {

	pixelHit result;
	int hitSensorId;
	double row, col;

	int clusterIndex = sdsHit->GetClusterIndex();

	//cout << "Cluster index: " << clusterIndex << "\n";

	std::vector<pixelCluster> clusters;

	PndSdsClusterPixel *clusterPixelCand = (PndSdsClusterPixel*) clusterCandidateArray->At(clusterIndex);

	int noOfClusters = clusterPixelCand->GetClusterSize();

	for (int iCluster = 0; iCluster < noOfClusters; iCluster++) {

		PndSdsDigiPixel* mcPixel = (PndSdsDigiPixel*) digiArray->At(clusterPixelCand->GetDigiIndex(iCluster));

		//TODO; do this properly
		if (!mcPixel) {
			exit(1);
		}

		hitSensorId = mcPixel->GetSensorID();

		col = mcPixel->GetPixelColumn();
		row = mcPixel->GetPixelRow();

		//skip decoding errors
		if (col < 0 || row < 0) {
			continue;
		} else {
			clusters.push_back(pixelCluster(pixelHit(hitSensorId, col, row)));
		}
	}

	//cout << "There are " << clusters.size() << " pixel hits.\n";

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

	//for every cluster, check every other cluster
	for (size_t i = 0; i < clusters.size(); i++) {
		//clusters are interchangeable, check every pair only once
		for (size_t j = i + 1; j < clusters.size(); j++) {

			//clusters must be on same sensor
			if (clusters[i]._sensorId != clusters[j]._sensorId) {
				continue;
			}

			if (clusters[i].isNeighbour(clusters[j])) {
				clusters[i].merge(clusters[j]);
				clusters.erase(clusters.begin() + j);
				j--;
			}
		}
	}

	//cout << "There are still " << clusters.size() << " clusters left before discarding large clusters.\n";

	//calculate cluster centers and discard large clusters
	//for statistis: count cluster ratio
	for (size_t i = 0; i < clusters.size(); i++) {
		clusters[i].calculateCenter();

		//FIXME: read from parameter file!
		if (clusters[i].clusterSize > 1) {
			hitsClustered++;

			//if ignoreClustres is set, skip clustered events
			//FIXME: get maximum cluster size from config file
			if (_ignoreClusters || clusters[i].clusterSize > 3) {
				clusters.erase(clusters.begin() + i);
				/*
				 * this is important! when you erase cluster i, cluster i+1 becomes cluster i, but the first i
				 * becomes i+1 itself.
				 * that means, cluster i (former i+1) never gets checked in the first line of the outer for loop!
				 */
				i--;
			}
		} else {
			hitsSinglePixel++;
		}
	}

	//cout << "There are " << clusters.size() << ". clusters left after discarding large clusters. Should be only one!\n";

	return pixelHit(hitSensorId, clusters[0].centerCol, clusters[0].centerRow);

}

bool LmdPairFinderTask::candHitsOverlappingArea(const PndLmdHitPair &candidate) {
	int firstSensorId, secondSensorId;
	firstSensorId = candidate.getId1();
	secondSensorId = candidate.getId2();

	//same sensor hit?
	if (firstSensorId == secondSensorId) {
		return false;
	}

	int fhalf, fplane, fmodule, fside, fsensor;
	int bhalf, bplane, bmodule, bside, bsensor;

	auto &infoOne = helper->getHitLocationInfo(firstSensorId);
	auto &infoTwo = helper->getHitLocationInfo(secondSensorId);

	fhalf = infoOne.detector_half;
	bhalf = infoTwo.detector_half;

	//the necessities for overlapping, must be on same half, plane, module and other side
	if (bhalf != fhalf) {
		return false;
	}

	fside = infoOne.module_side;
	bside = infoTwo.module_side;

	//sort them that hit0 is always upstream
	if (bside < fside) {
		cout << "WARNING. HitTwo is upstream, but this should never happen!\n";
	}

	fplane = infoOne.plane;
	bplane = infoTwo.plane;

	fmodule = infoOne.module;
	bmodule = infoTwo.module;

	fsensor = infoOne.module_sensor_id;
	bsensor = infoTwo.module_sensor_id;

	if (bplane != fplane) {
		return false;
	}
	if (bmodule != fmodule) {
		return false;
	}
	if (bside == fside) {
		return false;
	}

	//0to5
	if (fsensor == 0 && bsensor == 5) {
		return true;
	}
	//3to8
	if (fsensor == 3 && bsensor == 8) {
		return true;
	}
	//4to9
	if (fsensor == 4 && bsensor == 9) {
		return true;
	}
	//3to6
	if (fsensor == 3 && bsensor == 6) {
		return true;
	}
	//1to8
	if (fsensor == 1 && bsensor == 8) {
		return true;
	}
	//2to8
	if (fsensor == 2 && bsensor == 8) {
		return true;
	}
	//2to9
	if (fsensor == 2 && bsensor == 9) {
		return true;
	}
	//3to7
	if (fsensor == 3 && bsensor == 7) {
		return true;
	}
	//4to7
	if (fsensor == 4 && bsensor == 7) {
		return true;
	}
	//all other checks are negative? then the sensors don't overlap!
	return false;
}

void LmdPairFinderTask::Reset() {
}

