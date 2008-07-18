{

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("librtdbtut");

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile("dummysim.root");
  fRun->SetOutputFile("dummyana.root");

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open("par.root","READ");//RECREATE etc. like in TFile from root
  rtdb->setFirstInput(parInput1);

  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  parInput2->open("../ascii-example.par","in");
  rtdb->setSecondInput(parInput2);

  // -----   Tasks to run   -------------------------------------------------
  fRun->AddTask(new PndTutAccessRTDBTask());

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  Bool_t kParameterMerged=kTRUE;
  CbmParRootFileIo* parOutput = new CbmParRootFileIo(kParameterMerged);
  parOutput->open("par2.root","RECREATE");//RECREATE etc. like in TFile from root
  rtdb->setOutput(parOutput);
  rtdb->saveOutput();

  rtdb->print();

  fRun->Run(0);

}
