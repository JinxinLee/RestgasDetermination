void ana_jpsi_task(TString Fname="test", int nevts=0, double pbarmom = 6.232069)
{
	// *** the output file for FairRunAna
	TString InFile  = Fname;
	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
	TString OutFile = Fname + "_taskana.root";  
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetWriteRunInfoFile(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(Fname+"_dummy.root");

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);

	// *** HERE OUR ANALYSIS TASK GOES!
	PndScrutAnaTask *scrutTask = new PndScrutAnaTask(pbarmom, OutFile);
	fRun->AddTask(scrutTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
