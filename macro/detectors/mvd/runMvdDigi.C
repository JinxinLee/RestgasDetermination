// Macro to emulate the MVD digitization in pandaroot
// Updated 30.11.2009
// Ralf Kliemt
runMvdDigi(Int_t nEvents=10)
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString inFile = "Mvd_Test.root";
  TString parFile = "Mvd_Params.root";
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/params/all.par";

  // Output file
  PndFileNameCreator creator(inFile.Data());
  TString outFile = creator.GetDigiFileName().c_str(); 
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);
  
  // ======================================================================
  //    1) Default Task
  //    2) Separate pixels/strips/noise
  //    Set parameters in pandroot/macro/params/mvd.digi.par  

  // -----   Default Digitization   ---------------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(2);
  fRun->AddTask(mvddigi);
  
  // -----   OR custom settings to switch off something   -----------------
//  PndMvdStripHitProducer* mvdStripProd = new PndMvdStripHitProducer();
//  mvdStripProd->SetVerbose(iVerbose);
//  fRun->AddTask(mvdStripProd);
//  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
//  mvdPixProd->SetVerbose(2);
//  fRun->AddTask(mvdPixProd);
//  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
//  mvdNoiseMaker->SetVerbose(iVerbose);
//  fRun->AddTask(mvdNoiseMaker);

  rtdb->setOutput(parInput1);
  rtdb->print();
  
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
