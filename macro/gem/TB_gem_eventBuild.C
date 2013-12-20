// Macro created by Radoslaw Karabowicz
// This macro takes MC file and produces digis only

Int_t TB_gem_eventBuild  (Int_t nEvents = 1000, Int_t anaEv = 1000, Int_t nofC = 1000, Int_t cWid = 10, Int_t nStations=3, int verboseLevel = 0)
{ 
  if ( nStations != 3 && nStations != 4 ) {
    cout << "WRONG number of stations (" << nStations << "), only 3 or 4 allowed." << endl;
    return;
  }

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // Input file (MC events)
  TString baseName;
  baseName.Form("Gem_MvdStt_%dStations_DPM_n%d_orig",nStations,nEvents);

  baseName.Form("Gem_MvdStt_%dStations_DPM_n%d",nStations,nEvents);

  TString MCFile     = baseName + ".root";
  TString parFile    = baseName + "_par.root";
  TString trackFile  = baseName + Form("_CluHiTrack_a%d_c%dx%d.root",anaEv,nofC,cWid);
  TString rEventFile = baseName + Form("_RecoEvents_a%d_c%dx%d.root",anaEv,nofC,cWid);

  std::cout << "Output File: " << rEventFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(trackFile);
  fRun->SetOutputFile(rEventFile);
  
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

  // -----   Event Builder   -------------------------------------
  PndEventBuilderManager* eventBuilder = new PndEventBuilderManager("Panda Event Builder", 0);//verboseLevel);
  fRun->AddTask(eventBuilder);
  
  PndEventBuilderOnMCEvents*    eBOnMCEvents  = new PndEventBuilderOnMCEvents ("EBOnMCEvents",0);
  eventBuilder->AddEventBuilder(eBOnMCEvents);
  PndGemEventBuilderOnTracks*   gemEBOnTracks = new PndGemEventBuilderOnTracks("GemEBOnTracks",0);
  eventBuilder->AddEventBuilder(gemEBOnTracks);
  // ------------------------------------------------------------------------

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->RunEventReco(0,nEvents);


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << rEventFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;

}

