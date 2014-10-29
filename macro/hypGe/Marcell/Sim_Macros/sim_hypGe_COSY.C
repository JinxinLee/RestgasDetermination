// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
void sim_hypGe_COSY(Double_t ProtonMomentum = 2.95,Int_t nEvents = 100, Int_t PDG_ext = 2212,Double_t TargetThickness = 5, Double_t TargetAngle = 0, Bool_t GausSmearedBeam = 0, Bool_t BremsStahlungActivated = 0, Bool_t WithBeamDump = 1, Double_t ClusterRadius = 30 , Double_t BeamDumpDist = 200, Double_t RunNo)
//void sim_hypGe_COSY(Int_t nEvents, Int_t PDG_ext ,Double_t TargetThickness , Double_t TargetAngle , Bool_t GausSmearedBeam , Bool_t BremsStahlungActivated , Bool_t WithBeamDump , Double_t ClusterRadius , Double_t BeamDumpDist )
//sim_hypGe_COSY.C(10	,2212	,5	,0	,0	,1	,1	,30	,200)
{
	Bool_t FirstBeamTime = false; // set this true to activate in beam stuff and beam dump of 2013 beam time at TOF area
	Int_t ParticlePDG = PDG_ext; //(-)2212 = (Anti)Proton
	
	// Load basic libraries
  // If it does not work,  please check the path of the libs and put it 	by hands
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libHypGe");
	gSystem->Load("libHyp");  
	
	//create Target geometry
	//gROOT->ProcessLine(".L ../Geometry/hypGeTargetAbsorptionTest.C");
	//TString TargetFilename = hypGeTargetAbsorptionTest(TargetThickness);
	
	FairRunSim *fRun = new FairRunSim();

	TStopwatch timer;
	timer.Start();
	gDebug=0;
  
	//Choose geometry
	TString outFile="$SIMDATADIR/COSY/";							// If no SIMDATADIR, data in subdir COSY
	TString GeoFile;
	
		//GeoFile = "hypGeGeoSingle.root";
		outFile += "hypGeCOSYBeamTime_";
		if (ParticlePDG < 0)
			outFile += "ANTI";
		outFile += "protons_mom";
		outFile += ProtonMomentum;
		outFile += "GeV_";
		
	
  // Set the number of events

	outFile += nEvents;
	outFile += "Evts";

	if (GausSmearedBeam)
		outFile += "_GausianSmearedBeam";
	if (!BremsStahlungActivated)
		outFile += "_withoutBremsStahlung";
	
	outFile += "_Target";
	outFile +=TargetThickness;
	outFile +="cm";
	outFile +=TargetAngle;
	outFile +="Deg";

	outFile += "_DetectorDist";
	outFile += ClusterRadius;
	outFile += "cm";
	
	if (WithBeamDump)
	{
		outFile += "_WithDumpDist";
		outFile += BeamDumpDist;
		outFile += "cm__Run_";
	}
	outFile += RunNo;
	TString SimparamsFile;
	SimparamsFile=outFile;
	outFile +=".root";
	SimparamsFile += "__Simparams.root";

  // set the MC version used
  // ------------------------
  
	fRun->SetName("TGeant4");
	
	fRun->SetOutputFile(outFile);
  // Set Material file Name
  //-----------------------
  cout << outFile << endl;
	fRun->SetMaterials("media_pnd_hypGe.geo");//("moded_media_pnd_hypGe.geo");//("../macro/hypGe/Marcell/MiscFiles/moded_media_pnd_hypGe.geo");
  
  // Create and add detectors
  //-------------------------

	FairModule *Cave= new PndCave("CAVE");
	Cave->SetGeometryFileName("pndcave.geo");
	fRun->AddModule(Cave); 



	PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);					// Germaniums + SiPm + Piezo
		TString nam = gSystem->Getenv("VMCWORKDIR");						
		HypGe->SetPathGeo(nam.Data());
		if (FirstBeamTime) 
			gROOT->ProcessLine(".L ../Geometry/hypGeGeoCOSYGermaniums.C");		// load macro to create detecter geo name --> setup of 2013 beam time
		else 
			gROOT->ProcessLine(".L ../Geometry/hypGeGeoBuilderSingle.C");		// load macro to create detecter geo name --> setup of 2014 beam time
		GeoFile = BuildOutputNameDetector( ClusterRadius);					//add ClusterRadius and BeamDumpDist
		HypGe->SetGeometryFileName(GeoFile.Data());					
		fRun->AddModule(HypGe); 
		
		// add target
	FairModule *Target = new PndHyp("Target",kFALSE);
		//Target->SetPathGeo(nam.Data());
		gROOT->ProcessLine(".L ../Geometry/hypGeGeoCOSYTarget.C"); 	// load macro to create target geo name
		TString TargetFile = BuildOutputNameTarget(TargetThickness,TargetAngle);		// create target geo
		Target->SetGeometryFileName(TargetFile.Data());
		fRun->AddModule(Target);
		
	
	
		//PndHyp *BeamDump = new PndHyp("BeamDump",kFALSE);
			//BeamDump->SetPathGeo(nam.Data());
			//gROOT->ProcessLine(".L ../Geometry/hypGeGeoCOSYBeamDumpTOF.C");		// load macro to create beam dump geo name
			//TString BeamDumpFile = "hypGeGeoCOSYBeamDumpTOF_dist200cm.root";
			//BeamDump->SetGeometryFileName(BeamDumpFile.Data());		
			//fRun->AddModule(BeamDump);


	// add stuff in beam (in front of target)
	
	FairModule *InBeamStuff = new PndHyp("InBeamStuff",kFALSE);
		//InBeamStuff->SetPathGeo(nam.Data());
		gROOT->ProcessLine(".L ../Geometry/hypGeGeoCOSYInBeamStuff.C"); 	// load macro to create target geo name
		TString InBeamStuffFile = BuildOutputNameInBeamStuff();		// create target geo
		InBeamStuff->SetGeometryFileName(InBeamStuffFile.Data());
		//fRun->AddModule(InBeamStuff);

	
  PndDrc *Drc = new PndDrc("DIRC", kFALSE);
  Drc->SetGeometryFileName(InBeamStuffFile.Data());
  if (FirstBeamTime) fRun->AddModule(Drc);
  
  // add beam dump
  PndGemDetector *BeamDump = new PndGemDetector("BeamDump", kFALSE);
  BeamDump->SetGeometryFileName("hypGeGeoCOSYBeamDumpTOF_dist200cm.root");
   if (FirstBeamTime) fRun->AddModule(BeamDump);
  
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);
  
  
	PndBoxGenerator* boxGen = new PndBoxGenerator(ParticlePDG, 1);
	boxGen->SetPRange(ProtonMomentum,ProtonMomentum); // GeV/c
	
	boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
	boxGen->SetThetaRange(0, 0.2); // Polar angle in lab system range [degree]
  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
	boxGen->SetXYZ(0., 0., -1000); // vertex coordinates [cm]
	
	if (GausSmearedBeam)
	{
		primGen->SetBeam(0, 0, 0.2, 0.2);
		primGen->SmearVertexXY(1);
	}
	primGen->AddGenerator(boxGen);
	


  
	fRun->SetStoreTraj(kTRUE); // to store particle trajectories 



  
				// seg error somewhere!!!!

	cout << "init now" << endl;
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
  gRandom->SetSeed();			// sets a random seed, added 08.06.14
	fRun->Run(nEvents);
  
	timer.Stop();
  
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  

