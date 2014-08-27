// Check if file exists and readable
bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn, "READ"); 
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("cbmsim");
	if (t==0x0) fileok=false;
	
	if (!fileok) cout <<"Skipping broken file '"<<fn<<"'"<<endl;
	fff.Close();
	
	return fileok;
}

// Add many input files to FairRunAna
void attachFiles(FairRunAna* fRun, TString pref, int min, int max)
{
	bool firstfile=true;
	for (int i=min;i<=max;++i) {
		TString fname = TString::Format("%s_%d_pid.root",pref.Data(),i);

		if (checkfile(fname) ) {
			if (firstfile) fRun->SetInputFile(fname);
			else fRun->AddFile(fname);
			firstfile=false;
		}
	}
}

void qa_softtrig(TString pref="M9999", int mode, double pmom, int from=1, int to=1, int nEvents=0, int run=0)
{
  	TString OutFile   = TString::Format("%s_%d_%d_sof.root",pref.Data(),from, to); 
	
  	FairRunAna *fRun= new FairRunAna();
	
	// attach input files according to parameters
	attachFiles(fRun, pref, from, to);
	
	// turn off FairLogger output
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
	fRun->SetOutputFile(OutFile);
 	
	//---------------------Create and Set the Field(s)---------- 
  	//PndMultiField *fField= new PndMultiField("FULL");
  	//fRun->SetField(fField);
	
	RhoCalculationTools::ForceConstantBz(20.0);
  
	// *** SoftTriggerTask ***
	
	// this file contains the trigger line definitions
	TString triggercfg   = TString(gSystem->Getenv("VMCWORKDIR"))+"/triggerlines.cfg";
	
	// this file contains the cut setup for various modes
	// TString selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/selection_10ch_tight.cfg"; 
	TString selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/selection_10ch_loose.cfg"; 
	
	PndSoftTriggerTask *stTask = new PndSoftTriggerTask(pmom, mode, run, triggercfg);
	stTask->SetConfigurationFile(selectioncfg);
	
	//TString algo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts"; // FullSim
	TString algo = "PidChargedProbability";	
	stTask->SetPidAlgoAll(algo);
	
	stTask->SetTag_All(true);		// tag all modes
	//stTask->SetTag_Mode(120);     // switch single tags on/off; mode number has to match one from config file
	
	stTask->SetQA_All(true);		// ntuple output for all modes
	//stTask->SetQA_Mode(120);      // switch single QA on/off
	
	stTask->SetGammaMinE(0.10);		// global energy pre-cut for neutrals 
	stTask->SetTrackMinP(0.10);		// global momentum pre-cut for charged 	
	stTask->SetInitialPidCut(0.1);	// global PID pre-cut for charged 	
	
	fRun->AddTask(stTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nEvents);
}
