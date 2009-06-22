// Macro created by Radoslaw Karabowicz
// This macro takes MC file and produces IDEAL HITS only

void gem_idealHits(Int_t nEvents = 100, TString addString = "4Stations_15GeV", int verboseLevel = 0)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
//  Int_t iVerbose = 1;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");
  // Input file (MC events)
  TString baseName;
  baseName.Form("$VMCWORKDIR/data/Gem_%s_n%d",addString.Data(),nEvents);

  TString MCFile  = baseName + ".root";
  TString parFile = baseName + "_par.root";
  // ------------------------------------------------------------------------
  TString outFile = baseName + "_idealHits.root";
  
  std::cout << "RecoFile: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);
  

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo2->open("../params/gem_4Stations.digi.par","in");
  rtdb->setSecondInput(parIo2);

  fRun->LoadGeometry();

  // -----   Ideal Track Producers   ---------------------------------------
  PndGemIdealHitProducer* gemHitProducer = new PndGemIdealHitProducer("GEM Ideal hit producer", verboseLevel);
  fRun->AddTask(gemHitProducer);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);


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

