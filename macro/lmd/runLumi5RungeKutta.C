void runLumi5RungeKutta(const int nEvents=10, const double mom=15, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, const bool mergedHits=false)
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
  TString RecoFile = storePath;
  if(mergedHits)RecoFile+="/Lumi_recoMerged_";
  else RecoFile+="/Lumi_reco_";
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

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libSds");
  gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  gSystem->Load("libLmdReco");
  gSystem->Load("libLmdTrk");
  // ------------------------------------------------------------------------


  // Output file
    TString outFile = storePath+"/Lumi_Geane_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TCandFile: " << CandFile.Data()<< std::endl;
    std::cout << "TrackFile: " << TrkFile.Data()<< std::endl;
    std::cout << "GeaneFile: " << outFile.Data()<< std::endl;
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   ------ --------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(CandFile);
  fRun->AddFriend(TrkFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);


  // ======================================================================
  // ======               Back-Propagating  with Runge-Kutta algorithm                                                              ======
  // ======================================================================
  
  // -----    SDS hit producer   --------------------------------------------

 // PndEmcMapper *emcMap = PndEmcMapper::Instance(6);
  Double_t fpBeam = mom;
  TVector3 IP(0,0,0);
  // PndLmdGeaneTask* lmdgeane = new PndLmdGeaneTask(fpBeam, IP);
  // lmdgeane->SetVerbose(verboseLevel);
  PndLmdBPRungeKuttaTask* lmdbp = new PndLmdBPRungeKuttaTask(fpBeam, IP);
  lmdbp->SetVerbose(verboseLevel);
  fRun->AddTask(lmdbp);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                        End of Back-Propagating                                                                                            =====
  // ======================================================================

   // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  //  // PndEmcMapper *emcMap = PndEmcMapper::Instance(6);
  //  PndEmcMapper *emcMap = PndEmcMapper::Instance();
  // //Geane->SetField(fRun->GetField());
  fRun->Run(0,nEvents);
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
