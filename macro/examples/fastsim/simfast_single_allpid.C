/** Macro for running fast simulation it will only fill the 
*   PndData without any transport(M. Al-Turany)
*/


int simfast_single_allpid(int mult=5, int soff=0,int nevts=0,TString outfile="")
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
 
  if (outfile=="") outfile="fastout.root";


  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(outfile.Data());

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  double pmin    = 0.05;
  double pmax    = 8.0;
  double thtmin  = 0.;
  double thtmax  = 180.;
  
  int pdgcode[10]={11,-11,13,-13,211,-211,321,-321,2212,-2212};

  int i;

  FairBoxGenerator *boxGen[10];
  
  for (i=0;i<10;i++){
    boxGen[i] =new FairBoxGenerator(); 
    boxGen[i]->SetMultiplicity(mult);
    boxGen[i]->SetPDGType(pdgcode[i]);  
    boxGen[i]->SetThetaRange(thtmin,thtmax);
    boxGen[i]->SetPRange(pmin,pmax);
    //boxGen[i]->SetCosTheta();
    primGen->AddGenerator(boxGen[i]);
  }


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
  fastSim->AddDetector("EmcBarrel","thtMin=22.0 thtMax=140.0 Emin=0.01 barrelRadius=0.5");
  fastSim->AddDetector("EmcFwCap", "thtMin=5.0 thtMax=22.0 Emin=0.01 dist=2.5");
  fastSim->AddDetector("EmcBwCap", "thtMin=140.0 thtMax=170.0 Emin=0.01 dist=0.7");
  fastSim->AddDetector("EmcFS",    "thtMin=0.05 thtMax=5.0 aPar=0.02 bPar=0.0274 Emin=0.01 dist=8.0");

  //Tracking, Vertexing, dE/dx
  fastSim->AddDetector("Mvd",  "thtMin=0.0 thtMax=180.0 vtxRes=0.0001 pRes=0.05 phiRes=0.01 thetaRes=0.01 dEdxRes=0.25");
  fastSim->AddDetector("Stt",  "thtMin=7.765 thtMax=159.44 dEdxRes=0.2");
  fastSim->AddDetector("MdcTS","thtMin=0.5 thtMax=22.0");
  fastSim->AddDetector("MdcFS","thtMin=0.5 angleXMax=10.0 angleYMax=5.0");  
  //fastSim->AddDetector("Tpc",  "thtMin=7.765 thtMax=159.44 efficiency=1.0 pmin=0.0 pRes=0.005 phiRes=0.1 thetaRes=0.1 dEdxRes=0.07"); 

  //PID detectors
  fastSim->AddDetector("DrcBarrel","thtMin=22.0 thtMax=140.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("DrcDisc","thtMin=5.0 thtMax=22.0 dthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("Rich","angleXMax=5.0 angleYMax=10.0 efdthtc=0.01 nPhotMin=5 effNPhotons=0.075");
  fastSim->AddDetector("Tof","thtMin=22.0 thtMax=140.0 dp=0.01");
  
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
  
