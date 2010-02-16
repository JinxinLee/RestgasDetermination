{
	TStopwatch timer;
	timer.Start();
	gDebug=0;

	//gROOT->Reset();
	// Load basic libraries
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();

	Double_t pi=TMath::Pi();
	Double_t rad = 180./pi;
	Int_t nEvents = 30000;
	Double_t p_beam = 12.5;
	TString suffix="_sm_1250";//"dpm_0370";

	// Create a monte carlo
	// --------------------

	FairRunSim *fRun = new FairRunSim();

	//FileNames


	TString simFile = "sim_" + suffix +".root";
	TString parFile= "params_"+ suffix +".root";

	// Set the MC version used
	// ------------------------
	fRun->SetName("TGeant3");

	fRun->SetOutputFile(simFile);

	fRun->SetMaterials("media_pnd.geo");

	// Create and add detectors
	// ------------------------
	FairModule *Cave= new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave1.geo");
	fRun->AddModule(Cave);

	//FairModule *Magnet= new PndMagnet("MAGNET");
	//Magnet->SetGeometryFileName("magnet.geo");
	//fRun->AddModule(Magnet);

	//FairModule *Pipe= new PndPipe("PIPE");
	//Pipe->SetGeometryFileName("pipebeamtarget.geo");
	//fRun->AddModule(Pipe);

	FairDetector *Lumi = new PndLumi("Lumi", kTRUE);
	//Lumi->SetGeometryFileName("bonn_sensors.geo");
	Lumi->SetGeometryFileName("lumi.geo");
	Lumi->SetVerboseLevel(1);
    fRun->AddModule(Lumi);


    // Create and set event generator
    // ------------------------------
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

    //primGen->SetBeam(0.0,0.0, 0.16,0.16);
    //primGen->SetTarget(0.0, 0.1);
    //primGen->SmearVertexXY(kTRUE);
    //primGen->SmearVertexZ(kTRUE);
    //primGen->SetAngularDivergance(0.0003);
    //primGen->SmearAngle(kTRUE);

    fRun->SetGenerator(primGen);

    // Box Generator:
    FairBoxGenerator* boxGen = new FairBoxGenerator(-2212, 1);
    boxGen->SetPRange(p_beam, p_beam); // GeV/c
	boxGen->SetPhiRange(0.,360.);// Azimuth angle range [degree]
	boxGen->SetThetaRange(0.005*rad, 0.005*rad);
	boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [mm]
	primGen->AddGenerator(boxGen);

    //DPM generator:
	//PndDpmGenerator* dpmGen =
	//new PndDpmGenerator("/home/tsito/fairroot/pandasoft/cbuild/bin/Background-micro.root");
	//primGen->AddGenerator(dpmGen);

    //Single particle generator
    //FairParticleGenerator* partGen   = new FairParticleGenerator(-2212,1, 0.,0.,p_beam , 0.,0.,0.);
    //primGen->AddGenerator(partGen);

	// Field Map Definition
	// --------------------

	// MultiField:

	//fRun->SetBeamMom(p_beam);

	PndMultiField *fField= new PndMultiField();

	//PndTransMap    *map_t  = new PndTransMap("TransMap", "R");
	//PndDipoleMap   *map_d1 = new PndDipoleMap("DipoleMap1", "R");
	//PndDipoleMap   *map_d2 = new PndDipoleMap("DipoleMap2", "R");
	//PndSolenoidMap *map_s1 = new PndSolenoidMap("SolenoidMap1", "R");
	//PndSolenoidMap *map_s2 = new PndSolenoidMap("SolenoidMap2", "R");
	//PndSolenoidMap *map_s3 = new PndSolenoidMap("SolenoidMap3", "R");
	//PndSolenoidMap *map_s4 = new PndSolenoidMap("SolenoidMap4", "R");

	//fField->AddField(map_t);
	//fField->AddField(map_d1);
	//fField->AddField(map_d2);
	//fField->AddField(map_s1);
	//fField->AddField(map_s2);
	//fField->AddField(map_s3);
	//fField->AddField(map_s4);

	//Constant field:
	//PndConstField *fField=new PndConstField();
	//fField->SetField(0.,0.,20.); // values are in kG
	//fField->SetField(0.,0.,0.);
	//fField->SetFieldRegion(-50, 50,-50, 50, -150, 110); // values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)

	//fRun->SetField(fField);

	//fRun->SetStoreTraj(kTRUE);
	fRun->SetStoreTraj(kFALSE);

	fRun->Init();

	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();


	Bool_t kParameterMerged=kTRUE;
	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	output->open(parFile.Data(), "RECREATE");
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
	exit();

}


