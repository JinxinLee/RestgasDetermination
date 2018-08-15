// macro to fit hits with a straight line
// Simone Bianco 15/07/2010

int Fit()
{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose = 0;
	Int_t nEvents = 5100;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------

	TString trackFile = "DESY/data23NewAli.hit.hits.root";

	TString trkfitFile = "DESY/TEST/data23NewAliYWell.hit.fit.root";


	//Run Reconstruction
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(trackFile);

	fRun->SetOutputFile(trkfitFile);

	TtLinFitTask* linfit = new TtLinFitTask();

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
	//exit();
  return 0;
}
