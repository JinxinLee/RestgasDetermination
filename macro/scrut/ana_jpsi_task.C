void ana_jpsi_task(TString InFile="test_fast.root", int nevts=0, double pbarmom = 6.232069)
{
	// *** the output file for FairRunAna
	TString OutFile = InFile+"_taskana.root";  
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetWriteRunInfoFile(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile("fairout_dummy.root");

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);

	// *** HERE OUR ANALYSIS TASK GOES!
	PndScrutAnaTask *scrutTask = new PndScrutAnaTask(pbarmom, OutFile);
	fRun->AddTask(scrutTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
