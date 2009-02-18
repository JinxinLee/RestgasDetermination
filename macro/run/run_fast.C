/** Macro for running fast simulation it will only fill the 
*   PndData without any transport(M. Al-Turany)
*/
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
  gSystem->Load("libPassive");
  gSystem->Load("libPGen");
 
  FairRunSim *fRun = new FairRunSim();
  
  
  fRun->SetOutputFile("sim_fast.root");

    // Create and Set Event Generator
    //-------------------------------

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Urqmd  Generator
  // FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator("../../input/00-03fm.100ev.f14");
  // primGen->AddGenerator(urqmdGen);

  // Particle Generator
  //FairParticleGenerator* partGen = new FairParticleGenerator(13, 1, 0.5, 0., 0.);
  //primGen->AddGenerator(partGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  //  boxGen->SetPRange(1.,1.1); // GeV/c
  boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 90.); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  primGen->AddGenerator(boxGen);
 
  // proton 2212 pi+ 211 pi- -211 
 /* Double_t randx, randy;
  for (Int_t n =0; n<10; n++){
	randx= gRandom->Gaus(0,1);
	randy= gRandom->Gaus(0,1);
	FairParticleGenerator* partGen = new FairParticleGenerator(3312, 1, 0.3*randx, 0.3*randy, 0.3);
  	primGen->AddGenerator(partGen);
  }
*/

 //PndDpmGenerator *PndDpm = new PndDpmGenerator("../../input/dpmevt_noelastic_36755.root");
 //primGen->AddGenerator(PndDpm);
  // Ion Generator
  //FairIonGenerator *fIongen= new FairIonGenerator(79, 197,79,1, 0.,0., 25, 0.,0.,-1.);
  //  primGen->AddGenerator(fIongen);

 /**switch off the transport of particles*/
  primGen->DoTracking(kFALSE);

  fRun->Init();
  Int_t nEvents = 1000;
  fRun->Run(nEvents);
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}  
  
