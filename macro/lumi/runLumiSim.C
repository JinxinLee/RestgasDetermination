{
	TStopwatch timer;
	timer.Start();
	gDebug=0;

	Int_t nEvents = 100000;

	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Double_t pi=TMath::Pi();

	// Create a monte carlo
	// --------------------
	FairRunSim *fRun = new FairRunSim();

	//FileNames
	TString simOutput="lumisim.root";

	TString parOutput="lumiparasim.root";

	// Set the MC version used
	// ------------------------
	fRun->SetName("TGeant3");
	fRun->SetOutputFile(simOutput);
	fRun->SetMaterials("media_pnd.geo");

	// Create and add detectors
	// ------------------------
	FairModule *Cave= new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave1.geo");
	fRun->AddModule(Cave);
/*
	FairModule *Magnet= new PndMagnet("MAGNET");
	Magnet->SetGeometryFileName("magnet.geo");
	fRun->AddModule(Magnet);
*/
	FairModule *Pipe= new PndPipe("PIPE");
	Pipe->SetGeometryFileName("pipebeamtarget.geo");
	fRun->AddModule(Pipe);

	FairDetector *Lumi = new PndLumi("Lumi", kTRUE);
	Lumi->SetGeometryFileName("lumi.geo");
	Lumi->SetVerboseLevel(1);
    fRun->AddModule(Lumi);

    // Create and set event generator
    // ------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Box Generator:
    //FairBoxGenerator* boxGen = new FairBoxGenerator(-2212, 1);
    // first number: PDG particle code: 2nd number: particle multiplicity per event
	//boxGen->SetPRange(6.2, 6.2); // GeV/c
	//boxGen->SetPhiRange(0.,360.);// Azimuth angle range [degree]
	//boxGen->SetThetaRange(0.002*180./pi, 0.01*180./pi);
	//boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
	//primGen->AddGenerator(boxGen);

	PndDpmGenerator* dpmGen =
		new PndDpmGenerator("/home/tsito/fairroot/pandasoft/pandaroot/pgenerators/DpmEvtGen/Background-micro.root");
	primGen->AddGenerator(dpmGen);
	// Field Map Definition
	// --------------------

	// MultiField:

	PndMultiField *fField= new PndMultiField();

	PndTransMap    *map_t  = new PndTransMap("TransMap", "R");
//	PndDipoleMap   *map_d = new PndDipoleMap("DipoleMap", "R");
	PndSolenoidMap *map_s = new PndSolenoidMap("SolenoidMap", "R");
	PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

	fField->AddField(map_t);
	//fField->AddField(map_d);
	fField->AddField(map_s);
	//fField->AddField(map_s2);
	//fField->AddField(map_s3);
	//fField->AddField(map_s4);

	//Constant field:
	//PndConstField *fField=new PndConstField();
	//fField->SetField(0.,0.,20.); // values are in kG
	//fField->SetField(0.,0.,0.);
	//fField->SetFieldRegion(-50, 50,-50, 50, -150, 110); // values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)




	fRun->SetField(fField);

	fRun->SetStoreTraj(kTRUE);
	//fRun->SetStoreTraj(kFALSE);

	fRun->Init();

	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	Bool_t kParameterMerged=kTRUE;
	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	output->open(parOutput.Data(), "RECREATE");
	rtdb->setOutput(output);

	// Initalize the monte carlo
	// -------------------------

	PndMultiFieldPar* fieldPar = (PndMultiFieldPar*) rtdb->getContainer("PndMultiFieldPar");

	if ( fField ) {  fieldPar->SetParameters(fField); }

	fieldPar->setInputVersion(fRun->GetRunId(),1);
	fieldPar->setChanged(kTRUE);

	rtdb->saveOutput();
	rtdb->print();

	fRun->Run(nEvents);


	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}

