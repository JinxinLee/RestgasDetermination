void ana_triple_task(TString Fname="test", int mode, int nevts=0, double pbarmom = 9.0)
{
	// *** the output file for FairRunAna
	TString InFile  = Fname;
	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetWriteRunInfoFile(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(Fname+"_dummy.root");

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// *** HERE OUR ANALYSIS TASK GOES!
	PndTripleAnaTask *tripTask = new PndTripleAnaTask(pbarmom, Fname+"_taskana.root",mode,"PidChargedProbability");
	fRun->AddTask(tripTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
