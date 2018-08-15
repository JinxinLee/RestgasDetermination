void DecoratePzPt(TH1* myhisto) {
	myhisto->SetXTitle("p_{z} [GeV/c]");
	myhisto->SetYTitle("p_{t} [GeV/c]");
	myhisto->GetXaxis()->SetRangeUser(-3.0, 16.0);
	myhisto->GetYaxis()->SetRangeUser(0.0, 1.5);
	myhisto->GetZaxis()->SetRangeUser(0.0, 20.0);
}

void DecoratePtDPt(TH1* myhisto) {
	myhisto->SetXTitle("q #upoint p_{t} [GeV/c]");
	myhisto->SetYTitle("#Deltap_{t} / p_{t}");
	myhisto->GetXaxis()->SetRangeUser(-1500.0, 1500.0);
	myhisto->GetYaxis()->SetRangeUser(-1.0, 1.0);
	myhisto->GetZaxis()->SetRangeUser(0.0, 35.0);
}

void DecorateDPhi(TH1* myhisto) {
	myhisto->SetXTitle("q #upoint #Delta#phi [rad]");
	myhisto->GetXaxis()->SetRangeUser(-3.5, 3.5);
}