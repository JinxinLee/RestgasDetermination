{
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("librtdbtut");

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("dummyana.root");
  fRun->SetOutputFile("dummy.root");

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput = new FairParRootFileIo();
  parInput->open("par2.root");
  rtdb->setFirstInput(parInput);
  fRun->LoadGeometry();
 
  // -----   Tasks to run   -------------------------------------------------
  fRun->AddTask(new PndTutAccessRTDBTask());
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  // write an ascii file from that 
  FairParAsciiFileIo* parOutput2 = new FairParAsciiFileIo();
  parOutput2->open("ascii-out.par","out");
  rtdb->setOutput(parOutput2);
  rtdb->saveOutput();
  parOutput2->close(); // <---- !!!!!

  fRun->Run(0);

  rtdb->print();

}

