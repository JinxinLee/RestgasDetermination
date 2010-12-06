// Macro to emulate the MVD+ Stt  digitization in pandaroot
// Updated 6.12.2010
// Gianluigi Boca
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents  = 0;
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  TString inFile = "MvdStt_Test.root";
  TString parFile = "MvdStt_Params.root";
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
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);
  



  // -----   STT analysis tasks   --------------------------------------------
  // digitize ....

  //PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);
  // ------------------------------------------------------------------------





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
