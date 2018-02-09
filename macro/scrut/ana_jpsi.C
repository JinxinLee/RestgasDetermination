int ana_jpsi(TString InFile="test_fast.root", int nevts=0, double pbarmom = 6.231552)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);

	if (!InFile.EndsWith(".root")) InFile+="_fast.root";
	
	// *** the output file for FairRunAna
	TString OutFile="dummy_out.root";  
					
	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	FairRunAna* fRun = new FairRunAna();
    fRun->SetGenerateRunInfo(kFALSE);
	fRun->SetInputFile(InFile);
	fRun->SetOutputFile(OutFile); // only dummy; the real output is 
	
	fRun->Init(); 
	
	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(InFile+"_ana.root","RECREATE");
	
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
	
	// *** name of the only PidAlgo TClonesArray in fsim
	TString pidalg = "PidChargedProbability";

	// *** QA tool for simple dumping of analysis results in RhoRuple
	// *** WIKI: https://panda-wiki.gsi.de/foswiki/bin/view/Computing/PandaRootAnalysisJuly13#PndRhoTupleQA
	PndRhoTupleQA qa(theAnalysis, pbarmom); 

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
		
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
		
		// *** get MC list and store info in ntuple
	 	theAnalysis->FillList(mclist, "McTruth");
		
		nmc->Column("ev", (Int_t) i);
		qa.qaMcList("",   mclist, nmc);
		nmc->DumpData();
		
		
		// *** Setup event shape object
		theAnalysis->FillList(all,  "All", pidalg);
		PndEventShape evsh(all, ini, 0.05, 0.1);
			
		
		// *** Select with no PID info ('All'); type and mass are set 		
		theAnalysis->FillList(muplus,  "MuonAllPlus", pidalg);
		theAnalysis->FillList(muminus, "MuonAllMinus", pidalg);
		theAnalysis->FillList(piplus,  "PionAllPlus", pidalg);
		theAnalysis->FillList(piminus, "PionAllMinus", pidalg);
		
		
		// *****
		// *** combinatorics for J/psi -> mu+ mu-
		// *****
		
		jpsi.Combine(muplus, muminus);		
		jpsi.SetType(443);
		int njmct = theAnalysis->McTruthMatch(jpsi);
				
		for (j=0;j<jpsi.GetLength();++j) 
		{
		    // some general info about event (actually written for each candidate!)
			ntp1->Column("ev",		(Float_t) i);
			ntp1->Column("cand",	(Float_t) j);
			ntp1->Column("ncand",   (Float_t) jpsi.GetLength());
			ntp1->Column("nmct",    (Float_t) njmct);
			
			// store info about initial 4-vector
			qa.qaP4("beam", ini, ntp1);
			
			// store information about composite candidate tree recursively (see analysis/AnalysisTools/PndRhoTupleQA)
			qa.qaComp("j", jpsi[j], ntp1);
			
			// store info about event shapes
			qa.qaEventShapeShort("es",&evsh, ntp1);
		
			
			// store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
			RhoCandidate *truth = jpsi[j]->GetMcTruth();		
			TLorentzVector lv;
			if (truth) lv = truth->P4();
			qa.qaP4("trj", lv, ntp1);
			
			ntp1->DumpData();
		}
		
		
		// *****
		// *** combinatorics for psi(2S) -> J/psi pi+ pi-
		// *****

		// *** some rough mass selection on J/psi before
		jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		psi2s.SetType(100443);
		int npsimct = theAnalysis->McTruthMatch(psi2s);

		for (j=0;j<psi2s.GetLength();++j) 
		{
		    // some general info about event (actually written for each candidate!)
			ntp2->Column("ev",		(Float_t) i);
			ntp2->Column("cand",	(Float_t) j);
			ntp2->Column("ncand",   (Float_t) psi2s.GetLength());
			ntp2->Column("nmct",    (Float_t) npsimct);
			
			PndKinFitter kinfit(psi2s[j]);
			kinfit.Add4MomConstraint(ini);
			kinfit.Fit();
			
			RhoCandidate *psifit=psi2s[j]->GetFit();
			
			// store info about initial 4-vector
			qa.qaP4("beam", ini, ntp2);
			
			// store information about composite candidate tree recursively (see analysis/AnalysisTools/PndRhoTupleQA)
			qa.qaComp("psi", psi2s[j], ntp2);
			qa.qaComp("fpsi",psifit, ntp2);
			
			// store info about event shapes
			qa.qaEventShapeShort("es",&evsh, ntp2);
			
			// *** store the 4-vector of the truth matched candidate (or a dummy, if not matched to keep ntuple consistent)
			RhoCandidate *truth = psi2s[j]->GetMcTruth();
			TLorentzVector lv;
			if (truth) lv = truth->P4();
			qa.qaP4("trpsi", lv, ntp2);
			
			ntp2->DumpData();
		}			
	}
	
	// *** write out all the histos
	out->cd();
	
	ntp1->GetInternalTree()->Write();
	ntp2->GetInternalTree()->Write();
	nmc->GetInternalTree()->Write();
		
	out->Save();
	
  return 0;
}
