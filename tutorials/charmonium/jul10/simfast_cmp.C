// *******
// Macro for running fast simulation
// *******


void simfast_cmp(TString part="psi(2S)", TString decfile="PSI2S.DEC",int nevts=1000, double mom=15., TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // Load this example libraries
  //gSystem->Load("libRho");
  gSystem->Load("libfastsim");
 
  if (outfile=="") outfile=decfile+".fast.root";

  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(outfile.Data());
  fRun->SetName("TGeant3");

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  /// ... generate your signal on the fly
  PndEvtGenDirect *EvtGen = new PndEvtGenDirect(part,decfile,mom);
  EvtGen->SetStoreTree();
  primGen->AddGenerator(EvtGen);
//  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(infile.Data());
//  primGen->AddGenerator(evtGen);

  primGen->DoTracking(kFALSE);

  
  //Setup the Fast Simulation Task
  //-----------------------------
  PndFastSim* fastSim = new PndFastSim();
  fastSim->SetVerbosity(0);
  fastSim->AddDetector("CmpDet");
  fastSim->EnablePropagation();
  
  fRun->AddTask(fastSim);
  
  fRun->Init();
  if (nevts==0) nevts=10;
  fRun->Run(nevts);		
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
