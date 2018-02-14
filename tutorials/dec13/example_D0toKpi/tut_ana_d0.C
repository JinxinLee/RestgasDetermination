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

void tut_ana_d0(TString pref="pid_complete.root", int min=-1, int max=1, int nevts=0)
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
	
	RhoCandList kp, km, pip, pim, d0, d0b;
	TString pidsel = "PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoStt;PidAlgoMdtHardCuts";
	
	RhoMassParticleSelector *fD0Sel = new RhoMassParticleSelector("D0Sel",   1.864, 1.0);
	RhoVtxPoca fVtxPoca;
	
	RhoTuple *nd0 = new RhoTuple("nd0","my D0 tuple");
	
	// *** the event loop
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;	
		
		// ***
		// *** HERE YOUR ANALYSIS CODE GOES!
		// ***
		theAnalysis->FillList( kp,  "KaonAllPlus",  pidsel);
		theAnalysis->FillList( km,  "KaonAllMinus", pidsel);
		theAnalysis->FillList( pip, "PionAllPlus",  pidsel);
		theAnalysis->FillList( pim, "PionAllMinus", pidsel);

		d0.Combine(km, pip);
		d0.SetType(421);
		d0b.Combine(kp, pim);
		d0b.SetType(-421);
		d0.Append(d0b);
		
		d0.Select(fD0Sel);
		
		for (int j=0;j<d0.GetLength();++j)
		{
			RhoCandidate *dka = d0[j]->Daughter(0);
			RhoCandidate *dpi = d0[j]->Daughter(1);
			
			PndPidCandidate *dkar = (PndPidCandidate*)dka->GetRecoCandidate();
			PndPidCandidate *dpir = (PndPidCandidate*)dpi->GetRecoCandidate();

			TLorentzVector d0cm = d0[j]->P4();
			TLorentzVector kacm = dka->P4();
			TLorentzVector picm = dpi->P4();
			d0cm.Boost(-ini.BoostVector());
			kacm.Boost(-ini.BoostVector());
			picm.Boost(-ini.BoostVector());
					
			bool mct = theAnalysis->McTruthMatch(d0[j]);
			
			TVector3 vtx;
			double qavtx = fVtxPoca.GetPocaVtx(vtx, d0[j]);
		
			nd0->Column("ev", 		(Float_t) i);
			nd0->Column("cand",		(Float_t) j);
			nd0->Column("pdg",		(Float_t) d0[j]->PdgCode());
			nd0->Column("mct",		(Float_t) mct);
			
			nd0->Column("d0m",		(Float_t) d0[j]->M());
			nd0->Column("d0p",		(Float_t) d0[j]->P());
			nd0->Column("d0tht",	(Float_t) d0[j]->P3().Theta());
			nd0->Column("d0phi",	(Float_t) d0[j]->P3().Phi());
			nd0->Column("d0pt",		(Float_t) d0[j]->P3().Pt());
			nd0->Column("d0pcm",	(Float_t) d0cm.P());
			nd0->Column("d0thtcm",	(Float_t) d0cm.Theta());
			nd0->Column("d0miss",	(Float_t) (ini-(d0[j]->P4())).M());
			
			nd0->Column("d0vx",		(Float_t) vtx.X());
			nd0->Column("d0vy",		(Float_t) vtx.Y());
			nd0->Column("d0vz",		(Float_t) vtx.Z());
			nd0->Column("d0vqa",	(Float_t) qavtx);
			
			nd0->Column("kch",		(Float_t) dka->Charge());
			nd0->Column("kp",		(Float_t) dka->P());
			nd0->Column("ktht",		(Float_t) dka->P3().Theta());
			nd0->Column("kphi",		(Float_t) dka->P3().Phi());
			nd0->Column("kpt",		(Float_t) dka->P3().Pt());
			nd0->Column("kpid",		(Float_t) dka->GetPidInfo(3));
			nd0->Column("kapcm",	(Float_t) kacm.P());
			nd0->Column("kathtcm",	(Float_t) kacm.Theta());
			
			nd0->Column("pich",		(Float_t) dpi->Charge());
			nd0->Column("pip",		(Float_t) dpi->P());
			nd0->Column("pitht",	(Float_t) dpi->P3().Theta());
			nd0->Column("piphi",	(Float_t) dpi->P3().Phi());
			nd0->Column("pit",		(Float_t) dpi->P3().Pt());
			nd0->Column("pipid",	(Float_t) dpi->GetPidInfo(2));
			nd0->Column("pipcm",	(Float_t) picm.P());
			nd0->Column("pithtcm",	(Float_t) picm.Theta());
			
			nd0->DumpData();
		}
		
		
	}
	
	// *** write out all the histos
	out->cd();
	nd0->GetInternalTree()->Write();
	out->Save();
	out->Close();
	
}














