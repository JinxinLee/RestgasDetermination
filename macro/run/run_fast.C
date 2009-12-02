/** Macro for running fast simulation it will only fill the 
*   PndData without any transport(M. Al-Turany)
*/
run_fast(Int_t nEvents = 1000, )
{

 //-----User Settings:-----------------------------------------------
  TString  OutputFile     ="sim_fast.root";
  gDebug                  = 0;
  // choose your event generator 
  Bool_t UseEvtGen	      =kTRUE;     
  Bool_t UseDpm 	      =kFALSE;
  Bool_t UseBoxGenerator      =kFALSE; 
  
   // Load basic libraries---------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon(); 
  TStopwatch timer;
  timer.Start();
  gDebug=0;
 
  // Create the Simulation run manager--------------------------------
  FairRunSim *fRun = new FairRunSim();
  fRun->SetOutputFile(OutputFile.Data());
  
 // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);
	 
  if(UseBoxGenerator){	// Box Generator
     FairBoxGenerator* boxGen = new FairBoxGenerator(22, 5); // 13 = muon; 1 = multipl.
     boxGen->SetPtRange(mom,mom); // GeV/c
     boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
     boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
     boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
     primGen->AddGenerator(boxGen);
  }
  if(UseDpm){
  	  PndDpmDirect *Dpm= new PndDpmDirect(mom,1);
	  primGen->AddGenerator(Dpm);
  }
  if(UseEvtGen){	
	  TString  EvtInput =gSystem->Getenv("VMCWORKDIR");
	  EvtInput+="/input/psi2s_jpsi2pi_1k.evt";	
	  FairEvtGenGenerator* evtGen = new FairEvtGenGenerator(EvtInput.Data());
	  primGen->AddGenerator(evtGen);
  }	
	

 /**switch off the transport of particles*/
  primGen->DoTracking(kFALSE);
  //-------------------------  Initialize the RUN  -----------------  
  fRun->Init();
  //-------------------------  Run the Simulation  ----------------- 
  fRun->Run(nEvents);
  //------------------------Print some info and exit----------------  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  delete fRun;
  exit(0);
}  
  
