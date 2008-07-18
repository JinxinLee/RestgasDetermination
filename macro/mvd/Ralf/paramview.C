{

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");

  TString parFile = "../data/mvddpm6GeV_digipar.root";

  CbmRunAna *fRun= new CbmRunAna();

  fRun->SetInputFile("../data/mvddpm6GeV_digi.root");
  fRun->SetOutputFile("../data/dummy.root");

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  fRun->LoadGeometry();
  fRun->AddTask(new PndMvdAccessRTDBTask());
  fRun->Init();
  fRun->Run(0);
  rtdb->print();

}

