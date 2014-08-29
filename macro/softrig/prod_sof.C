void prod_sof(TString outpre="", double pmom=15.15, int mode=9999, int runid=0)
{
	// Parameter file
	TString inParFile = outpre+"_par.root"; 
	
	// PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
		
	// Output file
	TString outFile = outpre+"_sof.root";
	
	// ------------------------------------------------------------------------
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(outpre+"_pid.root");
	fRun->SetOutputFile(outFile);
	fRun->SetWriteRunInfoFile(kFALSE);
		
	// *** initialization
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
 	
	//---------------------Create and Set the Field(s)---------- 
  	PndMultiField *fField= new PndMultiField("FULL");
  	fRun->SetField(fField);

	//RhoCalculationTools::ForceConstantBz(20.0);
  
	
	// *** HERE OUR TASK GOES!
	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(pmom, mode, runid);
	
	stTask->SetQAAll(true);
	stTask->SetGammaMinE(0.10);
	stTask->SetTrackMinP(0.10);
	
	fRun->AddTask(stTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,0);
	
	//gObjectTable->Print();
}
