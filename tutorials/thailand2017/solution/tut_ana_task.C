
// **** some auxilliary functions in auxtut.C ****
// - FairRunAna* initrun(TString prefix, TString outfile, int min=-1, int max=-1) --> Init FairRunAna
// - plotmyhistos()               --> Plots all histograms in current TDirectory on a autosized canvas
// - writemyhistos()              --> Writes all histos in current TFile 
// - fillM(RhoCandList l, TH1* h) --> Fill mass histogram h with masses of candidates in l
// - RemoveGeoManager()           --> Temporary fix for error on macro exit   
// **** some auxilliary functions in auxtut.C ****
#include "auxtut.C"

void tut_ana_task(int nevts = 0, TString prefix = "signal")
{
	// *** Initialize FairRunAna with defaults	
	TString OutFile="out_dummy.root";  
	FairRunAna* fRun = initrun(prefix, OutFile);
	
	// *** HERE OUR TASK GOES!
	PndTutThaiTask *anaTask = new PndTutThaiTask();
	fRun->AddTask(anaTask);
	
	// *** and run analysis
	fRun->Init();
	fRun->Run(0,nevts);
	
	// *** temporaty fix to avoid error on macro exit
	RemoveGeoManager();
}
