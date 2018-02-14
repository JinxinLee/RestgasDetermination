// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
int sim_hypGe_COSY2014(Double_t ProtonMomentum = 2.78,Int_t nEvents = 100, Int_t PDG_ext = 2212, Bool_t GausSmearedBeam = 0, Bool_t BremsStahlungActivated = 0, Double_t RunNo = 0)
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
		outFile += "hypGeCOSYBeamTime2014_";
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
	
	
	outFile +="_";
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



	PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);					// Germaniums + SiPm + Piezo + everything else "active"
		TString nam = gSystem->Getenv("VMCWORKDIR");						
		HypGe->SetPathGeo(nam.Data());
		HypGe->SetGeometryFileName("hypGeGeoCOSY2014setupActives.root");
		fRun->AddModule(HypGe);
		cout << "active parts loaded" << endl;
		// add passive parts
	FairModule *Target = new PndHyp("Target",kFALSE);
		//Target->SetPathGeo(nam.Data());
	
		Target->SetGeometryFileName("hypGeGeoCOSY2014setupPassives.root");
		fRun->AddModule(Target);
		
	
  
	FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
	fRun->SetGenerator(primGen);
  
  
	PndBoxGenerator* boxGen = new PndBoxGenerator(ParticlePDG, 1);
	boxGen->SetPRange(ProtonMomentum,ProtonMomentum); // GeV/c
	
	boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
	boxGen->SetThetaRange(0, 0.2); // Polar angle in lab system range [degree]
  //boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
	boxGen->SetXYZ(0., 0., -50); // vertex coordinates [cm]
	
	if (GausSmearedBeam)
	{
		primGen->SetBeam(0, 0, 1, 1);
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
	cout << "Data written to file:\t" << outFile.Data() << endl;
  
  return 0;
}  
  

