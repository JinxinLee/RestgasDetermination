// *******
// Macro for running fast simulation
// *******


void simfast(TString infile="dsdsj20k.evt",int soff=0,int nevts=0,TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;

  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // Load this libraries
  //gSystem->Load("libRho");
  gSystem->Load("libfastsim");
 
  if (outfile=="") outfile=infile+".fast.root";

  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(outfile.Data());
  fRun->SetName("TGeant3");

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(infile.Data());
  primGen->AddGenerator(evtGen);

  primGen->DoTracking(kFALSE);

  
  //Setup the Fast Simulation Task
  //-----------------------------
  PndFastSim* fastSim = new PndFastSim();
  
  //increasing verbosity increases the amount of console output (mainly for debugging)
  fastSim->SetVerbosity(0);  

  //enable the producting of parametrized neutral (hadronic) split offs
  if (soff) 
    fastSim->EnableSplitoffs("splitpars.dat");
   
  //EM Calorimeters w/ default parameters (don't have to be set, just to list the available parameters
  //EM Calorimeters w/ default parameters (don't have to be set, just to list the available parameters
  fastSim->AddDetector("EmcBarrel","thtMin=22.0 thtMax=140.0 Emin=0.01 barrelRadius=0.5");
  fastSim->AddDetector("EmcFwCap", "thtMin=5.0 thtMax=22.0 Emin=0.01 dist=2.5");
  fastSim->AddDetector("EmcBwCap", "thtMin=140.0 thtMax=170.0 Emin=0.01 dist=0.7");
  fastSim->AddDetector("EmcFS",    "thtMin=0.05 thtMax=5.0 aPar=0.02 bPar=0.0274 Emin=0.01 dist=8.0");

  //Tracking, Vertexing, dE/dx
  fastSim->AddDetector("Mvd",  "thtMin=0.0 thtMax=180.0 vtxRes=0.0001 pRes=0.1 phiRes=0.01 thetaRes=0.01 dEdxRes=0.25");
  fastSim->AddDetector("Stt",  "thtMin=7.765 thtMax=159.44 dEdxRes=0.2");
  fastSim->AddDetector("MdcTS","thtMin=0.5 thtMax=22.0");
  fastSim->AddDetector("MdcFS","thtMin=0.5 angleXMax=10.0 angleYMax=5.0");  
  //fastSim->AddDetector("Tpc",  "thtMin=7.765 thtMax=159.44 efficiency=1.0 pmin=0.0 pRes=0.005 phiRes=0.1 thetaRes=0.1 dEdxRes=0.07"); 

  //PID detectors
  fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("Tof","thtMin=22.0 thtMax=140.0 dp=0.01");
  
  fRun->AddTask(fastSim);
  
  fRun->Init();
  if (nevts==0) nevts=10;
  fRun->Run(nevts);		
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
