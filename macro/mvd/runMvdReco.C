{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Input file (MC events)
  TString MCFile = "Mvd_Test.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  // Parameter output file
  // TString parOutFile = "MvdParams.root";
  // Number of events to process
  Int_t nEvents = 10;
  // ----  Load libraries   -------------------------------------------------
//   gROOT->Macro("Libs.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // ------------------------------------------------------------------------
  // Output file
    PndMvdFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString outFile = creator.GetRecoFileName(false).c_str();
    
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
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   CbmParAsciiFileIo* parInput1 = new CbmParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  /*Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parOutFile);
  rtdb->setOutput(output);
*/  
// fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD hit producer   --------------------------------------------
 
  Double_t noise = 1000.; /// put such stuff inside the parameter lists
  Double_t chargecut = 3. * noise;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut,creator.GetSimFileName(true));
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8,76,84, creator.GetSimFileName(true));//, slx, sly, sthreshold, snoise);
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);

//   CbmParRootFileIo* output=new CbmParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
//   PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
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


}
