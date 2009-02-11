// Macro created A.Sanchez
// It creates a geant simulation file for hyp
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive"); // add other detector's lib if you need them
  gSystem->Load("libgenfit");
  gSystem->Load("libTof");
  gSystem->Load("libtpc");
  gSystem->Load("libHyp");
  //gSystem->Load("libHypGe");
  //gSystem->Load("libGen");
  gSystem->Load("libDpmEvtGen");
  gSystem->Load("libPGen");
  //gSystem->Load("libPGen");
  FairRunSim *fRun = new FairRunSim();
  
  
  TString inFile3= "/d/panda02/urqmd_smm/pbarC_3_GeV.root";

  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  
  
  //fRun->SetOutputFile("./sim_tof_Hyp.root");
  //fRun->SetOutputFile("./sim_tof_allBg.root");
  fRun->SetOutputFile("testmomo.root");
  
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave); 
  /*
   FairModule *Magnet= new CbmMagnet("MAGNET");
  Magnet->SetGeometryFileName("magnet.geo");
  fRun->AddModule(Magnet);
  */
  
  FairDetector *Tof = new PndTof("TOF",kTRUE);
  Tof->SetGeometryFileName("tofbarrel.geo");  //Tof->SetGeometryFileName("tofSciF.geo"); 
  fRun->AddModule(Tof);
 // 
  
  /*FairDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
  Tpc->SetGeometryFileName("tpc.geo");
  fRun->AddModule(Tpc);

  */


 /* FairDetector *Hyp = new PndHyp("HYP",kTRUE);
  Hyp->SetGeometryFileName("HypST_new.geo"); 
  fRun->AddModule(Hyp);
*/
  
//   tpc->SetGeometryFileName("tpc.geo"); 
//   fRun->AddModule(tpc);
   
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

   



    // Box Generator: 
  PndBoxGenerator* boxGen = new PndBoxGenerator(11, 1); // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event

  
  boxGen->SetPRange(1.,1.); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(0., 50.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen->SetXYZ(0., 0., 0.); // vertex coordinates [cm]
  primGen->AddGenerator(boxGen);  
  
  
  
     /* PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(inFile3);
      primGen->SetTarget(-76.,0.);
      primGen->AddGenerator(AsciiGen);
      */
    Double_t randx, randy;
    /* for (Int_t n =0; n<1; n++)
       {
       randx= gRandom->Gaus(0,1);
       randy= gRandom->Gaus(0,1);
       FairParticleGenerator* partGen = new FairParticleGenerator(-321, 1, 0.2*randx, 0.4*randy,0.5,0,0,-76);
       primGen->AddGenerator(partGen);
       }*/
    
    
   
    
      /*PndMultiField *fField= new PndMultiField();
       PndTransMap *map= new PndTransMap("TransMap", "R");
       PndDipoleMap *map1= new PndDipoleMap("DipoleMap", "R");
       PndSolenoidMap *map2= new PndSolenoidMap("SolenoidMap", "R");
       fField->AddField(map);
       fField->AddField(map1);
       fField->AddField(map2);
       fRun->SetField(fField);*/
      
       PndConstField *fMagField=new PndConstField();
       fMagField->SetField(0, 0 ,20. ); // values are in kG
       // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
       fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
       fRun->SetField(fMagField);

       fRun->SetStoreTraj(kTRUE); // to store particle trajectories 
    
       /*FairTrajFilter* trajFilter = FairTrajFilter::Instance();
	 trajFilter->SetStepSizeCut(0.001); // 1 cm
	 //  trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
	 // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
	 //  trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
	 trajFilter->SetStorePrimaries(kTRUE);
	 trajFilter->SetStoreSecondaries(kTRUE);*/ // not used for the others.????
   
    fRun->Init();
  
    
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("simparams.root");
  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  // Transport nEvents
  // -----------------
   
  // Set the number of events
  Int_t nEvents = 100; 
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  
