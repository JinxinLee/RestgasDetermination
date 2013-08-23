void tut_ana_task(int nevts=0)
{
	TString OutFile="output_ana_task.root";  
					
	// *** the files coming from the simulation
	TString inPidFile = "pid_complete.root";    // this file contains the PndPidCandidates and McTruth
	TString inParFile = "simparams.root";

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libtutrhotask");
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	
	// *** HERE OUR TASK GOES!
	PndTutAnaTask *anaTask = new PndTutAnaTask();
	fRun->AddTask(anaTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);
}
