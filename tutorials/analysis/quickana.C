// ********************************************
// Quick analysis using PndSimpleCombinerTask/PndSimpleCombiner
// ********************************************

// The parameters are
// -------------------
// USAGE:\n";
// quickana.C+( <pref>, <decay>, [nevt], [parms] )
//    <pref>     : input file name with PndPidCandidates
//    <decay>    : the decay pattern to be reconstructed, e.g. 'phi -> K+ K-; D_s+ -> phi pi-'. charged conjugate is include unless you specify 'nocc' for certain decay
//    [nevt]     : number of events; default = 0 = all
//    [parms]    : parameters for the analysis, e.g. 'mwin=0.4:mwin(phi)=0.1:emin=0.1:pmin=0.1:qamc'

void quickana(TString Fname="", double Mom=0, TString anadecay="", int nevts=0, TString anaparms="", bool fastsim=false, bool runST=false, int run=0 )
{
	if (Fname=="" || anadecay=="") 
	{
		cout << "USAGE:\n";
		cout << "quickana.C+( <input>, <mom>, <decay>, [nevt], [parms], [fastsim], [runST], [runnum] )\n\n";
		cout << "   <input>   : input file name with PndPidCandidates\n";
		cout << "   <mom>     : pbar momentum; negative values are interpreted as -E_cm\n";
		cout << "   <decay>   : the decay pattern to be reconstructed, e.g. 'phi -> K+ K-; D_s+ -> phi pi- cc'\n";
		cout << "   [nevt]    : number of events; default: 0 = all\n";
		cout << "   [parms]   : parameters for the analysis, e.g. 'mwin=0.4:mwin(phi)=0.1:emin=0.1:pmin=0.1:qamc'\n";
		cout << "   [fastsim] : set true, if running fast sim (sets the PID algos properly); default: false'\n";
        cout << "   [runST]   : if 'true' runs Software Trigger (default: false)\n";
        cout << "   [runnum]  : integer run number (default: 0)\n\n";
		return;
	}
	
	// if Mom<0, interprete as -E_cm
	double mp = 0.938272;
	
	// if mom<0, it's -E_cm -> compute mom
	if (Mom<0)
	{
		double X = (Mom*Mom-2*mp*mp)/(2*mp);
		Mom = sqrt(X*X-mp*mp);
	}
	
	// PID algorithm for the PndSimpleCombinerTask (for Eventshape variables)
	TString pidalgo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	if (fastsim) pidalgo = "PidChargedProbability";
	
	// allow shortcuts
	anadecay.ReplaceAll("pbp","pbarpSystem");
	anadecay.ReplaceAll("pbp0","pbarpSystem0");
	
	// Prevent generator from throwing a lot of warnings
	//TLorentzVector fIni(0,0,Mom,0.938272+sqrt(Mom*Mom+0.938272*0.938272));
	TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",3,kFALSE,0.1,0, "",88888);
	TDatabasePDG::Instance()->AddParticle("pbarpSystem0","pbarpSystem0",3,kFALSE,0.1,0, "",88880);
	TDatabasePDG::Instance()->AddParticle("Z(3900)+","Z+",3.900,kFALSE,0.1,0, "",90000);
	TDatabasePDG::Instance()->AddParticle("Z(3900)-","Z-",3.900,kFALSE,0.1,0, "",-90000);
	
	// *** set this to your output path
	TString OutFile = Fname(Fname.Last('/')+1,Fname.Length()); // cut away input path
	OutFile.ReplaceAll(".root","_ana.root");
	
	// *** the output file for FairRunAna
	TString InFile  = Fname;
	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	FairRunAna* fRun = new FairRunAna();
	fRun->SetWriteRunInfoFile(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(OutFile);

	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	if (runST)
	{	
		// this file contains the trigger line definitions
		TString triggercfg, selectioncfg;
		if (fastsim) // fast sim settings
		{
			triggercfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines_fsim.cfg";         // trigger definitions
			selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/selection_fsim_dec2014.cfg";  // cut setup for complete tagging
		}
		else  // full sim settings
		{
			triggercfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines.cfg";              // trigger definitions
			selectioncfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/selection_full_dec2014.cfg";  // cut setup for complete tagging
		}
		
		PndSoftTriggerTask *stTask = new PndSoftTriggerTask(Mom, 0, 0, triggercfg);
		stTask->SetConfigurationFile(selectioncfg);
		stTask->ApplyFullSelection(1);  // apply complete tagging 
		
		if (fastsim) // set parameters for fast sim
		{
			stTask->SetPi0SignalParams(0.134, 0.0035);  // set parameters for pi0
			stTask->SetKs0SignalParams(0.497, 0.0055);  // set parameters for KS
			stTask->SetEtaSignalParams(0.549, 0.0055);  // set parameters for eta
			
			stTask->SetGammaMinE(0.10);		// global energy pre-cut for neutrals 
			stTask->SetTrackMinP(0.10);		// global momentum pre-cut for charged 	
			stTask->SetInitialPidCut(0.1);	// global PID pre-cut for charged 	
			stTask->SetDstMDiffCut(0.1);    // special cut on D*-D mass difference (to reduce comb and output file size)
		}
		else // set parameters for full sim
		{
			stTask->SetPi0SignalParams(0.134, 0.0035);  // set parameters for pi0
			stTask->SetKs0SignalParams(0.497, 0.0055);  // set parameters for KS
			stTask->SetEtaSignalParams(0.549, 0.0055);  // set parameters for eta
			
			stTask->SetGammaMinE(0.10);		// global energy pre-cut for neutrals 
			stTask->SetTrackMinP(0.10);		// global momentum pre-cut for charged 	
			stTask->SetInitialPidCut(0.1);	// global PID pre-cut for charged 	
			stTask->SetDstMDiffCut(0.1);    // special cut on D*-D mass difference (to reduce comb and output file size)
		}
		
		// set PID algos
		stTask->SetPidAlgoAll(pidalgo);
		
		stTask->SetTagAll(true);		// tag all modes
		stTask->SetQAAll(false);        // don't write any QA tuple	
		stTask->SetQAEvent(true);       // don't write any QA tuple	
		
		fRun->AddTask(stTask);
	}
	
	// --------------------------------
	// *** Analysis Task ***
	// --------------------------------

	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************
	if (fastsim) anaparms+=":algo="+pidalgo;
	PndSimpleCombinerTask *scTask = new PndSimpleCombinerTask(anadecay, anaparms, Mom, run);
	scTask->SetPidAlgo(pidalgo);
	fRun->AddTask(scTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
