// *************************************************************************
// Author: Martin Galuska
// martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
//  This class is modeled after the
// mvd/MvdTracking/PndRiemannTrackFinder
// sttmvdtracking/PndMvdSttGemRiemannTrackFinder
// classes
//
//
// This is a class version of the HoughTest.C macro tracking test implementation minus all the plotting stuff
// Take a look at the notes of the macro version
//
//
// Recent Changes
// Moved code from macro to class
// Major code cleanup and deletion of test code / unneeded code
// use PndFtsHoughTrackCand to store information about track candidates and Hough transforms
//
//
// TODO
// Fill PndTrackCands for output // I need help with this...
// Find all peaks with a minimum height (6 for 1+2, 8 for 3+4+5)
// Add straight line for stations 5+6 (6 hits at least)
// Add skewed hits
// Adaptive Hough
// Add drift circles
//
//
// Created: 18.06.2013
// Modified: 03.02.2014
//
// *************************************************************************











#ifndef PndFtsHoughTrackFinder_H
#define PndFtsHoughTrackFinder_H

#include "TClonesArray.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include <vector>
#include <map>
#include <fstream>


#include <cmath>
#include <vector>


#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"

class FairField;
class TClonesArray;
class PndGeoFtsPar;
class TH1F;
class TH2F;
class TGraph;



class FairHit;


// This class is modeled after
//PndMvdSttGemRiemannTrackFinder
//and
//PndRiemannTrackFinder

class PndFtsHoughTrackFinder
{
public:
	PndFtsHoughTrackFinder();
	virtual ~PndFtsHoughTrackFinder();

	void FindTracks();										///< Main function to start the track finding

	// Input
	//	void SetHits(std::vector<FairHit*> hits){fHits = hits;};				///< Replaces the existing array of hits with a new one
	//	void AddHits(std::vector<FairHit*> hits, Int_t branchId);				///< Appends the new array of hits to the existing one
	void SetHits(TClonesArray* hits, Int_t branchId);						///< Replaces the existing array of hits with a new one
	//	void AddHit(FairHit* hit){fHits.push_back(hit);};						///< Adds one new hit to the array of hits

	void SetField(FairField* field){fField=field;};						///<

	void SetVerbose (int verbose){fVerbose = verbose;};
	void SetSaveDebugInfo(Bool_t saveDebugInfo){ fSaveDebugInfo = saveDebugInfo;};

	// Output
	int NTracks(){return fTrackCand.size();};								///< Returns the number of found tracks
	PndFtsHoughTrackCand GetTrack(int i){return fTracks[i];};					///< Returns the track with the index i
	PndTrack GetPndTrack(int i){return fTracks[i].getPndTrack();};
	PndTrackCand GetTrackCand(int i) {return fTrackCand[i];};


	// Parameters
	void SetMinNumberOfHits(int val){fMinNumberOfHits = val;};

	//  write out histograms for debugging
	void WriteHistograms();


private:
	//! FTS Branch ID
	Int_t   fFtsBranchID;
	/** Input array of PndFtsHit **/
	TClonesArray *fFtsHitArray;
	// TODO double
	//	std::vector<FairHit*> fHits;	// vector with all hits of the current event
	std::vector<PndFtsHoughTrackCand> fTracks;									///< Resulting Hough Track Cands
	std::vector<PndTrackCand> fTrackCand; // resulting tracks
	Int_t    fMinNumberOfHits;												///< Minimum number of hits in track necessary for a match

	Int_t fVerbose;
































	static const Double_t meinpi = 3.14159265;








	// helper method (handy for multiple constructors)
	void Initialization_ClassVariables();


	// for B field access
	FairField* fField;


	// Debug
	////////
	Bool_t fSaveDebugInfo;
	static const Int_t ResolutionX = 800, ResolutionY = 600; // for plotting



	// for Hough
	///////////////////
	TH2F* fHoughspaceZxLineParabola;
	TH2F* fHoughspaceZxParabola;
	TH2F* fHoughspaceZxParabolaLine;
	TH2F* fHoughspaceZyLine;
	std::vector<PndFtsHoughTrackCand> fHoughTrackCandsNew;									///< Temporary Hough Track Cands are used internally to store track cands
	std::vector<PndFtsHoughTrackCand> fHoughTrackCands;									///< Hough Track Cands are used internally to store track cands




	// Which PeakFinder should be used?
	static TString peakfinderOption; //"maxwindow"; //"tspectrum2"; //"maxbin";;



	// for B field access
	//	Double_t BMeanForParabolapz;
	//	Double_t BMeanForParabola;

	// -----------------------------------------------------------------------


	// for HoughTransform
	/////////////////////
	// if kTRUE will correct the pz prediction according to values which should be obtained from a line fit mc truth momentum VS. reco momentum with high statistics
	static const Bool_t correctpz = kFALSE;



	// Line for stations 1+2
	static const Double_t onlyUseHitsFromzForLine = 100.; // Set = 100. if you want to use all hits, higher if you want to exclude hits that are closer to the interaction point than the value
	static const Double_t onlyUseHitsUpTozForLine = 380.; // 1000.; // Set = 1000. if you want to use all hits, lower if you want to exclude hits that are further away from the interaction point than the value
	// for example event 700. cuts away the interaction stuff

	// Hough transform scan steps
	static const Int_t invthetastepForLine = 8; // Je groesser, umso feiner wird in theta-Richtung gescannt
	static const Double_t thetastepForLine = 1./8.; // 1./invthetastepForLine; // Je kleiner, umso feiner wird in theta-Richtung gescannt TODO
	static const Int_t thetalimitlowForLineHoughSpacePlot = -20;
	static const Int_t thetalimithighForLineHoughSpacePlot = 20;
	static const Double_t thetalimitlowForLine= -20+1./8./2.; //thetalimitlowForLineHoughSpacePlot+thetastepForLine/2.0; // Search in theta from this value (in degree) TODO
	static const Double_t thetalimithighForLine = 20+1./8./2.; // thetalimithighForLineHoughSpacePlot+thetastepForLine/2.0; // Search in theta up to this value (in degree) TODO


	//Parabola stations 3-5
	// If kTRUE the y-component of the B-field is not used in the parabola hough transform
	// if kFALSE the parabola's shape will be adjusted based on the magnetic field maps
	// set only to kFALSE for testing
	static const Bool_t keepBConstant = kTRUE;



	// sets where the midpoint of the parabola is supposed to be
	static const Double_t zOffset = 368.; // Der Scheitel der Parabel sollte vermutlich dort sein, wo das Dipolfeld anfängt // 368. was ok // 350. => thetaParabola = 0.
	static const Double_t onlyusehitsfromzForParabola = 380.; // Set = 100. if you want to use all hits, higher if you want to exclude hits that are closer to the interaction point than the value
	static const Double_t onlyusehitsuptozForParabola = 700.; // 1000.; // Set = 1000. if you want to use all hits, lower if you want to exclude hits that are further away from the interaction point than the value
	// for example event 700. cuts away the interaction stuff







	Bool_t FilterFoundTracklets(
			UInt_t maxAcceptableSharedHits,
			std::vector<PndFtsHoughTracklet> &tracklets
	);
	// takes the heighest peak (according to peak finder)
	// of all peaks that share > maxSameHits


	Bool_t FindAllPeaks(
			TString option,
			TH2F *houghspace,
			const UInt_t minHeight,
			std::vector<PndFtsHoughTracklet> &tracklets
	);

	Bool_t MakeHoughSpace(
			TString option,
			const Double_t onlyusehitsfromz,
			const Double_t onlyusehitsuptoz,
			const Bool_t onlyUseHitsFromNonSkewedStraws,
			const Double_t thetalimitlow,
			const Double_t thetalimithigh,
			const Double_t thetastep,
			Double_t interceptZxOrZy,
			UInt_t &nHitsForHoughSpace,
			TH2F* houghspace);




	ClassDef(PndFtsHoughTrackFinder,1);
};

#endif /*PndFtsHoughTrackFinder_H*/
