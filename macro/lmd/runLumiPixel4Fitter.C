void runLumiPixel4Fitter(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const bool mergedHits=true)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";

  TString RecoFile = storePath;
  if(mergedHits) RecoFile +="/Lumi_recoMerged_";
  else  RecoFile +="/Lumi_reco_";
  RecoFile += startEvent;
  RecoFile += ".root";
  // TCand file
  TString CandFile = storePath+"/Lumi_TCand_";
  CandFile += startEvent;
  CandFile += ".root";
  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  gSystem->Load("libLmdReco");
  gSystem->Load("libLmdTrk");
  // ------------------------------------------------------------------------
  // Output file
    TString outFile = storePath+"/Lumi_Track_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
    std::cout << "TrackFile: " << outFile.Data()<< std::endl;
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(CandFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);


  // =========================================================================
  // ======                       Track Fitter                          ======
  // =========================================================================
  
  // -----  LMD collections names & importain parameters --------------------------------------------
  TString inHits = "LMDHitsPixel";
  if(mergedHits){
    inHits = "LMDHitsMerged";
  }
  PndLmdLinFitTask* lmdfit = new PndLmdLinFitTask("LMDTrackCand",inHits);
  TString tTCandBranchName, TString tRecoBranchName
  lmdfit->SetVerbose(verboseLevel);
  fRun->AddTask(lmdfit);

  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of TrackFinding                           =====
  // =========================================================================

  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  cout<<"fRun->Run(0, "<<nEvents<<") was done!"<<endl;
  // ------------------------------------------------------------------------

 rtdb->saveOutput();
 rtdb->print();
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
  // ------------------------------------------------------------------------


}
