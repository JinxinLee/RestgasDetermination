// Macro to reconstruct the MVD data in pandaroot
// Updated 30.11.2009
// Ralf Kliemt
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  TString MCFile = "Mvd_Test.root";
  TString parFile = "Mvd_Params.root";
  Int_t nEvents = 10;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  // ------------------------------------------------------------------------
  // Output file
  PndMvdFileNameCreator creator(MCFile.Data());
  TString DigiFile = creator.GetDigiFileName(false).c_str();
  TString outFile = creator.GetRecoFileName(false).c_str();
  
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << outFile.Data()<< std::endl;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  // -----    Default MVD hit producer   --------------------------------------------
  Double_t chargecut = 5000., pixelrad=1.8; // one day this will move to the parameter db.
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask(pixelrad,chargecut,MCFile);
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  
  // -----    OR separate tasks   ---------------------------------------------------
  //Double_t chargecut = 1.e5;
  //PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut,creator.GetSimFileName(true));
  //mvdmccls->SetVerbose(iVerbose);
  //fRun->AddTask(mvdmccls);
  //PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask(1.8,76,84, creator.GetSimFileName(true));//, slx, sly, sthreshold, snoise);
  //mvdClusterizer->SetVerbose(iVerbose);
  //fRun->AddTask(mvdClusterizer);

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
