#include "PndMakeInputTask.h"

void makeFile(const int NumEvt = 0, const char* inPutFile = "Reco.root",
	      const char* simInFile = "SimFile.root",
	      const char* parInput = "Param.root", 
	      const char* OutPutFile = "PidOut.root",
	      const std::string TreeName ="EenTree"
	      )
{
  std::cout << "Start to generate file " << std::endl;

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  
  // Input file (MC events)
  TString inFile = inPutFile;
  
  // Parameter file
  TString parFile = parInput;

  // Output file
  TString outFile = OutPutFile;
  
  // ----  Load libraries   --------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");

  // -----   Timer   ---------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // -------------------------------------------------------------------
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
  PndMakeInputTask* pid = new PndMakeInputTask(TreeName);

  // =========================  FIXME
  fRun->AddTask(pid);
  
  //First run 
  fRun->Init();
  
  fRun->Run(0,NumEvt);
  
  rtdb->saveOutput();
  rtdb->print();
  delete pid;
  // =========================  FIXME  

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
