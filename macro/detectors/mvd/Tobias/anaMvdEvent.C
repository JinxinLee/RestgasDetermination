{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file (MC events)
  TString MCFile = "Mvd_Pions_0GeV1_1.root";
  // Parameter file
  TString parFile = "Mvd_Pions_0GeV1_1Param.root";
  // Parameter output file
  TString parOutFile = parFile;
  // Number of events to process
  Int_t startEvent = 0;
  Int_t stopEvent = 300;
  // ----  Load libraries   -------------------------------------------------
//   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // ------------------------------------------------------------------------
  // Output file
    PndFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString RecoFile = creator.GetRecoFileName(false).c_str();
    TString RiemannFile = creator.GetTrackFindingFileName(false).c_str();
    TString outFile = "Dummy.root";
    
    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "RiemannFile: " << RiemannFile.Data() << std::endl;
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();

  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(RiemannFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   CbmParAsciiFileIo* parInput1 = new CbmParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
//  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
//  output->open(parOutFile.Data(),"RECREATE");

//  fRun->LoadGeometry();

  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD hit producer   --------------------------------------------

 
  PndMvdEventAnaTask* eventAna = new PndMvdEventAnaTask();
  eventAna->SetVerbose(iVerbose);
  fRun->AddTask(eventAna);

  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
//   PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(startEvent,stopEvent);
  
  TCanvas* c1 = new TCanvas();
  c1->Divide(3,4);
  c1->cd(1);
  eventAna->DrawTracksPerEvent();
  c1->cd(2);
  eventAna->DrawHitsPerTrack();
  c1->cd(3);
  eventAna->DrawEnergyPerHit();
  c1->cd(4);
  eventAna->DrawPointRes();
  eventAna->DrawPointResS("same");
  eventAna->DrawPointResD("same");
  eventAna->DrawPointResM("same");
  c1->cd(5);
  eventAna->DrawEnergyRes();
  c1->cd(6);
  eventAna->DrawDigisPerCluster();
  c1->cd(4);
  eventAna->DrawPointResStrip("same");
  c1->cd(5);
  eventAna->DrawEnergyResStrip("same");
  c1->cd(6);
  eventAna->DrawDigisPerClusterStrip("same");
  c1->cd(10);
  eventAna->DrawPtRes();
  c1->cd(11);
  eventAna->DrawPRes();
  c1->cd(7);
  eventAna->DrawRiemannRes();
  eventAna->DrawRiemannFakes("same");
  c1->cd(8);
  eventAna->DrawRiemannTracksPerTrack();
  eventAna->DrawRiemannTracksPerTrackAdd("same");
  
  // ------------------------------------------------------------------------

// rtdb->saveOutput();
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
