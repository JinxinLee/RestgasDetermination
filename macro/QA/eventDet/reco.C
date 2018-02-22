#include "../auxi.C"
int reco()
{

  Int_t iVerbose = 0;

	// Number of events to process
  Int_t nEvents = 0;  // if 0 all the events will be processed

  Bool_t timebased = kTRUE; // "Countinuous ReadOut"

  TString simFile = "sim.root";
  TString digiFile = "digi.root";
  TString outFile = "reco.root";

  TString parFile ="sim_params.root";
  TString digiParFile = "all.par";
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
    // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(digiFile);
  fRun->AddFriend(simFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetUseFairLinks(kTRUE);
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiParFile;

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  PndT0CandidateTask* T0CandidateTask = new PndT0CandidateTask();
  fRun->AddTask(T0CandidateTask);

  /*
  PndSolCorrTask* SolCorrTask = new PndSolCorrTask();
  SolCorrTask->SetOutBranchName("TOFCounter");
  SolCorrTask->AddInputBranch("SciTHit");
  ///SolCorrTask->AddInputBranch("SciTSortedHit");
  SolCorrTask->AddInputBranch("FtofHit");
  //SolCorrTask->AddInputBranch("MVDHitsPixel");
  //SolCorrTask->AddInputBranch("MVDHitsStrip");
  SolCorrTask->RunContinuous(timebased);
  fRun->AddTask(SolCorrTask);
  */


  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  cout << "fRun->Init()" << endl;

  fRun->Init();

  timer.Start();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


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
  cout << " Test passed" << endl;
  cout << " All ok " << endl;
  CloseGeoManager();
  return 0;
}
