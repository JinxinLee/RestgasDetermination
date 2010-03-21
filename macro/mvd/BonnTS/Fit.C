// macro to fit hits with a straight line
// Simone Bianco 21/03/2010

{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 3;
	Int_t nEvents = 56000;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------

	TString trackFile = "hits.root";

	TString trkfitFile = "fit.root";


	//Run Reconstruction
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(trackFile);
	//	fRun->AddFriend(simFile);
	fRun->SetOutputFile(trkfitFile);

	PndMvdLinFitTask* linfit = new PndMvdLinFitTask();
	fRun->AddTask(linfit);


	// Initialize and run
	fRun->Init();
	fRun->Run(0, nEvents);


	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	exit();
}
