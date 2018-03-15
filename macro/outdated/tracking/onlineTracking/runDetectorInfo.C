#include "TStopwatch.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "basefunctions.hxx"
//#include "FairRunAna.h"
//#include "FairParRootFileIo.h"
//#include "FairRuntimeDb.h"
//#include "PndSttTrackFinderReal.h"
//#include "PndMCTrack.h"
//#include "PndSttHit.h"
//#include "PndTrack.h"

// xy-projection of time-based MVD and STT Hits
// m.mertens@fz-juelich.de

int runDetectorInfo() {
	gROOT->Reset();
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0;

	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	//gSystem->Load("libSttMvdTracking");

	TString simFileName = "Sim_Dpm_500.root";
	TString parFileName = "Sim_Dpm_500_params.root";
	TString digiFileName = "Sim_Dpm_500_digi.root";
	TString recoFileName = "Sim_Dpm_500_reco.root";
	TString outFileName = "Sim_Dpm_500_sttinfo.root";

	TFile filedigi(digiFileName.Data());
	TFile filereco(recoFileName.Data());
	TFile filerecopixel(recoFileName.Data());

	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(simFileName.Data());
	//fRun->AddFriend(recoFile.Data());
	fRun->AddFriend(digiFileName.Data());
	//fRun->AddFriend(trackF.Data());
	fRun->SetOutputFile(outFileName.Data());
	//fRun->RunWithTimeStamps();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(parFileName.Data());
	rtdb->setFirstInput(parInput1);

	//works around the "geometry not supported by map" error
	PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
	PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
	sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
	//sttFindTracks->SetPersistence(kFALSE);
	fRun->AddTask(sttFindTracks);
	fRun->Init();
	PndGeoSttPar* fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
	PndSttMapCreator* mapper = new PndSttMapCreator(fSttParameters);
	TClonesArray* tubearray = mapper->FillTubeArray();

	c1 = new TCanvas("c1");
	c1->Range(-42,-42,42,42);
	c1->SetCanvasSize(2400, 2400);

	TText* mytext = new TText();

	PrintSttStats(tubearray);
  return 0;
}
