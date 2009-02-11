{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  FairRunSim *fRun = new FairRunSim();

  fRun->SetOutputFile("dummysim.root");
  fRun->SetMaterials("media_pnd.geo");

  FairModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  FairRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open("par.root","RECREATE");
  rtdb->setOutput(output);

  fRun->Init();
  fRun->Run(0);
  rtdb->saveOutput();
  rtdb->print();
}

