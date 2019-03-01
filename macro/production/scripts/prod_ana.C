// --------------------------------------------------------------------------------------
//
// Example analysis macro using PndSimpleCombiner(Task). !! MODIFY for your purpose !!
//
// USAGE:
// prod_ana.C( <pref>, <from>, <to>, [nevt] )
//
//    <pref>     : input/output file names prefix or full input file name
//    <from>     : first run number
//    <to>       : last run number
//    [mode]     : arbitrary mode number; default: 0
//    [nevt]     : number of events; default: 0 = all
//
// --------------------------------------------------------------------------------------

typedef std::vector<TString> StrVec;

bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn);
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("pndsim");
	if (t==0x0) fileok=false;

	if (!fileok) cout <<"Skipping broken file '"<<fn<<"'"<<endl;
	return fileok;
}

StrVec SplitString(TString s, TString delim=",")
{
	StrVec v;
	s.ReplaceAll("\t"," ");
	s += delim;
		
	while (s.Contains(delim))
	{
		TString tok = s(0,s.Index(delim));
		s.Remove(0,tok.Length()+delim.Length());
		tok = (TString)tok.Strip(TString::kBoth);
		v.push_back(tok);
	}
	
	return v;
}

StrVec ReadModeTab(TString filename)
{
	ifstream input(filename.Data(), std::ifstream::in);
	
	StrVec res;
	
	for( std::string line; getline( input, line ); )
	{
		TString modeline(line);
		if (modeline.Contains("#")) modeline = modeline(0, modeline.Index("#"));
		modeline = modeline.Strip(TString::kBoth);
		if (modeline!="") res.push_back(TString(modeline));
	}
	
	input.close();
	
	return res;
}


int prod_ana(TString prefix="", int from=1, int to=1, int mode=0, int nevts=0)
{
	// Read table with modes
	TString path     = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/production/scripts/";
	TString modefile = path+"table_modes_prod_ana.txt";
	std::vector<TString> modetab = ReadModeTab(modefile);
	int modefromtab = -1;

	// ****************************************
	// configuration for PndSimpleCombinerTask
	//
	//           APPLY CHANGES HERE!
	// ****************************************


	double   Mom      = 6.569;

	TString  anadecay = "D0 -> K- pi+; pbp->D0 D0_bar";
	TString  anaparms = "fit4cbest:mwin=0.8";

	// we look in the analysis table for analysis mode #mode
	for (int i=0; i<(int)modetab.size(); ++i)
	{
		StrVec v = SplitString(modetab[i],"//");
		
		if (v[0].Atoi() == mode)
		{
			Mom      = v[1].Atof();
			anadecay = v[2];
			anaparms = v[3];
			modefromtab = i;
			anaparms += ":algo=PidAlgoMvd;PidAlgoMdtHardCuts;PidAlgoDrc;PidAlgoStt;PidAlgoEmcBayes;PidAlgoSciT;PidAlgoFtof";
		}
	}

	// this sets fast/full sim mode automatically by checking for input file name suffix
	//bool     fastsim  = (prefix.EndsWith(".root") && prefix.Contains("_fsim")) || gSystem->AccessPathName(Form("%s_%d_pid.root",prefix.Data(),from));

	// --> if not wanted, set to either true or false
	bool     fastsim  = false;

	// the run number for PndSimpleAnalysis task;
	// running over multiple files sets run number to first input file number
	int      run      = from;

	// run software trigger (trigger definition might be outdated)
	bool     runST    = false;


	// ****************************************
	//           APPLY CHANGES HERE!
	//
	// configuration for PndSimpleCombinerTask
	// ****************************************


	// Print some help text
 	if (prefix=="" || prefix=="!")
	{
		cout << "Example analysis macro using PndSimpleCombiner(Task). !! MODIFY for your purpose !!\n\n";
		cout << "USAGE:\n";
		cout << "prod_ana.C( <pref>, <from>, <to>, [mode], [nevt] )\n\n";
		cout << "   <pref>     : input/output file names prefix or full input file name\n";
		cout << "   <from>     : first run number\n";
		cout << "   <to>       : last run number\n";
		cout << "   [mode]     : arbitrary mode number; default: 0\n";
		cout << "   [nevt]     : number of events; default: 0 = all\n\n";
		cout << "Example : root -l -b -q 'prod_ana.C(\"mysim\",1,20,10)'\n\n";
		
		if (modetab.size()>0) cout <<"\nFound "<<modetab.size()<<" analysis modes in "<<modefile<<".\n"<<endl;
		
		if (modetab.size()>0 && prefix=="!")
		{
			for (int i=0;i <(int)modetab.size(); ++i)
			{
				StrVec v = SplitString(modetab[i],"//");
				printf("(%02d)  Mode %4s @ %s = %7s GeV  :  %s  [PARM: %s]\n",  i, v[0].Data(), v[1].Atof()<0 ? "E" : "p",  v[1].Data(),   v[2].Data(), v[3].Data());
			}
			cout <<endl;
		}
	}

	// if Mom<0, interprete as -E_cm
	double mp = 0.938272;
	double Ecm = 0;

	// if mom<0, it's -E_cm -> compute mom
	if (Mom<0)
	{
		Ecm = -Mom;
		double X = (Mom*Mom-2*mp*mp)/(2*mp);
		Mom = sqrt(X*X-mp*mp);
	}
	else
	{
		Ecm = sqrt(pow(sqrt(Mom*Mom + mp*mp) + mp,2) - Mom*Mom);
	}

	// Print current analysis configuration
	cout << "------------------------------------------------\n";
	cout << "        Current analysis configuration\n";
	cout << "------------------------------------------------\n";
	printf( " Mode     : %d  (%d)\n",   mode, modefromtab);
	printf( " p_beam   : %.3f GeV/c\n", Mom);
	printf( " E_cm     : %.3f GeV\n",   Ecm);
	cout << " reco     : "<<anadecay<<endl;
	cout << " params   : "<<anaparms<<endl;
	cout << " softtrig : "<<(runST?"yes":"no")<<endl;
	cout << "------------------------------------------------\n\n";

	// if started without parameters -> stop here
	prefix.ReplaceAll("!","");
	if (prefix=="") return 0;


	TString suffix = fastsim ? "fsim" : "pid";

	TString outFile    = TString::Format("%s_ana_%d_%d.root",prefix.Data(), from, to);
	//TString inParFile  = TString::Format("%s_%d_par.root",prefix.Data(),from);
	TString firstFile  = TString::Format("%s_%d_%s.root",prefix.Data(),from,suffix.Data());

	// if prefix is a full file name, we skip the run number in the name
	if (prefix.EndsWith(".root"))
	{
		firstFile = prefix;
	    outFile   = prefix;
		outFile.ReplaceAll(".root","_ana.root");
		//inParFile = prefix; inParFile.ReplaceAll("_pid.root","_par.root");
		to = from;
	}
	// if only one file, we name outfile to 'prefix_<run>_ana.root'
	else if (from>=to)  outFile = TString::Format("%s_%d_ana.root", prefix.Data(), from);


	// Start a stop watch
	TStopwatch fTimer;
	fTimer.Start();

 	// --------------------------------
	// Create the Analysis run manager
	// --------------------------------
	FairRunAna     *fRun = new FairRunAna();
	FairFileSource *fSrc = new FairFileSource(firstFile);

  	// *** Add pid files
  	for (int i=from+1;i<=to;++i)
  	{
	  TString fname = TString::Format("%s_%d_%s.root",prefix.Data(),i,suffix.Data());
		if ( checkfile(fname) ) fSrc->AddFile(fname);
  	}

	fRun->SetSource(fSrc);

	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";

	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);

	/*
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

	// *** setup parameter database
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParRootFileIo* parIOdummy = new FairParRootFileIo();
	parIO->open("dummypar.root");
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	rtdb->setFirstInput(parIO);

	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIOdummy);
	rtdb->setContainersStatic();
	*/

	fRun->SetOutputFile(outFile);

	//---------------------Create and Set the Field(s)----------
  	//PndMultiField *fField= new PndMultiField("AUTO");
  	//fRun->SetField(fField);

	RhoCalculationTools::ForceConstantBz(20.0);

	// ***
	// *** HERE YOUR ANALYSIS CODE GOES!
	// ***


	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************

	// PID algorithm for the PndSimpleCombinerTask (for Eventshape variables)
	TString pidalgo = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts;PidAlgoSciT";
	if (fastsim) pidalgo = "PidChargedProbability";

	// allow shortcuts
	anadecay.ReplaceAll("pbp","pbarpSystem");
	anadecay.ReplaceAll("pbp0","pbarpSystem0");
	anaparms.ReplaceAll("pbp","pbarpSystem");
	anaparms.ReplaceAll("pbp0","pbarpSystem0");

	// Prevent generator from throwing a lot of warnings
	//TLorentzVector fIni(0,0,Mom,0.938272+sqrt(Mom*Mom+0.938272*0.938272));
	TDatabasePDG::Instance()->AddParticle("pbarpSystem","pbarpSystem",3,kFALSE,0.1,0, "",88888);
	TDatabasePDG::Instance()->AddParticle("pbarpSystem0","pbarpSystem0",3,kFALSE,0.1,0, "",88880);

	if (anadecay!="")
	{
	  if (fastsim) anaparms+=":algo="+pidalgo;
	  PndSimpleCombinerTask *scTask = new PndSimpleCombinerTask(anadecay, anaparms, Mom, run, mode);
	  scTask->SetPidAlgo(pidalgo);

	  fRun->AddTask(scTask);
	}

	// *****************************
	// *** PndSimpleCombinerTask ***
	// *****************************



	// *****************************
	// *** PndParticleQATask ***
	// *****************************

	// do particle QA?
	bool partQA  = (anaparms.Contains("qapart"));
	bool mc      = !(anaparms.Contains("!mc"));
	bool neut    = !(anaparms.Contains("!neut"));
	bool chrg    = !(anaparms.Contains("!chrg"));

	if (partQA)
	{
		PndParticleQATask *partQaTask = new PndParticleQATask(fastsim, chrg, neut, mc, mode); // particle QA task
		fRun->AddTask(partQaTask);
	}

	// *****************************
	// *** PndParticleQATask ***
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

	//------------------------Print some info and exit----------------
	fTimer.Stop();
	FairSystemInfo sysInfo;
	Float_t maxMemory=sysInfo.GetMaxMemory();
	Double_t rtime = fTimer.RealTime();
	Double_t ctime = fTimer.CpuTime();

	Float_t cpuUsage=ctime/rtime;

	cout << endl;
	cout << "[INFO   ] Macro call       : prod_fsim.C(\""<<prefix<<"\", "<<from<<", "<<to<<", "<<mode<<", "<<nevts<<")" <<endl;
	cout << "[INFO   ] Output file      : " << outFile << endl;
	cout << "[INFO   ] Real time        : " << rtime << " s, CPU time " << ctime << "s" << endl;
	cout << "[INFO   ] CPU usage        : " << cpuUsage*100. << "%" << endl;
	cout << "[INFO   ] Max Memory       : " << maxMemory << " MB" << endl;
	cout << "[INFO   ] Macro finished successfully." << endl<<endl;
  return 0;
}
