class FairRunAna;

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

void tut_ana_task_d0(TString pref="pid_complete.root", int min=-1, int max=1,  int nevts=0)
{
  	TString OutFile, inParFile;
	
	// pbarmom for analysis task
	double pbarmom = 9.808065;
	
  	// *** add input files
   	FairRunAna *fRun= new FairRunAna();
	
	// *** just open one file
	if (min<0)
	{
		// *** set input file, output file and par file
		OutFile   = "ana.root";
		inParFile = "simparams.root";

		fRun->SetInputFile(pref);
	}
	// *** attach many input files
	else {
		// *** set output file and par file
		OutFile   = TString::Format("%s_ana_%d_%d.root",pref.Data(), min, max);
		inParFile =	TString::Format("%s_%d_par.root",pref.Data(), min);
		
		attachFiles(fRun, pref, min, max);
	}
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	
	// *** HERE OUR TASK GOES!
	PndTutAnaTaskD0 *anaTask = new PndTutAnaTaskD0(pbarmom);
	fRun->AddTask(anaTask);
	
	// *** and run analysis
	fRun->Init(); 
	fRun->Run(0,nevts);
}
