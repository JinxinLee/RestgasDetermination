// digi macro for lmd
// created 23/07/2015 by A.Karavdina
int digi_LMD(const int nEvents=2000, const int startEvent=0, TString storePath="", const int verboseLevel=0)
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Input file (MC events)
  TString inFile=storePath+"Lumi_MC_";
  inFile += startEvent;
  inFile += ".root";
  // Parameter file
  TString parFile=storePath+"Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";

  // Parameter file
  TString digiparFile = "../params/lumi.digi.par";

  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  TString outFile = storePath+"Lumi_digi_";
  outFile += startEvent;
  outFile += ".root";
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
// fRun->SetEventMeanTime(50);//[ns] 50= interaction rate 20 MHz; 
  fRun->SetEventMeanTime(80);//[ns] 80= interaction rate 12.5 MHz; 
  // example: cross-section(DPM el+inlel, 1.5 GeV/c)= 125 mb, L=10^32 -> interaction rate 12.5 MHz
 // fRun->SetEventMeanTime(800);//[ns] 800= interaction rate 5 MHz; //15 GeV

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  // fRun->LoadGeometry();


  // -----   Digitization   ---------------------------------------
  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  PndLmdHybridHitProducer* lmdStripProd = new PndLmdHybridHitProducer();
  lmdStripProd->SetVerbose(verboseLevel);
  fRun->AddTask(lmdStripProd);
 
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
  PndSdsGeoPar* geoPar  = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar")); 
  
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

  return 0;
}
