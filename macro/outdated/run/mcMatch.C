int mcMatch(Int_t nEvents = 0)
{
    //-----User Settings:-----------------------------------------------
  TString  SimEngine      ="TGeant3"; 
  TString  InputFile     ="sim_complete.root";
  TString  DigiFile		 ="digi_complete.root";
  TString  RecoFile      ="reco_complete.root";
  TString  ParFile       ="simparams.root";

  //------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile("tst.root");

  TFile* testFile;
  testFile = new TFile(DigiFile.Data());
  if (!testFile->IsZombie()){
	  fRun->AddFriend(DigiFile.Data());
  }
  testFile->Close();

  testFile = new TFile(RecoFile.Data());
  if (!testFile->IsZombie()){
	  fRun->AddFriend(RecoFile.Data());
  }
  testFile->Close();

   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  FairEventManager *fMan= new FairEventManager();

  PndMCTruthMatch* mcTruth = new PndMCTruthMatch("EmcCluster");
  fRun->AddTask(mcTruth);

  cout << "fRun->Init()" << endl;
    fRun->Init();
    TStopwatch timer;

    timer.Start();
    fRun->Run(0,nEvents);
    // ------------------------------------------------------------------------


    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
//    cout << "Output file is "    << outFile << endl;
 //   cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
  return 0;
}
