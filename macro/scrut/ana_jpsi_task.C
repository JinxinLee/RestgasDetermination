void ana_jpsi_task(TString Fname="test", int nevts=0, double pbarmom = 6.232069)
{
	// *** set this to your output path
	TString OutPath = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/scrut/data";	
	TString OutFile = OutPath + "/" + Fname(Fname.Last('/')+1,Fname.Length()); // cut away input path
		
	// *** the output file for FairRunAna
	TString InFile  = Fname;
	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetGenerateRunInfo(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(OutFile+"_dummy.root");

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);

	// *** HERE OUR ANALYSIS TASK GOES!
	PndScrutAnaTask *scrutTask = new PndScrutAnaTask(pbarmom, OutFile+"_taskana.root");
	fRun->AddTask(scrutTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
