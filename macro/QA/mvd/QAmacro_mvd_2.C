int QAmacro_mvd_2()
{
	  cout << "QA module for the MVD Digitization and Hit Reconstruction." << endl;
  TStopwatch timer;
  timer.Start();
  Int_t iVerbose = 0;

  TString inFile = "mvdqasim.root";
  TString parFile = "mvdqapar.root";
  TString outFile = "mvdqarec.root";

  Int_t nEvents = 100;

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);

  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  fRun->Init();
  fRun->Run(0, nEvents);

//  rtdb->saveOutput();
//  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  cout << " DigiReco passed" << endl;
  cout << " All ok " << endl;

  return 0;
}
