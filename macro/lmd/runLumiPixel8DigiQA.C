void runLumiPixel8DigiQA(const int nEvents=100000, const int startEvent=0, TString storePath="tmpOutnewDesign", const int verboseLevel=5)
{
  // ========================================================================
  // Input file (MC events)
  TString MCFile = storePath+"/Lumi_MC_";
  MCFile += startEvent;
  MCFile += ".root";
  // Digi file
  TString DigiFile = storePath+"/Lumi_digi_";
  DigiFile += startEvent;
  DigiFile += ".root";
  // Parameter file
  TString parFile = storePath+"/Lumi_Params_";
  parFile += startEvent;
  parFile += ".root";
  
  // Output file
  TString DumFile = storePath+"/Lumi_DigisQA_";
  DumFile += startEvent;
  DumFile += ".root";

 
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//-lRooFit –lRooFitCore -lMinuit
 
  gSystem->Load("libSds");
  gSystem->Load("libSdsReco");
  gSystem->Load("libLmd");
  gSystem->Load("libLmdReco");
  gSystem->Load("libLmdTool"); 
  // ------------------------------------------------------------------------
    
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->SetOutputFile(DumFile);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  PndLmdDigiQTask *lmdqa = new PndLmdDigiQTask("LMDPixelDigis");
  lmdqa->SetVerbose(verboseLevel);
  //lmdqa->SetWriteMC(wrAllMC);
  fRun->AddTask(lmdqa);
  fRun->SetWriteRunInfoFile(kFALSE);
  //rtdb->setOutput(parInput1);
  //  rtdb->print();

  fRun->Init();

  fRun->Run(0,nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
