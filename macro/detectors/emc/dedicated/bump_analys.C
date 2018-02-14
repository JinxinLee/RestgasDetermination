{
	// Plot opening angle between 2 bumps and invariant mass of 2 bumps
	// It is assumed that high enrgy pi0 was generated which lead to 1 cluster with 2 bumps
	
	Double_t threshold=0.02;
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  rootlogon();
  basiclibs();
	
	TFile* f = new TFile("cluster_emc.root"); //file you want to analyse
	TTree *t=(TTree *) f->Get("cbmsim") ;
	TClonesArray* bump_array=new TClonesArray("PndEmcBump");
	t->SetBranchAddress("EmcBump",&bump_array);
	
	TFile* fsim = new TFile("sim_emc.root"); //file you want to analyse
	TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
	TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
	tsim->SetBranchAddress("MCTrack",&mctrack_array);
	
	TFile* fpar = new TFile("simparams.root"); 
	fpar->Get("FairBaseParSet");
	
	PndEmcMapper *emcMap=PndEmcMapper::Instance(2);

	TVector3 mc_momentum1, mc_momentum2;
	TVector3 mc_vertex1, mc_vertex2;

	TH1F *h_MC_angle= new TH1F("h_MC_angle","MC truth: opening angle of #pi^{0}",200,0.,15.);
	TH1F *h_angle= new TH1F("h_angle","reconstructed opening angle of #pi^{0}",200,0.,15.);
	TH1F *h_egamma= new TH1F("h_egamma","Energy of #gamma 's (MC truth)",100,0.0,10.0);
	TH1F *h_ebump= new TH1F("h_ebump","Bump energy",100,0.0,10.0);
	TH1F *h_MC= new TH1F("h_MC","Invariant mass",100,0.0,0.3);
	TH1F *h_MC_mass= new TH1F("h_MC_mass","Invariant mass MC",100,0.0,0.3);
	
	TH2F *h_energy_vs_angle_MC=new TH2F("h_energy_vs_angle_MC", "Energy vs. opening angle", 100,0.0,10.0,100,0.,15.0);
	
	
	TVector3 v1,v2; // Bump position
	Double_t bump_energy1=0;
	Double_t bump_energy2=0;
	
	// Bump energy
	cout<<"Number of events : "<<t->GetEntriesFast()<<endl;
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		t->GetEntry(j);
		for (Int_t i=0; i<bump_array->GetEntriesFast(); i++)
		{
			PndEmcBump *bump1=(PndEmcBump*)bump_array->At(i);
			bump_energy1=bump1->energy();
			if (bump_energy1<threshold) continue;
			h_ebump->Fill(bump_energy1);
			v1=bump1->where();
			
			for (Int_t k=i+1; k<bump_array->GetEntriesFast(); k++)
			{
				PndEmcBump *bump2=(PndEmcBump*)bump_array->At(k);
				bump_energy2=bump2->energy();
				if (bump_energy2<threshold) continue;
				v2=bump2->where();
				double alpha=v1.Angle(v2);
				double invMass=sqrt(2*bump_energy1*bump_energy2*(1-cos(alpha)));
				h_angle->Fill(alpha*TMath::RadToDeg());
				h_MC->Fill(invMass);
			}
			
		}
	}

	TVector3 pi0_momentum;
	for (Int_t j=0; j< t->GetEntriesFast(); j++)
	{
		tsim->GetEntry(j);
		
		for (Int_t i=0; i<mctrack_array->GetEntriesFast(); i++)
		{
			PndMCTrack *mctrack1=(PndMCTrack *) mctrack_array->At(i);
			if (mctrack1->GetPdgCode()==22) // photons
			{
				mc_momentum1=mctrack1->GetMomentum();
				mc_vertex1=mctrack1->GetStartVertex();
				if (mc_vertex1.Mag()>2.) continue;
				h_egamma->Fill(mc_momentum1.Mag());
				
				for (Int_t k=i+1; k<mctrack_array->GetEntriesFast(); k++)
				{
					PndMCTrack *mctrack2=(PndMCTrack *) mctrack_array->At(k);
					if (mctrack2->GetPdgCode()==22) // photons
					{
						mc_momentum2=mctrack2->GetMomentum();
						mc_vertex2=mctrack2->GetStartVertex();
						if (mc_vertex2.Mag()>2.) continue;
						double alphaMC=mc_momentum1.Angle(mc_momentum2);
						double invMassMC=sqrt(2*mc_momentum1.Mag()*mc_momentum2.Mag()*(1-cos(alphaMC)));
						h_MC_angle->Fill(alphaMC*TMath::RadToDeg());
						h_MC_mass->Fill(invMassMC);
						pi0_momentum=mc_momentum1+mc_momentum2;
						h_energy_vs_angle_MC->Fill(pi0_momentum.Mag(),alphaMC*TMath::RadToDeg());

					}
				}
			}
		}
	}

	TCanvas* c1 = new TCanvas("c1", "c1", 100, 100, 800, 800);
	c1->Divide(1,2);
	c1->cd(1);
	h_MC_angle->SetTitle("MC truth: opening angle of #pi^{0}");
	h_MC_angle->GetXaxis()->SetTitle("Opening angle, degree");
	h_MC_angle->Draw();
	
	c1->cd(2);
	h_angle->SetTitle("reconstructed opening angle of #pi^{0}");
	h_angle->GetXaxis()->SetTitle("Opening angle, degree");
	h_angle->Draw();
	
	TCanvas* c2 = new TCanvas("c2", "c2", 100, 100, 800, 800); 
	c2->Divide(1,2);
	c2->cd(1);
	h_egamma->SetTitle("Energy of #gamma 's (MC truth)");
	h_egamma->GetXaxis()->SetTitle("Energy, GeV");
	h_egamma->Draw();

	c2->cd(2);
	h_ebump->SetTitle("Energy of bumps");
	h_ebump->GetXaxis()->SetTitle("Energy, GeV");
	h_ebump->Draw();

	TCanvas* c3 = new TCanvas("c3", "c3", 100, 100, 800, 800);
	c3->Divide(1,2);
	c3->cd(1);
	h_MC->SetTitle("Invariant mass");
	h_MC->GetXaxis()->SetTitle("M, GeV");
	h_MC->Draw();
	
	c3->cd(2);
	h_MC_mass->SetTitle("Invariant mass MC");
	h_MC_mass->GetXaxis()->SetTitle("M, GeV");
	h_MC_mass->Draw();
	
	TCanvas* c4 = new TCanvas("c4", "c4", 100, 100, 800, 800);
	h_energy_vs_angle_MC->SetTitle("Energy vs. opening angle (MC Truth)");
	h_energy_vs_angle_MC->GetXaxis()->SetTitle("Energy, GeV");
	h_energy_vs_angle_MC->GetYaxis()->SetTitle("Angle, degree");
	h_energy_vs_angle_MC->Draw();
}

