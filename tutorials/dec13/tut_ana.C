class FairRunAna;

// Check if file exists and readable
bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn, "READ");
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("pndsim");
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

void tut_ana(TString pref="pid_complete.root", int min=-1, int max=1, int nevts=0)
{
  	TString OutFile, inParFile;

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

	fRun->SetOutputFile("dummyout.root");
	fRun->Init();

	// *** create an output file for all histograms
	TFile *out = TFile::Open(OutFile,"RECREATE");

	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();

	int i=0;

	// *** the event loop
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;

		// ***
		// *** HERE YOUR ANALYSIS CODE GOES!
		// ***

	}

	// *** write out all the histos
	out->cd();
	out->Save();
	out->Close();

}
