/*
 * anaLumiDigi.C
 *
 *  Created on: Nov 1, 2008
 *      Author: tsito
 */

//*********************************************//
// macro analyze the output from runLumiDigi.C //
//*********************************************//
{
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
	rootlogon();
	//gSystem->Load("libLumi");
	TFile *f_mc = new TFile("lumisim.root");
	TTree *t_mc = (TTree*) f_mc->Get("cbmsim");

	// Set a pointer to the file (tree) contains the output
	TFile *f = new TFile("lumidigi.root");
	TTree *t = (TTree*) f->Get("cbmsim");

	// Collect data in an array
	TClonesArray* mc_point = new TClonesArray("PndLumiPoint");
	t_mc->SetBranchAddress("LumiPoint",&mc_point);

	TClonesArray* clust_array = new TClonesArray("PndLumiDigi");
	t->SetBranchAddress("LumiDigi",&clust_array);

	// histograms definition
	TH1F *hCharge_m1 = new TH1F("", "", 200, 2.*1E3., 2.*1E5.);
	TH1F *hCharge_m2 = new TH1F("", "", 200, 2.*1E3., 2.*1E5.);
	TH1F *hCharge_m3 = new TH1F("", "", 200, 2.*1E3., 2.*1E5.);
	hCharge_m1->SetLineColor(3);
	hCharge_m2->SetLineColor(4);
	hCharge_m3->SetLineColor(2);
	hCharge_m1->SetXTitle("dE/dx [eV]");
	hCharge_m1->GetXaxis()->SetTitleSize(0.03);
	hCharge_m1->GetXaxis()->SetLabelFont(40);
	hCharge_m1->GetXaxis()->SetLabelSize(0.03);
	hCharge_m1->SetYTitle("Counts");
	hCharge_m1->GetYaxis()->SetTitleSize(0.03);
	hCharge_m1->GetYaxis()->SetLabelSize(0.03);
	hCharge_m1->GetYaxis()->SetLabelFont(40);

	TH1I *hMulti = new TH1I("","", 100, 0, 10);

	TProfile *hQRatio = new TProfile("","", 100, -0.0025, 0.0025);
	TProfile *hQRatio_reco = new TProfile("","", 100, -0.0025, 0.0025);
	hQRatio->SetLineColor(4);
	hQRatio_reco->SetLineColor(2);
	//hQRatio->SetXTitle("MCPosition [cm]");
	hQRatio->GetXaxis()->SetTitleSize(0.03);
	hQRatio->GetXaxis()->SetLabelFont(40);
	hQRatio->GetXaxis()->SetLabelSize(0.03);
	hQRatio->SetYTitle("Q_{r} / (Q_{r} + Q_{l})");
	hQRatio->GetYaxis()->SetTitleSize(0.03);
	hQRatio->GetYaxis()->SetLabelSize(0.03);
	hQRatio->GetYaxis()->SetLabelFont(40);
	hQRatio_reco->SetXTitle("Position [cm]");
	hQRatio_reco->GetXaxis()->SetTitleSize(0.03);
	hQRatio_reco->GetXaxis()->SetLabelFont(40);
	hQRatio_reco->GetXaxis()->SetLabelSize(0.03);
	hQRatio_reco->SetYTitle("Q_{r} / (Q_{r} + Q_{l})");
	hQRatio_reco->GetYaxis()->SetTitleSize(0.03);
	hQRatio_reco->GetYaxis()->SetLabelSize(0.03);
	hQRatio_reco->GetYaxis()->SetLabelFont(40);

	TH1F *hPosition = new TH1F("","", 100, -0.0025, 0.0025);
	TH1F *hPosition_m1 = new TH1F("","", 100, -0.005, 0.005);
	TH1F *hPosition_m2 = new TH1F("","", 100, -0.003, 0.003);
	TH1F *hPosition_m2_a = new TH1F("","", 100, -0.0025, 0.0025);
	hPosition_m1->SetLineColor(3);
	hPosition_m2->SetLineColor(4);
	hPosition_m2_a->SetLineColor(4);

	hPosition_m1->SetXTitle("(RecoPosition - MCPosition) [cm]");
	hPosition_m1->GetXaxis()->SetTitleSize(0.03);
	hPosition_m1->GetXaxis()->SetLabelFont(40);
	hPosition_m1->GetXaxis()->SetLabelSize(0.03);
	hPosition_m1->SetYTitle("Counts");
	hPosition_m1->GetYaxis()->SetTitleSize(0.03);
	hPosition_m1->GetYaxis()->SetLabelSize(0.03);
	hPosition_m1->GetYaxis()->SetLabelFont(40);
	hPosition_m2->SetXTitle("(RecoPosition - MCPosition) [cm]");
	hPosition_m2->GetXaxis()->SetTitleSize(0.03);
	hPosition_m2->GetXaxis()->SetLabelFont(40);
	hPosition_m2->GetXaxis()->SetLabelSize(0.03);
	hPosition_m2->SetYTitle("Counts");
	hPosition_m2->GetYaxis()->SetTitleSize(0.03);
	hPosition_m2->GetYaxis()->SetLabelSize(0.03);
	hPosition_m2->GetYaxis()->SetLabelFont(40);

	TProfile *pPosition_m1 = new TProfile("","",200, 0., 0.005,"S");
	TProfile *pPosition_m2 = new TProfile("","",200, -0.0025, 0.0025,"S");
	//pPosition_m1->SetLineColor(3);
	pPosition_m2->SetLineColor(4);
	pPosition_m1->SetXTitle("MCPosition [cm]");
	pPosition_m1->SetYTitle("(RecoPosition - MCPosition) [cm]");
	pPosition_m1->GetXaxis()->SetTitleSize(0.03);
	pPosition_m1->GetXaxis()->SetLabelFont(40);
	pPosition_m1->GetXaxis()->SetLabelSize(0.03);
	pPosition_m1->GetYaxis()->SetTitleSize(0.03);
	pPosition_m1->GetYaxis()->SetLabelFont(40);
	pPosition_m1->GetYaxis()->SetLabelSize(0.02);
	pPosition_m2->SetXTitle("MCPosition [cm]");
	pPosition_m2->SetYTitle("(RecoPosition - MCPosition) [cm]");
	pPosition_m2->GetXaxis()->SetTitleSize(0.03);
	pPosition_m2->GetXaxis()->SetLabelFont(40);
	pPosition_m2->GetXaxis()->SetLabelSize(0.03);
	pPosition_m2->GetYaxis()->SetTitleSize(0.03);
	pPosition_m2->GetYaxis()->SetLabelFont(40);
	pPosition_m2->GetYaxis()->SetLabelSize(0.02);

	TH1F *hError_m1 = new TH1F("","", 200,-0.0025, 0.0025);
	TH1F *hError_m2 = new TH1F("","", 199,-0.0025, 0.0025);
	hError_m1->SetLineColor(3);
	hError_m2->SetLineColor(4);
	hError_m2->SetXTitle("MCPosition [cm]");
	hError_m2->SetYTitle("ErrorBarWidth [cm]");
	hError_m2->GetXaxis()->SetTitleSize(0.03);
	hError_m2->GetXaxis()->SetLabelFont(40);
	hError_m2->GetXaxis()->SetLabelSize(0.03);
	hError_m2->GetYaxis()->SetTitleSize(0.03);
	hError_m2->GetYaxis()->SetLabelFont(40);
	hError_m2->GetYaxis()->SetLabelSize(0.03);

	TH1I *hMulti_2 = new TH1I("","",100, 2, 15);

	TH1F *hY_sim = new TH1F("","",100, 0., 0.005);

	TH1F *hEta = new TH1F ("","",100,0.,1.);

	//Variable declaration
	Int_t multi;
	PndLumiStrip stripcluster;
	Double_t Q_l, Q_r;
	Double_t inStripId, outStripId;
	Double_t p_sim, p_reco;//relative position of the hit along the path
	Double_t y_sim, y_reco;
	Double_t _reco;
	Int_t rId, index_hit;
	Double_t dQ;
	Double_t eLoss;
	Double_t dy_m1, dy_m2;
	
	Double_t Q;
	
	for ( Int_t i=0; i < t->GetEntriesFast(); i++) {
		t_mc->GetEntry(i);
		t->GetEntry(i);
		if (mc_point->GetEntriesFast() == 4) {
		
		for (Int_t k = 0; k < clust_array->GetEntriesFast(); k++) {

			PndLumiDigi *MyDigi = (PndLumiDigi*)clust_array->At(k);
			multi = MyDigi->GetMultiplicity();

			Q_r = MyDigi->GetChargeRatioRight();
			Q_l = MyDigi->GetChargeRatioLeft();
			stripcluster = MyDigi->GetStrip();
			inStripId = MyDigi->GetEntryStripId();
			outStripId = MyDigi->GetExitStripId();
			index_hit = Int_t(inStripId);
			rId = MyDigi->GetRightID();
			eLoss = MyDigi->GetEnergyLoss();


				hMulti->Fill(multi);

				p_sim = 0.5 * (inStripId + outStripId);

				if (multi == 1) {
					y_sim = (p_sim - Int_t(p_sim))* 0.005;
					y_reco = 0.5 * 0.005;
					dy_m1 = y_reco - y_sim;

					hCharge_m1 -> Fill(stripcluster.GetCharge());
					hPosition_m1->Fill(y_reco-y_sim);
					pPosition_m1->Fill(y_sim,(y_reco-y_sim));

					Q = stripcluster.GetCharge();

				} else {

					dQ = Q_r / (Q_r+Q_l);
					y_reco = 0.0008*TMath::Sqrt(2.0)*TMath::ErfInverse((2.0*dQ)-1.0);

					if (rId == Int_t(p_sim)) {
						y_sim = (p_sim - Int_t(p_sim))* 0.005;

					} else {
						y_sim = (p_sim - Int_t(p_sim)-1.0)* 0.005;
					}

					dy_m2 = y_reco - y_sim;

					hCharge_m2 -> Fill(stripcluster.GetCharge());
					hQRatio_reco->Fill(y_reco, dQ);
					hQRatio->Fill(y_sim, dQ);
					hEta->Fill(dQ);
					hPosition_m2->Fill(dy_m2);
					pPosition_m2->Fill(y_sim,dy_m2);

					Q = Q_r + Q_l;
					hMulti_2->Fill(multi);

				}

			}

			hPosition->Fill(y_reco-y_sim);

			
		}

	}


	//Visualization of the histograms

	 TCanvas* c_ana_1 = new TCanvas("c_ana_1", "c_ana_1",0, 0,600,600);
	 c_ana_1->SetGridx();
	 c_ana_1->SetGridy();
	 hQRatio_reco->Draw();
	 hQRatio->Draw("same");
	 TLegend *legend_1 = new TLegend(0.4,0.4,0.1,0.1);
	 legend_1->AddEntry(hQRatio, "MC Position");
	 legend_1->AddEntry(hQRatio_reco, "Reco Position");
	 legend_1->SetTextFont(30);
	 legend_1->SetTextSize(0.03);
	 legend_1->Draw();


//	 TCanvas* c_ana_2 = new TCanvas("c_ana_2", "c_ana_2",0, 0,600,600);
	 //c_ana_2->SetGridx();
	 //c_ana_2->SetGridy();
	 //hQRatio->Draw();
	 //TCanvas* c_ana_3 = new TCanvas("c_ana_3", "c_ana_3",0, 0,600,600);
	 //hPosition_m2->Draw();hPosition_m1->Draw("same");

	 //TCanvas* c_ana_4 = new TCanvas("c_ana_4", "c_ana_4",0, 0,1400,1400);
	 //hPosition_m1->Draw();

	// TCanvas* c_ana_5 = new TCanvas("c_ana_5", "c_ana_5",0, 0,600,600);
	//  pPosition_m2->Draw();

	 TCanvas* c_ana_6 = new TCanvas("c_ana_6", "c_ana_6",0, 0,600,600);
	 pPosition_m1->Draw();

	 TCanvas* c_ana_7 = new TCanvas("c_ana_7", "c_ana_7",0, 0,600,600);
	 hCharge_m1->Draw(); hCharge_m2->Draw("same");
	 
	 //TLegend *legend_7 = new TLegend(0.4,0.4,0.1,0.1);
	 //legend_7->AddEntry(hCharge_m2, "strip multiplicity 2");
	 //legend_7->AddEntry(hCharge_m1, "strip multiplicity 1");
	 //legend_7->SetTextFont(40);
	 //legend_7->SetTextSize(0.03);
	 //legend_7->Draw();

	 //TCanvas* c_ana_8 = new TCanvas("c_ana_8", "c_ana_8",0, 0,600,600);
	 //hError_m2->Draw();// hError_m1->Draw("same");
	 //c_ana_8->SetGridx();
	 //c_ana_8->SetGridy();
	 //TLegend *legend_8 = new TLegend(0.4,0.4,0.1,0.1);
	 //legend_8->AddEntry(hError_m2, "strip multiplicity 2");
	 //legend_8->AddEntry(hError_m1, "strip multiplicity 1");
	 //legend_8->SetTextFont(40);
	 //legend_8->SetTextSize(0.03);
	 //legend_8->Draw();


	 //TCanvas* c_ana_9 = new TCanvas("c_ana_9", "c_ana_9",0, 0,1400,1400);
	 //hEta->Draw();

}

