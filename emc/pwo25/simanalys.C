void simanalys()
{
	gSystem->Load("./PWO25SimEvent.so");

	TH1F *h1= new TH1F("h1","",200,0.7,1.05);
	h1->SetTitle("Energy distribution in 25 crystals, E=1 GeV");
	h1->GetXaxis()->SetTitle("Energy, GeV");

	double E;
	char *file_name="pwo25.root";
	TFile f(file_name);
	TTree *eventT =(TTree *) f.Get("Events");

	PWO25Event = new PWO25SimEvent();
	eventT->SetBranchAddress("crystal",&PWO25Event);

	TClonesArray*	fHits;

	int ientries =  eventT->GetEntries();

	for (int i=0; i<ientries;i++) //ientries
	{
    	E=0;
		eventT->GetEntry(i);
		int n=PWO25Event->GetEntries();
		fHits =PWO25Event->GetHits();
		for (int j=0;j<n;j++)
		{
			fHits=PWO25Event->GetHits();
			PWO25SimHit* aHit = (PWO25SimHit*) ((*fHits)[j]);
			E+=aHit->GetE();
		}
		h1->Fill(E/1000.);
	}

	h1->SetBinContent(1,0);

	c1 = new TCanvas("c1","1",100,10,700,500);
	h1->Draw();

}
