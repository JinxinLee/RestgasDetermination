bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn); 
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("cbmsim");
	if (t==0x0) fileok=false;
	
	if (!fileok) cout <<"Skipping broken file '"<<fn<<"'"<<endl;
	return fileok;
}

void prod_ana(TString outpre="", int from=1, int to=1, int mode=0, int nevts=0)
{
 	if (outpre=="") 
	{
		cout << "Example analysis macro using PndSimpleCombiner(Task). !! MODIFY for your purpose !!\n\n";
		cout << "USAGE:\n";
		cout << "prod_ana.C( <pref>, <from>, <to>, [nevt] )\n\n";
		cout << "   <pref>     : output file names prefix\n";
		cout << "   <from>     : first run number\n";
		cout << "   <to>       : last run number\n";
		cout << "   [mode]     : arbitrary mode number; default: 0\n";
		cout << "   [nevt]     : number of events; default: 0 = all\n\n";
		return;
	}
	
	TString OutFile1   = TString::Format("%s_ana1_%d_%d.root",outpre.Data(), from, to); 
 	TString OutFile2   = TString::Format("%s_ana2_%d_%d.root",outpre.Data(), from, to); 
	TString inParFile  = TString::Format("%s_%d_par.root",outpre.Data(),from);
	
  	FairRunAna *fRun= new FairRunAna();
  
   	bool firstfile=true;

  	// *** Add pid files
  	for (int i=from;i<=to;++i)
  	{
		TString fname = TString::Format("%s_%d_pid.root",outpre.Data(),i);

		if ( checkfile(fname) )
		{
			if (firstfile) 
				fRun->SetInputFile(fname);
			else 
				fRun->AddFile(fname);
			
			firstfile=false;
		}
  	}
  	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	//FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	
	// *** setup parameter database 	
	//FairParRootFileIo* parIO = new FairParRootFileIo();
	//parIO->open(inParFile);
	//FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	//parIOPid->open(pidParFile.Data(),"in");
	
	//rtdb->setFirstInput(parIO);
	//rtdb->setSecondInput(parIOPid);
	//rtdb->setOutput(parIO);  
	//rtdb->setContainersStatic();
	
	fRun->SetOutputFile(OutFile1);
	
	//---------------------Create and Set the Field(s)---------- 
  	PndMultiField *fField= new PndMultiField("AUTO");
  	fRun->SetField(fField);
	
	//RhoCalculationTools::ForceConstantBz(20.0);

	// ***
	// *** HERE YOUR ANALYSIS CODE GOES!
	// ***
	
	// configuration for PndSimpleCombinerTask (see below)
    double   Mom      = -3.872;
	
	TString  anadecay = "J/psi->e+ e-; rho0->pi+ pi-; pbarpSystem->J/psi rho0";
	TString  anaparms = "qaevtshape:qamc:fit4c:mwin(J/psi)=2.0|3.4:mwin(rho0)=0.27|1.0:!ntp1";
		
	bool     fastsim  = false;
	int      run      = from;	
	
	// run software trigger (trigger definition might be outdated)
	bool     runST    = false;
	

	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************
		
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
	
	if (fastsim) anaparms+=":algo="+pidalgo;
	PndSimpleCombinerTask *scTask = new PndSimpleCombinerTask(anadecay, anaparms, Mom, run, mode);
	scTask->SetPidAlgo(pidalgo);
	
	fRun->AddTask(scTask);
	
	
	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************
	
	// ***********************
	// *** SoftTriggerTask ***
	// ***********************
	
	if (runST)
	{	
		// this file contains the trigger line definitions
		TString      triggercfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines.cfg";       // fullsim trigger definitions 		
		if (fastsim) triggercfg = TString(gSystem->Getenv("VMCWORKDIR"))+"/softrig/triggerlines_fsim.cfg";  // fastsim trigger definitions	
		
		PndSoftTriggerTask *stTask = new PndSoftTriggerTask(Mom, 0, run, triggercfg);
		
		if (fastsim) stTask->SetFastSimDefaults();
		else         stTask->SetFullSimDefaults();
				
		fRun->AddTask(stTask);
	}
	
	// ***********************
	// *** SoftTriggerTask ***
	// ***********************


	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);	
	
}

















