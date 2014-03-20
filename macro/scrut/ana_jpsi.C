void ana_jpsi(TString input="test_fast.root", int nevts=0, double pbarmom = 6.231552)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** the output file for FairRunAna
	TString OutFile="output.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = input;    // this file contains the PndPidCandidates and McTruth
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	fRun->SetOutputFile(OutFile);
	fRun->Init(); 
	
	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(input+"_ana.root","RECREATE");
	
	// *** create some ntuples
	RhoTuple *ntp1 = new RhoTuple("ntp1", "jpsi analysis");
	RhoTuple *ntp2 = new RhoTuple("ntp2", "psi(2S) analysis");
	RhoTuple *nmc  = new RhoTuple("nmc",  "mctruth info");
	
	//
	// Now the analysis stuff comes...
	//
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, psi2s, all, mclist;
	
	// *** Mass selector for the jpsi cands
	double m0_jpsi = TDatabasePDG::Instance()->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",m0_jpsi,1.0);
	
	// *** the lorentz vector of the initial psi(2S)
	double m0_p    = TDatabasePDG::Instance()->GetParticle("proton")->Mass();   // Get nominal PDG mass of the proton
	TLorentzVector ini(0, 0, pbarmom, sqrt(m0_p*m0_p + pbarmom*pbarmom) + m0_p);
	
	// ***
	// the event loop
	// ***
	
	TString pidalg = "PidChargedProbability";
	
	PndRhoTupleQA qa(theAnalysis, pbarmom); 
	
	TLorentzVector LVdummy(0,0,0,0);
	
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
		
		// *** get MC list
		theAnalysis->FillList(mclist, "McTruth");
		for (j=0;j<mclist.GetLength();++j)
		{
			nmc->Column("ev",	 (Float_t) i);
			nmc->Column("part",	 (Float_t) j);
			nmc->Column("npart", (Float_t) mclist.GetLength());
			
			qa.qaCand("mc",mclist[j], nmc);
			nmc->DumpData();
		}
		
		// *** Setup event shape object
		theAnalysis->FillList(all,  "All", pidalg);
		PndEventShape evsh(all, ini, 0.05, 0.1);
								
		// *** Select with no PID info ('All'); type and mass are set 		
		theAnalysis->FillList(muplus,  "MuonAllPlus", pidalg);
		theAnalysis->FillList(muminus, "MuonAllMinus", pidalg);
		theAnalysis->FillList(piplus,  "PionAllPlus", pidalg);
		theAnalysis->FillList(piminus, "PionAllMinus", pidalg);
		
		// *** combinatorics for J/psi -> mu+ mu-
		jpsi.Combine(muplus, muminus);		
		jpsi.SetType(443);
		int njmct = theAnalysis->McTruthMatch(jpsi);
				
		for (j=0;j<jpsi.GetLength();++j) 
		{
			ntp1->Column("ev",		(Float_t) i);
			ntp1->Column("cand",	(Float_t) j);
			ntp1->Column("ncand",   (Float_t) jpsi.GetLength());
			ntp1->Column("nmct",    (Float_t) njmct);
			
			// dump information about composite candidate tree recursively (see PndTools/AnalysisTools/PndRhoTupleQA)
			qa.qaComp("j", jpsi[j], ntp1);
			// dump info about event shapes
			qa.qaEventShapeShort("es",&evsh, ntp1);
			
			RhoCandidate *truth = jpsi[j]->GetMcTruth();
			
			if (truth!=0) 
				qa.qaP4("trj", truth->P4(), ntp1);
			else 
				qa.qaP4("trj", LVdummy, ntp1, true);
			
			ntp1->DumpData();
		}
		
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		
		// *** combinatorics for psi(2S) -> J/psi pi+ pi-
		psi2s.Combine(jpsi, piplus, piminus);
		psi2s.SetType(100443);
		int npsimct = theAnalysis->McTruthMatch(psi2s);

		for (j=0;j<psi2s.GetLength();++j) 
		{
			ntp2->Column("ev",		(Float_t) i);
			ntp2->Column("cand",	(Float_t) j);
			ntp2->Column("ncand",   (Float_t) psi2s.GetLength());
			ntp2->Column("nmct",    (Float_t) npsimct);
			
			qa.qaComp("psi", psi2s[j], ntp2);
			qa.qaEventShapeShort("es",&evsh, ntp2);
			
			RhoCandidate *truth = psi2s[j]->GetMcTruth();
			
			if (truth!=0) 
				qa.qaP4("trpsi", truth->P4(), ntp2);
			else 
				qa.qaP4("trpsi", LVdummy, ntp2, true);
			
			ntp2->DumpData();
		}			
	}
	
	// *** write out all the histos
	out->cd();
	
	ntp1->GetInternalTree()->Write();
	ntp2->GetInternalTree()->Write();
	nmc->GetInternalTree()->Write();
		
	out->Save();
	
}
