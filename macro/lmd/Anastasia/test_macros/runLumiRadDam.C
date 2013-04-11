{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Int_t nEvents  = 500;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Input file (MC events)
  TString inFile = "testPixel/Lumi_MC_0.root";

  // Parameter file
  TString parFile = "testPixel/Lumi_Params_0.root";

  TString outFile = "testPixel/Lumi_RadDam.root";

     // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  // //  fRun->LoadGeometry();//???
  // PndLmdDetector *Lum = new PndLmdDetector("LUM", kTRUE);
  // Lum->SetExclusiveSensorType("LumActive");  //ignore MVD
  // //  Lum->SetGeometryFileName("../macro/lmd/geo/Test-Dipol-Design.root"); //sensors with trap shape
  // Lum->SetGeometryFileName("../macro/lmd/geo/HV_MAPS-Design.root"); // new sensors
  // //  Lum->SetVerboseLevel(verboseLevel);
  // fRun->AddModule(Lum);

  // -----   Weight Calculator   ---------------------------------------
  //  FairRadMapManager* fRunRadDam = new FairRadMapManager();
  //fRun->AddTask(fRunRadDam);

  PndMvdRadDamTask* mvdRadDam = new PndMvdRadDamTask();
  mvdRadDam->SetVerbose(iVerbose);
  fRun->AddTask(mvdRadDam);




   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
   output->open(parFile.Data());
   rtdb->setOutput(output);
//  rtdb->setOutput(parInput1);

//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);

  // =====                 End of HitProducers                           =====

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);

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
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
  cout << endl;

}
