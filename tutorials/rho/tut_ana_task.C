void tut_ana_task(int nevts=0)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	TString OutFile="output_ana_task.root";  
					
	// *** the files coming from the simulation
	TString inPidFile = "pid_complete.root";    // this file contains the PndPidCandidates and McTruth
	TString inParFile = "simparams.root";
	
	gStyle->SetOptFit(1011);
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gSystem->Load("libtutrhotask");
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	rtdb->setFirstInput(parIO);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	
	// *** HERE OUR TASK GOES!
	PndTutAnaTask *anaTask = new PndTutAnaTask();
	fRun->AddTask(anaTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);
}
