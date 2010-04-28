{

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Number of events to process
  Int_t nEvents = 10000;
  // Parameter file
  TString parFile = "../data/mvddpm6GeV_par.root";
  TString parOutFile = "../data/mvddpm6GeV_digipar.root";
//   TString parFile = "../data/mvdStrip_par.root";
//   TString parOutFile = "../data/mvdStrip_digipar.root";
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/mvd/MvdTools/mvd.digi.par";
  PndFileNameCreator namecreator("../data/mvddpm6GeV.root");
//   PndFileNameCreator namecreator("../data/mvdStrip.root");
  // Input file (MC events)
  std::string inFile = namecreator.GetSimFileName();
  // Output file
  std::string outFile = namecreator.GetDigiFileName();

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile.c_str());
  fRun->SetOutputFile(outFile.c_str());


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();

  // -----   Digitization   ---------------------------------------
//  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
//   mvddigi->SetVerbose(iVerbose);
//  fRun->AddTask(mvddigi);
  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  PndMvdStripHitProducer* mvdHitProd2 = new PndMvdStripHitProducer();
  mvdHitProd2->SetVerbose(iVerbose);

  fRun->AddTask(mvdHitProd2);

  // -----    MVD Pixel hit producer   ---------------------------------------
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);
  // =====                 End of HitProducers                           =====
  // =========================================================================
//   fRun->AddTask(new PndMvdAccessRTDBTask());

  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
  mvdNoiseMaker->SetVerbose(iVerbose);
  fRun->AddTask(mvdNoiseMaker);


     
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();


//   // save Parameters ??into a new file??
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutFile.Data(),"RECREATE");
  rtdb->setOutput(output);

  rtdb->saveOutput(); // save new parameter lists
  rtdb->print();


  fRun->Run(0,nEvents);


// // check param file
//   FairRuntimeDb* rtdb2 = fRun->GetRuntimeDb();
//   FairParRootFileIo* parInput1b = new FairParRootFileIo();
//   parInput1b->open(parFile.Data());
//   rtdb2->setFirstInput(parInput1b);
//   PndMvdStripDigiPar* par = (PndMvdStripDigiPar*)(rtdb2->getContainer("PndMvdStripDigiPar"));
//   par->Print();
// //

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

}

