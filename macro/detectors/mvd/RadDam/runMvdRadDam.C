{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  Int_t nEvents  = 500;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Input file (MC events)
  //TString inFile = "Mvd_DPMfixed_4GeV_10000.root"; //"MvdG4_DPM405_Mag_5000.root";
  //TString inFile = "data/mvdparams.root";
  TString inFile = "Mvd_GridUrqmdSim.root";

  // Parameter file
  TString parFile = "Mvd_GridUrqmdSimParam.root";
  //TString parFile = "data/mvdparams.root";
  // Parameter output file

//   TString parOutFile = "Test/testParamsOutput.root";

 //    PndFileNameCreator creator(inFile.Data());
 //      TString outFile = creator.GetDigiFileName().c_str();
  TString outFile = "Mvd_GridUrqmdSim.RadDam.root";

     // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
 // FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
 // parInput1->open(parFile.Data(),"UPDATE");
 // rtdb->setFirstInput(parInput1);
//  Bool_t kParameterMerged=kTRUE;

 // FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
 // parInput2->open(digiparFile.Data(),"in");
 // rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();


  // -----   Weight Calculator   ---------------------------------------

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
