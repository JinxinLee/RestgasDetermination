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
	
	// #### EXERCISE:  create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass",200,0,4.5);
	TH1F *hjpsim_ftm = new TH1F("hjpsim_ftm","J/#psi mass (full truth match)",200,0,4.5);
	// ...
	
	// ------------------------------
	// Now the analysis stuff comes...
	// ------------------------------
	
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, pbarp;
	
	// *** Mass selector for the jpsi cands
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",3.096,1.0);
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
		
		// #### EXERCISE:  Select with no PID info ('All'); type and mass are set 		
		theAnalysis->FillList(muplus,  "MuonAllPlus");
		// ...
		
		// *** combinatorics for J/psi -> mu+ mu-, AND specify PDG type as last parameter!
		jpsi.Combine(muplus, muminus, 443);
		
		// *** do the TRUTH MATCH for jpsi

		for (j=0;j<jpsi.GetLength();++j) 
		{
			// *** fill mass in histogram
			hjpsim_all->Fill( jpsi[j]->M() );
			
			// #### EXERCISE: fill mass in histogram for truth match candidates
			if (theAnalysis->McTruthMatch(jpsi[j])) 
			{
				//hjpsim_ftm->Fill(...
			}			
			// #### EXERCISE: fill mass in histogram for non match candidates
			else 
				//...->Fill( ...
		}
				
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// #### EXERCISE:  combinatorics for pbarpSystem (PDG=88888) -> J/psi pi+ pi-
		// pbarp.Combine(...
		
		// *** do the TRUTH MATCH for pbarpSystem (PDG == 88888)

		for (j=0;j<pbarp.GetLength();++j) 
		{
			// #### EXERCISE:  fill histos for pbarp candidates with and w/o MC truth match
			
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
