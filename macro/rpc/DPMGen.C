// Macro for  MC simulation RPC-ToF  with output file from  DPMGen
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
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
//  gSystem->Load("libGen");

//  gSystem->Load("../../pgenerators/DpmEvtGen/lib/libDpmEvtGen");
  gSystem->Load("libPGen");

  gSystem->Load("libtpc");
  gSystem->Load("libStt");
  gSystem->Load("libRpc");


  CbmRunSim *fRun = new CbmRunSim();
  
  // set the MC version used
  fRun->SetName("TGeant3");

  // Choose the Geant Navigation System
  // fRun->SetGeoModel("G3Native");
  
  fRun->SetOutputFile("test_dpm.root");

  // Set Material file Name
  fRun->SetMaterials("media_pnd.geo");
  
  // Create and add detectors
  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(Cave);

// CbmDetector *Stt = new PndStt("STT", kTRUE);
// Stt->SetGeometryFileName("straws_skewed_blocks.geo"); 
// fRun->AddModule(Stt);
  

//  CbmDetector *Tpc = new PndTpcDetector("TPC", kTRUE);
//  Tpc->SetGeometryFileName("tpc.geo");
//  fRun->AddModule(Tpc);

   CbmDetector *Rpc = new PndRpcDetector("Rpc", kTRUE);
   Rpc->SetGeometryFileName("rpc.geo");
   fRun->AddModule(Rpc);


 // Create and Set Event Generator
 //-------------------------------

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

// insert output file from DPM generator running
PndDpmGenerator *dpmGen = new PndDpmGenerator("Background-micro.root");
primGen->AddGenerator(dpmGen);
 
  // Field Map Definition
  // --------------------
  // 1- Reading the new field map in the old format
  // CbmFieldMap *fMagField= new CbmFieldMap("FIELD.v04_pavel.map");
  // Constant Field
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0, 0 ,20. ); // values are in kG
  // MinX=-75, MinY=-40,MinZ=-12 ,MaxX=75, MaxY=40 ,MaxZ=124 );  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -200, 200);
  
  // 2- Reading the new field map in the new format
  //  CbmField *fMagField= new CbmFieldMapSym3("FieldActive");
  // Active Shielding
  
  fRun->SetField(fMagField);
  fRun->SetStoreTraj(kTRUE);
  
  fRun->Init();
  
  
  // -Trajectories Visualization (TGeoManager Only )
  // -----------------------------------------------
  
  
  // Fill the Parameter containers for this run
  //-------------------------------------------
  
     CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
     Bool_t kParameterMerged=kTRUE;
     CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
     output->open("testparams.root");
     rtdb->setOutput(output);
     rtdb->saveOutput();
     rtdb->print();
  
  // Transport nEvents
  // -----------------

  Int_t nEvents =1000;
  fRun->Run(nEvents);
 
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
 printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);


 TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("CBMGeom");
 
 TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
 c1->SetFillColor(10);
 
 geoMan->SetVisLevel(2); // Number of folded geo levels; 3 for emc. 
                         //Try to change the number and 
                         //to see how the geometry changes.
 geoMan->GetMasterVolume()->Draw("same");

}  
  
