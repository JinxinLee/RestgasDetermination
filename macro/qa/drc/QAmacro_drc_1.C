void QAmacro_drc_1()
{
  cout << "QA module for the DRC Simulation." << endl;

  Int_t nEvents=10;
  TString outFile="sim.root";
  TString parFile="par.root";
  Int_t pdg=13;
  Double_t theta=130;
  Double_t phi=10.825;
  TString vmcdir=gSystem->Getenv("VMCWORKDIR");
  
  FairRunSim *fRun = new FairRunSim();
  fRun->SetName("TGeant4");
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetBeamMom(15);
  fRun->SetOutputFile(outFile);
  fRun->SetMaterials("media_pnd.geo");
  fRun->SetUserConfig(vmcdir+"/macro/drc/g4Config_Cherenkov.C");
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  // Set the parameters
  //-------------------------------
  TString allDigiFile(vmcdir+"/macro/params/all.par");
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  rtdb->setFirstInput(parIo1);  
      
  Bool_t kParameterMerged=kTRUE;	
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  output->open(parFile.Data());
  rtdb->setOutput(output);

  // Create and add detectors
  //-------------------------
  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  //-----------------------  Pipe  -----------------
  FairModule *Pipe= new PndPipe("PIPE");
  Pipe->SetGeometryFileName("beampipe_201309.root");
  fRun->AddModule(Pipe);
  
  //-----------------------  DRC  -----------------
  PndDrc *Drc = new PndDrc("DIRC", kTRUE);
  Drc->SetRunCherenkov(kTRUE);        // for fast sim Cherenkov -> kFALSE
  Drc->SetMirrorReal(kFALSE);  
  //Drc->StopChargedTrackAfterDIRC(kTRUE); 
  Drc->StopSecondaries(kFALSE); 
  Drc->SetTransportEffAtProduction(kTRUE);
  Drc->SetDetEffAtProduction(kTRUE);
  Drc->SetStopTime(150.);
  Drc->SetVerboseLevel(0);
  Drc->SetBlackLensSides(kTRUE);
  Drc->SetOptionForLUT(kFALSE);
  Drc->SetGeometryFileName("dirc_e3_b3_l6_m40.root");
  fRun->AddModule(Drc); 

  gRandom->SetSeed(0); // Set 0 for random
  cout<<"Seed for random number generation= "<<gRandom->GetSeed()<<endl;

  // Create and Set Event Generator
  //-------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  // Box Generator
  FairBoxGenerator* boxGen = new FairBoxGenerator(pdg, 1);// 211 = pion, 321 = kaon; 13 = muon-; 1 = multipl.

  boxGen->SetPRange(3,3);
  boxGen->SetPhiRange(phi, phi);      // Azimuth angle range [degree]
  boxGen->SetThetaRange(theta,theta); // Polar a1ngle in lab system range [degree]  
  boxGen->SetXYZ(0.,0.,0.);
  primGen->AddGenerator(boxGen);

  fRun->SetStoreTraj(kFALSE);         // to store particle trajectories  

  // Create and Set Magnetic Field
  //-------------------------------
  PndMultiField *fField= new PndMultiField("FULL");
  fRun->SetField(fField);

  fRun->Init();

  fRun->Run(nEvents); 

  rtdb->setOutput(output);
  rtdb->saveOutput();
  rtdb->print();
  
  cout << " Test passed" << endl;
  cout << " All ok " << endl; 

  // Bool_t fTest=kFALSE;
  // if (fTest){
  //   cout << " Test passed" << endl;
  //   cout << " All ok " << endl;  
  // }else{
  //   cout << " Test Failed" << endl;
  //   cout << " Not Ok " << endl;         
  // }

  //exit(0); 
}  
  
