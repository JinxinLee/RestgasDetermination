{
//Digi for the small testChamber
//Maxence Vandenbroucke 11/10/2010 from runDigi.C

  // ----  Load libraries   -------------------------------------------------
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  // ------------------------------------------------------------------------

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;


  //SET NUMBER OF EVENTS
  // ------------------------------------------------------------------------
  Int_t nEvents=0;


  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  // Set INPUT DIRECTORY (MC files) and JOBNAME
  // ------------------------------------------------------------------------
  TString inDir="TEST";
  TString jobname="FOPI1";

  inDir=(basedir+"/")+inDir;
  TString inFile=(inDir+"/")+jobname;
  inFile+=".mc.root";
 

  // make new subdir
  // TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
  //   TString cmd="mkdir ";
  //   cmd+=jobDir; 
  //   if(gSystem->Exec(cmd)){
  //     std::cout<<"Could not create Job-Directory "<<jobDir
  // 	     <<". Aborting."<<std::endl;
  //     return;
  //   }


  TString outFile = inFile;
  outFile.ReplaceAll(".mc.root", ".raw.root");
  TString paramIn = inFile;
  paramIn.ReplaceAll(".mc.root",".param.root");
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

  // ------ QAplots
  QAPlotCollection* qa=new QAPlotCollection("TpcDigiQAPlots");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/TestBench/tpc.TBtestChamber.par";
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

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
    
    // -----    Digi Sequence  --------------------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
  tpcClusterizer->SetPersistence();
  //ONLY USE THIS WHEN USING ALICE SETTINGS WITH GEANT3
  tpcClusterizer->SetMereChargeConversion();  
  tpcClusterizer->SetFirstPoti(15.13); //ArgonCO2 mixture in eV
  fRun->AddTask(tpcClusterizer);
  
  /**   use Alice Style MC    
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
  tpcDrifter->SetPersistence();
  tpcDrifter->SetDistort(false);
  tpcDrifter->SetQAPlotCol(qa);
  //fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
  tpcGem->SetPersistence();
  //fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  //tpcPadResponse->SetQAPlotCol(qa);
  //fRun->AddTask(tpcPadResponse);

  //PndTpcEvtMixTask* evtmixer = new PndTpcEvtMixTask();
  //  evtmixer->SetBkgFileName("bkg2.raw.root");
  //  evtmixer->SetNBkgEvts(500);
  //  evtmixer->SetEvtRate(1E7);
  //fRun->AddTask(evtmixer);

//  PndTpcTCcrossTalkTask * ct= new PndTpcTCcrossTalkTask();
//  ct->SetPersistence();
//  fRun->AddTask(ct);


  PndTpcElectronicsTask* tpcElec = new PndTpcElectronicsTask();
  tpcElec->SetPersistence();
  //tpcElec->SetQAPlotCol(qa);
  //fRun->AddTask(tpcElec);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

fRun->Run(0,nEvents); // process all events from input file
  // ------------------------------------------------------------------------

  
  //tpcDrifter->WriteHistograms();
  //tpcPadResponse->WriteHistograms();
  //tpcElec->WriteHistograms();

  FairRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
  FairRootManager::Instance()->GetOutFile()->cd("QAPlots");
  qa->Write();

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
