// Test digital filter to reconstruct parameters from waveforms
// Input paramters event index and index of waveform in the events
int testFilter(Int_t i_event=0, Int_t i_wf=0)
{
	gROOT->SetStyle("Plain");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();

	TFile* f = new TFile("digi_emc.root"); //file you want to analyse
	TTree *t=(TTree *) f->Get("pndsim") ;
	TClonesArray* waveform_array=new TClonesArray("PndEmcWaveform");
	t->SetBranchAddress("EmcWaveform",&waveform_array);

	PndEmcMapper::Init(6);

	TH1D * h1 = new TH1D("h1","Signal ",63,0,64.);
	TH1D * h2 = new TH1D("h2","Filtered signal ",63,0,64.);

		// Cluster energy
	t->GetEntry(i_event);
	PndEmcWaveform *wf=(PndEmcWaveform*)waveform_array->At(i_wf);
	std::vector<Double_t> signal = wf->GetSignal();
	std::vector<Double_t> output;

	// Moving average filter
	PndEmcFadcFilter *flt1  = new PndEmcFadcFilter();
	Double_t data[3]={1./3.,1./3.,1./3.};
	flt1->SetData(data,3,0);
	//flt1->Filter(signal, output);

	//Trapezoidal filter
	PndEmcFadcFilter *flt2  = new PndEmcFadcFilter();
	flt2->SetupTrapez(20,20);
	//flt2->SetNormalizeFloating(1./4.);
	flt2->Filter(signal, output);


	for (Int_t i=0; i<signal.size();i++)
	{
		h1->SetBinContent(i,signal[i]);
		h2->SetBinContent(i,output[i]);
	}

	TCanvas *c1 = new TCanvas();
	h1->Draw();
	TCanvas *c2 = new TCanvas();
	h2->Draw();

  return 0;
}
