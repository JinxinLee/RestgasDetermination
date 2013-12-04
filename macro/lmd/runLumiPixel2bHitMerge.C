void runLumiPixel2bHitMerge(const int nEvents=10, const int startEvent=0, TString storePath="tmpOutput", const int verboseLevel=0, bool misalign=false)
{
  // ========================================================================
  // Input file (Reco events)
  TString RecoFile = storePath+"/Lumi_reco_";
  RecoFile += startEvent;
  RecoFile += ".root";
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
    TString outFile = storePath+"/Lumi_recoMerged_";
    outFile += startEvent;
    outFile += ".root";
    
    std::cout << "RecoFile  : " << RecoFile.Data()<< std::endl;
    std::cout << "RecoMergedFile: " << outFile.Data()<< std::endl;
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
  fRun->SetInputFile(RecoFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
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
  
  // -----    MVD hit producer   --------------------------------------------
 
  PndLmdHitMergeTask* lmdmccls = new PndLmdHitMergeTask();
  lmdmccls->SetVerbose(verboseLevel);
  lmdmccls->SetAlignFlag(misalign);
  if(misalign) lmdmccls->SetMtxPath(storePath);
  fRun->AddTask(lmdmccls);

 // PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(chargecut, MCFile);//, slx, sly, sthreshold, snoise);
 // mvdClusterizer->SetVerbose(verboseLevel);
 // fRun->AddTask(mvdClusterizer);

//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
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
