
void runSim(const int nEvents=10, const double mom= 6.2, TString storePath=" /private/huagen/simdata", const int verboseLevel=0, const int particle=-2212)
{ //The macro is to make the simulation run after constructing the detector Geometry, defining
 // the physics process, creating the event generator.
	TStopwatch timer;
	timer.Start();
	gDebug=0;
	//int verboseLevel = 1;

     //   Int_t nEvents = 20;
	gROOT->LoadMacro("/home/huagen/fairroot/pandarootJun24/trunk/gconfig/rootlogon.C");
	rootlogon();

	Double_t pi=TMath::Pi();
	FairRunSim *fRun = new FairRunSim();
	TString simOutput =storePath+"/Lmd_Test.root";
	TString parOutput=storePath+"/LmdParams1.root";

	fRun->SetName("TGeant3");

	fRun->SetOutputFile(simOutput);
	fRun->SetMaterials("media_pnd.geo");

// *************************************************************
// Step 1 of the main flowchart to create detector
// *************************************************************
	// Create and add detectors	// Add cave of panda, it's one of the FAIR moduel
	FairModule *Cave= new PndCave("CAVE");
    Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave);
         // Add magnet of panda on this macro, also FAIR module

	FairModule *Pipe= new PndPipe("PIPE");
	Pipe->SetGeometryFileName("pipebeamtarget.geo");
//	fRun->AddModule(Pipe);

//    cout<<"before loading lmd"<<endl;
	FairDetector* Lmd= new PndLmdDetector("LMD",kTRUE);
//	cout<<"after loading lmd"<<endl;
	Lmd->SetGeometryFileName("lmd.geo");
//	cout<<"after loading lmd"<<endl;
	Lmd->SetVerboseLevel(verboseLevel);
//	cout<<"after loading lmd"<<endl;
        fRun->AddModule(Lmd);
 //       cout<<"after loading lmd"<<endl;


// ***************************************************************
// Step 2 of the main flowchart to define the event generator
// ***************************************************************
    // Create and set event generator    // Invoke the primGen for this simulation run
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Box Generator:
    FairBoxGenerator* boxGen = new FairBoxGenerator(-2212, 1);
    // first number: PDG particle code: 2nd number: particle multiplicity per event
	boxGen->SetPRange(mom, mom); // GeV/c
	boxGen->SetPhiRange(0.,360.);// Azimuth angle range [degree]
	boxGen->SetThetaRange(0.005*180./pi, 0.005*180./pi);
	boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
//    boxGen->SetBoxXYZ(-1,1,1,-1,0);
	primGen->AddGenerator(boxGen);


// Field Map Definition
	//	// MultiField:
	PndMultiField *fField= new PndMultiField();
/*
	PndTransMap    *map_t  = new PndTransMap("TransMap", "R");
	PndDipoleMap   *map_d = new PndDipoleMap("DipoleMap", "R");
//	PndSolenoidMap *map_s = new PndSolenoidMap("SolenoidMap", "R");
	PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");
*/
//	fField->AddField(map_t);
//	fField->AddField(map_d);
//	fField->AddField(map_s);
//	fField->AddField(map_s1);
//	fField->AddField(map_s2);
//	fField->AddField(map_s3);
//	fField->AddField(map_s4);

//switch offf the field on Jul. 13 2009
//	fRun->SetField(fField);

	fRun->SetStoreTraj(kTRUE);
	//fRun->SetStoreTraj(kFALSE);
	fRun->Init();
	cout<<"fRun Initialize successfully!"<<endl;

	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	Bool_t kParameterMerged=kTRUE;
	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	output->open(parOutput.Data(), "RECREATE");
	rtdb->setOutput(output);

	// Initalize the monte carlo	        // ------------------------
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
    exit();
}

int runLmdSim()
{
	runSim();
  return 0;
}

