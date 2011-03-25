// Macro to reconstruct the MVD Background event data.
//
// Updated 23.3.2011
// Gianluigi Boca
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

// --------------------------- INPUT FILES
  // the following are the sim, digi and param files of the background events.
  TString MCFile = "../dpm/Mix_Generation.root";
  TString DigiFile = "../dpm/Mix_Generation_digi.root";
  TString parFile = "../dpm/Mix_Params.root";

//----------------------------------------------------------------------------
// Output file
  TString outFile = "Mix_Generation_reco.root";
  
//---------------------------------------------------------------------------  

  Int_t nEvents = 0;
  // ----  Load libraries
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  Bool_t kParameterMerged=kTRUE;

  // -----    Default MVD hit producer   --------------------------------------------

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  
  // =========================================================================





  rtdb->setOutput(parInput1);
  //  rtdb->print();
  
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
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
