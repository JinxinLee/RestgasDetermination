class RhoCandList;
class RhoCandidate;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;

// **** some auxilliary functions in auxtut.C ****
// - FairRunAna* initrun(TString prefix, TString outfile, int min=-1, int max=-1) --> Init FairRunAna
// - plotmyhistos()               --> Plots all histograms in current TDirectory on a autosized canvas
// - writemyhistos()              --> Writes all histos in current TFile 
// - fillM(RhoCandList l, TH1* h) --> Fill mass histogram h with masses of candidates in l
// **** some auxilliary functions in auxtut.C ****
#include "auxtut.C"


void tut_ana_mcmatch(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	fRun->Init();
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ana_mcmatch.root","RECREATE");
	
	// *** create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass",200,0,4.5);
	TH1F *hpsim_all  = new TH1F("hpsim_all","#psi(2S) mass",200,0,5);
	
	TH1F *hjpsim_nm = new TH1F("hjpsim_nm","J/#psi mass (no truth match)",200,0,4.5);
	TH1F *hpsim_nm  = new TH1F("hpsim_nm","#psi(2S) mass (no truth match)",200,0,5);
	
	TH1F *hjpsim_ftm = new TH1F("hjpsim_ftm","J/#psi mass (full truth match)",200,0,4.5);
	TH1F *hpsim_ftm  = new TH1F("hpsim_ftm","#psi(2S) mass (full truth match)",200,0,5);
	
	TH1F *hjpsim_diff = new TH1F("hjpsim_diff","J/#psi mass diff to truth",100,-2,2);
	TH1F *hpsim_diff  = new TH1F("hpsim_diff","#psi(2S) mass diff to truth",100,-2,2);
	
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
		// *** do the TRUTH MATCH for jpsi
		// ***
		jpsi.SetType(443);
				
		for (j=0;j<jpsi.GetLength();++j) 
		{
			hjpsim_all->Fill( jpsi[j]->M() );
			
			if (theAnalysis->McTruthMatch(jpsi[j])) 
			{
				hjpsim_ftm->Fill( jpsi[j]->M() );
				hjpsim_diff->Fill( jpsi[j]->GetMcTruth()->M() - jpsi[j]->M() );
			}
			else 
				hjpsim_nm->Fill( jpsi[j]->M() );
		}
				
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// *** combinatorics for psi(2S) -> J/psi pi+ pi-
		psi2s.Combine(jpsi, piplus, piminus);
		
		// ***
		// *** do the TRUTH MATCH for psi(2S)
		// ***
		psi2s.SetType(88888);

		for (j=0;j<psi2s.GetLength();++j) 
		{
			hpsim_all->Fill( psi2s[j]->M() );
			
			if (theAnalysis->McTruthMatch(psi2s[j]))
			{ 
			 	hpsim_ftm->Fill( psi2s[j]->M() );
				hpsim_diff->Fill( psi2s[j]->GetMcTruth()->M() - psi2s[j]->M() );
			}
			else 
				hpsim_nm->Fill( psi2s[j]->M() );
		}			
	}
	
	// *** change to directory where histograms are created
	out->cd();

	// *** plot all histos
	plotmyhistos();
	
	// *** write out all the histos to file
	int nhist = writemyhistos();
	cout<<"Writing "<<nhist<<" histograms to file"<<endl;		
	out->Save();	
}
