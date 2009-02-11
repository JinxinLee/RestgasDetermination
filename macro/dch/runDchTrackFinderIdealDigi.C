runDchTrackFinderIdealDigi() {	

	//------ Loading libraries -------------------------
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libPndBase");
	gSystem->Load("libField");
	gSystem->Load("libPassive");
	gSystem->Load("libGen");
	gSystem->Load("libgenfit");
	gSystem->Load("libDch");
	//--------------------------------------------------



	//------ Verbosity level ---------------------------
	//(0 = quiet, 1 = event level, 2 = track level, 3 = debug)
	Int_t iVerbose = 0;

	//------ Input file (MC events) --------------------
	TString mcFile = "tst.root";

	//------ Input file (Hits file) --------------------
	TString hitFile = "tst_idealHits.root";

	//------ Input file (Digis file) -------------------
	TString digiFile = "tst_digis.root";

	//------ Output file -------------------------------
	TString outFile = mcFile;
	outFile.ReplaceAll(".root","_trackIdealDigi.root");

	//------ Parameter file ----------------------------
	//TString parFile = "DchParams.root";

	//------ Number of events to process ---------------
	Int_t nEvents = 0;  //if 0 all the events will be processed
	//--------------------------------------------------



	//------ Timer -------------------------------------
	TStopwatch timer;
	timer.Start();
	//--------------------------------------------------



	//------ Reconstruction run ------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(mcFile);
	fRun->SetOutputFile(outFile);
	fRun->AddFriend(hitFile);
	fRun->AddFriend(digiFile);
	//--------------------------------------------------



	//------ Ideal DCH track finder --------------------
	//Create and add finder task
	PndDchFindTracks* finderTask = new PndDchFindTracks("dchFindTracks");
	finderTask->SetUseHitOrDigi("digi");
	fRun->AddTask(finderTask);

	PndDchTrackFinderIdealDigi* mcTrackFinder = new  PndDchTrackFinderIdealDigi();
	mcTrackFinder->SetVerbose(3);  // verbosity level
	mcTrackFinder->SetPrimary(1);  // 1 = Only primary tracks are processed, 0 = all (default)
	finderTask->UseFinder(mcTrackFinder);
	//--------------------------------------------------



	//------ Match PndDchTracks and MCTracks tracks ----
	PndDchMatchTracks *matchTask = new PndDchMatchTracks();
	matchTask->SetUseHitOrDigi("digi");
	matchTask->SetVerbose(1);
	fRun->AddTask(matchTask);
	// -------------------------------------------------

	
	
	//------ Quality of PndDchTracks -------------------
	PndDchFindTracksQa *qualityTrack = new PndDchFindTracksQa();
	qualityTrack->SetUseHitOrDigi("digi");
	qualityTrack->SetVerbose(1);
	fRun->AddTask(qualityTrack);
	// -------------------------------------------------



	//------ Prefit tracks -----------------------------
	//Currently: Works ONLY when set "hit" in: finderTask->SetUseHitOrDigi("hit");
	//PndDchPreFitterTR* fitter= new PndDchPreFitterTR();
	//fRun->AddTask(fitter);
	//--------------------------------------------------



	//------ Intialise and run -------------------------
	cout << "fRun->Init()" << endl;
	fRun->Init();
	fRun->Run(0,nEvents);
	//--------------------------------------------------



	//------ Finish ------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is "    << outFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// -------------------------------------------------
	
	exit(0);
}

