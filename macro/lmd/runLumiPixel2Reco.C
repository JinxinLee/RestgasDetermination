int runLumiPixel2Reco(const int nEvents=10, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, bool misalign=false)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  TString DigiFile = storePath+"/Lumi_digi_";//"/Lumi_digi_noise_";//"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
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
    TString outFile = storePath+"/Lumi_reco_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << outFile.Data()<< std::endl;
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
  // fRun->SetInputFile(DigiFile);
  // fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);

  // alignment parameter file
  //  TString alignparFile = "lumi.align.par";//perfect! OLD
  //  TString alignparFile = "lumi.misalign.par";//misalignment!  OLD
  //  TString alignparFile = "lumi.align_pixel_ideal.par";//perfect
  TString alignparFile;
  if(misalign){ 
    alignparFile = storePath+"/lumi.misalign_pixel.par";//misalign
  }
  else{
    alignparFile = "LMD_alignment/lumi.align_pixel_ideal.par";//perfect
  }
 
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(alignparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  /*Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutFile);
  rtdb->setOutput(output);
*/  
// fRun->LoadGeometry();

  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    LMD hit producer   --------------------------------------------
 
  Double_t chargecut = 1.e5;
  PndLmdPixelClusterTask* lmdmccls = new PndLmdPixelClusterTask();
  //  PndLmdIdealClusterTask* lmdmccls = new PndLmdIdealClusterTask();
  lmdmccls->SetVerbose(verboseLevel);
  //  lmdmccls->SetMSflag(useMSerr);//switch on\off error calculation due to multiple scaterring
  lmdmccls->SetAlignFlag(misalign);
   if(misalign) lmdmccls->SetMtxPath(storePath);
  // 
  //  lmdmccls->SetMtxPath("../../../pandaroot/input/");
  fRun->AddTask(lmdmccls);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

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


  return 0;
}
