void ana_jpsi_task(TString input="test_fast.root", int nevts=0, double pmom = 6.231552)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** the output file for FairRunAna
	TString OutFile = input+"_taskana.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = input;    // this file contains the PndPidCandidates and McTruth
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	fRun->SetOutputFile(OutFile);
	
	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// *** HERE OUR ANALYSIS TASK GOES!
	PndScrutAnaTask *scrutTask = new PndScrutAnaTask(pmom);
	fRun->AddTask(scrutTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
