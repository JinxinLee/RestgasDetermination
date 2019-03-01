// -------------------------------------------------------------------------
// -----                PndMcCloner2 source file                     -----
// -----                  Created 08/07/13  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMcCloner2.h"

#include "PndMCTrack.h"
#include "PndPidCandidate.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMcCloner2::PndMcCloner2() : FairTask("Cloner of PndMCTrack"),
		fInputArray(), fPidChargedArray(), fPidNeutralArray(), fOutputArray(),
		mapMCIndex(), fCleanMC(kFALSE), fTrackBranchNamePidHypo("")
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMcCloner2::~PndMcCloner2() { }
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMcCloner2::Init() {

	cout << "-I- PndMcCloner2::Init: "
			<< "INITIALIZATION *********************" << endl;

	fPidHypoStr[0] = "Electron";
	fPidHypoStr[1] = "Muon";
	fPidHypoStr[2] = "Pion";
	fPidHypoStr[3] = "Kaon";
	fPidHypoStr[4] = "Proton";

	//FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
	//FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ) {
		cout << "-E- PndMcCloner2::Init: "
				<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get input array
	fInputArray = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( ! fInputArray ) {
		cout << "-E- PndMcCloner2::Init: "
				<< "No MCTrack array!" << endl;
		return kERROR;
	}
	bool anychargedtca = false;
	bool anyneutraltca = false;

	for(int i=0;i<5;i++){
		fPidChargedArray[i] = (TClonesArray*) ioman->GetObject("PidChargedCand"+fPidHypoStr[i]);
		if ( ! fPidChargedArray[i] ) {
			cout << "-E- PndMcCloner2::Init: "
					<< "No PidChargedCand"+fPidHypoStr[i]+" array. Continue looking." << endl;
//			return kERROR;
		}
		else {
			fHypoFlagCharged[i] = true;
			anychargedtca = true;
		}

		fPidNeutralArray[i] = (TClonesArray*) ioman->GetObject("PidNeutralCand"+fPidHypoStr[i]);
		if ( ! fPidNeutralArray[i] ) {
			cout << "-E- PndMcCloner2::Init: "
					<< "No PidNeutralCand"+fPidHypoStr[i]+" array. Continue looking." << endl;
//			return kERROR;
		}
		else {
			fHypoFlagNeutral[i] = true;
			anyneutraltca = true;
		}
	}

	if (!anychargedtca) {
		cout << "-E- PndMcCloner2::Init: "
				<< "No PidChargedCand array found!" << endl;
		return kERROR;
	}
	if (!anyneutraltca) {
		cout << "-E- PndMcCloner2::Init: "
				<< "No PidNeutralCand array found!" << endl;
		return kERROR;
	}


	// Create and register output array
	fOutputArray = new TClonesArray("PndMCTrack");

	ioman->Register("MCTrack"+fTrackBranchNamePidHypo,"MC",fOutputArray,kTRUE);

	cout << "-I- PndMcCloner2: Intialization successfull" << endl;

	return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMcCloner2::Exec(Option_t*) {

	// Reset output array
	if ( ! fOutputArray ) Fatal("Exec", "No Output Array");

	fOutputArray->Clear();
	mapMCIndex.clear();

	if (!fCleanMC)
	{
		CloneMCTrack();
	}
	else
	{
		FindUsedMCIndices();
		CloneAndCleanMCTrack();
		CorrectMotherIndices();
		CorrectPidIndices();
	}
}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner2::CloneMCTrack()
{
	// Copy 1:1 of the MCTrack TClonesArray

	Int_t nMCTracks = fInputArray->GetEntriesFast();
	for (Int_t iMC=0; iMC<nMCTracks; iMC++)
	{
		PndMCTrack *mctrack  = (PndMCTrack*) fInputArray->At(iMC);
		TClonesArray& clref = *fOutputArray;
		Int_t size = clref.GetEntriesFast();
		new(clref[size]) PndMCTrack(*mctrack);
	} // Loop over MCTracks
}

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner2::FindUsedMCIndices() {
	// Loop over PidChargedCand and PidNeutralCand, find the used MC indices, and fill the map
	// with this index and all the mother indices

	Int_t nCands = 0;

	// First store primary MC tracks
	nCands = fInputArray->GetEntriesFast();
	for (Int_t imc=0; imc<nCands; imc++)
	{
		PndMCTrack *mctrack = (PndMCTrack*)fInputArray->At(imc);
		if (mctrack->GetMotherID()==-1)
		{
			mapMCIndex[imc] = imc;
		}
		else break;
	}

	//todo:assume all arrays are of same length, use list0 to get ncands for now
	//todo:assuming also that all entries in the lists have the same ordering
	for (int ihyp = 0; ihyp < 5;ihyp++){
		if (fPidChargedArray[ihyp]) nCands = fPidChargedArray[ihyp]->GetEntriesFast();
	}
//	nCands = fPidChargedArray[0]->GetEntriesFast();
	for (Int_t iPid=0; iPid<nCands; iPid++)
	{
		Int_t mcIndex = -1;
		for(int i = 0;i<5;i++){
			if (!fHypoFlagCharged[i]) continue;
			PndPidCandidate *pidCand  = (PndPidCandidate*) fPidChargedArray[i]->At(iPid);
			mcIndex = pidCand->GetMcIndex();
			if (mcIndex !=-1) break;	//If any of the pidcandidates are pointing to a track, we exit loop
		}
		while (mcIndex!=-1)
		{
			PndMCTrack *mctrack = (PndMCTrack*)fInputArray->At(mcIndex);
			if (mctrack==NULL)
			{
				Error("FindUsedMCIndices","PndMCTrack is not existing!");
			}
			mapMCIndex[mcIndex] = mcIndex;
			mcIndex = mctrack->GetMotherID();
		}

	} // Loop over PidChargedCand

	for (int ihyp = 0; ihyp < 5;ihyp++){
		if (fPidNeutralArray[ihyp]) nCands = fPidNeutralArray[ihyp]->GetEntriesFast();
	}
//	nCands = fPidNeutralArray[0]->GetEntriesFast();
	for (Int_t iPid=0; iPid<nCands; iPid++)
	{

		Int_t mcIndex = -1;
		for(int i=0;i<5;i++){
			if (!fHypoFlagNeutral[i]) continue;
			PndPidCandidate *pidCand  = (PndPidCandidate*) fPidNeutralArray[i]->At(iPid);
			mcIndex = pidCand->GetMcIndex();
			if (mcIndex !=-1) break;	//If any of the pidcandidates are pointing to a track, we exit loop
		}
		while (mcIndex!=-1)
		{
			PndMCTrack *mctrack = (PndMCTrack*)fInputArray->At(mcIndex);
			if (mctrack==NULL)
			{
				Error("FindUsedMCIndices","PndMCTrack is not existing!");
			}
			mapMCIndex[mcIndex] = mcIndex;
			mcIndex = mctrack->GetMotherID();
		}
	} // Loop over PidNeutralCand

}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner2::CloneAndCleanMCTrack()
{
	// Copy only the MCTracks which were used, and update the mother indices

	for (std::map<Int_t,Int_t>::iterator it=mapMCIndex.begin(); it!=mapMCIndex.end(); ++it)
	{
		PndMCTrack *mctrack  = (PndMCTrack*) fInputArray->At(it->first);
		TClonesArray& clref = *fOutputArray;
		Int_t size = clref.GetEntriesFast();
		new(clref[size]) PndMCTrack(*mctrack);
		mapMCIndex[it->first] = size;
	}
}
// -------------------------------------------------------------------------


// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcCloner2::CorrectMotherIndices()
{
	// Loop over the new MCTrack TClonesArray and update the mother indices
	Int_t nmc = 0;
	nmc = fOutputArray->GetEntriesFast();
	for (Int_t imc=0; imc<nmc; imc++)
	{
		PndMCTrack *mctrack = (PndMCTrack*)fOutputArray->At(imc);
		Int_t motherID = mctrack->GetMotherID();
		Int_t secondMotherID = mctrack->GetSecondMotherID();
		if (motherID!=-1) mctrack->SetMotherID(mapMCIndex[motherID]);
		if (secondMotherID!=-1) mctrack->SetSecondMotherID(mapMCIndex[secondMotherID]);
	}
}
// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcCloner2::CorrectPidIndices()
{
	// Loop over Pid Candidates and set the mc indices with the new value

	Int_t nCands = 0;

	for(int i=0;i<5;i++){
		if (!fHypoFlagCharged[i]) continue;
		nCands = fPidChargedArray[i]->GetEntriesFast();
		for (Int_t iPid=0; iPid<nCands; iPid++)
		{

			PndPidCandidate *pidCand  = (PndPidCandidate*) fPidChargedArray[i]->At(iPid);
			Int_t mcIndex = pidCand->GetMcIndex();
			pidCand->SetMcIndex(mapMCIndex[mcIndex]);
		}
	}
	for(int i=0;i<5;i++){
		if (!fHypoFlagNeutral[i]) continue;
		nCands = fPidNeutralArray[i]->GetEntriesFast();
		for (Int_t iPid=0; iPid<nCands; iPid++)
		{
			PndPidCandidate *pidCand  = (PndPidCandidate*) fPidNeutralArray[i]->At(iPid);
			Int_t mcIndex = pidCand->GetMcIndex();
			pidCand->SetMcIndex(mapMCIndex[mcIndex]);
		}
	}


}
ClassImp(PndMcCloner2)
