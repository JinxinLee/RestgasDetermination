void digi()
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
	TString simFile = "test_G4.root";
	TString parFile = "params_G4.root";

	TString digiFile = "digi_G4.root";

	TString digiparFile = gSystem->Getenv("VMCWORKDIR");
	digiparFile += "/macro/params/lumi.digi.par";

	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(simFile);
	fRun->SetOutputFile(digiFile);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(),"UPDATE");
	rtdb->setFirstInput(parInput1);

	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	parInput2->open(digiparFile.Data(),"in");
	rtdb->setSecondInput(parInput2);

	/** Run Digitization */

	PndLumiDigiProducer* lumiDigiProducer = new PndLumiDigiProducer(iVerbose);
	fRun->AddTask(lumiDigiProducer);


	rtdb->setOutput(parInput1);


	// -----   Intialise and run   --------------------------------------------
	fRun->Init();

	fRun->Run(0,nEvents);

	rtdb->saveOutput();
	rtdb->print();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << digiFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
	cout << endl;
	exit();
}

