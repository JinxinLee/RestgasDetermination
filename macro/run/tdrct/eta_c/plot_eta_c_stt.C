{
	bool saveHistos=false;
	TString inFile="etac_histo_stt.root";
	
	TFile *f = TFile::Open(inFile);

	TH1F *h_etac_nocut=(TH1F *)f->Get("h_etac_nocut");
	TH1F *h_etac_pid=(TH1F *)f->Get("h_etac_pid");
	TH1F *h_etac_vtx=(TH1F *)f->Get("h_etac_vtx");
	TH1F *h_etac_4c=(TH1F *)f->Get("h_etac_4c");
	TH1F *h_etac_phimass_4c=(TH1F *)f->Get("h_etac_phimass_4c");
	TH1F *h_etac_phimass_vtx=(TH1F *)f->Get("h_etac_phimass_vtx");
	TH1F *h_etac_phimassfit=(TH1F *)f->Get("h_etac_phimassfit");
	
	TH1F *h_mphi_nocuts=(TH1F *)f->Get("h_mphi_nocuts");
	TH1F *h_mphi_pid=(TH1F *)f->Get("h_mphi_pid");
	TH1F *h_mphi_vtx=(TH1F *)f->Get("h_mphi_vtx");
	TH1F *h_mphi_4c=(TH1F *)f->Get("h_mphi_4c");
	TH1F *h_mphi_final_4c=(TH1F *)f->Get("h_mphi_final_4c");
	TH1F *h_mphi_final_vtx=(TH1F *)f->Get("h_mphi_final_vtx");
	TH1F *h_mphi_final_massfit=(TH1F *)f->Get("h_mphi_final_massfit");
	
	
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
	
	TH1F *n_etac_4c=(TH1F *)f->Get("n_etac_4c");
	TH1F *n_etac_vtx=(TH1F *)f->Get("n_etac_vtx");
	TH1F *n_events=(TH1F *)f->Get("n_events");
	
	double eff1=n_etac_4c->GetBinContent(1)/n_events->GetBinContent(1);
	std::cout<<"Efficiency (4C-fit) = "<<eff1<<std::endl;
	double eff2=n_etac_vtx->GetBinContent(1)/n_events->GetBinContent(1);
	std::cout<<"Efficiency (vertex fit) = "<<eff2<<std::endl;
	
	// Max efficiency
	// Numebr of events with >=4 reconstructed tracks
	std::cout<<"Max efficiency="<<nc->Integral(5,20)/nc->GetEntries()<<std::endl;

	TCanvas *c1=new TCanvas("c1","N charged",600,600);
	nc->Draw();
	if (saveHistos) c1->SaveAs("n_charged_stt.png");

	TCanvas *c2=new TCanvas("c2","No cuts",600,600);
	c2->Divide(1,2);
	c2->cd(1);
	h_mphi_nocuts->Draw();
	c2->cd(2);
	h_etac_nocut->Draw();
	if (saveHistos) c2->SaveAs("m_nocuts_stt.png");
	
	TCanvas *c3=new TCanvas("c3","MC PID",600,600);
	c3->Divide(1,2);
	c3->cd(1);
	h_mphi_pid->Draw();
	c3->cd(2);
	h_etac_pid->Draw();
	if (saveHistos) c3->SaveAs("m_pid_stt.png");

	//////////// 4C-fit fit ////////////////////////////////
	TCanvas *c4=new TCanvas("c4","chi2 (4C-fit)",600,600);
	c4->Divide(2,1);
	c4->cd(1);
	h_chi2_4c->Draw();
	c4->cd(2);
	h_chi2b_4c->Draw();
	if (saveHistos) c4->SaveAs("chi2_4c_stt.png");

	TCanvas *c5=new TCanvas("c5","m (4C-fit)",600,600);
	c5->Divide(1,2);
	c5->cd(1);
	h_mphi_4c->Draw();
	c5->cd(2);
	h_etac_4c->Draw();
	if (saveHistos) c5->SaveAs("m_4c_stt.png");

	//////////// Vetrex fit ////////////////////////////////
	TCanvas *c6=new TCanvas("c6","Vertex position",600,600);
	c6->Divide(2,1);
	c6->cd(1);
	hvpos->Draw();
	c6->cd(2);
	hvzpos->Draw();
	if (saveHistos) c6->SaveAs("vertex_pos_stt.png");
	
	TCanvas *c7=new TCanvas("c7","Vertex fit chi2",600,600);
	c7->Divide(2,1);
	c7->cd(1);
	h_chi2_vtx->Draw();
	c7->cd(2);
	h_chi2b_vtx->Draw();
	if (saveHistos) c7->SaveAs("chi2_vtx_stt.png");
	
	TCanvas *c8=new TCanvas("c8","Vertex resolution",600,600);
	c8->Divide(3,1);
	c8->cd(1);
	hvtxresX->Draw();

	TF1 *f1_vtxx= new TF1("f1_vtxx","gaus",-1.,1.);
	hvtxresX->Fit(f1_vtxx,"R","",-1,1.);
	
	c8->cd(2);
	hvtxresY->Draw();
	TF1 *f1_vtxy= new TF1("f1_vtxy","gaus",-1.,1.);
	hvtxresY->Fit(f1_vtxy,"R","",-1,1.);

	c8->cd(3);
	hvtxresZ->Draw();
	TF1 *f1_vtxz= new TF1("f1_vtxz","gaus",-1.,1.);
	hvtxresZ->Fit(f1_vtxz,"R","",-1,1.);
	if (saveHistos) c8->SaveAs("vertex_res_stt.png");
	
	TCanvas *c9=new TCanvas("c9","m vertex",600,600);
	c9->Divide(1,2);
	c9->cd(1);
	h_mphi_vtx->Draw();
	c9->cd(2);
	h_etac_vtx->Draw();
	if (saveHistos) c9->SaveAs("m_vtx_stt.png");


	double mean_phi, range_phi, sigma1_phi, mean1_phi, sigma2_phi, mean_etac, range_etac;
	double sigma1, mean1, sigma2;
	
	//////////////////// 4C fit ////////////////
	TCanvas *c10=new TCanvas("c10","Mass final (4C-fit)",600,600);
	c10->Divide(1,2);
	c10->cd(1);
	h_mphi_final_4c->Draw();
	
	mean_phi=1.02;
	range_phi=0.02;
	
	TF1 *f1_phi_4c = new TF1("f1_phi_4c","gaus",0.9,1.1);
	h_mphi_final_4c->Fit(f1_phi_4c,"R","",mean_phi-range_phi,mean_phi+range_phi);
	
	sigma1_phi=f1_phi_4c->GetParameter(2);
	mean1_phi=f1_phi_4c->GetParameter(1);
	
	TF1 *f2_phi_4c = new TF1("f2_phi_4c","gaus",0.9,1.1);
	h_mphi_final_4c->Fit(f2_phi_4c,"R","",mean1_phi-1.6*sigma1_phi,mean1_phi+1.6*sigma1_phi);

	sigma2_phi=f2_phi_4c->GetParameter(2);
	std::cout<<"!!!!!!!!!!!!!!!! sigma phi (4c-fit)="<<sigma2_phi<<std::endl;

	c10->cd(2);
	h_etac_phimass_4c->Draw();
	// fit eta_c
	mean_etac=2.98;
	range_etac=0.1;
	
	TF1 *f1_4c = new TF1("f1_4c","gaus",2.8,3.2);
	h_etac_phimass_4c->Fit(f1_4c,"R","",mean_etac-range_etac,mean_etac+range_etac);
	
	sigma1=f1_4c->GetParameter(2);
	mean1=f1_4c->GetParameter(1);
	
	TF1 *f2_4c = new TF1("f2_4c","gaus",2.8,3.2);
	h_etac_phimass_4c->Fit(f2_4c,"R","",mean1-1.6*sigma1,mean1+1.6*sigma1);

	sigma2=f2_4c->GetParameter(2);
	std::cout<<"!!!!!!!!!!!!! sigma eta_c (4c-fit) ="<<sigma2<<std::endl;
	
	if (saveHistos) c10->SaveAs("m_final_4c_stt.png");
	
	//////////////////// Vertex fit ////////////////
	TCanvas *c11=new TCanvas("c11","Mass final (Vertex fit)",600,600);
	c11->Divide(1,2);
	c11->cd(1);
	h_mphi_final_vtx->Draw();
	
	mean_phi=1.02;
	range_phi=0.02;
	
	TF1 *f1_phi_vtx = new TF1("f1_phi_vtx","gaus",0.9,1.1);
	h_mphi_final_vtx->Fit(f1_phi_vtx,"R","",mean_phi-range_phi,mean_phi+range_phi);
	
	sigma1_phi=f1_phi_vtx->GetParameter(2);
	mean1_phi=f1_phi_vtx->GetParameter(1);
	
	TF1 *f2_phi_vtx = new TF1("f2_phi_vtx","gaus",0.9,1.1);
	h_mphi_final_vtx->Fit(f2_phi_vtx,"R","",mean1_phi-1.6*sigma1_phi,mean1_phi+1.6*sigma1_phi);

	sigma2_phi=f2_phi_vtx->GetParameter(2);
	std::cout<<"!!!!!!!!!!!! sigma phi (vertex fit)="<<sigma2_phi<<std::endl;

	c11->cd(2);
	h_etac_phimass_vtx->Draw();
	// fit eta_c
	mean_etac=2.98;
	range_etac=0.1;
	
	TF1 *f1_vtx = new TF1("f1_vtx","gaus",2.8,3.2);
	h_etac_phimass_vtx->Fit(f1_vtx,"R","",mean_etac-range_etac,mean_etac+range_etac);
	
	sigma1=f1_vtx->GetParameter(2);
	mean1=f1_vtx->GetParameter(1);
	
	TF1 *f2_vtx = new TF1("f2_vtx","gaus",2.8,3.2);
	h_etac_phimass_vtx->Fit(f2_vtx,"R","",mean1-1.6*sigma1,mean1+1.6*sigma1);

	sigma2=f2_vtx->GetParameter(2);
	std::cout<<"!!!!!!!!!!!!!!! sigma eta_c (vertex fit) ="<<sigma2<<std::endl;
	
	if (saveHistos) c11->SaveAs("m_final_vtx_stt.png");
	
	//////////////////// Phi mass fit ////////////////
// 	TCanvas *c12=new TCanvas("c12","Mass final (phi mass fit)",600,600);
// 	c12->Divide(1,2);
// 	c12->cd(1);
// 	h_mphi_final_massfit->Draw();
// 	
// 	mean_phi=1.02;
// 	range_phi=0.02;
// 	
// 	TF1 *f1_phi_mass = new TF1("f1_phi_mass","gaus",0.9,1.1);
// 	h_mphi_final_massfit->Fit(f1_phi_mass,"R","",mean_phi-range_phi,mean_phi+range_phi);
// 	
// 	sigma1_phi=f1_phi_mass->GetParameter(2);
// 	mean1_phi=f1_phi_mass->GetParameter(1);
// 	
// 	TF1 *f2_phi_mass = new TF1("f2_phi_mass","gaus",0.9,1.1);
// 	h_mphi_final_massfit->Fit(f2_phi_mass,"R","",mean1_phi-1.6*sigma1_phi,mean1_phi+1.6*sigma1_phi);
// 
// 	sigma2_phi=f2_phi_mass->GetParameter(2);
// 	std::cout<<"!!!!!!!!!!!! sigma phi (phi mass fit)="<<sigma2_phi<<std::endl;
// 
// 	c12->cd(2);
// 	h_etac_phimassfit->Draw();
// 	// fit eta_c
// 	mean_etac=2.98;
// 	range_etac=0.1;
// 	
// 	TF1 *f1_mass = new TF1("f1_mass","gaus",2.8,3.2);
// 	h_etac_phimassfit->Fit(f1_mass,"R","",mean_etac-range_etac,mean_etac+range_etac);
// 	
// 	sigma1=f1_mass->GetParameter(2);
// 	mean1=f1_mass->GetParameter(1);
// 	
// 	TF1 *f2_mass = new TF1("f2_mass","gaus",2.8,3.2);
// 	h_etac_phimassfit->Fit(f2_mass,"R","",mean1-1.6*sigma1,mean1+1.6*sigma1);
// 
// 	sigma2=f2_mass->GetParameter(2);
// 	std::cout<<"!!!!!!!!!!!!!!! sigma eta_c (phi mass fit) ="<<sigma2<<std::endl;
// 	
// 	if (saveHistos) c12->SaveAs("m_final_phimass_stt.png");

}
