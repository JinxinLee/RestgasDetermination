{
  // Macro loads plots different information for the cluster
  
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libMCStack");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	gSystem->Load("libPassive");
	// add the detector libraries you need
	gSystem->Load("libEmc");	 
	gSystem->Load("libGeom.so");
	
	TFile* fsim = new TFile("sim_emc.root");
	TTree *tsim=(TTree *) fsim->Get("cbmsim");
	PndEmcMapper *emcMap=PndEmcMapper::Instance(1);

	TFile* f = new TFile("full_emc.root");
	TTree *t=(TTree *) f->Get("cbmsim") ;
	
	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	t->SetBranchAddress("EmcCluster",&cluster_array);

	TH1F *h= new TH1F("h","Cluster energy",100,0.0,1.05);
	
	// energy of central crystal
	TH1F *h1= new TH1F("h1","Cluster energy",100,0.0,1.05);
	// energy of 9 central crystals
	TH1F *h9= new TH1F("h9","Cluster energy",100,0.0,1.05);
	// energy of 25 central crystals
	TH1F *h25= new TH1F("h25","Cluster energy",100,0.0,1.05);
	
	Double_t cluster_energy, e1,e9,e25;
	// Cluster energy
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
		{
			PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
			cluster_energy=cluster->energy();
			if ((cluster->numberOfDigis()>1)&&(cluster_energy>0.02)) {
				h->Fill(cluster_energy);
				PndEmcClusterEnergySums esums = cluster->Esums();
				e1=esums.E1();
				e9=esums.E9();
				e25=esums.E25();
				h1->Fill(e1);
				h9->Fill(e9);
				h25->Fill(e25);
			}
		}
	}

	TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
	c1->Divide(2,2);
	c1->cd(1);
	h->SetTitle("Cluster energy of 1 GeV photon");
	h->GetXaxis()->SetTitle("Energy, GeV");
	h->Draw();

	c1->cd(2);
	h1->SetTitle("Energy of central crystal");
	h1->GetXaxis()->SetTitle("Energy, GeV");
	h1->Draw();
	
	c1->cd(3);
	h9->SetTitle("Energy of 9 central crystal");
	h9->GetXaxis()->SetTitle("Energy, GeV");
	h9->Draw();
	
	c1->cd(4);
	h25->SetTitle("Energy of 25 central crystal");
	h25->GetXaxis()->SetTitle("Energy, GeV");
	h25->Draw();
	

}

