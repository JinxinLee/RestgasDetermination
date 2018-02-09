// *******
// Macro for running fast simulation
// *******

void simfast_dpm_cmp(double mom, int mode=0,int nevts=100,TString outfile="")
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  
  // Load basic libraries
  // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  // Load this libraries
  gSystem->Load("libRho");
  gSystem->Load("libfastsim");
 
  double mp=0.938272;
  double p=0,M=0;
  
  // determine the pbar mom and E_cms for DPM generator
  if (mom>0) 
  {
	p=mom;
  	double E=sqrt(mp*mp+mom*mom)+mp;
    M=sqrt(E*E-mom*mom);
  }
  else
  {
  	M=-mom;
  	double X = (M*M-2*mp*mp)/(2*mp);
 	p = sqrt(X*X-mp*mp);  	
  }
  
  char tmp[100];
  sprintf(tmp,"dpm%d_%6.4f_%dk.fast.root",mode,M,nevts/1000);
  if (outfile=="") outfile=TString(tmp);

  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(outfile.Data());
  fRun->SetName("TGeant3");

  // Create and Set Event Generator
  //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  //PndDpmGenerator* dpmGen = new PndDpmGenerator(infile.Data());
  /* * 
  * 0. - No elastic scattering, only inelastic
  * 1. - Elastic and inelastic interactions
  * 2. - Only elastic scattering, no inelastic one
  */
  PndDpmDirect *dpmGen=new PndDpmDirect(p,mode);
  primGen->AddGenerator(dpmGen);

  primGen->DoTracking(kFALSE);

  
  //Setup the Fast Simulation Task
  //-----------------------------
  PndFastSim* fastSim = new PndFastSim();
  fastSim->SetVerbosity(0);
  fastSim->AddDetector("CmpDet");
  fastSim->EnablePropagation();
  
  fRun->AddTask(fastSim);
  
  fRun->Init();
  fRun->Run(nevts);		
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
