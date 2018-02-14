int digi_mvd()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits for EMC

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose =3; // just forget about it, for the moment

  // Input file (MC events)
  TString inFile = "sim_mvd.root";

  // Parameter file
  TString parFile = "simparams.root"; // at the moment you do not need it

  // Digitisation file (ascii)
  TString digiFile = "all.par";

  // Output file
  TString outFile = "digi_mvd.root";

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // -----  Parameter database   --------------------------------------------
  TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
  emcDigiFile += "/macro/params/";
  emcDigiFile += digiFile;

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(emcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);


  // -----   EMC hit producers   ---------------------------------
  // The file name should be the same of the geometry file which was used for the simulation

//  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
//  emcHitsToWaveform->SetStorageOfData(kFALSE);
//  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
//  fRun->AddTask(emcHitsToWaveform);  // full digitization
//  fRun->AddTask(emcWaveformToDigi);  // full digitization

//  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
//  fRun->AddTask(sttHitProducer);

//  PndDchDigiProducer* digiProducer= new PndDchDigiProducer();
//  fRun->AddTask(digiProducer);
//  PndDchCylinderHitProducer* cylHitProducer= new PndDchCylinderHitProducer();
//  fRun->AddTask(cylHitProducer);
//
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

//  PndTofHitProducerIdeal* tofhit = new PndTofHitProducerIdeal();
//  tofhit->SetVerbose(iVerbose);
//  fRun->AddTask(tofhit);
//
//  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
//  mdtHitProd->SetPositionSmearing(0.2); // position smearing [cm]
//  fRun->AddTask(mdtHitProd);

//  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", iVerbose);
//  fRun->AddTask(gemDigitize);
//
//  PndGemFindHits* gemFindHits = new PndGemFindHits("GEM Hit Finder",  iVerbose);
//  fRun->AddTask(gemFindHits);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  timer.Start();
  fRun->Run();

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

  return 0;
}
