// TEST 1: only check EvtGenDirect
// void qa_evtgendirect(){
int QAmacro_evtgen_2()
{
  TStopwatch timer;
  timer.Start();

  TString BaseDir =  gSystem->Getenv("VMCWORKDIR");
  TString DECAYInput = BaseDir + "/pgenerators/EvtGen/DECAY.DEC";
  TString PDLInput = BaseDir + "/pgenerators/EvtGen/evt.pdl";
  TString USERInput = BaseDir + "/macro/qa/evtgen/test.dec";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Bool_t fTest=kFALSE;
  if( gSystem->Load("libEvtGen")>=0 && gSystem->Load("libEvtGenDirect")>=0){ 
  
  FairRunSim *fRun = new FairRunSim();

  // set the MC version used
  // ------------------------

  fRun->SetName("TGeant3");
  //fRun->SetName("TGeant4");

  fRun->SetOutputFile("qa_evtgen2.root");

  // Set Material file Name
  //-----------------------
  fRun->SetMaterials("media_pnd.geo");

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // EvtGen Generator
  PndEvtGenDirect* evtGen = new PndEvtGenDirect("psi(2S)",USERInput,7,123456,DECAYInput,PDLInput);
  evtGen->SetStoreTree();
  primGen->AddGenerator(evtGen);

 
  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run(10);// just a few events
  // ------------------------------------------------------------------------

  fTest = kTRUE;
	}

  if (fTest){
    cout << " Test Passed" << endl;
    cout << " All Ok " << endl;  
  }else{
    cout << " Test Failed" << endl;
    cout << " Not Ok " << endl;         
  }
 timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  exit(0); 
}  
  
