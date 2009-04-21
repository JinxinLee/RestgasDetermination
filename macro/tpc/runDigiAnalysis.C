void 
runDigiAnalysis(){
   // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  // ------------------------------------------------------------------------


  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (Signals events)
  TString inFile="/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009/new_PndTpcDetector/GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD/sigSlice1/evtmix3/2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts.raw.root";
  
  TString outFile = inFile; 
  outFile.ReplaceAll(".raw.root",".digiA.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");


std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;



  // Number of events to process
  Int_t nEvents = 1;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramIn.Data());
  //FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  //TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  //tpcDigiFile += "/tpc/tpc.fullplane.par";
  //parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);

   fRun->LoadGeometry();
  // ------------------------------------------------------------------------


// ----- PndTpc DigiAnalysis -------------

  PndTpcDigiAnalysisTask* tpcDigiAna = new PndTpcDigiAnalysisTask();
  fRun->AddTask(tpcDigiAna);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  rtdb->print();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  tpcDigiAna->WriteHistos();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
