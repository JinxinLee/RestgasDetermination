{
  // Invariant mass reconstruction
	double threshold=0.02;
  
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	rootlogon();
	basiclibs();
	
	TFile* f = new TFile("full_emc.root"); //file you want to analyse
	PndEmcMapper::Init(6);
	TTree *t=(TTree *) f->Get("cbmsim") ;
	
	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	t->SetBranchAddress("EmcCluster",&cluster_array);
	
	double cluster_energy;
	TH1F *h1= new TH1F("h1","Invariant mass",100,0.05,0.18);
	
	TVector3 v1,v2;
	// Cluster energy
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
		{
			PndEmcCluster *cluster1=(PndEmcCluster*)cluster_array->At(i);
			if (cluster1->energy()<threshold) continue;
			v1=cluster1->where();
			
			for (Int_t k=i; k<cluster_array->GetEntriesFast(); k++)
			{
				PndEmcCluster *cluster2=(PndEmcCluster*)cluster_array->At(k);
				if (cluster2->energy()<threshold) continue;
				v2=cluster2->where();
				double alpha=v1.Angle(v2);
				double invMass=sqrt(2*cluster1->energy()*cluster2->energy()*(1-cos(alpha)));
				h1->Fill(invMass);
			}
			
		}
	}

	TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800); 	
	h1->SetTitle("#pi^{0} Invariant mass reconstruction");
	h1->SetStats(kTRUE);
	h1->GetXaxis()->SetTitle("Energy, GeV");
	h1->Draw();

	TF1 *f1 = new TF1("f1","gaus(0)",50.,150.);
	Double_t par1[3]={60,0.130,0.01};
	f1->SetParameters(par1);
	f1->SetLineColor(2);

	h1->Fit("f1");
	double mu1=f1->GetParameter(1);
	double sigma1=f1->GetParameter(2);
	
	

}

