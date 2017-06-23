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


void tut_ana_mclist(int nevts = 0, TString prefix = "signal")
{
 	// *** some variables
	int i=0,j=0, k=0, l=0;
	gStyle->SetOptFit(1011);
	
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	fRun->Init();
	
	//
	// Now the analysis stuff comes...
	//
	
	
	// *** the data reader object
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();
	
	// *** RhoCandLists for the analysis
	RhoCandList mctruth;
	
	// ***
	// the event loop
	// ***
	while (theAnalysis->GetEvent() && i++<nevts)
	{
		cout<<"****** Evt " << i << endl;
		
		// *** the MC Truth objects
		theAnalysis->FillList(mctruth,"McTruth");
		
		//
		// Print MC Truth list with mother-daughter relations
		//
		for (j=0;j<mctruth.GetLength();++j)
		{
			RhoCandidate *mcmother = mctruth[j]->TheMother();
		  
		  	int muid = -1;
		  	if (mcmother) muid = mcmother->GetTrackNumber();
		  
		  	cout << "Track "<< mctruth[j]->GetTrackNumber()<<" (PDG:"<<mctruth[j]->PdgCode() <<") has mother "<<muid;
		  
		  	if (mctruth[j]->NDaughters()>0) cout <<" and daughter(s) ";
		  	for (k=0;k<mctruth[j]->NDaughters();++k) cout <<mctruth[j]->Daughter(k)->GetTrackNumber()<<"  ";
		  
		  	cout<<endl;
		}
		cout <<endl;
	}
	
	// *** temporaty fix to avoid error on macro exit
	RemoveGeoManager();
}
