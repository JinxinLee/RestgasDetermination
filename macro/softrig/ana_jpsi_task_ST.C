// ********************************************
// Example analysis in a task with accessing the OnlineEventFilterInfo
// written by the SoftTriggerTask
// ********************************************

void ana_jpsi_task_ST(TString Fname="test", int nevts=0, double pbarmom = 6.232069)
{
	// *** set this to your output path
	TString WorkDir = TString(gSystem->Getenv("VMCWORKDIR"));
	TString OutPath = WorkDir+"/macro/softrig";	
	TString OutFile = OutPath + "/" + Fname(Fname.Last('/')+1,Fname.Length()); // cut away input path
		
	// *** the output file for FairRunAna
	TString InFile  = Fname;
	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetWriteRunInfoFile(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(OutFile+"_loose.root");

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// --------------------------------
	// *** SoftTriggerTask ***
	// --------------------------------
	
	// this file contains the trigger line definitions
	TString triggercfg   = WorkDir+"/softrig/triggerlines.cfg";
	
	// this file contains the cut setup for the 10 channels from soft trigger report
	TString selectioncfg = WorkDir+"/softrig/selection_10ch_loose.cfg"; 
	
	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(pbarmom,0,0,triggercfg);
	stTask->SetConfigurationFile(selectioncfg);
	
	// set parameters for pi0, KS, eta selection
	stTask->SetPi0SignalParams(0.136, 0.0045);
	stTask->SetEtaSignalParams(0.552, 0.009);
	stTask->SetKs0SignalParams(0.497, 0.008);
	
	//TString algo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts"; // FullSim
	TString algo = "PidChargedProbability";	// Fast Sim
	stTask->SetPidAlgoAll(algo);
	
	// Full selection switch
	// 0: detailed selection turned off
	// 1: exclusive mode (modes w/o detailed cut definitions are rejected)
	// 2: open mode (modes w/o detailed cut definitions just have to fulfill their mass window criterion)
	stTask->ApplyFullSelection(1);
	
	stTask->SetTagAll(true);         // tag all modes
	//stTask->SetTagMode(120,true);  // example to switch single tags on/off; mode number has to match one from config file
	
	stTask->SetQAAll(true);          // ntuple output for all modes off
	stTask->SetQAEvent(true);        // ntuple output for event info
	stTask->SetQAMode(201,true);     // example to switch single QA on/off
	
	stTask->SetGammaMinE(0.15);      // global energy pre-cut for neutrals 
	stTask->SetTrackMinP(0.15);      // global momentum pre-cut for charged 	
	stTask->SetInitialPidCut(0.1);   // global PID pre-cut for charged 	
	
	stTask->McMatchAllowPhotos(1,0.05);  // MC truth match shall partially ignore photos photons (here max 1 with E<50 MeV)
	
	fRun->AddTask(stTask);

	// --------------------------------
	// *** Analysis Task ***
	// --------------------------------

	PndAnaWithTrigger *anaTask = new PndAnaWithTrigger(pbarmom, OutFile+"_ana.root");
	fRun->AddTask(anaTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
