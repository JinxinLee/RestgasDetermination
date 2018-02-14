// QA macro for EMC cluster energy correction
// Depending on "test" input parametr checks
// 1 - energy ratio of photon vs theta and E
// 2 - energy distribution for single 1 GeV gammas
// 3 - pi0 invariant mass
// Two methods can be tested depending on input parametr (method=1 or 2)
int emc_correction_QA(TString InputFile="emc_complete_QA.root", Int_t test=1, Int_t version=1)
{
	// test=1
	TH1F *h_energy= new TH1F("h_energy","Cluster energy",100,0.8,1.2);
	TH1F *h_energy_corr1= new TH1F("h_energy_corr1","Cluster energy (corrected), (hist)",100,0.8,1.2);
	TH1F *h_energy_corr2= new TH1F("h_energy_corr2","Cluster energy (corrected), (parametrization)",100,0.8,1.2);
	TH1F *h_energy_corr_old= new TH1F("h_energy_corr_old","Cluster energy (corrected), (old)",100,0.8,1.2);
	TH1F *h_energy_shashlyk= new TH1F("h_energy_shashlyk","Cluster energy (shashlyk)",100,0.8,1.2);
	TH1F *h_energy_shashlyk_corr1= new TH1F("h_energy_shashlyk_corr1","Cluster energy (shashlyk) (corrected), (hist)",100,0.8,1.2);
	TH1F *h_energy_shashlyk_corr2= new TH1F("h_energy_shashlyk_corr2","Cluster energy (shashlyk) (corrected), (parametrization)",100,0.8,1.2);
	TH1F *h_energy_shashlyk_corr_old= new TH1F("h_energy_shashlyk_corr_old","Cluster energy (shashlyk) (corrected)",100,0.8,1.2);

	// test=2
	TH2F *hEnergyRatioTheta=new TH2F("hEnergyRatioTheta","Energy_reco/Energy_MC",100, 0., 180., 100, 0.8, 1.2);
	hEnergyRatioTheta->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
	TH2F *hEnergyRatioEnergy=new TH2F("hEnergyRatioEnergy","Energy_reco/Energy_MC",100, 0., 10., 100, 0.8, 1.2);
	hEnergyRatioEnergy->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
	TH2F *hEnergyRatioCorrEnergy1=new TH2F("hEnergyRatioCorrEnergy1","Energy_reco (corrected)/Energy_MC, (hist)",100, 0., 10., 100, 0.8, 1.2);
	hEnergyRatioCorrEnergy1->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
	TH2F *hEnergyRatioCorrTheta1=new TH2F("hEnergyRatioCorrTheta1","Energy_reco (corrected)/Energy_MC, (hist)",100, 0., 180., 100, 0.8, 1.2);
	hEnergyRatioCorrTheta1->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
	TH2F *hEnergyRatioCorrEnergy2=new TH2F("hEnergyRatioCorrEnergy2","Energy_reco (corrected)/Energy_MC, (parametrization)",100, 0., 10., 100, 0.8, 1.2);
	hEnergyRatioCorrEnergy2->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
	TH2F *hEnergyRatioCorrTheta2=new TH2F("hEnergyRatioCorrTheta2","Energy_reco (corrected)/Energy_MC, (parametrization)",100, 0., 180., 100, 0.8, 1.2);
	hEnergyRatioCorrTheta2->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
	TH2F *hEnergyRatioCorrEnergyOld=new TH2F("hEnergyRatioCorrEnergyOld","Energy_reco (corrected)/Energy_MC, (old)",100, 0., 10., 100, 0.8, 1.2);
	hEnergyRatioCorrEnergyOld->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
	TH2F *hEnergyRatioCorrThetaOld=new TH2F("hEnergyRatioCorrThetaOld","Energy_reco (corrected)/Energy_MC, (old)",100, 0., 180., 100, 0.8, 1.2);
	hEnergyRatioCorrThetaOld->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
  
	// test=3
	TH1F *h_mpi0= new TH1F("h_mpi0","pi0 invariant mass",100,0.05,0.2);
	TH1F *h_mpi0_corr1= new TH1F("h_mpi0_corr1","pi0 invariant mass (corrected) (hist)",100,0.05,0.2);
	TH1F *h_mpi0_corr2= new TH1F("h_mpi0_corr2","pi0 invariant mass (corrected) (parametrization)",100,0.05,0.2);
	TH1F *h_mpi0_corr_old= new TH1F("h_mpi0_corr_old","pi0 invariant mass (corrected) (old)",100,0.05,0.2);

	TFile *f=new TFile(InputFile);
	TTree *t=(TTree *) f->Get("cbmsim") ;
	
	TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
	t->SetBranchAddress("MCTrack",&mctrack_array);

	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
	t->SetBranchAddress("EmcCluster",&cluster_array);

	Double_t cluster_energy, cluster_theta, cluster_phi; //position of the cluster
	TVector3 mc_momentum;
	Double_t thetaMC, phiMC, energyMC;
	Double_t max_energy;
	
	// Here the cluster calibrator is set
	// first parameter is method (1-histogram, 2-parametrization)
	// second parameter is version (defined by geometry)
	PndEmcAbsClusterCalibrator * calibrator1= PndEmcClusterCalibrator::MakeEmcClusterCalibrator(1, version);
	PndEmcAbsClusterCalibrator * calibrator2= PndEmcClusterCalibrator::MakeEmcClusterCalibrator(2, version);
	
	if ((test==1)||(test==2))
	{
		for (Int_t j=0; j<t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0);
			mc_momentum=mctrack->GetMomentum();
			energyMC=mc_momentum.Mag();
			thetaMC=mc_momentum.Theta()*180./TMath::Pi();
			max_energy=0;
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
				cluster_energy=cluster->energy();

				if (cluster_energy>max_energy)
				{
					max_energy=cluster_energy;
					TVector3 cluster_pos=cluster->where();
					cluster_theta=cluster_pos.Theta()*180./TMath::Pi();
					Double_t energy_corr1 = calibrator1->Energy(cluster);
					Double_t energy_corr2 = calibrator2->Energy(cluster);
					Double_t energy_corr_old = cluster->GetEnergyCorrected();
				}
				
				Double_t e_ratio=max_energy/energyMC;
				Double_t e_ratio_corr1=energy_corr1/energyMC;
				Double_t e_ratio_corr2=energy_corr2/energyMC;
				Double_t e_ratio_corr_old=energy_corr_old/energyMC;

				if (cluster_theta<5.)
				{
					h_energy_shashlyk->Fill(max_energy);
					h_energy_shashlyk_corr1->Fill(energy_corr1);
					h_energy_shashlyk_corr2->Fill(energy_corr2);
					h_energy_shashlyk_corr_old->Fill(energy_corr_old);
				} else {
					h_energy->Fill(max_energy);
					h_energy_corr1->Fill(energy_corr1);
					h_energy_corr2->Fill(energy_corr2);
					h_energy_corr_old->Fill(energy_corr_old);
				}
				
				hEnergyRatioEnergy->Fill( energyMC, e_ratio);
				hEnergyRatioTheta->Fill( thetaMC, e_ratio);
				hEnergyRatioCorrEnergy1->Fill(energyMC, e_ratio_corr1);
				hEnergyRatioCorrTheta1->Fill(thetaMC, e_ratio_corr1);
				hEnergyRatioCorrEnergy2->Fill(energyMC, e_ratio_corr2);
				hEnergyRatioCorrTheta2->Fill(thetaMC, e_ratio_corr2);
				hEnergyRatioCorrEnergyOld->Fill(energyMC, e_ratio_corr_old);
				hEnergyRatioCorrThetaOld->Fill(thetaMC, e_ratio_corr_old);
			}
		}
	}
	
	if (test==3)
	{
		double threshold=0.02;
		TVector3 v1,v2;
		TVector3 v1_corr1,v2_corr1;
		TVector3 v1_corr2,v2_corr2;
		Double_t energy1, energy2, energy1_corr1, energy2_corr1;
		Double_t energy1_corr2, energy2_corr2, energy1_corr_old, energy2_corr_old;
		// Cluster energy
		for (Int_t j=0; j< t->GetEntriesFast(); j++)
		{
			t->GetEntry(j);
			for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
			{
				PndEmcCluster *cluster1=(PndEmcCluster*)cluster_array->At(i);
				energy1=cluster1->energy();
				if (energy1<threshold) continue;
				v1=cluster1->where();
				energy1_corr1 = calibrator1->Energy(cluster1);
				v1_corr1 = calibrator1->Where(cluster1);
				energy1_corr2 = calibrator2->Energy(cluster1);
				v1_corr2 = calibrator2->Where(cluster1);
				energy1_corr_old = cluster1->GetEnergyCorrected();
				
				for (Int_t k=i; k<cluster_array->GetEntriesFast(); k++)
				{
					PndEmcCluster *cluster2=(PndEmcCluster*)cluster_array->At(k);
					if (cluster2->energy()<threshold) continue;
					energy2=cluster2->energy();
					energy2_corr1 = calibrator1->Energy(cluster2);
					energy2_corr2 = calibrator2->Energy(cluster2);
					energy2_corr_old = cluster2->GetEnergyCorrected();
					v2=cluster2->where();
					v2_corr1 = calibrator1->Where(cluster2);
					v2_corr2 = calibrator2->Where(cluster2);
					double alpha=v1.Angle(v2);
					double invMass=sqrt(2*energy1*energy2*(1-cos(alpha)));
					h_mpi0->Fill(invMass);
					double alpha_corr1=v1_corr1.Angle(v2_corr1);
					double invMass_corr1=sqrt(2*energy1_corr1*energy2_corr1*(1-cos(alpha_corr1)));
					h_mpi0_corr1->Fill(invMass_corr1);
					double alpha_corr2=v1_corr2.Angle(v2_corr2);
					double invMass_corr2=sqrt(2*energy1_corr2*energy2_corr2*(1-cos(alpha_corr2)));
					h_mpi0_corr2->Fill(invMass_corr2);
					double invMass_corr_old=sqrt(2*energy1_corr_old*energy2_corr_old*(1-cos(alpha)));
					h_mpi0_corr_old->Fill(invMass_corr_old);
				}
			}
		}
	}	
		
	
if (test==1)
{
	TCanvas* c1 = new TCanvas("c1", "Energy reconstruction vs Energy", 100, 100, 800, 800);
	c1->Divide(2,2);
	c1->cd(1);
	hEnergyRatioEnergy->Draw("contz");
	TLine *l=new TLine(0.,1.,10.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	
	c1->cd(2);
	hEnergyRatioCorrEnergy1->Draw("contz");
	TLine *l=new TLine(0.,1.,10.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	
	c1->cd(3);
	hEnergyRatioCorrEnergy2->Draw("contz");
	TLine *l=new TLine(0.,1.,10.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();

// 	c1->cd(4);
// 	hEnergyRatioCorrEnergyOld->Draw("contz");
// 	TLine *l=new TLine(0.,1.,10.,1.0);
// 	l->SetLineColor(2);
// 	l->SetLineWidth(2);
// 	l->Draw();
	
	TCanvas* c2 = new TCanvas("c2", "Energy reconstruction vs Theta", 100, 100, 800, 800);
	c2->Divide(2,2);
	c2->cd(1);
	hEnergyRatioTheta->Draw("contz");
	TLine *l=new TLine(0.,1.,180.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	c2->cd(2);
	hEnergyRatioCorrTheta1->Draw("contz");
	TLine *l=new TLine(0.,1.,180.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	c2->cd(3);
	hEnergyRatioCorrTheta2->Draw("contz");
	TLine *l=new TLine(0.,1.,180.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
// 	c2->cd(4);
// 	hEnergyRatioCorrThetaOld->Draw("contz");
// 	TLine *l=new TLine(0.,1.,180.,1.0);
// 	l->SetLineColor(2);
// 	l->SetLineWidth(2);
// 	l->Draw();
}

if (test==2)
{
	TCanvas* c3 = new TCanvas("c3", "Energy (TS)", 100, 100, 800, 800);
	c3->Divide(2,2);
	c3->cd(1);
	h_energy->Draw();
	c3->cd(2);
	h_energy_corr1->Draw();
	c3->cd(3);
	h_energy_corr2->Draw();
// 	c3->cd(4);
// 	h_energy_corr_old->Draw();
	
	TCanvas* c4 = new TCanvas("c4", "Energy (shashlyk)", 100, 100, 800, 800);
	c4->Divide(2,2);
	c4->cd(1);
	h_energy_shashlyk->Draw();
	c4->cd(2);
	h_energy_shashlyk_corr1->Draw();
	c4->cd(3);
	h_energy_shashlyk_corr2->Draw();
// 	c4->cd(4);
// 	h_energy_shashlyk_corr_old->Draw();
}
if (test==3)
{
	gStyle->SetOptFit(kTRUE);
	
	double pars[5]={800,0.13,0.01,100,-500};
	TF1 *f1 = new TF1("f1","gaus(0)+pol1(3)",0.08,0.18);
	TF1 *f2 = new TF1("f2","gaus(0)+pol1(3)",0.08,0.18);
	TF1 *f3 = new TF1("f3","gaus(0)+pol1(3)",0.08,0.18);
	TF1 *f4 = new TF1("f4","gaus(0)+pol1(3)",0.08,0.18);
	f1->SetParameters(pars);
	f2->SetParameters(pars);
	f3->SetParameters(pars);
	f4->SetParameters(pars);
	
	TCanvas* c5 = new TCanvas("c5", "pi0 mass", 100, 100, 800, 800);
	c5->Divide(2,2);
	c5->cd(1);
	h_mpi0->Draw();
	h_mpi0->Fit(f1,"R");
	c5->cd(2);
	h_mpi0_corr1->Draw();
	h_mpi0_corr1->Fit(f2,"R");
	c5->cd(3);
	h_mpi0_corr2->Draw();
	h_mpi0_corr2->Fit(f3,"R");
// 	c5->cd(4);
// 	h_mpi0_corr_old->Draw();
// 	h_mpi0_corr_old->Fit(f4,"R");
}
  return 0;
}
