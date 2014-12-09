void prod_sof(TString outpre="", Float_t Mom=15.15, int mode=9999, int run=-1, int applyfull=0)
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
  	PndMultiField *fField= new PndMultiField("AUTO");
  	fRun->SetField(fField);

	//RhoCalculationTools::ForceConstantBz(20.0);
  
	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	int modeshort = mode;
	if (modeshort>1000000) modeshort /= 1000;
	if (modeshort>1000)    modeshort %= 1000;
	
	cout <<" ****** mode:"<<mode<<"  modeshort:"<<modeshort<<endl;
	
	// this file contains the trigger line definitions
	//TString triggercfg   = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/softrig/triggerlines_10ch.cfg";
	TString triggercfg   = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines.cfg";
	
	// this file contains the cut setup for various modes
	//TString selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/selection_10ch_tight.cfg"; 
	TString selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/selection.cfg"; 
	
	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(Mom, mode, run, triggercfg);
	stTask->SetConfigurationFile(selectioncfg);
	
	//stTask->McMatchAllowPhotos(10, 1.0);
		
	stTask->ApplyFullSelection(applyfull);
	
	stTask->SetPi0SignalParams(0.134, 0.0035);
	stTask->SetKs0SignalParams(0.497, 0.0055);
	stTask->SetEtaSignalParams(0.549, 0.0055);
	
	// set PID algos
	TString algo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts;PidAlgoMvd";	
	stTask->SetPidAlgoAll(algo);
	
	stTask->SetTagAll(true);		// tag all modes
	//stTask->SetTagMode(modeshort);  // switch single tags on/off; mode number has to match one from config file
	
	if (modeshort==900)	
	{
		stTask->SetQAAll(true);		// ntuple output for all modes
	}
	else
	{
		stTask->SetQAAll(false);
		stTask->SetQAMode(modeshort);
		stTask->SetQAMctOnly();
	}
	
	stTask->SetQAEvent(true);		// event info
	stTask->SetQAMc(true);
	
	stTask->SetGammaMinE(0.10);		// global energy pre-cut for neutrals 
	stTask->SetTrackMinP(0.10);		// global momentum pre-cut for charged 	
	stTask->SetInitialPidCut(0.1);	// global PID pre-cut for charged 	
	stTask->SetDstMDiffCut(0.1);    // special cut on D*-D mass difference (to reduce comb and output file size)
	
	//stTask->McMatchAllowPhotos(1,0.05);  // MC truth match shall partially ignore photos photons (here max 1 with E<50 MeV)

	fRun->AddTask(stTask);

	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,0);
	
	//gObjectTable->Print();
}
