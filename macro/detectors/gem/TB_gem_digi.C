// Macro created by Radoslaw Karabowicz
// This macro takes MC file and produces digis only

Int_t TB_gem_digi   (Int_t nEvents = 1000, Int_t anaEv=1000, Int_t nofC=1000, Int_t cWid=10, Int_t nStations=3, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations (" << nStations << "), only 3 or 4 allowed." << endl;
    return;
  }

  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_MvdStt_%dStations_DPM_n%d",nStations,nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  TString outFile = baseName + Form("_digiSorted_a%d_c%dx%d.root",anaEv,nofC,cWid);

  std::cout << "Output File: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);
  
  fRun->SetEventMeanTime(50);

  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = sysFile+"/macro/params/gem_3Stations.digi.par";
  if ( nStations == 4 ) allDigiFile = sysFile+"/macro/params/gem_4Stations.digi.par";

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
  gemDigitize->SetRealisticResponse();
  gemDigitize->RunTimeBased();
  fRun->AddTask(gemDigitize);

  PndGemDigiSorterTask* gemSorter = new PndGemDigiSorterTask(nofC, cWid, "GEMDigi", "GEMDigiSorted", "PndGEM");
  fRun->AddTask(gemSorter);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,anaEv);//nEvents);


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

