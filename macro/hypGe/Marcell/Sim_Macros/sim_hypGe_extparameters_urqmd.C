// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
void sim_hypGe_extparameters_urqmd(Int_t nEvents, Int_t WhichDetector,Int_t iurqmdFile,Int_t StartEvent,Int_t SecTarget){
	// Load basic libraries
  // If it does not work,  please check the path of the libs and put it 	by hands
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libHypGe");  
  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  gROOT->LoadMacro("$VMCWORKDIR/macro/hypGe/Marcell/SharedMacros/SharedMacroFunctions.C");
  FairRunSim *fRun = new FairRunSim();
  gRandom->SetSeed();
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  TString urqmdFile = "pbarC_";
	urqmdFile += iurqmdFile;
  TString inFile="/home/steinen/work/urqmd/pbarC_3GeV/";
	inFile += urqmdFile;
	inFile += ".root";
	//Choose geometry
	TString outFile="$SIMDATADIR/Neutron/Geo";							// If no SIMDATADIR, same folder as the macro
	TString GeoFile= GeoFileChooser(WhichDetector);
	outFile += WhichDetector;
	//compose the name of the output file and the simparams file
		outFile += "urqmd_";
		outFile += urqmdFile;
		outFile +="_";
		outFile += nEvents;
		outFile += "Evts_StartEvent_";
		outFile += StartEvent;
		if(SecTarget)
			outFile += "_SecTar";

	
	TString SimparamsFile;
	SimparamsFile=outFile;
	outFile +=".root";
	SimparamsFile += "__Simparams.root";

	//TString inFile="/d/panda02/urqmd_smm/pbarC_3_GeV.root";
  //TString inFile="/u/asanchez/razhyp_gt12.dat";

	//TString inFile="/data/work/kpha1/bleser/GiBUU-data2b/jobs/carbon0003/Conversion_Class_apr13_carbon0003_allParticles/carbon0003_allParticles_1_32.root"; //GiBUU file 1.5*10⁷ events
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
	
	if(SecTarget)
	{
		//acc sec. target
		PndHyp *Hyp= new PndHyp("HYP",kFALSE);
			Hyp->SetAbsorberVol("Absorber"); // absorber layer
			Hyp->SetSensorVol("Sensor");   // silicon sensor
			Hyp->SetGeometryFileName("../macro/hyp/Sebastian/SekTarget_open_varAbs4Si5_3Q_HYPbe_1mm_MVD.root");
			fRun->AddModule(Hyp); 
		
		//add MVD outer barrel
		FairDetector *Mvd = new PndMvdDetector("MVD", kFALSE);
			Mvd->SetGeometryFileName("../macro/hyp/Sebastian/Mvd-2.2_Simplified_onlyStrip5_z-verschoben550.root");  
			fRun->AddModule(Mvd);
	}
  PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);
		TString nam = gSystem->Getenv("VMCWORKDIR");
		HypGe->SetPathGeo(nam.Data());
		HypGe->SetGeometryFileName(GeoFile);
		fRun->AddModule(HypGe); 

  //  FairDetector *Hyp= new FairHyp("HYP",kTRUE);
  //    Hyp->SetGeometryFileName("HypST_prueba24pipe.geo");
  //    fRun->AddModule(Hyp); 

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
  
	PndUrqmdSmmGenerator* UrqmdGen = new PndUrqmdSmmGenerator(inFile,StartEvent);
		primGen->SetTarget(-55.,0.);
		primGen->AddGenerator(UrqmdGen);
	

  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 


  //magnetic field: no field when commented put
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0.,0.,10.); // values are in kG
  fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);
   //values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  fRun->SetField(fMagField);
  
  


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
  
}  
  

