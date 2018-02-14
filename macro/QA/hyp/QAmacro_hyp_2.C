int QAmacro_hyp_2()
{
	  cout << "QA module for the MVD Digitization and Hit Reconstruction." << endl;

  TStopwatch timer;
  timer.Start();
  Int_t iVerbose = 0;

  TString inFile = "hypqasim.root";
  TString parFile = "hypqapar.root";
  TString outFile = "hypqarec.root";

  Int_t nEvents = 100;

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data());

 
  rtdb->setFirstInput(parInput1);
  
   // -----    HYP hit producer   --------------------------------------------
  PndHypIdealRecoTask* hypirec = new PndHypIdealRecoTask(0.01,0.01,0.005);
  hypirec->SetVerbose(iVerbose);
  fRun->AddTask(hypirec);


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
