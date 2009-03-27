/*
 * Macro to use the trained classifiers to perform 
 * particle classification.
 * Created by:
 * S.Vanniarajan
 * Modified:
 * M.Babai
 */
void run_pid_reco(const int NumEvt = 0, const char* inPutFile = "RecoOut.root",
		  const char* simInFile = "SimOut.root",
		  const char* parInput = "ParamOut.root", 
		  const char* OutPutFile = "PidTaskOutPut.root")
{
  // ================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  // Input file (MC events)
  //TString inFile = "el_reco.root";
  TString inFile = inPutFile;
  
  // Parameter file
  //TString parFile = "params_sttcombi.root";
  TString parFile = parInput;

  // Output file
  //TString outFile = "pid_OutPut.root";
  TString outFile = OutPutFile;

  // ----  Load libraries   --------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // -------------------------------------------------------------------
  // -----   Timer   ---------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // -------------------------------------------------------------------
  
  //PndEmcMapper *emcMap=PndEmcMapper::Instance(2,".root");
  PndEmcMapper *emcMap=PndEmcMapper::Instance(2, simInFile);
  
  // -----   Digitization run   ----------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  
  
  TString allDigiFile = sysFile+"/macro/params/all.par";
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
  
  // Create the PID task
  PndGpidTaskLhe* pid = new PndGpidTaskLhe();
  
  // Set Task parameters
  pid->SetAPPNAME("test");
  pid->SetDIR("./weights/");
  
  /*
   * Select which MVA to use for classification and set the parameters.
   * Possible MVA's are:
   * TMKNN, TMBDT, TMMLP, MulClsKNN, LVQ1
   */
  MVAType bla = LVQ1;
  pid->SetInFileName("LVQ1TestOut.root");
  //pid->SetInFileName("LVQ2TestOut.root");
  pid->SetMVA(bla);
  
  fRun->AddTask(pid);
  fRun->Init();
 
  fRun->Run(0, NumEvt);
  
  rtdb->saveOutput();
  //rtdb->print();
  
  // ----------------------------------------------------------------
  // -----   Finish   -----------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " 
       << ctime << " s" << endl;
  cout << endl;
  exit(0);
}
