{	
		TString inFile="singlepion_4pi_a.root";
	

	TFile *f = TFile::Open(inFile);

	TCanvas *c2=new TCanvas("c2","c2",600,600);
	c2->cd();
	mom_TOT->Draw();

	TCanvas *c3=new TCanvas("c3","c3",600,600);
	c3->cd();
	theta->Draw();

	TCanvas *c4=new TCanvas("c4","c4",600,600);
	c4->cd();
	diffmomvstheta->Draw();

	TCanvas *c7=new TCanvas("c7","c7",600,600);
	c7->cd();
	momvstheta->Draw();

	TCanvas *c5=new TCanvas("c5","c5",600,600);
	c5->cd();
	TF1 *f1 = new TF1("f1","gaus",-0.6,0.6);
	reco_mc_diff_phi->Fit(f1,"R");

	TCanvas *c6=new TCanvas("c6","c6",600,600);
	c6->cd();
	TF1 *f1 = new TF1("f1","gaus",-0.06,0.06);
	reco_mc_diff->Fit(f1,"R");

	TCanvas *c8=new TCanvas("c8","c8",600,600);
	c8->cd();
	TF1 *f1 = new TF1("f1","gaus",-0.18,0.18);
	reco_mc_diff_theta->Fit(f1,"R");
	

	
}
