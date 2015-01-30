// ********************************************
// Quick analysis using PndSimpleCombinerTask/PndSimpleCombiner
// ********************************************

// The parameters are
// -------------------
// USAGE:\n";
// quickana.C+( <pref>, <decay>, [nevt], [parms] )
//    <pref>     : output file names prefix
//    <decay>    : the decay pattern to be reconstructed, e.g. 'phi -> K+ K-; D_s+ -> phi pi- cc'
//    [nevt]     : number of events; default = 0 = all
//    [parms]    : parameters for the analysis, e.g. 'mwin=0.4:mwin(phi)=0.1:emin=0.1:pmin=0.1:qamc'

void quickana(TString Fname="test", TString anadecay="", int nevts=0, TString anaparms="")
{
	if (Fname=="" || anadecay=="") 
	{
		cout << "USAGE:\n";
		cout << "quickana.C+( <filename>, <decay> ,[nevt], [parms] )\n\n";
		cout << "   <pref>    : input file name with PndPidCandidates\n";
		cout << "   <decay>   : the decay pattern to be reconstructed, e.g. 'phi -> K+ K-; D_s+ -> phi pi- cc'\n";
		cout << "   [nevt]    : number of events; default = 0 = all\n";
		cout << "   [parms]   : parameters for the analysis, e.g. 'mwin=0.4:mwin(phi)=0.1:emin=0.1:pmin=0.1:qamc'\n\n";
		return;
	}
	
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
	

	// --------------------------------
	// *** Analysis Task ***
	// --------------------------------

	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************
	
	PndSimpleCombinerTask *scTask = new PndSimpleCombinerTask(anadecay, anaparms);
	scTask->SetPidAlgo("PidChargedProbability");
	fRun->AddTask(scTask);

	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
}
