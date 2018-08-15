{
  // This script run simple digitization.
  // I.e. just convert EmcHit array to EmcDigi array
  // with enery above certain treshold
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "hit_emc.root";
  
  // Simulation file, to obtaine geometry
  TString simFile = "sim_emc.root";
  
  // Number of events to process
  Int_t nEvents = 0;  // if 0 all the vents will be processed
  	
  // Digitisation file (ascii)
	TString digiFile = "emc.par";
  
  // Parameter file
  TString parFile = "simparams.root"; // at the moment you do not need it
  
  // Output file
  TString outFile = "digi_emc.root";
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, please check the path of the libs and put it by hands
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------
  
  // -----   Reconstruction run   ------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile);
  fRun->AddFriend(inFile);
  fRun->SetOutputFile(outFile);
  
  // -----  Parameter database   --------------------------
	TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
	emcDigiFile += "/macro/params/";
	emcDigiFile += digiFile;

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
	parIo1->open(emcDigiFile.Data(),"in");
	rtdb->setFirstInput(parIo1);
	
	// ----------------------------------------------------
	fRun->LoadGeometry();
   // ----------------------------------------------------

   PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
	
	fRun->AddTask(emcMakeDigi);
  
  // -----   Intialise and run   -----------------------
  cout << "fRun->Init()" << endl;
  fRun->Init();
  fRun->Run(0,nEvents);
  // ----------------------------------------------------


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
