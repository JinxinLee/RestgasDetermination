{
	bool vtxfit=0;
	
	if (vtxfit)
		TString inFile="etac_histo_vtx_tpc.root";
	else 
		TString inFile="etac_histo_4c_tpc.root";
	
	TFile *f = TFile::Open(inFile);

	TH1F *h_etac_nocut=(TH1F *)f->Get("h_etac_nocut");
	TH1F *h_etac_pid=(TH1F *)f->Get("h_etac_pid");
	TH1F *h_etac_phimass=(TH1F *)f->Get("h_etac_phimass");
	TH1F *h_etac_vtx=(TH1F *)f->Get("h_etac_vtx");
	TH1F *h_etac_4c=(TH1F *)f->Get("h_etac_4c");

	TH1F *h_mphi_nocuts=(TH1F *)f->Get("h_mphi_nocuts");
	TH1F *h_mphi_pid=(TH1F *)f->Get("h_mphi_pid");
	TH1F *h_mphi_vtx=(TH1F *)f->Get("h_mphi_vtx");
	TH1F *h_mphi_4c=(TH1F *)f->Get("h_mphi_4c");
	TH1F *h_mphi_final=(TH1F *)f->Get("h_mphi_final");
	
	TH1F *nc=(TH1F *)f->Get("nc");
	 
	TH1F *h_chi2_4c=(TH1F *)f->Get("h_chi2_4c");
	TH1F *h_chi2b_4c=(TH1F *)f->Get("h_chi2b_4c");
	
	TH1F *h_chi2_vtx=(TH1F *)f->Get("h_chi2_vtx");
	TH1F *h_chi2b_vtx=(TH1F *)f->Get("h_chi2b_vtx");
	TH1F *hvzpos=(TH1F *)f->Get("hvzpos");
	TH2F *hvpos=(TH2F *)f->Get("hvpos");
	
	TH1F *hvtxresX=(TH1F *)f->Get("hvtxresX");
	TH1F *hvtxresY=(TH1F *)f->Get("hvtxresY");
	TH1F *hvtxresZ=(TH1F *)f->Get("hvtxresZ");
	
	TH1F *n_etac=(TH1F *)f->Get("n_etac");
	TH1F *n_events=(TH1F *)f->Get("n_events");
	
	double eff=n_etac->GetBinContent(1)/n_events->GetBinContent(1);
	std::cout<<"Efficiency = "<<eff<<std::endl;
	
	// Max efficiency
	// Numebr of events with >=4 reconstructed tracks
	std::cout<<"Max efficiency="<<nc->Integral(5,20)/nc->GetEntries()<<std::endl;

	TCanvas *c1=new TCanvas("c1","c1",600,600);
	nc->Draw();
	c1->SaveAs("c1_tpc.png");

	TCanvas *c2=new TCanvas("c2","c2",600,600);
	c2->Divide(1,2);
	c2->cd(1);
	h_mphi_nocuts->Draw();
	c2->cd(2);
	h_etac_nocut->Draw();
	c2->SaveAs("c2_tpc.png");
	
	TCanvas *c3=new TCanvas("c3","c3",600,600);
	c3->Divide(1,2);
	c3->cd(1);
	h_mphi_pid->Draw();
	c3->cd(2);
	h_etac_pid->Draw();
	c3->SaveAs("c3_tpc.png");
	
	if (!vtxfit)
	{
		TCanvas *c6=new TCanvas("c6","c6",600,600);
		c6->Divide(2,1);
		c6->cd(1);
		h_chi2_4c->Draw();
		c6->cd(2);
		h_chi2b_4c->Draw();
		c6->SaveAs("c6_tpc.png");
		
		TCanvas *c4=new TCanvas("c4","c4",600,600);
		c4->Divide(1,2);
		c4->cd(1);
		h_mphi_4c->Draw();
		c4->cd(2);
		h_etac_4c->Draw();
		c4->SaveAs("c4_tpc.png");
	}
	
	if (vtxfit)
	{
		TCanvas *c7=new TCanvas("c7","c7",600,600);
		c7->Divide(2,1);
		c7->cd(1);
		hvpos->Draw();
		c7->cd(2);
		hvzpos->Draw();
		
		TCanvas *c8=new TCanvas("c8","c8",600,600);
		c8->Divide(2,1);
		c8->cd(1);
		h_chi2_vtx->Draw();
		c8->cd(2);
		h_chi2b_vtx->Draw();
		
		TCanvas *c9=new TCanvas("c9","c9",600,600);
		c9->Divide(3,1);
		c9->cd(1);
		hvtxresX->Draw();
		c9->cd(2);
		hvtxresY->Draw();
		c9->cd(3);
		hvtxresZ->Draw();
		
		TCanvas *c10=new TCanvas("c10","c10",600,600);
		c10->Divide(1,2);
		c10->cd(1);
		h_mphi_vtx->Draw();
		c10->cd(2);
		h_etac_vtx->Draw();
		c10->SaveAs("c10_tpc.png");
	}

	TCanvas *c5=new TCanvas("c5","c5",600,600);
	c5->Divide(1,2);
	c5->cd(1);
	h_mphi_final->Draw();
	
	double mean_phi=1.02;
	double range_phi=0.02;
	
	TF1 *f1_phi = new TF1("f1_phi","gaus",0.9,1.1);
	h_mphi_final->Fit(f1_phi,"R","",mean_phi-range_phi,mean_phi+range_phi);
	
	double sigma1_phi=f1_phi->GetParameter(2);
	double mean1_phi=f1_phi->GetParameter(1);
	
	TF1 *f2_phi = new TF1("f2_phi","gaus",0.9,1.1);
	h_mphi_final->Fit(f2_phi,"R","",mean1_phi-1.6*sigma1_phi,mean1_phi+1.6*sigma1_phi);

	double sigma2_phi=f2_phi->GetParameter(2);
	std::cout<<"sigma phi="<<sigma2_phi<<std::endl;

	c5->cd(2);
	h_etac_phimass->Draw();
	// fit eta_c
	double mean_etac=2.98;
	double range_etac=0.1;
	
	TF1 *f1 = new TF1("f1","gaus",2.8,3.2);
	h_etac_phimass->Fit(f1,"R","",mean_etac-range_etac,mean_etac+range_etac);
	
	double sigma1=f1->GetParameter(2);
	double mean1=f1->GetParameter(1);
	
	TF1 *f2 = new TF1("f2","gaus",2.8,3.2);
	h_etac_phimass->Fit(f2,"R","",mean1-1.6*sigma1,mean1+1.6*sigma1);

	double sigma2=f2->GetParameter(2);
	std::cout<<"sigma="<<sigma2<<std::endl;
	
	c5->SaveAs("c5_tpc.png");
	
}
