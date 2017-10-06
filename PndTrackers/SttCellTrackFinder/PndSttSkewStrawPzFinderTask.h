/*
 * PndSttSkewStrawPzFinderTask.h
 *
 *  Created on: Feb 8, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERTASK_H_
#define PNDSTTSKEWSTRAWPZFINDERTASK_H_

#include <FairTask.h>
#include <PndSttSkewStrawPzFinder.h>
//#include <PndSttSkewStrawPzFinderData.h>

class TClonesArray;
class TCanvas;
class PndGeoSttPar;
class PndSttHit;
class PndTrack;
class PndTrackCand;

using namespace std;

class PndSttSkewStrawPzFinderTask: public FairTask {
public:
	ClassDef(PndSttSkewStrawPzFinderTask,1);

	PndSttSkewStrawPzFinderTask();

	virtual ~PndSttSkewStrawPzFinderTask();

	virtual InitStatus Init();

	virtual void Exec(Option_t* opt);

	virtual void FinishEvent();

	void StoreData(bool set){
		fStoredata = set;
	}

private:
	bool fStoredata;
	PndSttSkewStrawPzFinder *PzFinder;
	//PndSttSkewStrawPzFinderData *PzFinderData;

	TClonesArray *fEventHeader;
	TCanvas *SttXYproj;
	vector<PndSttHit*> fSkewedHits;
	TClonesArray *fSTTHits;
	PndGeoSttPar *fSttParameters;     // for filling fTubeArray
	TClonesArray *fTubeArray;
	map<int, vector<int> > fMapTubeIDToHits;
	map<int, int> fMapHitIndexToTubeID;

	TClonesArray *fFirstTrackCand;
	TClonesArray *fFirstTrack;
	TClonesArray *fFirstRiemannTrack;

	TClonesArray *fCombiTrackCand;
	TClonesArray *fCombiTrack;
	TClonesArray *fCombiRiemannTrack;

	vector<PndTrackCand> fVectorPndTrackCand;
	vector<PndTrack> fVectorPndTrack;

	TClonesArray* fFinalTrackCand;
	TClonesArray* fFinalTrack;
	TClonesArray* fFinalRiemannTrack;
	TClonesArray* fFinalPzData;
};

#endif /* PNDSTTSKEWSTRAWPZFINDERTASK_H_ */
