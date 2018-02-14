// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
// 16.09.13: steinen: added sec target (air version) to use PndVolGenerator without material
//void sim_Gamma(Int_t nEvents = 1000, Int_t WhichDetector = 36,Double_t Energy = 0.001,Bool_t addSecTar = 1)


int sim_Gamma(	Int_t WhichDetector ,
				Double_t Energy,
				Int_t nAllEvents,
				Int_t nEvents,
				Int_t GenType,
				Int_t addSecTar,
				Int_t JobNr,
				Bool_t OmegaMode = 0,
				Double_t mu = 0,
				Double_t Q = 0)
{

	// Load basic libraries
  // If it does not work,  please check the path of the libs and put it 	by hands
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/hypGe/Marcell/SharedMacros/SharedMacroFunctions.C");
	gSystem->Load("libHypGe");
	gSystem->Load("librazhyp");
	gSystem->Load("libHyp");  
	FairRunSim *fRun = new FairRunSim();

	TStopwatch timer;
	timer.Start();
	gDebug=0;
 
	Double_t EnergyGeV = Energy/1000;
	//Choose geometry
	TString outFile="$SIMDATADIR/Gamma/";							// If no SIMDATADIR, same folder as the macro
	TString GeoFile=GeoFileChooser(WhichDetector);
	TString outFolderPureName = ComposeOutputFoldername("Sim" ,WhichDetector,Energy,nAllEvents, nEvents, GenType,addSecTar,OmegaMode,mu,Q);
	TString outFilePureName = ComposeOutputFilename("Sim" ,WhichDetector,Energy,nAllEvents, nEvents, GenType,addSecTar, JobNr,OmegaMode,mu,Q);
	
	outFile += outFolderPureName;						// subfolder until here
	
	char CommandBuffer[400];
	sprintf(CommandBuffer,".!mkdir -p %s",outFile.Data());
	cout << "Processing " << CommandBuffer<< endl;	
	gROOT->ProcessLine(CommandBuffer);		// create subfolder for the output	
	
	outFile += "/";
	outFile += outFilePureName;
	
	TString SimparamsFile;
	SimparamsFile=outFile;
	outFile +=".root";
	SimparamsFile += "__Simparams.root";

	cout << outFile << endl;

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
		//add sec. target
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
		Hyp->SetListMat("acrylicGlass");
		Hyp->SetMatbud(true);			//activate hits in other parts than silicons or absorber
		switch (addSecTar)
		{
			case 1: Hyp->SetGeometryFileName("TargetSystem_WindowTi_filledCorners_TcT1500um_longBP.root");break ;
			case 2: Hyp->SetGeometryFileName("TargetSystem_AbsWindow_filledCorners_addSens.root");break ;
			case 3: Hyp->SetGeometryFileName("TargetSystem_WindowAbsB_insideAbsB_primTarget.root");break;
			case 12: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_iron.root");break;
			case 13: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v3_iron.root");break;
			case 22: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_titanium.root");break;
			case 23: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v3_titanium.root");break;
			case 32: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_tantalum.root");break;
			case 33: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_tantalum.root");break;
			case 42: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_lead.root");break;
			case 43: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v3_lead.root");break;
			case 52: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v2_gold.root");break;
			case 53: Hyp->SetGeometryFileName("hypTargetSystem_XiAtoms_v3_gold.root");break;
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
  	bool isCo60 = false;
		PndBoxGenerator* boxGen = new PndBoxGenerator(22, 1);
		boxGen->SetXYZ(0., 0., -55.); // vertex coordinates [cm]
		if (!isCo60) 
			boxGen->SetPRange(EnergyGeV,EnergyGeV); // GeV/c
		else
			boxGen->SetPRange(0.001172,0.001172);	//set first line of Co60 in GeV/c
		boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
		boxGen->SetThetaRange(90., 180.); // Polar angle in lab system range [degree]
		boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
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
		//TString inFile= "$VMCWORKDIR/hypGe/hypGeTools/partGenFiles/testgam_1_99All.root";							//22517 events
		//TString inFile= "/home/steinen/work/GeneratorInput/XiAtoms/CombinedHypfile_Geo3_simruns_Events5000000.root";							//9686 events
		TString inFile= "/home/steinen/work/GeneratorInput/XiAtoms/CombinedHypfile_Geo_3_simruns_Events10000000.root";							//52867 events
		TString inFile= "/data/work/kpha1/steinen/XiAtoms/data/Hypfiles/CombinedHypfile_Geo_";
		inFile+=addSecTar;
		inFile+="_simruns_Events20000000";
		if ((addSecTar % 10)==3)
			inFile+="_Omega";
		inFile+=".root";


//		if (addSecTar ==4 || addSecTar == 7)
//			TString inFile= "/home/steinen/work/GeneratorInput/XiAtoms/CombinedHypfile_Geo_3_simruns_Events10000000.root";							//52867 events
//		else if (addSecTar ==6 || addSecTar == 8)
//			TString inFile= "/home/steinen/work/GeneratorInput/XiAtoms/CombinedHypfile_Geo_5_simruns_Events10000000.root";
//			else
//			{
//				cout << "no valid input file for partgen!!!!";
//				return -1;
//			}
		PndHypBupGenerator* partGen = new PndHypBupGenerator(inFile.Data());
	  partGen->GammaEmissPar(kTRUE);
	  partGen->SetPRange(EnergyGeV,EnergyGeV);
	  partGen->SetPhiRange(0,360);
	  partGen->SetThetaRange(90,180);			// only 2 pi of solid angle, since there are no germaniums in forward direction. This has to be taken into account in analysis of efficiency!!!
	  primGen->AddGenerator(partGen);
	}
  
	
  //boxGen->SetCosTheta(); // Set uniform distribution in cos(theta)
	
	

  
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
  
  return 0;
}  
  

