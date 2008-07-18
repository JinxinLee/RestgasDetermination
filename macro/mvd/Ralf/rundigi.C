{

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Number of events to process
  Int_t nEvents = 100;
  // Parameter file
//   TString parFile = "../data/mvddpm6GeV_par.root";
//   TString parOutFile = "../data/mvddpm6GeV_digipar.root";
  TString parFile = "../data/mvdStrip_par.root";
  TString parOutFile = "../data/mvdStrip_digipar.root";
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/mvd/MvdTools/mvd.digi.par";
//   PndMvdFileNameCreator namecreator("../data/mvddpm6GeV.root");
  PndMvdFileNameCreator namecreator("../data/mvdStrip.root");
  // Input file (MC events)
  std::string inFile = namecreator.GetSimFileName();
  // Output file
  std::string outFile = namecreator.GetDigiFileName();

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile.c_str());
  fRun->SetOutputFile(outFile.c_str());


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();

  // -----   Digitization   ---------------------------------------
// 	PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
//   mvddigi->SetVerbose(iVerbose);
// 	fRun->AddTask(mvddigi);
  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  PndMvdStripHitProducer* mvdHitProd2 = new PndMvdStripHitProducer();
  mvdHitProd2->SetVerbose(iVerbose);

  fRun->AddTask(mvdHitProd2);

  // -----    MVD Pixel hit producer   ---------------------------------------
//   Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
//   PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
//   mvdPixProd->SetVerbose(iVerbose);
//   fRun->AddTask(mvdPixProd);
  // =====                 End of HitProducers                           =====
  // =========================================================================
  fRun->AddTask(new PndMvdAccessRTDBTask());

     
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();


//   // save Parameters ??into a new file??
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open(parOutFile.Data(),"RECREATE");
  rtdb->setOutput(output);

  rtdb->saveOutput(); // save new parameter lists
  rtdb->print();


  fRun->Run(0,nEvents);


// // check param file
//   CbmRuntimeDb* rtdb2 = fRun->GetRuntimeDb();
//   CbmParRootFileIo* parInput1b = new CbmParRootFileIo();
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

