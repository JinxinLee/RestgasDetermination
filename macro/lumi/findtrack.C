void findtrack()
{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 2;
	Int_t nEvents = 0;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------

	TString clustFile = "reco_G4.root";
	TString trackFile = "track_G4.root";

	//Run Track finder
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(clustFile);

	fRun->SetOutputFile(trackFile);


	Double_t n = 2.0;
	Double_t mom = 8.9;
	Double_t eps = 0.03;
	Double_t alpha = 40 * 1E-3;
	Bool_t usehough =false;
	PndLumiTrackFinder* lumitrackfinder = new PndLumiTrackFinder(n, mom, eps, alpha, iVerbose, usehough);
	fRun->AddTask(lumitrackfinder);

	// Initialize and run
	fRun->Init();
	fRun->Run(0, nEvents);

	//rtdb->saveOutput();
	//rtdb->print();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	exit();
}
