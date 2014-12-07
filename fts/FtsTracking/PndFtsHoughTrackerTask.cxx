#include "PndFtsHoughTrackerTask.h"


#include <iostream>
#include <math.h>

// FTS
#include "PndGeoFtsPar.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndFtsHit.h"
#include "FairHit.h"

// magnetic field
#include "FairField.h"


// (Hough) tracking
#include "PndFtsHoughTrackFinder.h"
#include "PndFtsHoughSpace.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"



#include "TString.h"


// TODO this list can probably be shorter
//#include "PndDetectorList.h"

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"


#include "TObjectTable.h"

#include "PndFtsMapCreator.h"


using std::cout;
using std::endl;






// ---- Default constructor -------------------------------------------
PndFtsHoughTrackerTask::PndFtsHoughTrackerTask(Int_t verbose, Bool_t persistence, Bool_t saveDebugInfo)
: FairTask("PndFtsHoughTrackerTask", verbose),
  fSaveDebugInfo(saveDebugInfo),
  fPersistence(persistence),
  fEventNr(0),
  //  fOutFile(0),

  // arrays
  fFtsParameters(0),
  fFtsTubeArray(0),
  fFtsHitArray(0),
  fFtsBranchId(0),
  fTracksArrayName("FTSTrkHough"),

  // B field
  fField(0),

  // Debugging
  //fHoughSpaces(0),
  fHoughTrackCands(0),
  fLogger(FairLogger::GetLogger()),

  // output
  fTrackCands(0),
  fTracks(0)
{
	if(3<fVerbose) std::cout << "PndFtsHoughTrackerTask is the tracker ptr " << this << '\n';
}

// ---- Destructor ----------------------------------------------------
PndFtsHoughTrackerTask::~PndFtsHoughTrackerTask()
{
	if(fVerbose>3) fLogger->Info(MESSAGE_ORIGIN,"Destructor of PndFtsHoughTrackerTask");
	//	fOutFile->Close();
}




// ----  Initialisation  ----------------------------------------------
void PndFtsHoughTrackerTask::SetParContainers()
{
	if(fVerbose>3) fLogger->Info(MESSAGE_ORIGIN,"SetParContainers of PndFtsHoughTrackerTask");

	// FTS parameters
	FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
	fFtsParameters=(PndGeoFtsPar*)(rtdb->getContainer("PndGeoFtsPar"));
}

// ---- Init ----------------------------------------------------------
InitStatus PndFtsHoughTrackerTask::Init()
{
	if(fVerbose>3) Info("Init","Initilization of PndFtsHoughTrackerTask");

	// Get a handle from the IO manager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ) {
		fLogger->Fatal(MESSAGE_ORIGIN,"RootManager not instantiated, return!");
		return kFATAL;
	}

	// Get a pointer to the previous already existing data level
	/*
    <InputDataLevel> = (TClonesArray*) ioman->GetObject("InputDataLevelName");
    if ( ! <InputLevel> ) {
    fLogger->Error(MESSAGE_ORIGIN,"No InputDataLevelName array!\n PndFtsHoughTrackerTask will be inactive");
    return kERROR;
    }
	 */

	// Create the TClonesArray for the output data and register
	// it in the IO manager
	/*
    <OutputDataLevel> = new TClonesArray("OutputDataLevelName", 100);
    ioman->Register("OutputDataLevelName","OutputDataLevelName",<OutputDataLevel>,kTRUE);
	 */

	// Do whatever else is needed at the initilization stage
	// Create histograms to be filled
	// initialize variables




	// FTS Hits
	fFtsHitArray= (TClonesArray *)ioman->GetObject("FTSHit");
	if ( ! fFtsHitArray ) {
		if(fVerbose>3) fLogger->Info(MESSAGE_ORIGIN,"No FTSHit array!");
		return kERROR;
	}

	// FTS Branch
	fFtsBranchId = 	ioman->GetBranchId("FTSHit");

	// FTS Tube Array
	PndFtsMapCreator *mapperFts = new PndFtsMapCreator(fFtsParameters);
	fFtsTubeArray = mapperFts->FillTubeArray();



	// B field
	if(fVerbose>3) Info("Init","Try to get B field.");
	fField = FairRunAna::Instance()->GetField();
	if ( ! fField ) {
		if(fVerbose>3) fLogger->Info(MESSAGE_ORIGIN,"No fField array!");
		return kERROR;
	}

	// Debugging
	//	if (fSaveDebugInfo){
	//		InitOutFileForDebugging();
	//	}
	fHoughTrackCands = new TClonesArray("PndFtsHoughTrackCand");
	ioman->Register("FTSTrkDebugCand", "HoughTrackCand", fHoughTrackCands, fSaveDebugInfo);

	//fHoughSpaces = new TClonesArray("PndFtsHoughSpace");
	//ioman->Register("FTSTrkDebugHS", "HoughSpaces", fHoughSpaces, fSaveDebugInfo);


	// Output
	fTrackCands = new TClonesArray("PndTrackCand");
	fTracks = new TClonesArray("PndTrack");
	ioman->Register(fTracksArrayName,"FTSTrk", fTracks, fPersistence); // for PndTrack
	ioman->Register(fTracksArrayName+"Cand","FTSTrk", fTrackCands, fPersistence); // for PndTrackCand // TODO Is that correct, should it not be FTSTrkCand or something?

	if(3<fVerbose) Info("Register","Done.");

	return kSUCCESS;

}


//void PndFtsHoughTrackerTask::InitOutFileForDebugging(){
//	fOutFile = FairRootManager::Instance()->GetOutFile();
//	if (0==fOutFile)
//	{
//		std::cout << "InitOutFileForDebugging: Cannot get outfile.\n";
//	}
//	else
//	{
//		fOutFile->cd();
//		fOutFile->mkdir("PndFtsHoughTrackerTask");
//		std::cout << "InitOutFileForDebugging: Outfile initialised for debugging output.\n";
//	}
//}

//void PndFtsHoughTrackerTask::AddNewEventToOutFileForDebugging(UInt_t eventNr){
//	fOutFile = FairRootManager::Instance()->GetOutFile();
//	if (0==fOutFile)
//	{
//		std::cout << "AddNewEventToOutFileForDebugging: Cannot get outfile.\n";
//	}
//	else
//	{
//		fOutFile->cd();
//		fOutFile->cd("PndFtsHoughTrackerTask");
//		fOutFile->mkdir(""+eventNr);
//	}
//}


void PndFtsHoughTrackerTask::WriteHistogram(const PndFtsHoughSpace *const houghSpace, Int_t index) const{
	//	Int_t index = fHoughSpaces->GetEntriesFast();
	//	PndFtsHoughSpace* myHoughSpace = new ((*fHoughSpaces)[index])PndFtsHoughSpace(*houghSpace);


	TString outName = "plots/";
	outName += houghSpace->GetName();
	outName+=fEventNr;
	if (-1!=index){
		outName+="-";
		outName+=index;
	}
	outName+=".C";
	houghSpace->SaveAs(outName, "LEGO2");


	//	fOutFile = FairRootManager::Instance()->GetOutFile();
	//	if (0==fOutFile)
	//	{
	//		std::cout << "WriteHistograms: Cannot get outfile.\n";
	//	}
	//	else
	//	{
	//		//			fOutFile->cd();
	//		//			fOutFile->cd("PndFtsHoughTrackerTask");
	//		if(3<fVerbose) std::cout << "WriteHistograms: Got outfile for debugging output.\n";
	//		if (0!=houghSpace)
	//		{
	//			TString histNameOld = houghSpace->GetName();
	//			TString histNameNew = houghSpace->GetName();
	//			histNameNew+=fEventNr;
	//			houghSpace->SetName(histNameNew);
	//			houghSpace->Write();
	//			houghSpace->SetName(histNameOld);
	//		}
	//		//			fOutFile->cd();
	//	}
}


// ---- ReInit  -------------------------------------------------------
InitStatus PndFtsHoughTrackerTask::ReInit()
{
	InitStatus stat=kSUCCESS;
	if(3<fVerbose) fLogger->Info(MESSAGE_ORIGIN,"Re- Initilization of PndFtsHoughTrackerTask");
	return stat;
}


const TVector3 PndFtsHoughTrackerTask::GetFtsHitPosErrors(const PndFtsHit* ftsHit) const
{
	const PndFtsTube *const tube = GetFtsTube(ftsHit);

	const Double_t sizeSigmaCoeff = 1.5; // TODO Check value
	const Double_t rhoError = tube->GetRadIn()/sizeSigmaCoeff;
	const Double_t zError = tube->GetHalfLength()/sizeSigmaCoeff; // TODO might need additional factor

	TVector3 hitPosErrors(rhoError,rhoError,zError);

	return hitPosErrors;
}

const TMatrixT<Double_t> PndFtsHoughTrackerTask::GetFtsHitCovMatrix(const PndFtsHit* ftsHit) const
{
	const PndFtsTube *const tube = GetFtsTube(ftsHit);

	const Double_t sizeSigmaCoeff = 1.5; // TODO Check value
	const Double_t rhoError = tube->GetRadIn()/sizeSigmaCoeff;
	const Double_t zError = tube->GetHalfLength()/sizeSigmaCoeff; // TODO might need additional factor
	TMatrixT<Double_t> rotationMatrix = tube->GetRotationMatrix();

	TMatrixT<Double_t> unrotatedCovMatrix(3,3);
	// initialize with 0
	for (Int_t firstIdx=0; firstIdx < 3; ++firstIdx){
		for (Int_t secondIdx=0; secondIdx < 3; ++secondIdx){
			unrotatedCovMatrix[firstIdx][secondIdx] = 0;
			}
	}
	unrotatedCovMatrix[0][0] = pow(rhoError, 2);
	unrotatedCovMatrix[1][1] = pow(rhoError, 2);
	unrotatedCovMatrix[2][2] = pow(zError, 2);

	TMatrixT<Double_t> rotatedCovMatrix = rotationMatrix*unrotatedCovMatrix;
	rotatedCovMatrix *= rotationMatrix.Transpose(rotationMatrix);

	return rotatedCovMatrix;
}


// ---- Exec ----------------------------------------------------------
void PndFtsHoughTrackerTask::Exec(Option_t* option)
{
	++fEventNr;
	if(0<fVerbose) Info("Exec","Exec of PndFtsHoughTrackerTask on event %i", fEventNr);

	// Reset output array
	if ( ! fTrackCands ) Fatal("Exec", "No track cand array");

	fTrackCands->Delete();
	fTracks->Delete();
	fHoughTrackCands->Delete();
	//fHoughSpaces->Delete();



	if(3<fVerbose) std::cout << "PndFtsHoughTrackFinder::Exec tracker ptr " << this << '\n';
	PndFtsHoughTrackFinder trackFinder(this);
	//	trackFinder.SetMinPeakHeightZxLineParabola(4);
	//	trackFinder.SetMinPeakHeightZxParabola(6);
	//	trackFinder.SetMinPeakHeightZxParabolaLine(4);
	//	trackFinder.SetMinPeakHeightZyLine(4);
	trackFinder.FindTracks();


	// have a look at PndMvdRiemannTrackFinderTask line 161 for reference
	// store the found tracks as PndTrack and PndTrackCand
	for (Int_t iFoundTrack = 0; iFoundTrack < trackFinder.NTracks(); ++iFoundTrack){

		// for debug output get PndFtsHoughTrackCand
		// TODO Check if that works
		if (kTRUE==fSaveDebugInfo) {
			PndFtsHoughTrackCand* myHoughCand = new ((*fHoughTrackCands)[iFoundTrack])PndFtsHoughTrackCand(trackFinder.GetHoughTrack(iFoundTrack));
		}

		// get output as PndTrackCand and store into TCA
		PndTrackCand* myCand = new ((*fTrackCands)[iFoundTrack])PndTrackCand(trackFinder.GetPndTrackCand(iFoundTrack));
		if (1<fVerbose)
		{
			std::cout << "Track " << iFoundTrack << '\n';
			std::cout << "Links: ";
			((FairMultiLinkedData*) myCand)->Print();
			std::cout << '\n';
		}

		myCand->CalcTimeStamp(); // TODO Why is this needed?
		if (1<fVerbose) trackFinder.GetHoughTrack(iFoundTrack).Print();


		PndTrack* myTrack = new ((*fTracks)[iFoundTrack])PndTrack(trackFinder.GetPndTrack(iFoundTrack)); // TODO Some parameters are missing
		if (myCand->GetTimeStamp() == 0){
			myTrack->SetTimeStamp(0.0001 * (iFoundTrack+1));
			myCand->SetTimeStamp(0.0001 * (iFoundTrack+1));

		} else {
			myTrack->SetTimeStamp(myCand->GetTimeStamp());
			myTrack->SetTimeStampError(myCand->GetTimeStampError());
		}
		myTrack->SetLink(FairLink("FTSHoughTrackCand", iFoundTrack));
		myTrack->SetTrackCandRef(myCand);


		if (1<fVerbose) {
			std::cout << iFoundTrack << ": ";
			myTrack->Print();
		}
	}
	fTrackCands->Sort();
	fTracks->Sort();



	if(3<fVerbose) Info("Exec","End eventloop.");
}



void PndFtsHoughTrackerTask::FinishEvent()
{

}


// ---- Finish --------------------------------------------------------
void PndFtsHoughTrackerTask::Finish()
{
	if(3<fVerbose) Info("Finish","Finish of PndFtsHoughTrackerTask");
	// Get a handle from the IO manager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ) {
		fLogger->Fatal(MESSAGE_ORIGIN,"RootManager not instantiated, return!");
	}
	ioman->Write();
	if(fVerbose>3) Info("Finish","Found %i tracks.",fTrackCands->GetEntriesFast());
}





ClassImp(PndFtsHoughTrackerTask)
