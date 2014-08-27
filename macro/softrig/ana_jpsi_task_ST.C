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
	stTask->SetPi0SignalParams(0.135, 0.01);
	stTask->SetEtaSignalParams(0.547, 0.03);
	stTask->SetKs0SignalParams(0.493, 0.05);
	
	//TString algo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts"; // FullSim
	TString algo = "PidChargedProbability";	// Fast Sim
	stTask->SetPidAlgoAll(algo);
	
	stTask->ApplyFullSelection();
	stTask->SetTag_All(true);		// tag all modes
	//stTask->SetTag_Mode(120);     // example to switch single tags on/off; mode number has to match one from config file
	
	stTask->SetQA_All(true);		// ntuple output for all modes
	//stTask->SetQA_Mode(120);      // example to switch single QA on/off
	
	stTask->SetGammaMinE(0.15);		// global energy pre-cut for neutrals 
	stTask->SetTrackMinP(0.15);		// global momentum pre-cut for charged 	
	//stTask->SetInitialPidCut(0.1);	// global PID pre-cut for charged 	
	
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
