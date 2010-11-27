// QA macro for EMC cluster energy correction
// Two methods can be tested depending on input parametr (method=1 or 2)
// Details concerning "geometry" parameter are in PndEmcClusterCalibrator
void emc_correction_QA(TString InputFile="emc_complete_1000.root", Int_t method=2, Int_t geometry=1)
{
	gROOT->SetStyle("Plain");
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	
	TH1F *h_energy= new TH1F("h_energy","Cluster energy",100,0.2,1.5);
	TH1F *h_energy_corr= new TH1F("h_energy_corr","Cluster energy (corrected)",100,0.2,1.5);
	TH1F *h_energy_shashlyk= new TH1F("h_energy_shashlyk","Cluster energy (shashlyk)",100,0.2,1.5);
	TH1F *h_energy_shashlyk_corr= new TH1F("h_energy_shashlyk_corr","Cluster energy (shashlyk) (corrected)",100,0.2,1.5);
	TH1F *h_theta= new TH1F("h_theta","Polar angle",100,-5.0,5.0);
	TH1F *h_theta_corr= new TH1F("h_theta_corr","Polar angle (corrected)",100,-5.0,5.0);
	
	TH2F *hEnergyRatioTheta=new TH2F("hEnergyRatioTheta","Energy_reco/Energy_MC",100, 0., 180., 100, 0.2, 1.5);
	hEnergyRatioTheta->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
	
	TH2F *hEnergyRatioEnergy=new TH2F("hEnergyRatioEnergy","Energy_reco/Energy_MC",100, 0., 10., 100, 0.2, 1.5);
	hEnergyRatioEnergy->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
  
	TH2F *hThetaDiffEnergy=new TH2F("hThetaDiffEnergy","Theta_MC - Theta_reco",100,0.,10., 100, -5.,5.);
	hThetaDiffEnergy->GetXaxis()->SetTitle("Cluster Reconstructed  Energy (GeV)");
	
	TH2F *hThetaDiffTheta=new TH2F("hThetaDiffTheta","Theta_MC - Theta_reco",100,0.,180., 100, -5.,5.);
	hThetaDiffTheta->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");

	TH2F *hEnergyRatioCorrEnergy=new TH2F("hEnergyRatioCorrEnergy","Energy_reco (corrected)/Energy_MC",100, 0., 10., 100, 0.2, 1.5);
	hEnergyRatioCorrEnergy->GetXaxis()->SetTitle("Cluster Reconstructed Energy (GeV)");
	
	TH2F *hEnergyRatioCorrTheta=new TH2F("hEnergyRatioCorrTheta","Energy_reco (corrected)/Energy_MC",100, 0., 180., 100, 0.2, 1.5);
	hEnergyRatioCorrTheta->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
  
	TH2F *hThetaDiffCorrEnergy=new TH2F("hThetaDiffCorrEnergy","Theta_MC - Theta_reco (corrected)",100,0.,10., 100, -5.,5.);
	hThetaDiffCorrEnergy->GetXaxis()->SetTitle("Cluster Reconstructed  Energy (GeV)");
	
	TH2F *hThetaDiffCorrTheta=new TH2F("hThetaDiffCorrTheta","Theta_MC - Theta_reco (corrected)",100,0.,180., 100, -5.,5.);
	hThetaDiffCorrTheta->GetXaxis()->SetTitle("Cluster Reconstructed #theta Angle (#circ)");
	
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
	// third parameter is version (defined by geometry)
	PndEmcAbsClusterCalibrator * calibrator= PndEmcClusterCalibrator::MakeEmcClusterCalibrator(method, 0, "TGeant3");
	
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
				Double_t energy_corr = calibrator->Energy(cluster);
				TVector3 pos_corr = calibrator->Where(cluster);
				Double_t theta_corr=pos_corr.Theta()*180./TMath::Pi();
			}
			
			Double_t e_ratio=max_energy/energyMC;
			Double_t delta_theta=thetaMC-cluster_theta;
			
			Double_t e_ratio_corr=energy_corr/energyMC;
			Double_t delta_theta_corr=thetaMC-theta_corr;
			
			if (cluster_theta<5.)
			{
				h_energy_shashlyk->Fill(e_ratio);
				h_energy_shashlyk_corr->Fill(e_ratio_corr);
			} else {
				h_energy->Fill(e_ratio);
				h_energy_corr->Fill(e_ratio_corr);
			}
			
			h_theta->Fill(delta_theta);
			h_theta_corr->Fill(delta_theta_corr);
			hEnergyRatioEnergy->Fill( energyMC, e_ratio);
			hEnergyRatioTheta->Fill( thetaMC, e_ratio);
			hThetaDiffEnergy->Fill(energyMC, delta_theta);
			hThetaDiffTheta->Fill(thetaMC, delta_theta);
			hEnergyRatioCorrEnergy->Fill(energyMC, e_ratio_corr);
			hEnergyRatioCorrTheta->Fill(thetaMC, e_ratio_corr);
			hThetaDiffCorrEnergy->Fill(energyMC, delta_theta_corr);
			hThetaDiffCorrTheta->Fill(thetaMC, delta_theta_corr);
		}
	}
	
	TCanvas* c1 = new TCanvas("c1", "Energy reconstruction vs Energy", 100, 100, 800, 800);
	c1->Divide(2,1);
	c1->cd(1);
	hEnergyRatioEnergy->Draw("contz");
	TLine *l=new TLine(0.,1.,10.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	
	c1->cd(2);
	hEnergyRatioCorrEnergy->Draw("contz");
	TLine *l=new TLine(0.,1.,10.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	
	
	TCanvas* c2 = new TCanvas("c2", "Energy reconstruction vs Theta", 100, 100, 800, 800);
	c2->Divide(2,1);
	c2->cd(1);
	hEnergyRatioTheta->Draw("contz");
	TLine *l=new TLine(0.,1.,180.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	c2->cd(2);
	hEnergyRatioCorrTheta->Draw("contz");
	TLine *l=new TLine(0.,1.,180.,1.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();


	TCanvas* c3 = new TCanvas("c3", "Theta reconstruction vs Energy", 100, 100, 800, 800);
	c3->Divide(2,1);
	c3->cd(1);
	hThetaDiffEnergy->Draw("contz");
	TLine *l=new TLine(0.,0.,10.,0.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	c3->cd(2);
	hThetaDiffCorrEnergy->Draw("contz");	
	TLine *l=new TLine(0.,0.,10.,0.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();

	
	TCanvas* c4 = new TCanvas("c4", "Theta reconstruction vs Theta", 100, 100, 800, 800);
	c4->Divide(2,1);
	c4->cd(1);
	hThetaDiffTheta->Draw("contz");
	TLine *l=new TLine(0.,0.,180.,0.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();
	c4->cd(2);
	hThetaDiffCorrTheta->Draw("contz");
	TLine *l=new TLine(0.,0.,180.,0.0);
	l->SetLineColor(2);
	l->SetLineWidth(2);
	l->Draw();

	TCanvas* c5 = new TCanvas("c5", "Energy", 100, 100, 800, 800);
	c5->Divide(2,1);
	c5->cd(1);
	h_energy->Draw();
	c5->cd(2);
	h_energy_corr->Draw();
	
	TCanvas* c6 = new TCanvas("c6", "Energy", 100, 100, 800, 800);
	c6->Divide(2,1);
	c6->cd(1);
	h_energy_shashlyk->Draw();
	c6->cd(2);
	h_energy_shashlyk_corr->Draw();
	
	TCanvas* c7 = new TCanvas("c7", "Theta", 100, 100, 800, 800);
	c7->Divide(2,1);
	c7->cd(1);
	h_theta->Draw();
	c7->cd(2);
	h_theta_corr->Draw();
}