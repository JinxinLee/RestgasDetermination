{
  // Macro tests gamma energy correction
	
        gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
        gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
        rootlogon();
        basiclibs();

	TFile* f = new TFile("full_emc.root"); //file you want to analyse
	TTree *t=(TTree *) f->Get("cbmsim") ;
	TClonesArray* bump_array=new TClonesArray("PndEmcBump");
	t->SetBranchAddress("EmcBump",&bump_array);

	TFile* fsim = new TFile("sim_emc.root"); //file you want to analyse
	TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
	PndEmcMapper::Init(6);

	TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
	tsim->SetBranchAddress("MCTrack",&mctrack_array);
	
	double bump_energy;
	double bump_energy_corrected;
	
	TH1F *h1= new TH1F("h1","Cluster energy",100,0.0,1.10);
	TH1F *h2= new TH1F("h2","Cluster energy corrected",100,0.0,1.10);
	
		
	// Cluster energy
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<bump_array->GetEntriesFast(); i++)
		{
			PndEmcBump *bump=(PndEmcBump*)bump_array->At(i);
			bump_energy=bump->energy();
			bump_energy_corrected=bump->GetEnergyCorrected();

			if (bump_energy>0.1){
				h1->Fill(bump_energy);
				h2->Fill(bump_energy_corrected);
			}
		}
	}
	
	TCanvas* c1 = new TCanvas("c1", "Cluster Energy", 100, 100, 800, 800); 	
	c1->Divide(1,2);
	c1->cd(1);
	h1->SetTitle("Cluster energy of 1 GeV photon");
	h1->GetXaxis()->SetTitle("Energy, GeV");
	h1->Draw();
	c1->cd(2);
	h2->SetTitle("Cluster energy of 1 GeV photon (corrected)");
	h2->GetXaxis()->SetTitle("Energy, GeV");
	h2->Draw();

}

