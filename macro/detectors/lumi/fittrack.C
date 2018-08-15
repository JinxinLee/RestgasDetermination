int fittrack()
{

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iverb = 0;
	Int_t nEvents = 0;
	Double_t pbeam = 8.9;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// --------------
	TString trackFile = "track_G4.root";
	TString trkfitFile = "lintrk_G4.root";

	TString FitParFile = gSystem->Getenv("VMCWORKDIR");
	FitParFile += "/macro/params/lumi.linfit.890.par";

	//Run Reconstruction
	// ------------
	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(trackFile);
	fRun->SetOutputFile(trkfitFile);

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
	parInput->open(FitParFile.Data(),"in");
	rtdb->setFirstInput(parInput);

	Double_t sig = 0.005/TMath::Sqrt(12);
	Double_t dx=6*sig; dy=6*sig;
	PndLumiTrackFitter* linfit = new PndLumiTrackFitter(dx, dy, iverb);
	fRun->AddTask(linfit);

	// Initialize and run
	fRun->Init();
	fRun->Run(0, nEvents);

	rtdb->saveOutput();
	rtdb->print();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
  return 0;
}
