{
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
  gSystem->Load("libMvd");
  // ------------------------------------------------------------------------

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  Int_t nEvents=10000;
  
  // Input file (MC events)
  TString inDir="/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/07_01_2009/";
  TString job="GEANT3_ALICE_L5_1MeV_cuts_withPIPE_MVD";
  inDir+=(job+"/");
  TString jobname="2Gev_G3_ALICE_L5_1MeV_cuts_with_PIPE_MVD_10k_evts";

  TString inFile=inDir+jobname;
  TString inFile2=inFile;  //if this file does not exist it should not matter,
                           //FairRunAna tests for Zombie

  inFile+=".mc.root";
  inFile2+=".mc_1.root";
  // make new subdir
  // TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
//   TString cmd="mkdir ";
//   cmd+=jobDir; 
//   if(gSystem->Exec(cmd)){
//     std::cout<<"Could not create Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }


  TString outDir=inDir;
  TString outFile = outDir+jobname; 
  outFile+=".raw.root";

  TString paramIn = inFile;
  paramIn.ReplaceAll(".mc.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".raw.root",".param.digi.root");

// // copy this macro to the data directory
//   cmd="cp macro/tpc/runDigi.C ";
//   cmd+=jobDir; 
//  if(gSystem->Exec(cmd)){
//     std::cout<<"Could not copy runDigi.C to Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }

std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
std::cout<<"ParamOut: "<<paramOut<<std::endl;
 


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFile(inFile2);  //ADDING THE SECOND INPUT FILE
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // ------ QAplots
  QAPlotCollection* qa=new QAPlotCollection("TpcDigiQAPlots");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);

  PndTpcDigiPar* par = (PndTpcDigiPar*) rtdb->getContainer("PndTpcDigiPar");
  par->setInputVersion(fRun->GetRunId(),1);
  par->setChanged(kTRUE);

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  // -----    Digi Sequence  --------------------------------------------

  PndTpcSpaceChargeTask* tpcSP = new PndTpcSpaceChargeTask();
  //tpcSP->setAliceMode(kTRUE);
  tpcSP->setBins(26,149);
  fRun->AddTask(tpcSP);  

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  rtdb->print();

  fRun->Run(0,nEvents); // process all events from input file
  // ------------------------------------------------------------------------


//tpcDrifter->WriteHistograms();
//tpcPadResponse->WriteHistograms();
//tpcElec->WriteHistograms();

FairRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
FairRootManager::Instance()->GetOutFile()->cd("QAPlots");
qa->Write();

tpcSP->writeToFile(outDir+"SpaceCharge_smallsteps_"+job+".dat");

  // -----   Finish   -------------------------------------------------------


//delete tpcSplitter;

  rtdb->saveOutput();
  rtdb->print();


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
