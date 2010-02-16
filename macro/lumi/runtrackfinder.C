{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 0;
	Int_t nEvents = 0;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------
	TString suffix="_sm_1250";//dpm_0370";

	TString simFile = "sim_" + suffix +".root";
	TString clustFile = "clust_"+ suffix +".root";
	TString parFile = "params_"+ suffix +".root";

	TString trackFile = "track_"+ suffix+".root";


	//Run Track finder
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(clustFile);
	fRun->AddFriend(simFile);
	fRun->SetOutputFile(trackFile);


	//Cell dimension theta, phi space
	Double_t dt = 0.0003;//unit on theta
	Double_t dp = 0.01;//unit on phi

	PndLumiTrackFinder* lumitrackfinder = new PndLumiTrackFinder(dt,dp, iVerbose);
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
