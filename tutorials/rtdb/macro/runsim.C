{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  CbmRunSim *fRun = new CbmRunSim();

  fRun->SetOutputFile("dummysim.root");
  fRun->SetMaterials("media_pnd.geo");

  CbmModule *Cave= new PndCave("CAVE");
  Cave->SetGeometryFileName("pndcave.geo");
  fRun->AddModule(Cave); 

  CbmPrimaryGenerator* primGen = new CbmPrimaryGenerator();
  fRun->SetGenerator(primGen);

  CbmRuntimeDb *rtdb=fRun->GetRuntimeDb();
  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
  output->open("par.root","RECREATE");
  rtdb->setOutput(output);

  fRun->Init();
  fRun->Run(0);
  rtdb->saveOutput();
  rtdb->print();
}

