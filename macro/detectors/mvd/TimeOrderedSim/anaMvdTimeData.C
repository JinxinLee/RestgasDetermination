// Macro to reconstruct the MVD data in pandaroot
// Updated 30.11.2009
// Ralf Kliemt
anaMvdTimeData(Int_t nEvents=1000)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  TString MCFile = "Mvd_Sim.root";
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libMCMatchExamples");
  // ------------------------------------------------------------------------
  // Output file
  PndFileNameCreator creator(MCFile.Data());
  TString DigiFile = creator.GetDigiFileName();
  TString RecoFile = creator.GetRecoFileName();
  TString TrackFFile = creator.GetTrackFindingFileName();
  TString parFile = creator.GetParFileName();
  TString outFile = "Test.root";
  
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
  std::cout << "TrackFindingFile: " << TrackFFile.Data()<< std::endl;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->AddFriend(RecoFile);
 // fRun->AddFriend(TrackFFile);
  fRun->SetOutputFile(outFile);
  //fRun->RunWithTimeStamps();
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  // -----    Default MVD hit producer   --------------------------------------------

  PndMCTestDataCrawler* crawler = new PndMCTestDataCrawler();
  crawler->SetStartBranch("MVDHitsPixel");
  crawler->SetStopBranch("EventHeader.");
  crawler->SetVerbose(2);
  crawler->SetHistoRange(-20, 40, 601);
  fRun->AddTask(crawler);
  
  // -----    OR separate tasks   ---------------------------------------------------
  //Double_t chargecut = 1.e5;
  //PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask();
  //mvdmccls->SetVerbose(iVerbose);
  //fRun->AddTask(mvdmccls);
//  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask();
//  mvdClusterizer->SetVerbose(iVerbose);
//  fRun->AddTask(mvdClusterizer);

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
