{

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");

  TString parFile = "../data/mvddpm6GeV_digipar.root";

  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile("../data/mvddpm6GeV_digi.root");
  fRun->SetOutputFile("../data/dummy.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  fRun->LoadGeometry();
  fRun->AddTask(new PndMvdAccessRTDBTask());
  fRun->Init();
  fRun->Run(0);
  rtdb->print();

}

