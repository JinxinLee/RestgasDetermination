#include "TStopwatch.h"
#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "basefunctions.hxx"
#include "basefunctions_skew.hxx"
//#include "FairRunAna.h"
//#include "FairParRootFileIo.h"
//#include "FairRuntimeDb.h"
//#include "PndSttTrackFinderReal.h"
//#include "PndMCTrack.h"
//#include "PndSttHit.h"
//#include "PndTrack.h"

// xy-projection of time-based MVD and STT Hits
// m.mertens@fz-juelich.de

void runStreamDisplay() {
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
	TString outFileName = "Sim_Dpm_500_streamdisplay.root";

	TFile filedigi(digiFileName.Data());
	TFile filereco(recoFileName.Data());
	TFile filerecopixel(recoFileName.Data());

	TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
	TClonesArray *sttsortedhits = new TClonesArray("PndSttHit");
	treedigi->SetBranchAddress("STTSortedHits",&sttsortedhits);

	TTree *mvdstriprecotree = (TTree*) filereco.Get("cbmsim");
	TClonesArray *mvdstripreco = new TClonesArray("PndSdsHit");
	mvdstriprecotree->SetBranchAddress("MVDHitsStrip",&mvdstripreco);

	TTree *mvdpixelrecotree = (TTree*) filereco.Get("cbmsim");
	TClonesArray *mvdpixelreco = new TClonesArray("PndSdsHit");
	mvdpixelrecotree->SetBranchAddress("MVDHitsPixel",&mvdpixelreco);
  
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

	TCanvas* rzcanvas = new TCanvas("rzcanvas");
	rzcanvas->Range(-54,-42,114,42);
	rzcanvas->SetCanvasSize(1200, 600);
	rzcanvas->cd();

	TCanvas* c2 = new TCanvas("c2");
	c2->Range(-54,-42,114,42);
	c2->SetCanvasSize(1200, 600);
	c2->cd();

	TCanvas* c1 = new TCanvas("c1");
	c1->Range(-42, -42, 42, 42);
	c1->SetCanvasSize(1200, 1200);
	c1->cd();

	Int_t starttime = 0;
	Int_t timelimit = 2200;
	Int_t timewindow = 245;
	Int_t revtimewindow = 10;
	Int_t mvdtimewindow = 20;
	Int_t mvdrevtimewindow = 5;
	Int_t nexttime = 0;
	Int_t evpos = 0;
	Int_t clonespos = 0;
	Int_t stripnexttime = 0;
	Int_t stripevpos = 0;
	Int_t stripclonespos = 0;
	Int_t pixelnexttime = 0;
	Int_t pixelevpos = 0;
	Int_t pixelclonespos = 0;
	TObjArray *drawstackhits = new TObjArray();
	drawstackhits->SetName("drawstackhits");
	drawstackhits->SetOwner(kTRUE);
	TObjArray *drawstackstriphits = new TObjArray();
	drawstackstriphits->SetName("drawstackstriphits");
	drawstackstriphits->SetOwner(kTRUE);
	TObjArray *drawstackpixelhits = new TObjArray();
	drawstackpixelhits->SetName("drawstackpixelhits");
	drawstackpixelhits->SetOwner(kTRUE);

	TObjArray* innertriplets = new TObjArray();
	innertriplets->SetName("innertriplets");
	TObjArray* midtriplets = new TObjArray();
	midtriplets->SetName("midtriplets");
	TObjArray* outertriplets = new TObjArray();
	outertriplets->SetName("outertriplets");
	TObjArray* innerskewlets = new TObjArray();
	innerskewlets->SetName("innerskewlets");
	TObjArray* outerskewlets = new TObjArray();
	outerskewlets->SetName("outerskewlets");


	TText* mytext = new TText();

	treedigi->GetEntry(0);
	mvdstriprecotree->GetEntry(0);
	mvdpixelrecotree->GetEntry(0);

	for (Int_t currenttime = starttime; currenttime < timelimit; currenttime+=5) {
		//Get events up to mytime + timewindow
		GetHitsInTime(currenttime, timewindow, drawstackhits, nexttime, evpos, clonespos, treedigi, sttsortedhits);
		GetHitsInTime(currenttime, mvdtimewindow, drawstackstriphits, stripnexttime, stripevpos, stripclonespos, mvdstriprecotree, mvdstripreco);
		GetHitsInTime(currenttime, mvdtimewindow, drawstackpixelhits, pixelnexttime, pixelevpos, pixelclonespos, mvdpixelrecotree, mvdpixelreco);

		cout << "STT Hits active: " << drawstackhits->GetEntriesFast() << endl;
		cout << "MVD Strip Hits active: " << drawstackstriphits->GetEntriesFast() << endl;
		cout << "MVD Pixel Hits active: " << drawstackpixelhits->GetEntriesFast() << endl;
		if ((drawstackhits->IsEmpty()) && (drawstackstriphits->IsEmpty()) && (drawstackpixelhits->IsEmpty())) continue;

		//Discard events < mytime
		RemoveOldHits(currenttime, revtimewindow, drawstackhits);
		RemoveOldHits(currenttime, mvdrevtimewindow, drawstackstriphits);
		RemoveOldHits(currenttime, mvdrevtimewindow, drawstackpixelhits);

		//Calculate Isochrones and Triplets
		innertriplets->Clear();
		midtriplets->Clear();
		outertriplets->Clear();
		innerskewlets->Clear();
		outerskewlets->Clear();
		//FindTriplets(drawstackhits, tubearray, innertriplets, 104, 215);
		//FindTriplets(drawstackhits, tubearray, midtriplets, 714, 855);
		//FindTriplets(drawstackhits, tubearray, outertriplets, 2956, 3175);
		FindSkewlets(drawstackhits, tubearray, innerskewlets, 1000, 1395, 1394, 1813);

		//Draw the results in YZ
		c2->Clear();
		c2->cd();
		DrawDetectorYZ();
		DrawFairHitsYZ(currenttime-mvdrevtimewindow, drawstackstriphits, mvdtimewindow+mvdrevtimewindow);
		DrawFairHitsYZ(currenttime-mvdrevtimewindow, drawstackpixelhits, mvdtimewindow+mvdrevtimewindow);
		DrawVector3YZ(innerskewlets);
		mytext->DrawText(-54, -42, TString::Format("%d ns",currenttime));

		//Draw the results in RZ
		rzcanvas->Clear();
		rzcanvas->cd();
		DrawDetectorRZ();
		DrawFairHitsRZ(currenttime-mvdrevtimewindow, drawstackstriphits, mvdtimewindow+mvdrevtimewindow);
		DrawFairHitsRZ(currenttime-mvdrevtimewindow, drawstackpixelhits, mvdtimewindow+mvdrevtimewindow);
		DrawVector3RZ(innerskewlets);
		mytext->DrawText(-54, -42, TString::Format("%d ns",currenttime));


		//Draw the results in XY
		c1->Clear();
		c1->cd();
		DrawDetector();
		cout << " Drawing stuff " << endl;
		DrawIsochrones(currenttime, drawstackhits, tubearray);
		DrawFairHits(currenttime-mvdrevtimewindow, drawstackstriphits, mvdtimewindow+mvdrevtimewindow);
		DrawFairHits(currenttime-mvdrevtimewindow, drawstackpixelhits, mvdtimewindow+mvdrevtimewindow);
		DrawTripletTracks(innertriplets, midtriplets, 10);
		DrawTripletTracks(innertriplets, outertriplets, 45);
		DrawTripletTracks(midtriplets, outertriplets, 25);
		DrawVector3(innerskewlets);

		mytext->DrawText(-42, -42, TString::Format("%d ns",currenttime));
		
 		c1->Update();
 		c1->Print("hitstream.gif+15");
		c2->Update();
		c2->Print("hitstreamyz.gif+15");
		rzcanvas->Update();
		rzcanvas->Print("hitstreamrz.gif+15");
	}
	//c1->Print("hitstream.gif++");
}
