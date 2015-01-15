/*
 * PndTrackingQualityAnalysis.h
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#ifndef PndTrackingQualityAnalysis_H_
#define PndTrackingQualityAnalysis_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndMCMatch.h"
#include "PndMCResult.h"
#include "PndTrackCand.h"

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include <vector>
#include <map>

#include <functional>

class PossibleTrackFunctor : public std::binary_function<FairMultiLinkedData* , Bool_t, Bool_t>
{
  public :
    virtual Bool_t operator() (FairMultiLinkedData* a, Bool_t primary) {return Call(a, primary);};
    virtual Bool_t Call(FairMultiLinkedData* a, Bool_t primary) = 0;
    virtual void Print() = 0;

    virtual ~PossibleTrackFunctor() {};

};

class StandardTrackFunctor : public PossibleTrackFunctor
{
	Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
		FairRootManager* ioman = FairRootManager::Instance();
		Bool_t possibleTrack = kFALSE;
		possibleTrack = possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
										 a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() > 3);

		possibleTrack = possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
										 a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() +
										 a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() ) > 5;

		return possibleTrack;
	}

	void Print(){
		std::cout << "StandardTrackFunctor: > 3 Hits in MVD or > 5 Hits in (MVD+Stt)" << std::endl;
	}
};

class OnlySttFunctor : public PossibleTrackFunctor
{
	Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
		FairRootManager* ioman = FairRootManager::Instance();
		Bool_t possibleTrack = kFALSE;

		possibleTrack = possibleTrack | a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5;

		return possibleTrack;
	}
	void Print(){
		std::cout << "OnlySttFunctor: > 5 Hits in Stt" << std::endl;
	}
};

class RiemannMvdSttGemFunctor : public PossibleTrackFunctor
{
	Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
		FairRootManager* ioman = FairRootManager::Instance();
		Bool_t possibleTrack = kFALSE;
		Bool_t mvdHits =  ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
							a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks()) > 2);

		if (mvdHits){
			possibleTrack = a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 1 | a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks() > 1;
		}
		return possibleTrack;
	}
	void Print(){
		std::cout << "RiemannMvdSttGemFunctor: > 2 Hits in MVD and >0 Hits in (Stt+Gem)" << std::endl;
	}

};

class CircleHoughTrackFunctor : public PossibleTrackFunctor
{
	Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
		if (primary == kFALSE) return kFALSE;
		FairRootManager* ioman = FairRootManager::Instance();
		Bool_t possibleTrack = ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
									a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks()) > 3);

		return possibleTrack;
	}
	void Print(){
		std::cout << "CircleHoughTrackFunctor: > 3 Hits in MVD and primary track" << std::endl;
	}

};

class PndTrackingQualityAnalysis : public TObject
{
public:
	PndTrackingQualityAnalysis(TString trackBranchName, Bool_t pndTrackData = kTRUE);
	PndTrackingQualityAnalysis(TString trackBranchName, PossibleTrackFunctor* posTrack, Bool_t pndTrackData = kTRUE);
	virtual ~PndTrackingQualityAnalysis();

	virtual void Init();
	void SetVerbose(Int_t val){fVerbose = val;}

	//! Adds branch names of detector data which should be taken into account in the analysis
	void AddHitsBranchName(TString name){ fBranchNames.push_back(name);}
	void SetHitsBranchNames(std::vector<TString> names){ fBranchNames = names;}

	void AnalyseEvent();

	Int_t GetNIdealHits(Int_t trackId, TString branchName);
	Int_t GetNIdealHits(FairMultiLinkedData& track, TString branchName);
	std::map<Int_t, Int_t> GetMCTrackFound()						{return fMCTrackFound;}
	std::map<Int_t, Int_t> GetTrackQualifikation()					{return fMapTrackQualifikation;}
	std::map<Int_t, Int_t> GetTrackMCStatus()							{return fMapTrackMCStatus;}
	std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t > > > GetEfficiencies()	{return fMapEfficiencies;}
	std::map<Int_t, Double_t> GetPResolution()						{return fMapPResolution;}
	std::map<Int_t, Double_t> GetPtResolution()						{return fMapPtResolution;}
	std::map<Int_t, Int_t> GetTrackIdMCId()							{return fTrackIdMCId;}
	Int_t GetNGhosts()												{return fNGhosts;}



	void PrintTrackDataSummary(FairMultiLinkedData& trackData, Bool_t detailedInfo = kFALSE);

	/** Prints the information of the track quality map.
	* First output value is the MC track ID. Second output the quality indicator:
	* -1 : Track with at least one hit in a tracking detector but of type 0
	* 0  : Track with sufficient hits in tracking detectors for track finding but not found
	* 1  : Not assigned
	* 2  : All hits of the MC track were found and no additional once
	* 3  : Some hits of the MC track were found and no additional once
	* 4  : 70 % of all hits found belong to this MC track
	*/
	void PrintTrackQualityMap(Bool_t detailedInfo = kFALSE);
	void PrintTrackMCStatusMap();


private:


	virtual void FillMapTrackQualifikation();
//	virtual Bool_t PossibleTrack(FairMultiLinkedData& mcForward);
	Int_t GetSumOfAllValidMCHits(FairMultiLinkedData* trackData);
	virtual Int_t AnalyseTrackInfo(std::map<TString, FairMultiLinkedData>& trackInfo);
	virtual void CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData>& trackInfo);

	virtual Bool_t IsCorrectGemHit(FairLink& gemLink);

	Int_t fVerbose;
	FairRootManager* ioman;
	Int_t fNGhosts;

	TString fTrackBranchName;
	Bool_t fPndTrackOrTrackCand; //kTRUE if track and kFALSE if track cand
	PossibleTrackFunctor* fPossibleTrack;

	Bool_t fUseCorrectedSkewedHits;

	std::vector<TString> fBranchNames;
	std::map<Int_t, Int_t> fTrackIdMCId;				//< map between track id and most probable MC track id
	std::map<Int_t, Int_t> fMCTrackFound;				//< How often was a MC Track (key) found
	PndMCResult fIdealTracksData;

	std::map<Int_t, Int_t> fMapTrackMCStatus;			//< TrackId vs TrackStatus from MC
	std::map<Int_t, Int_t> fMapTrackQualifikation;		//< TrackId vs TrackStatus after analysis of track finding
	std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t> > > fMapEfficiencies;  //< MostProbable TrackId, BranchName, Efficiency, #FoundHits / #MCHits, #MCHits
	std::map<Int_t, Double_t> fMapPResolution;
	std::map<Int_t, Double_t> fMapPtResolution;


	PndMCMatch* fMCMatch;
	TClonesArray* fTrack;
	TClonesArray* fMCTrack;


	ClassDef(PndTrackingQualityAnalysis, 2)
};

#endif /* PNDTRACKINGQUALITY_H_ */
