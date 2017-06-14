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

void tut_ana_fit(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	fRun->Init();
	
	// *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_ana_fit.root","RECREATE");
	
	// #### EXERCISE:  create some histograms
	TH1F *hjpsim_all     = new TH1F("hjpsim_all","J/#psi mass",200,0,4.5);
	TH1F *hjpsim_vf      = new TH1F("hjpsim_vf", "J/#psi mass (vertex fit)",200,0,4.5);	
	TH1F *hjpsi_chi2_vf  = new TH1F("hjpsi_chi2_vf", "J/#psi: #chi^{2} vertex fit",100,0,10);
	TH1F *hjpsi_prob_vf  = new TH1F("hjpsi_prob_vf", "J/#psi: Prob vertex fit",100,0,1);
	// ....
	
	// ------------------------------
	// Now the analysis stuff comes...
	// ------------------------------
	
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus, jpsi, pbarp;
	
	// *** Mass selector for the jpsi cands
	double m0_jpsi = TDatabasePDG::Instance()->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",m0_jpsi,1.0);
	
	// *** the lorentz vector of the initial pbarp, needed by 4C fitter
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
			// #### EXERCISE:  fill unfitted mass in histogram
			// ... ->Fill(...
			
			PndKinVtxFitter vtxfitter(jpsi[j]);	// instantiate a vertex fitter
			vtxfitter.Fit();
			
			double chi2_vtx = vtxfitter.GetChi2();	// access chi2 of fit
			double prob_vtx = vtxfitter.GetProb();	// access probability of fit
			
			// #### EXERCISE:  fill chi2 and prob in histogram		
			// hjpsi_chi2_vf->Fill(...
			// ...
						
			if ( prob_vtx > 0.01 )				// when good enough, fill some histos
			{
				RhoCandidate *jfit = jpsi[j]->GetFit();	// access the fitted cand
				TVector3 jVtx=jfit->Pos();				// and the decay vertex position
				
				// #### EXERCISE:  fill fitted mass and vertex position in histograms	
				// ...
			}
		}
		
		// *** some rough mass selection
		jpsi.Select(jpsiMassSel);
		
		// *** combinatorics for pbarp -> J/psi pi+ pi-
		pbarp.Combine(jpsi, piplus, piminus);
		
		// ***
		// *** do 4C FIT (initial psi(2S) system)
		// ***
		for (j=0;j<pbarp.GetLength();++j) 
		{
			// #### EXERCISE:  fill unfitted mass in histogram
			// ... ->Fill(...
			
			PndKinFitter fitter(pbarp[j]);  // instantiate the kin fitter in psi(2S)
			fitter.Add4MomConstraint(ini);  // set 4 constraint
			fitter.Fit();                   // do fit
			
			double chi2_4c = fitter.GetChi2();	// get chi2 of fit
			double prob_4c = fitter.GetProb();	// access probability of fit
			
			// #### EXERCISE:  fill chi2 and prob in histogram		
			// hpbarp_chi2_vf->Fill(...
			// ...
			
			if ( prob_4c > 0.01 )            // when good enough, fill some histo
			{
				RhoCandidate *jfit = pbarp[j]->Daughter(0)->GetFit();	// get fitted J/psi

				// #### EXERCISE:  fill fitted mass and vertex position in histograms	
				// ...
			}
		}		
		
		// ***
		// *** do MASS CONSTRAINT FIT (J/psi)
		// ***
		for (j=0;j<jpsi.GetLength();++j) 
		{
			PndKinFitter mfitter(jpsi[j]);      // instantiate the PndKinFitter in psi(2S)
			mfitter.AddMassConstraint(m0_jpsi); // add the mass constraint
			mfitter.Fit();                      // do fit
			
			// #### EXERCISE:  fill chi2 and prob in histogram
			// double chi2_m = ...
			// hpbarp_chi2_vf->Fill(...
			
			// #### EXERCISE:  cut on prob and fill surviving candidates mass in histogram
			// if (....
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
