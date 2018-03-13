// Martin Galuska
// currently not used, might be useful in the future

int plotHoughSpaces()
{

	UInt_t fVerbose = 0;


	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	//	  basiclibs(); // this gives an error when compiling

	// Load the example libraries
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libField");



	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->SetStyle("Plain");
	gStyle->SetMarkerStyle(20);
	gStyle->SetOptStat(0);
	gStyle->SetPalette(1);



	// -----------------------------------------------------------------------
	// FileNames
	TString RecoFile = "reco_complete.root";
	TString SimFile = "sim_complete.root";
	// -----------------------------------------------------------------------






	// General stuff
	const Double_t BeamMomentum = 15.; //6.991;
	const Double_t meinpi = 3.14159265;
	const Int_t ResolutionX = 800, ResolutionY = 600; // for plotting

	// -----------------------------------------------------------------------








	// get Hough spaces
	TFile* fReco = new TFile(RecoFile.Data());
	TTree* tReco = (TTree*) (fReco->Get("pndsim"));
	//tReco->StartViewer();

	PndFtsHoughSpace* fHoughSpace = new PndFtsHoughSpace();
	// TODO here I should do something else instead
	tReco->SetBranchAddress("FTSHit", &fHoughSpace);


	if (nEvents==0) nEvents =  tReco->GetEntriesFast();
	for (Int_t iEvent=0; iEvent< nEvents; ++iEvent)
	{
		cout << "processing event " << iEvent << "\n";
		tReco->GetEntry(iEvent); // Look at event iEvent






		TCanvas *choughspaceLine = new TCanvas("choughspaceLine", "choughspaceLine",
				ResolutionX, ResolutionY);
		fHoughSpace->DrawCopy("Lego2z");


	} // end of event loop
  return 0;

}
