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
	
	// *** create some histograms
	TH1F *hjpsim_all = new TH1F("hjpsim_all","J/#psi mass (all)",200,0,4.5);
	TH1F *hpsim_all  = new TH1F("hpsim_all","#psi(2S) mass (all)",200,0,5);
	
	TH1F *hjpsim_pcut = new TH1F("hjpsim_pcut","J/#psi mass (comb. by hand with p cut)",200,0,4.5);
	TH1F *hpsim_pcut  = new TH1F("hpsim_pcut","#psi(2S) mass (comb. by hand with p cut))",200,0,5);
	
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
		
		// ***
		// *** SIMPLE COMBINATORICS for J/psi -> mu+ mu-
		// ***
		jpsi.Combine(muplus, muminus);
		fillM(jpsi, hjpsim_all);

		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// ***
		// *** SIMPLE COMBINATORICS for psi(2S) -> J/psi pi+ pi-
		// ***
		psi2s.Combine(jpsi, piplus, piminus);
		fillM(psi2s,hpsim_all);

		// ***
		// *** COMBINATORICS BY HAND for J/psi -> mu+ mu- with extra p cut
		// ***
		
		// clean jpsi list and fill it with mu+ mu- combinations
		jpsi.Cleanup();
		
		for (j=0;j<muplus.GetLength();++j)
		{
			if (muplus[j]->P()<0.3) continue;	// apply momentum selection (can be done easier
								// with a momentum selector, just for demonstration here)
			for (k=0;k<muminus.GetLength();++k)
			{
				if (muminus[k]->P()<0.3) continue;
				
 				RhoCandidate *combCand = muplus[j]->Combine(muminus[k]);
  				jpsi.Append(combCand);
			}
		}
		
		fillM(jpsi,hjpsim_pcut);
		
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// ***
		// *** COMBINATORICS BY HAND for psi(2S) -> J/psi pi+ pi- with extra p cut
		// ***
		
		// clean psi(2S) list and fill it with J/psi pi+ mpi- combinations
		psi2s.Cleanup();
		
		for (j=0;j<jpsi.GetLength();++j)
		{
			for (k=0;k<piplus.GetLength();++k)
			{
				// momentum selection and check clash with J/psi
				if ( piplus[k]->P()<0.2 || piplus[k]->Overlaps(jpsi[j]) ) continue;
				
				for (l=0;l<piminus.GetLength();++l)
				{
					// momentum selection and check clash with J/psi
					if ( piminus[l]->P()<0.2 || piminus[l]->Overlaps(jpsi[j]) ) continue;
					
					RhoCandidate *combCand = jpsi[j]->Combine(piplus[k], piminus[l]);
  					psi2s.Append(combCand);
				}
			}
		}
		
		fillM(psi2s,hpsim_pcut);
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
	RemoveGeoManager();
}
