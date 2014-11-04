// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
// 16.09.13: steinen: added sec target (air version) to use PndVolGenerator without material
//void sim_Gamma(Int_t nEvents = 1000, Int_t WhichDetector = 36,Double_t Energy = 0.001,Bool_t addSecTar = 1)
void sim_Gamma(Int_t nEvents , Int_t WhichDetector ,Double_t Energy ,Int_t GenType, Bool_t addSecTar, Int_t nAllEvents, Int_t JobNr)
{

	// Load basic libraries
  // If it does not work,  please check the path of the libs and put it 	by hands
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	cout << "blabal" << endl;
	gSystem->Load("libHypGe");
	gSystem->Load("librazhyp");
	gSystem->Load("libHyp");  
	FairRunSim *fRun = new FairRunSim();

	TStopwatch timer;
	timer.Start();
	gDebug=0;

  

	//Choose geometry
	TString outFile="$SIMDATADIR/Gamma/";							// If no SIMDATADIR, same folder as the macro
	TString GeoFile;
	if (WhichDetector == 3)
	{
		GeoFile ="hypGe_GeoMarcell.root";
		outFile += "TripleV2_";
	}
	else if (WhichDetector == 31)
	{
		GeoFile = "hypGeGeoTripleCluster_V3.root";
		outFile += "TripleBall30_";
	}
	else if (WhichDetector == 32)
	{
		GeoFile = "hypGeGeoTripleCluster_Straight.root";
		outFile += "TripleStraight_";
	}
	else if (WhichDetector == 33)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset10.root";
		outFile += "TripleBall40Offset10_";
	}
	else if (WhichDetector == 34)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset20.root";
		outFile += "TripleBall40Offset20_";
	}
	else if (WhichDetector == 35)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root";
		outFile += "TripleBall40Offset10STT_";
	}
	else if (WhichDetector == 36)
	{
		GeoFile = "hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root";
		outFile += "TripleBall40Offset20STT_";
	}
	
	else if (WhichDetector == 2)
	{
		GeoFile = "hypGe_GeoMarcell_2er.root";
		outFile += "DoubleV2_";
	}
	else if (WhichDetector == 21)
	{
		GeoFile = "hypGeGeoDoubleCluster_V3.root";
		outFile += "DoubleV3_";
	}

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
	outFile += nAllEvents;
	outFile += "Evts_Gen";
	outFile += GenType;
	outFile +="_ST";
	outFile += int(addSecTar);
	outFile += "__";
	outFile += JobNr;
	
	
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
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave); 


	if (addSecTar)
	{  
		//acc sec. target
		PndHyp *Hyp= new PndHyp("HYP",kTRUE);
		Hyp->SetAbsorberVol("Absorber"); // absorber layer
		Hyp->SetSensorVol("Sensor");   // silicon sensor
		Hyp->SetGeoVersion("List");
		Hyp->SetListMat("HYPboron");
		Hyp->SetListMat("HYPaluminium");
		Hyp->SetListMat("Al97Mg3");
		Hyp->SetListMat("titanium");
		Hyp->SetListMat("HYPcarbon");
		Hyp->SetListMat("siliconinactive");
		switch (addSecTar)
		{
			case 1: Hyp->SetGeometryFileName("TargetSystem_Ti_TcT150um_filledCorners_addSens.root");break ;
			case 2: Hyp->SetGeometryFileName("TargetSystem_AbsWindow_filledCorners_addSens.root");break ;
		}
		fRun->AddModule(Hyp);
	}
	
	PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);
		TString nam = gSystem->Getenv("VMCWORKDIR");
		//HypGe->SetPathGeo(nam.Data());
		HypGe->SetGeometryFileName(GeoFile);
		fRun->AddModule(HypGe); 
  
  // Create and Set Event Generator
  //-------------------------------
  
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);

  // Box Generator: 
  //  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); 
  // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  bool boxgen=0;
  bool partgen=0;
  
  switch(GenType)
  {
	case 0 : boxgen = true;	break;
	case 1 : partgen = true;	break;
  }
  
  
  if(boxgen)
	{	
		PndBoxGenerator* boxGen = new PndBoxGenerator(22, 1);
		boxGen->SetXYZ(0., 0., -55.); // vertex coordinates [cm]
		if (!isCo60) 
			boxGen->SetPRange(Energy,Energy); // GeV/c
		else
			boxGen->SetPRange(0.001172,0.001172);	//set first line of Co60 in GeV/c
		boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
		boxGen->SetThetaRange(90., 180.); // Polar angle in lab system range [degree]
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
	}

	if (partgen)
	{
		//TString inFile= "$VMCWORKDIR/hypGe/hypGeTools/partGenFiles/hypBupV1T_Decay_gam_test.root"; 	//220 events
		//TString inFile= "$VMCWORKDIR/hypGe/hypGeTools/partGenFiles/testgam_1_6All.root";						// ~1300 events
		TString inFile= "$VMCWORKDIR/hypGe/hypGeTools/partGenFiles/testgam_1_99All.root";							//22517 events
		PndHypBupGenerator* partGen = new PndHypBupGenerator(inFile.Data());
	  partGen->GammaEmissPar(kTRUE);
	  partGen->SetPRange(Energy,Energy);
	  partGen->SetPhiRange(0,360);
	  partGen->SetThetaRange(90,180);
	  primGen->AddGenerator(partGen);
	}
  
	
  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
	
	

  
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
	gRandom->SetSeed();
	fRun->Run(nEvents);
  
	timer.Stop();
  
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  

