class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;


void tut_ana_fit(int nevts=0)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	TString OutFile="output.root";  
					
	// *** the files coming from the simulation
	TString inPidFile = "pid_complete.root";    // this file contains the PndPidCandidates and McTruth
	TString inParFile = "simparams.root";
	
	gStyle->SetOptFit(1011);
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inPidFile);
	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	rtdb->setFirstInput(parIO);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	fRun->Init(); 
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open("output_ana_fit.root","RECREATE");
	
	// *** create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass",200,0,4.5);
	TH1F *hpsim_all  = new TH1F("hpsim_all","#psi(2S) mass",200,0,5);
	
	TH1F *hjpsim_vf   = new TH1F("hjpsim_vf", "J/#psi mass (vertex fit)",200,0,4.5);
	TH1F *hjpsim_4cf  = new TH1F("hjpsim_4cf","J/#psi mass (4C fit)",200,0,4.5);
	TH1F *hjpsim_mcf  = new TH1F("hjpsim_mcf","J/#psi mass (mass constraint fit)",200,0,4.5);
	
	TH1F *hjpsi_chi2_vf  = new TH1F("hjpsi_chi2_vf", "J/#psi: #chi^{2} vertex fit",100,0,10);
	TH1F *hpsi_chi2_4c   = new TH1F("hpsi_chi2_4c",  "#psi(2S): #chi^{2} 4C fit",100,0,250);
	TH1F *hjpsi_chi2_mf  = new TH1F("hjpsi_chi2_mf", "J/#psi: #chi^{2} mass fit",100,0,10);
	
	TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-2,2,100,-2,2);
	
	//
	// Now the analysis stuff comes...
	//
	
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, psi2s;
	
	// *** Mass selector for the jpsi cands
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",3.096,1.0);
	
	// *** the lorentz vector of the initial psi(2S), needed by 4C fitter
	TLorentzVector ini(0, 0, 6.231552, 7.240065);
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
				
		// *** Select with no PID info ('All'); type and mass are set 		
		theAnalysis->FillList(muplus,  "MuonAllPlus");
		theAnalysis->FillList(muminus, "MuonAllMinus");
		theAnalysis->FillList(piplus,  "PionAllPlus");
		theAnalysis->FillList(piminus, "PionAllMinus");
		
		// *** combinatorics for J/psi -> mu+ mu-
		jpsi.Combine(muplus, muminus);
		
		// ***
		// *** do VERTEX FIT (J/psi)
		// ***
		for (j=0;j<jpsi.GetLength();++j) 
		{
			hjpsim_all->Fill(jpsi[j]->M());		// fill histo for all J/psi candidates
			
			PndKinVtxFitter vtxfitter(jpsi[j]);	// instantiate a vertex fitter
			vtxfitter.Fit();
			
			double chi2_vtx=vtxfitter.GetChi2();	// access chi2 of fit
			hjpsi_chi2_vf->Fill(chi2_vtx);
			
			if (chi2_vtx<1)				// when good enough, fill some histos
			{
				RhoCandidate *jfit = jpsi[j]->GetFit();	// access the fitted cand
				TVector3 jVtx=jfit->Pos();		// and the decay vertex position
				
				hjpsim_vf->Fill(jfit->M());            
				hvpos->Fill(jVtx.X(),jVtx.Y());
			}
		}
		
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// *** combinatorics for psi(2S) -> J/psi pi+ pi-
		psi2s.Combine(jpsi, piplus, piminus);
		
		// ***
		// *** do 4C FIT (initial psi(2S) system)
		// ***
		for (j=0;j<psi2s.GetLength();++j) 
		{
			hpsim_all->Fill(psi2s[j]->M());		// fill histo for all psi(2S) candidates
			
			Pnd4CFitter fitter(psi2s[j],ini);	// instantiate the 4C fitter in psi(2S)
			fitter.FitConserveMasses();		// do fit, conserving masses of final state particles
			
			double chi2_4c=fitter.GetChi2();	// get chi2 of fit
			hpsi_chi2_4c->Fill(chi2_4c);
			
			if (chi2_4c<40)			// when good enough, fill some histo
			{
				RhoCandidate *jfit = psi2s[j]->Daughter(0)->GetFit();	// get fitted J/psi
				
				hjpsim_4cf->Fill(jfit->M());
			}
		}		
		
		// ***
		// *** do MASS CONSTRAINT FIT (J/psi)
		// ***
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinFitter mfitter(jpsi[j]);		// instantiate the PndKinFitter in psi(2S)
			mfitter.AddMassConstraint(3.0965);	// add the mass constraint
			mfitter.Fit();				// do fit
			
			double chi2_m = mfitter.GetChi2();	// get chi2 of fit
			hjpsi_chi2_mf->Fill(chi2_m);
			
			if (chi2_m<1)				// when good enough, fill some histo
			{
				RhoCandidate *jfit = jpsi[j]->GetFit();	// access the fitted cand
				hjpsim_mcf->Fill(jfit->M());
			}
		}		
		
	}
	
	// *** write out all the histos
	out->cd();
	
	hjpsim_all->Write();
	hpsim_all->Write();
	
	hjpsim_vf->Write();
	hjpsim_4cf->Write();
	hjpsim_mcf->Write();
	
	hjpsi_chi2_vf->Write();
	hpsi_chi2_4c->Write();
	hjpsi_chi2_mf->Write();
			
	hvpos->Write();
		
	out->Save();
	
}
