void runLumiPixel7QA(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutnewDesign", const int verboseLevel=5, const double Plab=15, const bool isClean=false)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  // Digi file
  TString RecoMergedFile = storePath+"/Lumi_recoMerged_";
  RecoMergedFile += startEvent;
  RecoMergedFile += ".root";
  TString RecoFile = storePath+"/Lumi_reco_";
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
  // Track file
  TString TrkFile = storePath+"/Lumi_Track_";
  TrkFile += startEvent;
  TrkFile += ".root";
  // Geane file
// Geane file
  TString GeaFile;
  if(isClean){
    GeaFile = storePath+"/Lumi_GeaneFiltered_";
    GeaFile += startEvent;
    GeaFile += ".root";
  }
  else{
    GeaFile = storePath+"/Lumi_Geane_";
    GeaFile += startEvent;
    GeaFile += ".root";
  }

  // Dummy file
  TString DumFile = storePath+"/Lumi_IGNORE_";
  DumFile += startEvent;
  DumFile += ".root";
  // Output file
  TString OutFile = storePath+"/Lumi_QA_";
  OutFile += startEvent;
  OutFile += ".root";
  // Par file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

 //  // ----  Load libraries   -------------------------------------------------
//   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
// //-lRooFit –lRooFitCore -lMinuit
//   gSystem->Load("libSds");
//   gSystem->Load("libSdsReco");
//   gSystem->Load("libLmd");
//   gSystem->Load("libLmdReco");
//   gSystem->Load("libLmdTrk");
//   gSystem->Load("libLmdTool");
//   // ------------------------------------------------------------------------
    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
  std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
  std::cout << "TrackFile: " << TrkFile.Data()<< std::endl;
  std::cout << "GeaneFile: " << GeaFile.Data()<< std::endl;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(GeaFile);
  fRun->AddFriend(TrkFile);
  fRun->AddFriend(CandFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->AddFriend(RecoMergedFile);
  fRun->SetOutputFile(DumFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  PndLmdQATask* lmdqa;
 if(isClean){
    lmdqa = new PndLmdQATask("LMDPoint","MCTrack","LMDPixelClusterCand","LMDPixelDigis","LMDHitsMerged","LMDTrackCand","LMDPndTrackFilt","LMDCleanTrack",OutFile);
 }
 else{
 lmdqa = new PndLmdQATask("LMDPoint","MCTrack","LMDPixelClusterCand","LMDPixelDigis","LMDHitsMerged","LMDTrackCand","LMDPndTrackFilt","GeaneTrackFinal",OutFile);
 }
  lmdqa->SetVerbose(verboseLevel);
  fRun->AddTask(lmdqa);
  fRun->SetGenerateRunInfo(kFALSE);
  //rtdb->setOutput(parInput1);
  //  rtdb->print();

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
