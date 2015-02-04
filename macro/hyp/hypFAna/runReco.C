void runReco(TString base="Hit_hypIdReco", int nEvents = 0)
{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Number of events to process
  //Int_t nEvents = 2000; // process all events in input file
  // Base File name
  //TString base="Lambda";

  // Input file (MC events)
  /*TString inFile = base+".mc.root";
  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".mcreco.root";*/

 /*
   TString inFile = "/d/panda01/asanchez/PANDA/BuphypAllXXb.root";//../hyp/sim_hyp5HeL.root";
  //Parameter file
  TString parFile = "../hyp/Bupcheckparams.root";//udgenparams.root";
  // Output file
  TString outFile = base+".mcrecdos.root";
  TString RecoFile = "../hyp/hit_idealreco.root";
*/
 
  /* //full Simu 1.2.steps G3
  TString inFile = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck.root";
  TString inFile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__1.root";
  TString inFile3 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__2.root";
  TString inFile4 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__3.root";
  TString inFile5 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck__4.root";
  TString parFile = "../hyp/BupG3checkparams.root";

  TString outFile = base+".G3recdos.root";
  TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3Idealreco.root";*/

  /*
  //full Simu 1.2.steps G3--xim
  TString inFile = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2.root";
  TString inFile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck2_1.root";
 
  TString parFile = "../hyp/BupG3check2params.root";

  TString outFile = base+".G3Ximrecdos2.root";
  TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3XimIdealreco.root";*/


//full Simu 1.2.steps G3--xixibar razhyp/sim_BupG3hypcheck3.root
  /*TString inFile = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3.root";
  TString inFile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_1.root";
  TString inFile3 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_2.root";
  TString inFile4 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck3_3.root";
 
  TString parFile = "../hyp/BupG3check3params.root";

  TString outFile = "/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3RZHrecdos3.root";
  TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3RZHIdealreco.root";
  */

  //full Simu 1.2.steps G3--xixibar razhyp/sim_BupG3hypcheck3.root
  /*
    TString inFile = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck4.root";
    TString inFile2 = "/d/panda01/asanchez/PANDA/sim_BupG3hypcheck4_1.root";
    TString parFile = "/d/panda02/asanchez/fairsoft_jul08new/trunk/macro/hyp/BupG3check4params.root";
    
    TString outFile = "/d/panda01/asanchez/PANDA/Hit_hypIdReco.G3RZHximrecdos2.root";
    TString RecoFile = "/d/panda01/asanchez/PANDA/hit_G3RZHximIdealreco.root";
  */

 TString inFile = "/d/pndint02/asanchez/sim_BupG3hypcheck5.root";
 TString inFile2 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_1.root";
 TString inFile3 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_2.root";
 TString inFile4 = "/d/pndint02/asanchez/sim_BupG3hypcheck5_3.root";
 TString parFile = "/d/panda02/asanchez/fairsoft_jul08new/trunk/macro/hyp/BupG3check5params.root";
 
 TString outFile = "/d/pndint02/asanchez/PANDA/Hit_hypIdReco.G3RZHm2recdos.root";
 //TString RecoFile = "/d/pndint02/asanchez/PANDA/hit_G3RZHIdrec2.root";


/*
 TString inFile = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08.root";//../hyp/sim_hyp5HeL.root";
 TString inFile2 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____1.root";
  TString inFile3 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____2.root";
  TString inFile4 = "/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____3.root";
  //Parameter file
  TString parFile = "../tof/simparams.root";//udgenparams.root";
  // Output file
  TString outFile = base+".Bgrecdos.root";
  TString RecoFile = "../hyp/hit_BgIdealreco.root";
*/



 /* TChain *mychain = new TChain("cbmsim");
 mychain->Add("/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08.root" );
 mychain->Add("/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____1.root" );
 mychain->Add("/d/panda01/asanchez/PANDA/sim_tof_Bgfulljun08____3.root" );
*/

// ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  /*
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libgeant321");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
 gSystem->Load("libHypGe");
  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  //gSystem->Load("libGen");
  //gSystem->Load("libPGen");
  gSystem->Load("libPGen");
 
  gSystem->Load("libTrkBase");
  gSystem->Load("libGeane");
  gSystem->Load("libTof");
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  */
 gSystem->Load("libHypGe");
  gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  //gSystem->Load("libHypGe");
  //gSystem->Load("libanatask");

  gSystem->ListLibraries();



// ------------------------------------------------------------------------

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

	// Digitisation file (ascii)
	TString digiFile = "emc.par";


  // -----   Reco run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
 
  fRun->SetInputFile(inFile.Data());
  //fRun->AddFriend(RecoFile);
  fRun->AddFile(inFile2.Data());
  //fRun->AddFriend(RecoFile);
  fRun->AddFile(inFile3.Data());
  //fRun->AddFriend(RecoFile);
  fRun->AddFile(inFile4.Data());
  /*fRun->AddFriend(RecoFile);*/
//   fRun->AddFile(inFile5.Data());
//   fRun->AddFriend(RecoFile);
  //fRun->AddAndMerge(inFile.Data());
 //fRun->AddFile(inFile3.Data());
  
  
  //fRun->AddFile(inFile2.Data());
  //fRun->AddFile(inFile2.Data());
  //fRun->AddFile(inFile3.Data());
  
 
  fRun->SetOutputFile(outFile);

  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  CbmGeane *Geane = new CbmGeane(inFile);

  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
 /*
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  rtdb->Print();
  
  // Set the field(if any) to Geane
    Geane->SetField(fRun->GetField());
      */
  // ------------------------------------------------------------------------
 // -----  Parameter database   --------------------------------------------
/*	TString emcDigiFile = gSystem->Getenv("VMCWORKDIR");
	emcDigiFile += "/emc/";
	emcDigiFile += digiFile;
*/
	
  /*CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  
  //CbmParAsciiFileIo* parIo1 = new CbmParAsciiFileIo();
  //parIo1->open(emcDigiFile.Data(),"in");
  
  rtdb->setFirstInput(parInput1);
  //	rtdb->setSecondInput(parIo1);
  rtdb->Print();
  */

    // ----------------------------------------------------------
  
  fRun->LoadGeometry();
	//Geane->SetField(fRun->GetField());
      
  // ------------------------------------------------------------------------
  // -----  Reco Sequence   --------------------------------------------

  DemoPatternRecoTask* DemoPR = new DemoPatternRecoTask();
  DemoPR->AddHitBranch(2,"HypPoint");
  //DemoPR->AddHitBranch(2,"PndTpcPoint");
//  DemoPR->AddHitBranch(3,"MVDPoint");
  DemoPR->SetPersistence();
  //DemoPR->UseGeane();
  fRun->AddTask(DemoPR);

  DemoKalmanTask* DemoKalman = new DemoKalmanTask();
  DemoKalman->AddHitBranch(2,"HypPoint");
  //DemoKalman->AddHitBranch(2,"PndTpcPoint");
//  DemoKalman->AddHitBranch(3,"MVDPoint");
  DemoKalman->SetPersistence();
//  DemoKalman->SetSmooth(true);
  fRun->AddTask(DemoKalman);

   // -----   EMC hit producers   ---------------------------------
  // The file name should be the same of the geometry file which was used for the simulation
  /*

  PndEmcHitProducer* emcHitProd = new PndEmcHitProducer();
  fRun->AddTask(emcHitProd); // hit production 
  
  PndEmcMakeDigi* emcMakeDigi=new PndEmcMakeDigi();
//fRun->AddTask(emcMakeDigi); // fast digitization

  PndEmcHitsToWaveform* emcHitsToWaveform= new PndEmcHitsToWaveform(iVerbose);
  PndEmcWaveformToDigi* emcWaveformToDigi=new PndEmcWaveformToDigi(iVerbose);
  fRun->AddTask(emcHitsToWaveform);  // full digitization
  fRun->AddTask(emcWaveformToDigi);  // full digitization
 
  PndEmcMakeCluster* emcMakeCluster= new PndEmcMakeCluster(iVerbose);
  fRun->AddTask(emcMakeCluster);

  PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  fRun->AddTask(emcHdrFiller); // ECM header

  PndEmcMakeBump* emcMakeBump= new PndEmcMakeBump();
  fRun->AddTask(emcMakeBump);

  PndEmcMakeRecoHit* emcMakeRecoHit= new PndEmcMakeRecoHit();
  fRun->AddTask(emcMakeRecoHit);
 */

// ----------------------------



  // DemoToolsTask* DemoTools = new DemoToolsTask();
  //fRun->AddTask(DemoTools);
 

// ----------------------------

//  V0Selector* V0Sel2 = new V0Selector();
//  V0Sel2->SetTrackBranchName("Track");
//  V0Sel2->SetV0BranchName("Lambda");
//  V0Sel2->SetPositivePartMass(0.938272);
//  V0Sel2->SetNegativePartMass(0.13957);
//  V0Sel2->SetPersistence();
//fRun->AddTask(V0Sel2);


//  LambdaStatTask* lambdaStat = new LambdaStatTask();
//  lambdaStat->SetPersistence();
//fRun->AddTask(lambdaStat);

  // -----   Intialise and run   --------------------------------------------
  //fRun->SetStoreTraj(kFALSE);
  fRun->Init();

  //rtdb->print();
  //Geane->SetField(fRun->GetField());

  fRun->Run(0,8000);

  //DemoKalman->WriteHistograms("KalmanHistos.root");
  //DemoTools->WriteHistograms();

  // ------------------------------------------------------------------------
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
  // ------------------------------------------------------------------------
}
