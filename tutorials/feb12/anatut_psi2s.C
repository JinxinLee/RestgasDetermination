class TCandList;
class TCandidate;
class TFitParams;
class PndAnaPidSelector;
class PndAnaPidCombiner;


// *** routine to only keep PID matched candidates in list
int SelectPdgCode(TCandList &mct, TCandList &l)
{
	int removed = 0;
	int pdgcode=0;
	
	PndMcTruthMatch mcm;
	
	if (l.GetLength()>0) pdgcode = l[0].PdgCode();
	for (int ii=0;ii<l.GetLength();++ii)
	{
		if (!mcm.MctMatch(l[ii],mct))
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	return removed;
}

void anatut_psi2s(int nevts=0)
{
	// *** some variables
	int i=0,j=0, k=0, l=0;
	
	TString OutFile="output.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = "pid_sttcombi.root";    // this file contains the PndPidCandidates
	TString inRecoFile  = "reco_sttcombi.root";
	TString inSimFile = "points_sttcombi.root";  // this file contains the MC truth
	TString inParFile = "params_sttcombi.root";
	
	gStyle->SetOptFit(1011);
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetInputFile(inSimFile);
	fRun->AddFriend(inPidFile);
	fRun->AddFriend(inRecoFile);
	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	rtdb->setFirstInput(parIO);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(OutFile);
	fRun->Init(); 
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open("output_psi2sana.root","RECREATE");
	
	// *** create some histograms
	TH1F *hjpsim_nopid = new TH1F("hjpsim_nopid","J/#psi mass (no pid)",200,0,4);
	TH1F *hpsim_nopid  = new TH1F("hpsim_nopid","#psi(2S) mass (no pid)",200,0,5);
	TH1F *hjpsim_lpid = new TH1F("hjpsim_lpid","J/#psi mass (loose pid)",200,0,4);
	TH1F *hpsim_lpid  = new TH1F("hpsim_lpid","#psi(2S) mass (loose pid)",200,0,5);
	TH1F *hjpsim_tpid = new TH1F("hjpsim_tpid","J/#psi mass (true pid)",200,0,4);
	TH1F *hpsim_tpid  = new TH1F("hpsim_tpid","#psi(2S) mass (true pid)",200,0,5);
	
	TH1F *hjpsim_ftm = new TH1F("hjpsim_ftm","J/#psi mass (full truth match)",200,0,4);
	TH1F *hpsim_ftm  = new TH1F("hpsim_ftm","#psi(2S) mass ( truth match)",200,0,5);
	TH1F *hjpsim_nm = new TH1F("hjpsim_nm","J/#psi mass (no truth match)",200,0,4);
	TH1F *hpsim_nm  = new TH1F("hpsim_nm","#psi(2S) mass (no truth match)",200,0,5);
	
	TH1F *hjpsim_diff = new TH1F("hjpsim_diff","J/#psi mass diff to truth",100,-2,2);
	TH1F *hpsim_diff  = new TH1F("hpsim_diff","#psi(2S) mass diff to truth",100,-2,2);
	
	TH1F *hjpsim_vf   = new TH1F("hjpsim_vf","J/#psi mass vertex fit",200,0,4);
	TH1F *hjpsim_4cf  = new TH1F("hjpsim_4cf","J/#psi mass (4C fit)",200,0,4);
	TH1F *hjpsim_mcf  = new TH1F("hjpsim_mcf","J/#psi mass (4C fit)",200,0,4);
	
	TH1F *hjpsi_chi2_vf  = new TH1F("hjpsi_chi2_vf","J/#psi, #chi^{2} vertex fit",100,0,10);
	TH1F *hpsi_chi2_4c   = new TH1F("hpsi_chi2_4c","#psi, #chi^{2} 4C fit",100,0,250);
	TH1F *hjpsi_chi2_mf  = new TH1F("hjpsi_chi2_mf","J/#psi, #chi^{2} mass fit",100,0,10);
	
	TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-2,2,100,-2,2);
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** TCandLists for the analysis
	TCandList all, chrg, mctrk, el, eplus, eminus, piplus, piminus, jpsi, psi2s;
	
	// *** Mass selector for the jpsi cands
	TPidMassSelector *jpsiMassSel=new TPidMassSelector("jpsi",3.096,1.0);
	
	// *** the MC truth matcher
	PndMcTruthMatch mcm;
	
	// *** the lorentz vector of the initial psi(2S)
	TLorentzVector ini(0, 0, 6.231552, 7.240065);
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
		
		// *** the MC Truth objects
		theAnalysis->FillList(mctrk,"McTruth");
			
		// *** Select with no PID info ('All'); type and mass are set 		
		theAnalysis->FillList(eplus, "ElectronAllPlus");
		theAnalysis->FillList(eminus, "ElectronAllMinus");
		theAnalysis->FillList(piplus, "PionAllPlus");
		theAnalysis->FillList(piminus, "PionAllMinus");
		
		jpsi.Combine(eplus, eminus);
		
		// *** do the truth match for jpsi
		mcm.SetType(jpsi,"J/psi");
		for (j=0;j<jpsi.GetLength();++j) 
		{
			hjpsim_nopid->Fill( jpsi[j].M() );
			if (mcm.MctMatch(jpsi[j], mctrk)) hjpsim_ftm->Fill( jpsi[j].M() );
			else hjpsim_nm->Fill( jpsi[j].M() );
		}
		
		// *** do vertex fitting (J/psi)
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinVtxFitter vtxfitter(jpsi[j]);        // instantiate a vertex fitter
			
			vtxfitter.Fit();
			TCandidate *jfit = vtxfitter.FittedCand(jpsi[j]);  // access the fitted cand
			TVector3 jVtx=jfit->Pos();                         // and the decay vertex position
			
			double chi2_vtx=vtxfitter.GlobalChi2();            // access chi2 of fit
			hjpsi_chi2_vf->Fill(chi2_vtx);
			
			if (chi2_vtx<2)                                    // when good enough, fill some histos
			{
				hjpsim_vf->Fill(jfit->M());
				hvpos->Fill(jVtx.X(),jVtx.Y());
			}
		}
		
		// *** some rough mass selection
		//jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		
		// *** do the truth match for psi2s
		mcm.SetType(psi2s,"psi(2S)");
		
		for (j=0;j<psi2s.GetLength();++j) 
		{
			hpsim_nopid->Fill( psi2s[j].M() );
			if (mcm.MctMatch(psi2s[j], mctrk)) 
			{
				hpsim_ftm->Fill( psi2s[j].M() );
				
				TCandidate truePsi = mctrk[psi2s[j].GetMcIdx()];
				TCandidate trueJ   = mctrk[psi2s[j].Daughter(0)->GetMcIdx()];
				
				hjpsim_diff->Fill(trueJ.M()-psi2s[j].Daughter(0)->M());
				hpsim_diff->Fill(truePsi.M()-psi2s[j].M());
			}
			else hpsim_nm->Fill( psi2s[j].M() );
		}	
		
		// *** do 4c fit (initial psi(2S) system)
		for (j=0;j<psi2s.GetLength();++j) 
		{
			Pnd4CFitter fitter(psi2s[j],ini);
			fitter.FitConserveMasses();
			
			double chi2_4c=fitter.GetChi2();
			hpsi_chi2_4c->Fill(chi2_4c);
			
			TCandidate *jfit = fitter.FittedCand(*(psi2s[j].Daughter(0)));
			
			TCandidate *epfit = fitter.FittedCand(*(psi2s[j].Daughter(0)->Daughter(0)));
			TCandidate *emfit = fitter.FittedCand(*(psi2s[j].Daughter(0)->Daughter(1)));
			
			TLorentzVector tlvepf = epfit->P4();
			TLorentzVector tlvemf = emfit->P4();
			
			hjpsim_4cf->Fill((tlvepf+tlvemf).M());
		}		
		
		// do mass constraint fit
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinFitter mfitter(jpsi[j]);
			mfitter.AddMassConstraint(3.0965);
			mfitter.Fit();
			double chi2_m = mfitter.GlobalChi2();
			hjpsi_chi2_mf->Fill(chi2_m);
			
			if (chi2_m<2) hjpsim_mcf->Fill(jpsi[j].M());
		}		
		
		// *** do MC truth match for PID type
		SelectPdgCode(mctrk, eplus);
		SelectPdgCode(mctrk, eminus);
		SelectPdgCode(mctrk, piplus);
		SelectPdgCode(mctrk, piminus);
				
		// *** all combinatorics again with true PID
		jpsi.Combine(eplus, eminus);
		for (j=0;j<jpsi.GetLength();++j) hjpsim_tpid->Fill( jpsi[j].M() );
		//jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		for (j=0;j<psi2s.GetLength();++j) hpsim_tpid->Fill( psi2s[j].M() );
		
		// *** and again with PidAlgoEmcBayes and loose selection
		theAnalysis->FillList(eplus, "ElectronLoosePlus","PidAlgoEmcBayes");
		theAnalysis->FillList(eminus, "ElectronLooseMinus","PidAlgoEmcBayes");
		theAnalysis->FillList(piplus, "PionLoosePlus","PidAlgoEmcBayes");
		theAnalysis->FillList(piminus, "PionLooseMinus","PidAlgoEmcBayes");
		
		jpsi.Combine(eplus, eminus);
		for (j=0;j<jpsi.GetLength();++j) hjpsim_lpid->Fill( jpsi[j].M() );
		//jpsi.Select(jpsiMassSel);
		
		psi2s.Combine(jpsi, piplus, piminus);
		for (j=0;j<psi2s.GetLength();++j) hpsim_lpid->Fill( psi2s[j].M() );
		
	}
	
	// *** write out all the histos
	out->cd();
	
	hjpsim_nopid->Write();
	hpsim_nopid->Write();
	hjpsim_lpid->Write();
	hpsim_lpid->Write();
	hjpsim_tpid->Write();
	hpsim_tpid->Write();
	hjpsim_ftm->Write();
	hpsim_ftm->Write();
	
	hpsim_diff->Write();
	hjpsim_diff->Write();
	
	hjpsim_vf->Write();
	hjpsim_4cf->Write();
	hjpsim_mcf->Write();
	
	hjpsi_chi2_vf->Write();
	hpsi_chi2_4c->Write();
	hjpsi_chi2_mf->Write();
			
	hvpos->Write();
	
	out->Save();
	
}
