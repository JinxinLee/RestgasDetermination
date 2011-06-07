// Macro created 5/04/2007 by A.Sanchez
// It creates a geant simulation file for hypGe
{
  TStopwatch timer;
  timer.Start();
  gDebug=0;
  // Load basic libraries
  // If it does not work,  please check the path of the libs and put it by hands
 gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  
   gSystem->Load("libHypGe");
  
  FairRunSim *fRun = new FairRunSim();
  
  // Set the number of events
  Int_t nEvents = 2000; //6000 gamma spectra(hyp)
  TString inFile="/d/panda02/urqmd_smm/pbarC_3_GeV.root";
  //TString inFile="/u/asanchez/razhyp_gt12.dat";
  // set the MC version used
  // ------------------------
  
  fRun->SetName("TGeant4");
  //0.5 cm thickness in scint absorver
  //fRun->SetOutputFile("/d/panda01/asanchez/PANDA/sim_hypGe_test2.root");
  //1cm thichness scint absorver
  //fRun->SetOutputFile("/d/panda01/asanchez/PANDA/sim_hypGe_test3.root");
  //capsula geometry
  //fRun->SetOutputFile("/d/panda01/asanchez/PANDA/sim_hypGe_test5.root");
  //capsula geometry + sci geometry
  //fRun->SetOutputFile("/d/panda01/asanchez/PANDA/sim_hypGe_test6.root");
  
  //fRun->SetOutputFile("/d/panda01/asanchez/PANDA/sim_hypGe_BgUrqmd.root");
  fRun->SetOutputFile("sim_hypGe_BgUrqmd2.root");
  // Set Material file Name
  //-----------------------
  
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  //-------------------------

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  PndHypGe *HypGe= new PndHypGe("HYPGE",kTRUE);
  TString nam = gSystem->Getenv("VMCWORKDIR");
  HypGe->SetPathGeo(nam.Data());
  HypGe->SetGeometryFileName("HPGeCluster.root");
  fRun->AddModule(HypGe); 

  //  FairDetector *Hyp= new FairHyp("HYP",kTRUE);
  //    Hyp->SetGeometryFileName("HypST_prueba24pipe.geo");
  //    fRun->AddModule(Hyp); 

  //    FairDetector *Drc = new FairDrc("DIRC", kTRUE);
  //    Drc->SetGeometryFileName("dirc.geo");
  //    fRun->AddModule(Drc);

  //  FairModule *Magnet= new FairMagnet("MAGNET");
  //   Magnet->SetGeometryFileName("magnet.geo");
  //   fRun->AddModule(Magnet);

  //  FairDetector *Emc = new FairEmc("EMC",kTRUE);
  //   Emc->SetGeometryFileName("emc_module123.dat"); 
  //   fRun->AddModule(Emc);
  
  // Create and Set Event Generator
  //-------------------------------
  
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator: 
  //  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 1); 
  // 13 = muon; 1 = multipl. // 211 = pi+
  // first number: PDG particle code: 2nd number: particle multiplicity per event
  
  
  PndBoxGenerator* boxGen = new PndBoxGenerator(22, 1); 
  boxGen->SetPRange(0.007,0.008); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen->SetThetaRange(120., 180.); // Polar angle in lab system range [degree]
  boxGen->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
  
  primGen->AddGenerator(boxGen);  /*

  PndBoxGenerator* boxGen2 = new PndBoxGenerator(-211, 1); 
  boxGen2->SetPRange(0.09,0.1); // GeV/c
  // boxGen->SetPtRange(1.,1.); // GeV/c
  boxGen2->SetPhiRange(0., 360.); // Azimuth angle range [degree]
  boxGen2->SetThetaRange(120., 180.); // Polar angle in lab system range [degree]
  boxGen2->SetCosTheta(); // Set uniform ditribution in cos(theta)
  boxGen2->SetXYZ(0., 0., -76.); // vertex coordinates [cm]
				  
  primGen->AddGenerator(boxGen);  */

  /*
  TChain *t =new TChain("data");
   t->Add(inFile);
   PndUrqmdSmmGenerator* AsciiGen = new PndUrqmdSmmGenerator(t);
     primGen->SetTarget(-76.,0.);
     primGen->AddGenerator(AsciiGen);
  */
  
    /*FairAsciiGenerator* AsciiGen = new FairAsciiGenerator(inFile);
    //primGen->SetTarget(-150.,0.);
    primGen->AddGenerator(AsciiGen);
    */
  
  fRun->SetStoreTraj(kTRUE); // to store particle trajectories 


  //magnetic field: no field when commented put
  //FairFieldConst *fMagField=new FairFieldConst();
  //fMagField->SetField(0.,0.,20.); // values are in kG
  //fMagField->SetFieldRegion(-50, 50,-50, 50, -100, 100);
  // values are in cm (xmin,xmax,ymin,ymax,zmin,zmax)
  //fRun->SetField(fMagField);
  
  


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
  
  fRun->Run(nEvents);
  
  timer.Stop();
  
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
}  
  

