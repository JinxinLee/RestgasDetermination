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

void tut_ana_d0_qahelper(TString pref="pid_complete.root", int min=-1, int max=1, int nevts=0)
{
  	TString OutFile, inParFile;
	
	double pbarmom = 9.808065;
	double mp=0.938272;
	TLorentzVector ini(0,0,pbarmom, sqrt(pbarmom*pbarmom+mp*mp)+mp);
	
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
	
	RhoCandList kp, km, pip, pim, d0, d0b, all;
	TString pidsel = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	
	RhoMassParticleSelector *fD0Sel = new RhoMassParticleSelector("D0Sel",   1.864, 1.0);
	RhoVtxPoca fVtxPoca;
	
	RhoTuple *nd0 = new RhoTuple("nd0","my D0 tuple");
	
	// *** practical helper class to dump stuff to ntuple
	PndRhoTupleQA qa(theAnalysis,pbarmom);
	
	// *** the event loop
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;	
		
		// ***
		// *** HERE YOUR ANALYSIS CODE GOES!
		// ***
		theAnalysis->FillList( all, "All",          pidsel); // only needed for PndEventShape object
		theAnalysis->FillList( kp,  "KaonAllPlus",  pidsel);
		theAnalysis->FillList( km,  "KaonAllMinus", pidsel);
		theAnalysis->FillList( pip, "PionAllPlus",  pidsel);
		theAnalysis->FillList( pim, "PionAllMinus", pidsel);
		
		PndEventShape evsh(all,ini,0.03,0.1);

		d0.Combine(km, pip);
		d0.SetType(421);
		d0b.Combine(kp, pim);
		d0b.SetType(-421);
		d0.Append(d0b);
		
		// *** do some preselection
		d0.Select(fD0Sel);
				
		// *** store all kinf of information to RhoTuple
		for (int j=0;j<d0.GetLength();++j)
		{		
			nd0->Column("ev", 		(Float_t) i);
			nd0->Column("cand",		(Float_t) j);
			nd0->Column("d0miss",	(Float_t) (ini-(d0[j]->P4())).M());
			
			qa.qaComp("d0", d0[j], nd0);

			qa.qaP4("beam", ini, nd0);
			qa.qaEventShapeShort("es",&evsh,nd0);
			
			nd0->DumpData();
		}
		
		
	}
	
	// *** write out all the histos
	out->cd();
	nd0->GetInternalTree()->Write();
	out->Save();
	out->Close();
	
}














