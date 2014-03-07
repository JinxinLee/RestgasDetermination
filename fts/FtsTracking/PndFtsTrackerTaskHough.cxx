#include "PndFtsTrackerTaskHough.h"
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
#include "TVector3.h"

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
#include "TClonesArray.h"
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
PndFtsTrackerTaskHough::PndFtsTrackerTaskHough()
: FairTask("PndFtsTrackerTaskHough")
{
	if(fVerbose>3) Info(MESSAGE_ORIGIN,"Default Constructor of PndFtsTrackerTaskHough");
	Initialization_ClassVariables();
}

// ---- Destructor ----------------------------------------------------
PndFtsTrackerTaskHough::~PndFtsTrackerTaskHough()
{
	if(fVerbose>3) Info(MESSAGE_ORIGIN,"Destructor of PndFtsTrackerTaskHough");
	fOutFile->Close();
}




void PndFtsTrackerTaskHough::Initialization_ClassVariables()
{
	// in case of multiple constructors this method is useful
	fEventNr=0;
	// general
	fOutFile=0;
	fSaveDebugInfo=kFALSE;
	fVerbose = 0;
	fPersistence = kTRUE;

	// arrays
	fFtsParameters=0;
	fFtsTubeArray=0;
	fFtsHitArray=0;
	fFtsBranchId=0;
	fTracksArrayName="FTSTrkHough";
	SetTrackOutput();
	fHoughTrackCands = 0;
	fTrackCands = 0;
	fTracks = 0;
}




// ----  Initialisation  ----------------------------------------------
void PndFtsTrackerTaskHough::SetParContainers()
{
	if(fVerbose>3) Info(MESSAGE_ORIGIN,"SetParContainers of PndFtsTrackerTaskHough");

	// FTS parameters
	FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
	fFtsParameters=(PndGeoFtsPar*)(rtdb->getContainer("PndGeoFtsPar"));
}

// ---- Init ----------------------------------------------------------
InitStatus PndFtsTrackerTaskHough::Init()
{
	if(fVerbose>3) Info("Init","Initilization of PndFtsTrackerTaskHough");

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
    fLogger->Error(MESSAGE_ORIGIN,"No InputDataLevelName array!\n PndFtsTrackerTaskHough will be inactive");
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
		if(fVerbose>3) Info(MESSAGE_ORIGIN,"No FTSHit array!");
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
		if(fVerbose>3) Info(MESSAGE_ORIGIN,"No fField array!");
		return kERROR;
	}



	fHoughTrackCands = new TClonesArray("PndFtsHoughTrackCand");
	ioman->Register("PndFtsHoughTrackCand", "FTSTrkDebug", fHoughTrackCands, fSaveDebugInfo);


	fTrackCands = new TClonesArray("PndTrackCand");
	fTracks = new TClonesArray("PndTrack");
	ioman->Register(fTracksArrayName,"FTSTrk", fTracks, fPersistence); // not needed for pattern recognition
	ioman->Register(fTracksArrayName+"Cand","FTSTrk", fTrackCands, fPersistence); // TODO Is that correct, should it not be FTSTrkCand or something?

	if (fSaveDebugInfo){
		InitOutFileForDebugging();
	}

	if(3<fVerbose) Info("Register","Done.");

	return kSUCCESS;

}


void PndFtsTrackerTaskHough::InitOutFileForDebugging(){
	fOutFile = FairRootManager::Instance()->GetOutFile();
	if (0==fOutFile)
	{
		std::cout << "InitOutFileForDebugging: Cannot get outfile.\n";
	}
	else
	{
		fOutFile->cd();
		fOutFile->mkdir("PndFtsTrackerTaskHough");
		std::cout << "InitOutFileForDebugging: Outfile initialised for debugging output.\n";
	}
}

//void PndFtsTrackerTaskHough::AddNewEventToOutFileForDebugging(UInt_t eventNr){
//	fOutFile = FairRootManager::Instance()->GetOutFile();
//	if (0==fOutFile)
//	{
//		std::cout << "AddNewEventToOutFileForDebugging: Cannot get outfile.\n";
//	}
//	else
//	{
//		fOutFile->cd();
//		fOutFile->cd("PndFtsTrackerTaskHough");
//		fOutFile->mkdir(""+eventNr);
//	}
//}


void PndFtsTrackerTaskHough::WriteHistogram(PndFtsHoughSpace* houghSpace){
	if (0==fOutFile)
	{
		std::cout << "WriteHistograms: Cannot get outfile.\n";
	}
	else
	{
		fOutFile->cd();
		fOutFile->cd("PndFtsTrackerTaskHough");
		if(3<fVerbose) std::cout << "WriteHistograms: Got outfile for debugging output.\n";
		if (0!=houghSpace)
		{
			TString histNameOld = houghSpace->GetName();
			TString histNameNew = houghSpace->GetName();
			histNameNew+=fEventNr;
			houghSpace->SetName(histNameNew);
			houghSpace->Write();
			houghSpace->SetName(histNameOld);
		}
		fOutFile->cd();
	}
}


// ---- ReInit  -------------------------------------------------------
InitStatus PndFtsTrackerTaskHough::ReInit()
{
	InitStatus stat=kSUCCESS;
	if(fVerbose>3) Info(MESSAGE_ORIGIN,"Re- Initilization of PndFtsTrackerTaskHough");
	return stat;
}


void PndFtsTrackerTaskHough::SetHitPositionErrors()
{
	// TODO: Do NOT overwrite the original TCA of FTS hits https://forum.gsi.de/index.php?t=msg&goto=15924
	if (1<fVerbose) {
		std::cout << "All FTS hits in event " << fFtsHitArray->GetEntriesFast() << "\n";
	}

	for (int iHit = 0; iHit < fFtsHitArray->GetEntriesFast(); iHit++)
	{
		PndFtsHit* myHit = (PndFtsHit*) fFtsHitArray->At(iHit);
		Int_t tubeID = myHit->GetTubeID();
		PndFtsTube *tube = (PndFtsTube*) fFtsTubeArray->At(tubeID);
		const Double_t zError = 2*tube->GetHalfLength();
		// TODO: Read out radius of FTS tube
		const Double_t xError = 1.01 + 0.003; // in cm // Straw diameter: 10.1 mm, tube wall 0.03 mm Mylar
		const Double_t yError = xError;
		TVector3 hitPosError(xError,yError,zError);
		myHit->SetPositionError(hitPosError);
		// TODO: Take rotation into account for skewed straws

	} // for loop over all hits
}


// ---- Exec ----------------------------------------------------------
void PndFtsTrackerTaskHough::Exec(Option_t* option)
{
	++fEventNr;
	if(0<fVerbose) Info("Exec","Exec of PndFtsTrackerTaskHough on event %i", fEventNr);

	// Reset output array
	if ( ! fTrackCands )
		Fatal("Exec", "No trackCandArray");

	fTrackCands->Delete();
	fTracks->Delete();
	fHoughTrackCands->Delete();



	SetHitPositionErrors();



	PndFtsHoughTrackFinder trackFinder(this, fFtsBranchId, fFtsHitArray, fField);
	trackFinder.SetVerbose(fVerbose);
	trackFinder.SetSaveDebugInfo(fSaveDebugInfo);
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
		if (1<fSaveDebugInfo) {
			PndFtsHoughTrackCand* myHoughCand = new ((*fHoughTrackCands)[iFoundTrack])PndFtsHoughTrackCand(trackFinder.GetHoughTrack(iFoundTrack));
		}

		// convert to PndTrackCand and store into TCA
		PndTrackCand* myCand = new ((*fTrackCands)[iFoundTrack])PndTrackCand(trackFinder.GetPndTrackCand(iFoundTrack));
		if (1<fVerbose)
		{
			std::cout << "Track " << iFoundTrack << std::endl;
			std::cout << "Links: ";
			((FairMultiLinkedData*) myCand)->Print();
			std::cout << std::endl;
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



void PndFtsTrackerTaskHough::FinishEvent()
{
	// TODO Check if this is necessary, I think it can be left out!
	//	fTrackCands->Delete();
	//	fTracks->Delete();
	//	fHoughTrackCands->Delete();
}


// ---- Finish --------------------------------------------------------
void PndFtsTrackerTaskHough::Finish()
{
	if(3<fVerbose) Info("Finish","Finish of PndFtsTrackerTaskHough");
	// Get a handle from the IO manager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman ) {
		fLogger->Fatal(MESSAGE_ORIGIN,"RootManager not instantiated, return!");
	}
	ioman->Write();
	if(fVerbose>3) Info("Finish","Found %i tracks.",fTrackCands->GetEntriesFast());
}





ClassImp(PndFtsTrackerTaskHough)
