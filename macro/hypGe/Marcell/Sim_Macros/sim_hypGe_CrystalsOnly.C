// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
int sim_hypGe_CrystalsOnly(Int_t nEvents, Int_t WhichDetector,Double_t Energy,Bool_t addSecTar)
{
	// Load basic libraries
  // If it does not work,  please check the path of the libs and put it 	by hands
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libHypGe");
	gSystem->Load("libHyp");  
	FairRunSim *fRun = new FairRunSim();

	TStopwatch timer;
	timer.Start();
	gDebug=0;
  
	//Choose geometry
	TString outFile="$SIMDATADIR/CrystalsOnly/";							// If no SIMDATADIR, same folder as the macro
	TString GeoFile;
	if (WhichDetector == 3)
	{
		GeoFile ="hypGe_GeoMarcellCrystalsOnly.root";
		outFile += "TripleV2CrystalsOnly_";
	}
	else if (WhichDetector == 31)
	{
		GeoFile = "hypGeGeoTripleCluster_V3CrystalsOnly.root";
		outFile += "TripleBall30CrystalsOnly_";
	}
	else if (WhichDetector == 32)
	{
		GeoFile = "hypGeGeoTripleCluster_StraightCrystalsOnly.root";
		outFile += "TripleStraightCrystalsOnly_";
	}
	else if (WhichDetector == 33)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset10CrystalsOnly.root";
		outFile += "TripleBall40Offset10CrystalsOnly_";
	}
	else if (WhichDetector == 34)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset20CrystalsOnly.root";
		outFile += "TripleBall40Offset20CrystalsOnly_";
	}
	else if (WhichDetector == 35)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset10_STTFittingCrystalsOnly.root";
		outFile += "TripleBall40Offset10STTCrystalsOnly_";
	}
	else if (WhichDetector == 36)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset20_STTFittingCrystalsOnly.root";
		outFile += "TripleBall40Offset20STTCrystalsOnly_";
	}
	else if (WhichDetector == 2)
	{
		GeoFile = "hypGe_GeoMarcell_2erCrystalsOnly.root";
		outFile += "DoubleV2CrystalsOnly_";
	}
	else if (WhichDetector == 21)
	{
		GeoFile = "hypGeGeoDoubleCluster_V3CrystalsOnly.root";
		outFile += "DoubleV3CrystalsOnly_";
	}
  // Set the number of events

	// Set the gamma energy

	// choose Co60
	bool isCo60 = false;
	//compose the name of the output file and the simparams file
	
	if (!isCo60)
	{
		outFile += Energy*1000;
		outFile += "MeV_";
	}
	else
	{
		outFile += "Co60_";
	}
	outFile += nEvents;
	outFile += "Evts";
	if (addSecTar)
	{
		outFile += "withSecTar";
	}
	
	TString SimparamsFile;
	SimparamsFile=outFile;
	outFile +=".root";
	SimparamsFile += "__Simparams.root";

	//TString inFile="/d/panda02/urqmd_smm/pbarC_3_GeV.root";
  //TString inFile="/u/asanchez/razhyp_gt12.dat";
  // set the MC version used
  // ------------------------
  
	fRun->SetName("TGeant4");

	fRun->SetOutputFile(outFile);
  // Set Material file Name
  //-----------------------
  
	fRun->SetMaterials("media_pnd_hypGe.geo");
  
  // Create and add detectors
  //-------------------------

	FairModule *Cave= new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave_vacuum.geo");
	fRun->AddModule(Cave); 

	PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);
		TString nam = gSystem->Getenv("VMCWORKDIR");
		HypGe->SetPathGeo(nam.Data());
		HypGe->SetGeometryFileName(GeoFile);
		fRun->AddModule(HypGe); 

	if (addSecTar)
	{  
		PndHyp *Hyp= new PndHyp("HYP",kFALSE);
		Hyp->SetAbsorverVol("Absorber"); // absorber layer
		Hyp->SetSensorVol("Sensor");   // silicon sensor
		Hyp->SetGeometryFileName("SekTarget_open_varAbs4_3Q_HYPbe_1mm.root");
		fRun->AddModule(Hyp); 
	}
  //    FairDetector *Drc = new FairDrc("DIRC", kTRUE);
  //    Drc->SetGeometryFileName("dirc.geo");
  //    fRun->AddModule(Drc);

  //  FairModule *Magnet= new FairMagnet("MAGNET");
  //   Magnet->SetGeometryFileName("magnet.geo");
  //   fRun->AddModule(Magnet);

  //  FairDetector *Emc = new FairEmc("EMC",kTRUE);
  //   Emc->SetGeometryFileName("emc_module123.dat"); 
  //   fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
  
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);

  // Box Generator: 
  //  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); 
  // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  
	PndBoxGenerator* boxGen = new PndBoxGenerator(22, 1);
	if (!isCo60) 
		boxGen->SetPRange(Energy,Energy); // GeV/c
	else
		boxGen->SetPRange(0.001172,0.001172);	//set first line of Co60 in GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
	boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
	boxGen->SetThetaRange(90., 180.); // Polar angle in lab system range [degree]
  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
	boxGen->SetXYZ(0., 0., -55.); // vertex coordinates [cm]
	primGen->AddGenerator(boxGen);

	if (isCo60)
	{ 
		PndBoxGenerator* boxGen2 = new PndBoxGenerator(22, 1); 
		boxGen2->SetPRange(0.001332,0.001332); // GeV/c
		//boxGen2->SetPtRange(1.,1.); // GeV/c
		boxGen2->SetPhiRange(0., 360.); // Azimuth angle range [degree]
		boxGen2->SetThetaRange(90., 180.); // Polar angle in lab system range [degree]
		//boxGen2->SetCosTheta(); // Set uniform ditribution in cos(theta)
		boxGen2->SetXYZ(0., 0., -55.); // vertex coordinates [cm]
		primGen->AddGenerator(boxGen2);
	}

	
/*
  PndBoxGenerator* boxGen2 = new PndBoxGenerator(-211, 1); 
  boxGen2->SetPRange(0.09,0.1); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen2->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen2->SetThetaRange(120., 180.); // Polar angle in lab system range [degree]
  boxGen2->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen2->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
				  */
  //primGen->AddGenerator(boxGen);  
  /*
  TChain *t =new TChain("data");
   t->Add(inFile);
   PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(t);
     primGen->SetTarget(-76.,0.);
     primGen->AddGenerator(AsciiGen);
  */
  
    /*FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
    //primGen->SetTarget(-150.,0.);
    primGen->AddGenerator(AsciiGen);
	*/
  
	fRun->SetStoreTraj(kTRUE); // to store particle trajectories 


  //magnetic field: no field when commented put
  //FairFieldConst *fMagField=new FairFieldConst();
  //fMagField->SetField(0.,0.,20.); // values are in kG
  //fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);
  // values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  //fRun->SetField(fMagField);
  
  


	fRun->Init();
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
	FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
	Bool_t kParameterMerged=kTRUE;
  
   
	FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
	output->open(SimparamsFile);
	rtdb->setOutput(output);
	rtdb->saveOutput();
	rtdb->print();


  // Transport nEvents
  // -----------------
  
	fRun->Run(nEvents);
  
	timer.Stop();
  
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  return 0;
}  
  

