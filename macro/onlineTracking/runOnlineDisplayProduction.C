#include "drawhelpers.hxx"

void runOnlineDisplayProduction()  {
	gROOT->Reset();
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0;

	//enum DetectorId {
	//	kDCH,kDRC,kDSK,kEMC,kGEM,kLUMI,kMDT,kMVD,kRPC,kSTT,kTPC,kTOF,kFTS,kHYPG,kHYP};

	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	// Reco Parameters
	Double_t stthitlifetime = 245;

	TString simFileName = "Sim_Dpm_500.root";
	TString parFileName = "Sim_Dpm_500_params.root";
	TString digiFileName = "Sim_Dpm_500_digi.root";
	TString recoFileName = "Sim_Dpm_500_reco.root";
	TString outFileName = "Sim_Dpm_500_streamdisplay.root";

	TString digiFile = "all.par";
	TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
	allDigiFile += "/macro/params/";
	allDigiFile += digiFile;

	TFile filedigi(digiFileName.Data());
	TFile filereco(recoFileName.Data());
	TFile filerecopixel(recoFileName.Data());

	TTree *treedigi = (TTree*) filedigi.Get("cbmsim");
	TTree *recotree = (TTree*) filereco.Get("cbmsim");

	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(simFileName.Data());
	fRun->AddFriend(digiFileName.Data());
	fRun->AddFriend(recoFileName.Data());
	fRun->SetOutputFile(outFileName.Data());

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo();
	parInput1->open(parFileName.Data());
	FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
	parIo1->open(allDigiFile.Data(),"in");
	rtdb->setFirstInput(parInput1);
	rtdb->setSecondInput(parIo1);
	rtdb->print();

	fRun->Init();

	PndOnlineGeometryManager *online_geometry = new PndOnlineGeometryManager(rtdb, parFileName.Data());

	PndOnlineManager *online = new PndOnlineManager();
	online->AddGeometryManager(online_geometry);
	online->LoadStream(PndOnline::kSTT,"STTSortedHits",stthitlifetime);
	online->LoadStream(PndOnline::kMVDPixel,"MVDHitsPixel",stthitlifetime);
	online->LoadStream(PndOnline::kMVDStrip,"MVDHitsStrip",stthitlifetime);

	TClonesArray* tubearray = online_geometry->GetDetectorGeometry(PndOnline::kSTT);

	// load task
	PndOnlineSttTripletFinder *triplet_finder = new PndOnlineSttTripletFinder(online, tubearray);
	online->AddTask((FairTask*)triplet_finder);

	// initialize display
	TCanvas* c1 = new TCanvas("c1");
	c1->Range(-42,-42,42,42);
	c1->SetCanvasSize(1200, 1200);
	TText* mytext = new TText();

	// event loop!
	int current_time = 0;
	int drawtime = 0;
	int delta_t = 2050;
	int drawdeltat = 2;
	int final_time = 500;

	online->SetHESRRevolutionDuration(delta_t);

	online->Init();

	TObjArray* online_fairhits_stt = 0;
	TObjArray* online_tracks = 0;
	TObjArray* online_tracks2 = 0;
	TObjArray* online_fairhits_mvdpixel = 0;
	TObjArray* online_fairhits_mvdstrip = 0;
    
	for (current_time = 0; current_time < final_time; current_time += delta_t) {
		//online->LoadHits( PndOnlineManager::kHESRRevolution );
		online->ClearTracks();
		online->LoadHits( delta_t );
		//online->LoadHits( current_time );
		online->Process();
		//online->Clear();
	
		//Draw the results

	for (drawtime = current_time; drawtime < (current_time + delta_t) && (drawtime < final_time); drawtime += drawdeltat) {
		c1->Clear();
		c1->cd();
		DrawDetector();
		//cout << " Drawing stuff " << endl;

		//online_fairhits_stt = online->GetFairHitObjectList(kSTT);
		//online_tracks = online->GetTrackObjectList();
		//online_fairhits_mvd = online->GetFairHitObjectList(kMVD);

		online_fairhits_stt = online->GetFairHitObjectTimedList(PndOnline::kSTT, drawtime-stthitlifetime-20, stthitlifetime+20);
		online_tracks = online->GetTrackObjectTimedList(drawtime-stthitlifetime-stthitlifetime, stthitlifetime);
		online_fairhits_mvdpixel = online->GetFairHitObjectTimedList(PndOnline::kMVDPixel, drawtime-stthitlifetime-20, 40);
		online_fairhits_mvdstrip = online->GetFairHitObjectTimedList(PndOnline::kMVDStrip, drawtime-stthitlifetime-20, 40);

		// isochrones
		DrawIsochrones(drawtime-stthitlifetime, online_fairhits_stt, tubearray);
		// triplet location
		DrawOnlineTrackVertices(drawtime-stthitlifetime, online_tracks);
		// tracks through triplets
		DrawOnlineTrackHackHitCheck(drawtime-stthitlifetime, online_tracks, online_fairhits_stt, tubearray);
		// mvd hits
		DrawFairHits(drawtime-stthitlifetime, online_fairhits_mvdpixel, 20);
		DrawFairHits(drawtime-stthitlifetime, online_fairhits_mvdstrip, 20);

		c1->cd();
		mytext->DrawText(-42, -42, TString::Format("%d ns",drawtime));
		c1->Update();
		c1->Print("hitdisplay.gif+20");

		delete online_fairhits_stt;
		delete online_tracks;
		delete online_fairhits_mvdpixel;
		delete online_fairhits_mvdstrip;
	}

	}

	c1->Print("hitdisplay.gif++");
    
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
