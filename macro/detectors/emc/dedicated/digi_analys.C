{
  // Macro loads a file after digitization and plots the waveform


        gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
        gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
        rootlogon();
        basiclibs();

	TFile* f = new TFile("digi_emc.root"); // the sim file you want to analyse
	TTree *t=(TTree *) f->Get("pndsim") ;

	TClonesArray* digi_array=new TClonesArray("EmcDigi");

	t->SetBranchAddress("EmcDigi",&digi_array);

	double digi_energy, hit_energy;
	TH1F *h1= new TH1F("h1","Energy ratio",100,0.,10.);
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<digi_array->GetEntriesFast(); i++)
		{
			EmcDigi *digi=(EmcDigi*)digi_array->At(i);
			digi_energy=digi->GetEnergy();
			std::vector<CbmEmcHit*> hitList=digi->itsHitList();
			if (hitList.size()!=0)
			{
				hit_energy=hitList[0]->GetEnergy();
				if (hit_energy>1e-3) h1->Fill(digi_energy/hit_energy);
			}
		}
	}

	TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
	h1->Draw();
	c1->Update();

/*	t->GetEntry(0);
	EmcDigi *digi=(EmcDigi*)digi_array->At(0);
	digi->ValidateTCI();
	TwoCoordIndex *tci=digi->GetTCI();
	cout<<"tci->itsIndex()="<<tci->itsIndex()<<endl;*/

/*	TClonesArray *wfa=new TClonesArray("EmcWaveform");
	wfa->BypassStreamer(kFALSE);
	t->SetBranchAddress("EmcWaveform",&wfa);
	t->GetEntry(0);
	EmcWaveform *wf=(EmcWaveform*) wfa->At(0);
	TwoCoordIndex *tci=wf->GetTCI();*/
	//cout<<"tci->itsIndex()="<<tci->itsIndex()<<endl;





   if(0)
	{
	TClonesArray *wfa=new TClonesArray("EmcWaveform");
	t->SetBranchAddress("EmcWaveform",&wfa);

	t->GetEntry(0);

	char ch;

	TH1F *h2= new TH1F("h2","Waveform",64,1.,64.);
	TCanvas* c2 = new TCanvas("c2", "", 100, 100, 800, 800);
	for (Int_t i=0; i<wfa->GetEntriesFast(); i++)
	{
		h2->Reset();
		EmcWaveform *wf=(EmcWaveform*)wfa->At(i);
		vector<double> signal = wf->GetSignal();

		for (Int_t j=0;j<signal.size();j++)
		{
			h2->SetBinContent(j,signal[j]);
			j++;
		}

		h2->Draw();
		c2->Update();
		gPad->WaitPrimitive();
		cin.get(ch);
	}
	}
	//t->Draw("EmcWaveform.fSignal");

}

