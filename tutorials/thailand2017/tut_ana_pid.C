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


// *** routine to only keep PID matched candidates in list
int SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
	int removed = 0;
	
	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(ana->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}
	
	return removed;
}


void tut_ana_pid(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	fRun->Init();
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ana_pid.root","RECREATE");
	
	// #### EXERCISE:  create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass (all)",200,0,4.5);	           // -> no PID
	TH1F *hjpsim_lpid = new TH1F("hjpsim_lpid","J/#psi mass (loose pid)",200,0,4.5);     // -> loose PID
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
				
		// *** combinatorics NO PID 
		
		// #### EXERCISE: ---> Fill mu+, mu-, pi+ and pi- lists 
		theAnalysis->FillList(muplus,  "MuonAllPlus");
		//...
		
		// *** combinatorics for J/psi -> mu+ mu-
		jpsi.Combine(muplus, muminus);
		fillM(jpsi, hjpsim_all);
		jpsi.Select(jpsiMassSel);
		
		// *** combinatorics for pbarp -> J/psi pi+ pi-
		pbarp.Combine(jpsi, piplus, piminus);
		
		
		// *** TRUE PID combinatorics
		
		// #### EXERCISE:  do MC truth match for PID type
		 SelectTruePid(theAnalysis, muplus);
		//....
		
						
		// #### EXERCISE:  all combinatorics again with true PID
		//jpsi.Combine(...
		//jpsi.Select(....
		//pbarp.Combine(...
		
		
		// #### EXERCISE:  LOOSE PID combinatorics
		
		// *** and again with PidAlgoMvd;PidAlgoStt;PidAlgoDrc and loose selection
		// theAnalysis->FillList(muplus,  "MuonLoosePlus",  "PidAlgoMvd;PidAlgoStt;PidAlgoDrc");
		// ...
		
		//jpsi.Combine(..
		//jpsi.Select(....
		//pbarp.Combine(...
		
		
		// #### EXERCISE:  TIGHT PID combinatorics
		
		// *** and again with tight selection
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
