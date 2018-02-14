{
  // Loads a filewith hits and makes digitization for EMC
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 3; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "sim_emc.root";
  
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
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetEventMeanTime(50);
  
  // -----  Parameter database   --------------------------
	TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
	emcDigiFile += "/macro/params/";
	emcDigiFile += digiFile;

        TString parFile = "simparams.root"; 

	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
	parIo1->open(emcDigiFile.Data(),"in");
        FairParRootFileIo* parInput1 = new FairParRootFileIo();
        parInput1->open(parFile.Data());
	
	
	rtdb->setFirstInput(parIo1);
        rtdb->setSecondInput(parInput1);
  
  // ----------------------------------------------------
  
	PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
	PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
	emcWaveformToDigi->RunTimeBased();
	
	fRun->AddTask(emcHitsToWaveform);
	fRun->AddTask(emcWaveformToDigi);
	
	PndEmcDigiSorterTask* emcSortedDigi = new PndEmcDigiSorterTask(300, 1, "EmcDigi", "EmcSortedDigi", "Emc");
	fRun->AddTask(emcSortedDigi);
  
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
