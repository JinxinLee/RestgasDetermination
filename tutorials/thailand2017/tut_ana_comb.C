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


void tut_ana_comb(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	fRun->Init();
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ana_comb.root","RECREATE");
	
	// #### EXERCISE:  create some histograms
	//TH1F *hjpsiMass = ...
	
	// ------------------------------
	// Now the analysis stuff comes...
	// ------------------------------
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// #### EXERCISE: ---> Declare mu+, mu-, pi+, pi-, jpsi and pbarp RhoCandLists 

	// RhoCandList muplus...
	
	
	// *** Mass selector for the jpsi cands
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",3.096,1.0);
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
				
		// #### EXERCISE: ---> Fill mu+, mu-, pi+ and pi- lists 

		// theAnalysis->FillList(muplus,  "MuonAllPlus");
		// ....
		
		// #### EXERCISE:  Do combinatorics with the generic lists to form J/psi -> mu+ mu- and pbarp -> J/psi pi+ pi-
		
		//jpsi.Combine(...)
		//pbarp.Combine(...)
		
		// #### EXERCISE:  Fill histograms with masses of J/psi and ppbar candidates
		// fillM(jpsi, hjpsiMass);
		// ...
		
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
