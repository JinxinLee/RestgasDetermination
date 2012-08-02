void ana_task()
{
	TString OutFile="outanatask.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = "pid_sttcombi.root";    // this file contains the PndPidCandidates
	TString inRecoFile  = "reco_sttcombi.root";
	TString inSimFile = "points_sttcombi.root";  // this file contains the MC truth
	TString inParFile = "params_sttcombi.root";
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inSimFile);
	fRun->AddFriend(inPidFile);
	fRun->AddFriend(inRecoFile);
	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	rtdb->setFirstInput(parIO);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	
	// here the analysis task comes
  	
  	PndMyAnalysisTask *anaTask=new PndMyAnalysisTask();
  	fRun->AddTask(anaTask);

  	fRun->Init();
  	fRun->Run(0,0);
  
}
	
