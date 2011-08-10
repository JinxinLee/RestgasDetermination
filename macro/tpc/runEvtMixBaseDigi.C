void 
runEvtMixBaseDigi( TString inFile, 
	       TString outDir,
	       TString jobname,
	       Int_t startEvent, // event number where we start processing
	       Int_t nEvents,
	       bool doDistort,
	       double scale ){
  // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  // ------------------------------------------------------------------------
  //SET NUMBER OF EVENTS
  // ------------------------------------------------------------------------

  

  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  // Set INPUT DIRECTORY (MC files) and JOBNAME
  // ------------------------------------------------------------------------
  //TString inDir="/nfs/nas/data/panda/tpc/SIM/evtmix/";
  // TString jobname="phys_JPsiPiPi.skim";
  //TString jobname="DPM";
  TString parfile="/nfs/nas/data/panda/tpc/SIM/evtmix/all.par";
  //Int_t nEvents=1000;

  //inDir=(basedir+"/")+inDir;
  //TString inFile=(inDir+"/")+jobname;
  //inFile+=".mc.root";

  //Bool_t doDistort=true;
  //double scale=2; // distortion scale
  TString outFile = (outDir)+jobname;
  outFile+=".mc.root";
  if(!doDistort)scale=0;
  TString batchID(".DD");batchID+=scale;batchID+=".";batchID+=startEvent;batchID+=".raw.root";
  outFile.ReplaceAll(".mc.root", batchID);
  TString paramIn = inFile;
  paramIn.ReplaceAll(".skim.mc.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.root");
  

  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;
  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;
 
  

  // In general, the following parts need not be touched
  // ========================================================================
  


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile;// = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += parfile;
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();


  PndGeoHandling* geoH = PndGeoHandling::Instance();
 
  // ------------------------------------------------------------------------
     
  // -----    Digi Sequence  --------------------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
  //tpcClusterizer->SetPersistence();
  //ONLY USE THIS WHEN USING ALICE SETTINGS WITH GEANT3
  tpcClusterizer->SetMereChargeConversion();  
  fRun->AddTask(tpcClusterizer);
  
  /**   Nots for the use of Alice Style MC    
   				make one hit per collision with atom
				use other straggling
		WARNING:	
   	    1. geant3 has to be used!
   	    2. LOSS = 5 has to be set!
	    3. DCUTE und DCUTM should be set to 10 keV.	
	    4. For Digitaization: PndTpcClusterizerTask
	    		tpcClusterizer->SetMereChargeConversion() has to be set!
	    5. if you do not use this option make sure 2., 4. are not set!
		:-(	
	    6. SetMaxNStep should be set to a high value
   */ 
 
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
  //tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(doDistort, scale);
  tpcDrifter->SetDeviationFile("tpc/DevMap_Efield_march09_official_B_Maps.dat");
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
  //tpcGem->SetPersistence();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  //tpcPadResponse->SetPersistence();
  fRun->AddTask(tpcPadResponse);

  PndTpcEvtTimeGenTask* evttimegen = new PndTpcEvtTimeGenTask();
  evttimegen->SetPersistence();
  evttimegen->SetEvtRate(1E7);
  evttimegen->SetT0(-evttimegen->MeanEvtSpacing()*0.5*nEvents);
  fRun->AddTask(evttimegen);


  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
 tpcElec->SetPSATimeCalib(2.8);
 //tpcElec->SetSamplePersistence();
  fRun->AddTask(tpcElec);
  

 // -----   MDV digi producers   --------------------------------- 
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(0);
  fRun->AddTask(mvddigi);

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(0);
  fRun->AddTask(mvdmccls); 


  // -----   GEM hit producers   ---------------------------------
  Int_t verboseLevel = 0;
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);

  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder", verboseLevel);
  fRun->AddTask(gemFindHits);



  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(startEvent,startEvent+nEvents); // process all events from input file
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------

  
  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
