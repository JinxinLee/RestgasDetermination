// QA macro for lmd
// created 23/07/2015 by A.Karavdina

void QA_LMD(const int nEvents=2000, const int startEvent=0, TString storePath="", const int verboseLevel=0)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  
  // Reco file
  TString GeaFile = storePath+"Lumi_Geane_";
  GeaFile += startEvent;
  GeaFile += ".root";
  

  // Dummy file
  TString DumFile = storePath+"Lumi_IGNORE_";
  DumFile += startEvent;
  DumFile += ".root";
  // Output file
  TString OutFile = storePath+"Lumi_QA_";
  OutFile += startEvent;
  OutFile += ".root";
  // Par file
  TString parFile = storePath+"Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << GeaFile.Data()<< std::endl;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(GeaFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->SetOutputFile(DumFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  PndLmdQATask* lmdqa = new PndLmdQATask("LMDPoint","MCTrack","LMDPixelClusterCand","LMDPixelDigis","LMDHitsMerged","LMDTrackCand","LMDPndTrackFilt","GeaneTrackFinal",OutFile);
  lmdqa->SetVerbose(verboseLevel);
  fRun->AddTask(lmdqa);
  fRun->SetGenerateRunInfo(kFALSE);

  fRun->Init();
  fRun->Run(0,nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
