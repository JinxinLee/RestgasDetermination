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
// - RemoveGeoManager()           --> Temporary fix for error on macro exit   
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


void tut_ana_fast(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** the output file for FairRunAna
	TString OutFile="out_dummy.root";  
					
	// *** the files coming from the simulation
	TString inPidFile  = prefix+"_fast.root";    // this file contains the PndPidCandidates and McTruth
	
	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";	
	
	// *** initialization
	FairRunAna* fRun = new FairRunAna();
	fRun->SetSource(new FairFileSource(inPidFile));
	fRun->SetOutputFile(OutFile);
	
	// *** take constant field; needed for PocaVtx
	RhoCalculationTools::ForceConstantBz(20.0);
	
	fRun->Init(); 
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ana_fast.root","RECREATE");

	// ------------------------------
	// Now the analysis stuff comes...
	// ------------------------------

	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();

	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// This macro combines all tasks (combinatorics, MC truth, PID, fitting) 
	// try to complete everything, perhaps for a different decay mode
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------
	// ---------------------------------------------------------------------

	// #### EXERCISE:  create all necessary histograms
	// ...
		
	// #### EXERCISE:  create RhoCandLists for the analysis, mass selector, etc...
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		// BE AWARE : The pid algos are named differently for FastSim, so simply take this one
		TString pidalgo = "PidChargedProbability";
		
		// #### EXERCISE
		// (1) combinatorics with desired PID algorithms (for FastSim use 'PidChargedProbability') and tightness, and PDG set for truth match
		// (2) select rought mass window for resonances
		// (3) do vertex fitting for resonances to charged particles
		// (4) fill final histograms, with and without truth match	
	}
	
	// *** change to directory where histograms are created
	out->cd();

	// *** plot all histos
	plotmyhistos();
	
	// *** write out all the histos to file
	int nhist = writemyhistos();
	cout<<"Writing "<<nhist<<" histograms to file"<<endl;		
	out->Save();	
	
	// *** temporaty fix to avoid error on macro exit
	//RemoveGeoManager();
}
