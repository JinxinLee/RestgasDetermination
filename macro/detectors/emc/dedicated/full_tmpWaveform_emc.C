{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits for EMC
  
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0; // just forget about it, for the moment
	
	// Input file (MC events)
	TString inFile = "sim_emc.root";	
	
	// Number of events to process
	Int_t nEvents = 0;  // if 0 all the vents will be processed
	
	// Parameter file
	TString parFile = "simparams.root"; // at the moment you do not need it
  
	// Digitisation file (ascii)
	TString digiFile = "emc.par";
  
	// Output file
	TString outFile = "full_tmpWaveform_emc.root";
	
	// Loading libraries
	// If the macro gives error messages in loading libraries, please check the path of the libs and put it by hands
	
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();

	// -----   Timer   --------------------------------------------------------
	TStopwatch timer;
	timer.Start();
  // ------------------------------------------------------------------------
  
	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(inFile);
	fRun->SetOutputFile(outFile);
  
  // -----  Parameter database   --------------------------------------------
	TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
	emcDigiFile += "/macro/params/";
	emcDigiFile += digiFile;

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
        FairParRootFileIo* parInput1 = new FairParRootFileIo();
        parInput1->open(parFile.Data());
	
        FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
	parIo1->open(emcDigiFile.Data(),"in");
        
        rtdb->setFirstInput(parInput1);
	rtdb->setSecondInput(parIo1);
    // ----------------------------------------------------------
  
	fRun->LoadGeometry();
  // ------------------------------------------------------------------------


	PndGlobalEvtTask*  pndGlobalEvtTask = new PndGlobalEvtTask(); 
 	fRun->AddTask(pndGlobalEvtTask); 

  // -----   EMC hit producers   ---------------------------------
  // The file name should be the same of the geometry file which was used for the simulation

  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd); // hit production 
  
  PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
  //  fRun->AddTask(emcMakeDigi); // fast digitization

  PndEmcHitsToTmpWaveform* emcHitsToTmpWaveform= new PndEmcHitsToTmpWaveform(iVerbose);
  PndEmcTmpWaveformToDigi* emcTmpWaveformToDigi=new PndEmcTmpWaveformToDigi(iVerbose);
  fRun->AddTask(emcHitsToTmpWaveform);  // full digitization
  fRun->AddTask(emcTmpWaveformToDigi);  // full digitization
 
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);

  PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
  fRun->AddTask(emcMakeRecoHit);

  // -----   Intialise and run   --------------------------------------------
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  
}
