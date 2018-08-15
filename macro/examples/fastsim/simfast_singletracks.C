/** Macro for running fast simulation it will only fill the 
*   PndData without any transport(M. Al-Turany)
*/


int simfast_singletracks(int abslund=211, int mult=10, int soff=0,int nevts=0,TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libGen");
  gSystem->Load("libRho");
  gSystem->Load("libfsim");
 
  if (outfile=="") outfile="singletracks.root";


  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(outfile.Data());

    // Create and Set Event Generator
    //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairBoxGenerator *boxGen1=new FairBoxGenerator();
  boxGen1->SetMultiplicity(mult);
  boxGen1->SetPDGType(abslund);  
  boxGen1->SetThetaRange(0,180);
  boxGen1->SetPRange(0.05,8);
  primGen->AddGenerator(boxGen1);

  FairBoxGenerator *boxGen2=new FairBoxGenerator();
  boxGen2->SetMultiplicity(mult);
  boxGen2->SetPDGType(-abslund);  
  boxGen2->SetThetaRange(0,180);
  boxGen2->SetPRange(0.05,8);
  primGen->AddGenerator(boxGen2);


  //  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(infile.Data());
  //  primGen->AddGenerator(evtGen);

  primGen->DoTracking(kFALSE);

  
  //Setup the Fast Simulation Task
  //-----------------------------
  PndFastSim* fastSim = new PndFastSim();
  
  //increasing verbosity increases the amount of console output (mainly for debugging)
  fastSim->SetVerbosity(0);  

  //enable the producting of parametrized neutral (hadronic) split offs
  if (soff) 
    fastSim->EnableSplitoffs("../../fsim/splitpars.dat");
   
  //EM Calorimeters w/ default parameters (don't have to be set, just to list the available parameters)
  fastSim->AddDetector("EmcBarrel","thtMin=22.0 thtMax=140.0 efficiency=1.0 aPar=0.01 bPar=0.01 xtalDim=0.02 Emin=0.03 barrelRadius=0.5 resFactor=0.25");
  fastSim->AddDetector("EmcFwCap", "thtMin=5.0 thtMax=22.0 efficiency=1.0 aPar=0.01 bPar=0.01 xtalDim=0.02 Emin=0.03 dist=2.5 resFactor=0.25");
  fastSim->AddDetector("EmcBwCap", "thtMin=140.0 thtMax=170.0 efficiency=1.0aPar=0.01 bPar=0.01 xtalDim=0.02 Emin=0.03 dist=0.7 resFactor=0.25");
  fastSim->AddDetector("EmcFS",    "thtMin=0.05 thtMax=5.0 efficiency=1.0 aPar=0.02 bPar=0.0274 xtalDim=0.1 Emin=0.03 dist=8.0 resFactor=0.25");

  //Tracking, Vertexing, dE/dx
  fastSim->AddDetector("Mvd",  "thtMin=0.0 thtMax=180.0 efficiency=1.0 pmin=0.0 vtxRes=0.0001 pRes=0.1 phiRes=0.01 thetaRes=0.01 dEdxRes=0.25");
  fastSim->AddDetector("Stt", "thtMin=7.765 thtMax=159.44 efficiency=1.0 pmin=0.1 n=11 sigXY=0.00015 BField=2.0 Lpath=0.27 X0=0.0 sigTht=0.0006 dEdxRes=0.2");
  fastSim->AddDetector("MdcTS","thtMin=0.5 thtMax=22.0 efficiency=1.0 pmin=0.0 a3Par=0.00033 a4Par=0.00077");
  fastSim->AddDetector("MdcFS","thtMin=0.5 angleXMax=10.0 angleYMax=5.0 efficiency=1.0 pmin=0.7 a1Par=0.00113 a2Par=0.00227 Bl=2.0");  
  //fastSim->AddDetector("Tpc",  "thtMin=7.765 thtMax=159.44 efficiency=1.0 pmin=0.0 pRes=0.005 phiRes=0.1 thetaRes=0.1 dEdxRes=0.07"); 
  //fastSim->AddDetector("EffTracker");

  //PID detectors
  fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 pmin=0.0 dthtc=0.01 nPhotMin=5 nRefrac=1.472 Bfield=2 effNPhotons=0.075 rBarrel=0.48 dSlab=0.017");
  fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 pmin=0.0 dthtc=0.01 nPhotMin=5 nRefrac=1.472 Bfield=2 effNPhotons=0.075 dDisc=0.017");
  fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 efficiency=1.0 pmin=0.0 dthtc=0.01 nPhotMin=5 nRefrac=1.05 Bfield=2 effNPhotons=0.075 dRich=1.0");
  fastSim->AddDetector("Tof","thtMin=22.0 thtMax=140.0 efficiency=1.0 pmin=0.0 BField=2.0 rBarrel=0.38 dSlab=0.01 dp=0.01");
  
  fRun->AddTask(fastSim);
  
  fRun->Init();
  if (nevts==0) nevts=10;
  fRun->Run(nevts);		
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}  
  
