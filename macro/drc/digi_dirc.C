

// construction site!

// quick hack for testing during the timestamp based tutorial 6.9.2011

// not yet working, Maria! 

// Macro created by Radoslaw Karabowicz
// This macro takes MC file and produces digis only

Int_t digi_dirc(Int_t nStations, Double_t momentum = 15., Int_t nEvents = 1000, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) 
    {
      cout << "WRONG number of stations, only 3 or 4 allowed." << endl;
      return;
    }

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_%dStations_%gGeV_n%d",nStations,momentum,nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  TString outFile = baseName + "_digi.root";

  std::cout << "Output File: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);
  

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/pid.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  // ------------------------------------------------------------------------

  // -----   GEM Digitizer   -----------------------------------------------
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

}

