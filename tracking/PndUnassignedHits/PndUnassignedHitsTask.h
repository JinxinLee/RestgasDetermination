/*
 * PndUnassignedHitsTask.h
 *
 *  Created on: Nov 22, 2018
 *      Author: Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#ifndef PndUnassignedHitsTask_H_
#define PndUnassignedHitsTask_H_

#include "PndPersistencyTask.h"

#include "FairMultiLinkedData.h"

#include "FairLogger.h"

#include<map>

class TClonesArray;
class FairMultiLinkedData;

class PndUnassignedHitsTask: public PndPersistencyTask {
public:
    PndUnassignedHitsTask(): PndPersistencyTask("Unassigned Hits Task"), fUnassignedBranchExtension("Unassigned"){};
    virtual ~PndUnassignedHitsTask(){};

	/** Virtual method Init **/
	virtual void SetParContainers();
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	virtual void AddHitBranch(TString branchName);
	virtual void AddTrackCands(TString trackCandName);
        virtual void SetUnassignedBranchExtension(TString name) { fUnassignedBranchExtension = name; };
        void SetVerbose(Int_t verbose){ fVerbose = verbose;};

protected:
    void RegisterBranches();
    void RegisterTrackCands();
    void FillTrackCands();
    void FillUnassignedHits(TString branchName);
    void FillOutputBranch(TString branchName, std::vector<int> unusedHits);

private:
    std::map<TString, TClonesArray*> fHitBranches;              //!
    std::map<TString, TClonesArray*> fUnassignedHitBranches;    //!
    std::map<TString, TClonesArray*> fTrackCands;               //!
    FairMultiLinkedData fHitsInTracks;                          //!
    TString fUnassignedBranchExtension;

	ClassDef(PndUnassignedHitsTask,1);


};

#endif /* PndUnassignedHitsTask_H_ */
