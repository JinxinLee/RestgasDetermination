int runLumiPixel5BackProp(const int nEvents = 10, const int startEvent = 0,
		TString storePath = "tmpOutput", const int verboseLevel = 0,
		const TString Method = "Geane", const bool mergedHits = true,
		const double mom = 15, double ipc_x = 0.0, double ipc_y = 0.0,
		double ipc_z = 0.0, bool is_prefiltered = true) {
	// ========================================================================
	// Parameter file
	TString parFile = storePath + "/Lumi_Params_";
	parFile += startEvent;
	parFile += ".root";
	// Track file
	TString TrkFile = storePath + "/Lumi_Track_";
	TrkFile += startEvent;
	TrkFile += ".root";

	// Output file
	TString outFile = storePath + "/Lumi_Geane_";
	outFile += startEvent;
	outFile += ".root";

	std::cout << "TrackFile: " << TrkFile.Data() << std::endl;
	std::cout << "GeaneFile: " << outFile.Data() << std::endl;

	// -----   Timer   ------ --------------------------------------------------
	TStopwatch timer;
	timer.Start();
	// ------------------------------------------------------------------------

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun = new FairRunAna();
	FairFileSource input_source(TrkFile);
	fRun->SetSource(&input_source);
	fRun->SetOutputFile(outFile);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data());
	rtdb->setFirstInput(parInput1);

	// =========================================================================
	// ======                 Back-Propagating                     ======
	// =========================================================================

	Double_t fpBeam = mom;
	TVector3 IP(ipc_x, ipc_y, ipc_z);
	if (Method == "Geane") {
		FairGeane *Geane = new FairGeane();
		fRun->AddTask(Geane);
		PndLmdGeaneTask* lmdgeane = new PndLmdGeaneTask(fpBeam, IP, is_prefiltered);
		lmdgeane->SetVerbose(verboseLevel);
		fRun->AddTask(lmdgeane);
	} else {
		if (Method == "RK") {
			//PndLmdBPRungeKuttaTask have dependences from genfit -> switched off until movement to gentfit2
			cout
					<< "I'm very sorry, but BPRungeKuttaTask  option for LMD is currently switched off :("
					<< endl;
			// PndLmdBPRungeKuttaTask* lmdbp = new PndLmdBPRungeKuttaTask(fpBeam, IP);
			// lmdbp->SetVerbose(verboseLevel);
			// fRun->AddTask(lmdbp);
		} else {
			cout << "Method " << Method.Data() << " doesn't exist!" << endl;
		}
	}
	rtdb->setOutput(parInput1);
	rtdb->print();
	// =====                        End of Geane                           =====
	// =========================================================================

	// -----   Intialise and run   --------------------------------------------
	fRun->Init();
	//  // PndEmcMapper *emcMap = PndEmcMapper::Instance(6);
	//  PndEmcMapper *emcMap = PndEmcMapper::Instance();
	// //Geane->SetField(fRun->GetField());
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
