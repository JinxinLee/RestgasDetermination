{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits for EMC
  
	// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
	Int_t iVerbose = 0; // just forget about it, for the moment
	
	// Input file (MC events)
	TString inFile = "sim_emc_apd.root";
	
	
	// Number of events to process
	Int_t nEvents = 0;  // if 0 all the vents will be processed
	
	// Parameter file
	TString parFile = "simparams.root"; // at the moment you do not need it

	// Output file
	TString outFile = "hit_emc_apd.root";

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
        FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
        FairParRootFileIo* parInput1 = new FairParRootFileIo();
        parInput1->open(parFile.Data());
  
        rtdb->setFirstInput(parInput1);
 // ----------------------------------------------------------
  
	fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----   EMC APD hit producers   ---------------------------------

  PndEmcApdHitProducer* emcApdHitProd = new PndEmcApdHitProducer();
  fRun->AddTask(emcApdHitProd);
  
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
