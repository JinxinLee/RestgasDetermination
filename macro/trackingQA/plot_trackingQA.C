int plot_trackingQA(TString fileName){
	TFile* f = new TFile(fileName);
	TH1* histMVDPixel = (TH1*)f->Get("MVDHitsPixel");
	histMVDPixel->GetXaxis()->SetRangeUser(0., 10.);
	histMVDPixel->SetXTitle("NHits");
	histMVDPixel->SetYTitle("Efficiency");
	TH1* histMVDStrip = (TH1*)f->Get("MVDHitsStrip");
	histMVDStrip->SetXTitle("NHits");
	histMVDStrip->SetYTitle("Efficiency");
	histMVDStrip->GetXaxis()->SetRangeUser(0., 10.);

	TH1* histSTTHit = (TH1*)f->Get("STTHit");
	histSTTHit->SetXTitle("NHits");
	histSTTHit->SetYTitle("Efficiency");
	TH1* histGEMHit = (TH1*)f->Get("GEMHit");
	histGEMHit->GetXaxis()->SetRangeUser(0., 10.);
	histGEMHit->SetXTitle("NHits");
	histGEMHit->SetYTitle("Efficiency");
	TH1* histFTSHit = (TH1*)f->Get("FTSHit");
	histFTSHit->SetXTitle("NHits");
	histFTSHit->SetYTitle("Efficiency");

	TCanvas* c1 = new TCanvas();
	c1->Divide(2,3);
	c1->cd(1);
	histMVDPixel->Draw("colz");
	c1->cd(2);
	histMVDStrip->Draw("colz");
	c1->cd(3);
	histSTTHit->Draw("colz");
	c1->cd(4);
	histGEMHit->Draw("colz");
	c1->cd(5);
	histFTSHit->Draw("colz");

	TH1* histoP = (TH1*)f->Get("fPHisto");
	TH1* histoPRel = (TH1*)f->Get("fPRelHisto");
	TH1* histoPt = (TH1*)f->Get("fPtHisto");
	TH1* histoPtRel = (TH1*)f->Get("fPtRelHisto");


	TCanvas* c2 = new TCanvas();
	c2->Divide(2,2);
	c2->cd(1);
	histoP->Draw();
	c2->cd(2);
	histoPRel->Draw();
	c2->cd(3);
	histoPt->Draw();
	c2->cd(4);
	histoPtRel->Draw();

	TH1* histoQualy = (TH1*)f->Get("fQualyHisto");
	TH1* histoQualyColor = (TH1*)f->Get("fQualyHistoColor");
	TH1* histoQualyRelAll = (TH1*)f->Get("fQualyHisto_rel_all");
	TH1* histoQualyRelPos = (TH1*)f->Get("fQualyHisto_rel_possible");

	TCanvas* c3 = new TCanvas();
	c3->Divide(2,2);
	c3->cd(1);
	histoQualy->Draw("TEXT HISTO");
	c3->cd(2);
	histoQualyColor->Draw("TEXT HISTO");
	c3->cd(3);
	histoQualyRelAll->Draw("TEXT HISTO");
	c3->cd(4);
	histoQualyRelPos->Draw("TEXT HISTO");





	return 0;
}
