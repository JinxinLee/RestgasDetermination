int runLumiPixel4aFilter(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const bool mergedHits=true, const bool SkipFilt=false, const bool XthFilt=true, const bool YphFilt=true, const bool BoxFilt=false, const double dX=0, const double dY=0)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  

  TString RecoFile = storePath;
  if(mergedHits) RecoFile +="/Lumi_recoMerged_";
  else  RecoFile +="/Lumi_reco_";
  RecoFile += startEvent;
  RecoFile += ".root";
  // TCand file
  TString CandFile = storePath+"/Lumi_TCand_";
  CandFile += startEvent;
  CandFile += ".root";
  // Track file
  TString TrkFile = storePath+"/Lumi_Track_";
  TrkFile += startEvent;
  TrkFile += ".root";

  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // // ----  Load libraries   -------------------------------------------------
  // gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // gSystem->Load("libSds");
  // gSystem->Load("libSdsReco");
  // gSystem->Load("libLmd");
  // gSystem->Load("libLmdReco");
  // gSystem->Load("libLmdTrk");
  // // ------------------------------------------------------------------------
  // Output file
    TString outFile = storePath+"/Lumi_TrackFiltered_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
    std::cout << "TrackInFile: " << TrkFile.Data()<< std::endl;
    std::cout << "TrackOutFile: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(CandFile);
  fRun->AddFriend(TrkFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);


  // ======================================
  // ======                       Track Filter                          ======
  // ======================================
  
  // -----  LMD collections names & importain parameters --------------------------------------------
  TString inHits = "LMDHitsPixel";
  if(mergedHits){ 
    inHits = "LMDHitsMerged";
  }
  PndLmdTrksFilterTask* lmdfilt = new PndLmdTrksFilterTask();

  if(SkipFilt){ 
    cout<<"Skip Kinematic Cuts!"<<endl;
    lmdfilt->SetSkipKinFilt(true);
  }
  if(BoxFilt){
    cout<<"Set BOX Cuts!"<<endl;
    lmdfilt->SetBOXFilt(true);
  }
  if(XthFilt){
    cout<<"Set X Cuts!"<<endl;
    lmdfilt->SetXThFilt(true);
    lmdfilt->SetDX(dX);
  }
  if(YphFilt){
    cout<<"Set Y Cuts!"<<endl;
    lmdfilt->SetYPhFilt(true);
    lmdfilt->SetDY(dY);
  }
  lmdfilt->SetVerbose(verboseLevel);
  fRun->AddTask(lmdfilt);

  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of Track Filter                          =====
  // ======================================

  
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


  return 0;
}
