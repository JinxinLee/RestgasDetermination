{	
		TString inFile="invariantmass_4pi_a.root";
	

	TFile *f = TFile::Open(inFile);

	TCanvas *c2=new TCanvas("c2","c2",600,600);
	c2->cd();
	chivtx->Draw();

	TCanvas *c3=new TCanvas("c3","c3",600,600);
	c3->cd();
	hvpos->Draw();

	TCanvas *c4=new TCanvas("c4","c3",600,600);
	c4->cd();
	hvzpos->Draw();


	TCanvas *c5=new TCanvas("c5","c3",600,600);
	c5->cd();
	TF1 *f1 = new TF1("f1","gaus",3.00,3.15);
	invmasschicut_best->Fit(f1,"R");

	

	
}
