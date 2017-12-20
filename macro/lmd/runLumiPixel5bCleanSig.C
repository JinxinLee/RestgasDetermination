int runLumiPixel5bCleanSig(const int nEvents = 10, const int startEvent = 0,
		TString storePath = "tmpOutput", const int verboseLevel = 0,
		const double mom = 15, const double dX = 0, const double dY = 0) {
	// ========================================================================
	TString parFile = storePath + "/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";
	// // Track file
	TString TrkFile = storePath + "/Lumi_Track_";
	TrkFile += startEvent;
	TrkFile += ".root";
	// Geane output file
	TString GeaneFile = storePath + "/Lumi_Geane_";
	GeaneFile += startEvent;
	GeaneFile += ".root";

	//Output file
	TString outFile = storePath + "/Lumi_GeaneFiltered_";
	outFile += startEvent;
	outFile += ".root";

	std::cout << "TrackFile: " << TrkFile.Data() << std::endl;
	std::cout << "GeaneFile: " << GeaneFile.Data() << std::endl;
	std::cout << "OutputFile: " << outFile.Data() << std::endl;

	// ---  Now choose concrete engines for the different tasks   -------------
	// -----------------------------------------------------------------------------------

	// -----   Timer   ------ --------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	FairFileSource input_source(GeaneFile);
	input_source.AddFriend(TrkFile);
	fRun->SetSource(&input_source);
	fRun->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data());
	rtdb->setFirstInput(parInput1);

	// =========================================================================
	// ======                 Cleaning                                                                                                                  ======
	// =========================================================================
	Double_t fpBeam = mom;
	TString dir = "TMVAweights/"; //trained on samples with elastic and inelastic events
	PndLmdSigCleanTask* lmdclean = new PndLmdSigCleanTask(fpBeam, dir);
	lmdclean->SetVerbose(verboseLevel);
	lmdclean->SetDX(dX);
	lmdclean->SetDY(dY);
	fRun->AddTask(lmdclean);
	rtdb->setOutput(parInput1);
	rtdb->print();
	// =========================================================================

	// -----   Intialise and run   --------------------------------------------
	fRun->Init();
	fRun->Run(0, nEvents);
	// ------------------------------------------------------------------------
	rtdb->saveOutput();
	rtdb->print();
	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
	// temporary fix to avoid double frees at the destruction of te program for pandaroot/fairroot with root6
	gGeoManager->GetListOfVolumes()->Delete();
	gGeoManager->GetListOfShapes()->Delete();
	delete gGeoManager;

	return 0;
}
