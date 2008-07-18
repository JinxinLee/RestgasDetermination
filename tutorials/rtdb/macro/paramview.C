{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("librtdbtut");

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile("dummyana.root");
  fRun->SetOutputFile("dummy.root");

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  CbmParRootFileIo* parInput = new CbmParRootFileIo();
  parInput->open("par2.root");
  rtdb->setFirstInput(parInput);
  fRun->LoadGeometry();
 
  // -----   Tasks to run   -------------------------------------------------
  fRun->AddTask(new PndTutAccessRTDBTask());
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  // write an ascii file from that 
  CbmParAsciiFileIo* parOutput2 = new CbmParAsciiFileIo();
  parOutput2->open("ascii-out.par","out");
  rtdb->setOutput(parOutput2);
  rtdb->saveOutput();
  parOutput2->close(); // <---- !!!!!

  fRun->Run(0);

  rtdb->print();

}

