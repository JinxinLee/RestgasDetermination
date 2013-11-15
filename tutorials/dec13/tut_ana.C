class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;
class FairRunAna;

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

void attachFiles(FairRunAna* fRun, TString pref, int min, int max)
{
	bool firstfile=true;
	// *** set output file and par file

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
	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
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
	// *** open many input files
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
		
	// 
	// Now the analysis stuff comes...
	// 
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
		
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;	

	}
	
	// *** write out all the histos
	out->cd();
		
	out->Save();
	
}
