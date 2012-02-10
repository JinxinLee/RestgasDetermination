void reco()
{
	// Load basic libraries\par
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Int_t iVerbose =3;
	Int_t nEvents = 0;

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();

	TString directory = gSystem->Getenv("VMCWORKDIR");
	TString simFile = "test_G4.root";
	TString parFile = "params_G4.root";
	TString digiFile = "digi_G4.root";

	TString recoFile = "reco_G4.root";

	TString digiparFile = gSystem->Getenv("VMCWORKDIR");
	digiparFile += "/macro/params/lumi.digi.par";

	FairRunAna *fRun = new FairRunAna();
	fRun->SetInputFile(digiFile);
	fRun->SetOutputFile(recoFile);

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
	parInput1->open(parFile.Data(),"UPDATE");
	rtdb->setFirstInput(parInput1);

	FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
	parInput2->open(digiparFile.Data(),"in");
	rtdb->setSecondInput(parInput2);

//	fRun->SetGeomFile(geofile);
//   fRun->LoadGeometry();

	/** Run Clusterization **/
	Double_t charge_cut = 7000000;

	PndLumiClusterTask* lumiCluster = new PndLumiClusterTask(charge_cut,0,1,0, iVerbose);
	fRun->AddTask(lumiCluster);

	// Initialize and run\par
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
	exit();
}
