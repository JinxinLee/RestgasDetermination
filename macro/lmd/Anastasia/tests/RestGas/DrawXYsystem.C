void DrawXYsystem(){
 TFile * fin0cm = new TFile("/panda/pandaroot/macro/lmd/Anastasia/tests/RestGas/testRestGas_z_0cm/mom_1_5/Lumi_TrksQA_0.root","READ");
 TTree *tree0cm = (TTree*)fin0cm->Get("cbmsim");
 TH2D *hxdx0cm = new TH2D("hxdx0cm",";X_{MC},cm;(X_{MC}-X_{REC}), cm",2e1,-1,1,1e3,-10,10);
 tree0cm->Project("hxdx0cm","(LMDTrackQ.fXmc-LMDTrackQ.fXrec):LMDTrackQ.fXmc","LMDTrackQ.fTrkRecStatus==0 && LMDTrackQ.fSecondary<0");
 //hxdx0cm->Draw("colz");
 hxdx0cm->FitSlicesY();
 TH1D *hxdx0cm_1 = (TH1D*)gDirectory->Get("hxdx0cm_1");
 hxdx0cm_1->SetMarkerStyle(20);
 hxdx0cm_1->SetMarkerColor(2);
 hxdx0cm_1->SetLineColor(2);
 

 TFile *fin10cm = new TFile("/panda/pandaroot/macro/lmd/Anastasia/tests/RestGas/testRestGas_z_10cm/mom_1_5/Lumi_TrksQA_0.root","READ");
 TTree *tree10cm = (TTree*)fin10cm->Get("cbmsim");
 TH2D *hxdx10cm = new TH2D("hxdx10cm",";X_{MC},cm;(X_{MC}-X_{REC}), cm",2e1,-1,1,1e3,-10,10);
 tree10cm->Project("hxdx10cm","(LMDTrackQ.fXmc-LMDTrackQ.fXrec):LMDTrackQ.fXmc","LMDTrackQ.fTrkRecStatus==0 && LMDTrackQ.fSecondary<0");
 //hxdx0cm->Draw("colz");
 hxdx10cm->FitSlicesY();
 TH1D *hxdx10cm_1 = (TH1D*)gDirectory->Get("hxdx10cm_1");
 hxdx10cm_1->SetMarkerStyle(20);
 hxdx10cm_1->SetMarkerColor(8);
 hxdx10cm_1->SetLineColor(8);

TFile *fin20cm = new TFile("/panda/pandaroot/macro/lmd/Anastasia/tests/RestGas/testRestGas_z_20cm/mom_1_5/Lumi_TrksQA_0.root","READ");
 TTree *tree20cm = (TTree*)fin20cm->Get("cbmsim");
 TH2D *hxdx20cm = new TH2D("hxdx20cm",";X_{MC},cm;(X_{MC}-X_{REC}), cm",2e1,-1,1,1e3,-10,10);
 tree20cm->Project("hxdx20cm","(LMDTrackQ.fXmc-LMDTrackQ.fXrec):LMDTrackQ.fXmc","LMDTrackQ.fTrkRecStatus==0 && LMDTrackQ.fSecondary<0");
 //hxdx0cm->Draw("colz");
 hxdx20cm->FitSlicesY();
 TH1D *hxdx20cm_1 = (TH1D*)gDirectory->Get("hxdx20cm_1");
 hxdx20cm_1->SetMarkerStyle(20);
 hxdx20cm_1->SetMarkerColor(4);
 hxdx20cm_1->SetLineColor(4);

TFile *fin50cm = new TFile("/panda/pandaroot/macro/lmd/Anastasia/tests/RestGas/testRestGas_z_50cm/mom_1_5/Lumi_TrksQA_0.root","READ");
 TTree *tree50cm = (TTree*)fin50cm->Get("cbmsim");
 TH2D *hxdx50cm = new TH2D("hxdx50cm",";X_{MC},cm;(X_{MC}-X_{REC}), cm",2e1,-1,1,1e3,-10,10);
 tree50cm->Project("hxdx50cm","(LMDTrackQ.fXmc-LMDTrackQ.fXrec):LMDTrackQ.fXmc","LMDTrackQ.fTrkRecStatus==0 && LMDTrackQ.fSecondary<0");
 //hxdx0cm->Draw("colz");
 hxdx50cm->FitSlicesY();
 TH1D *hxdx50cm_1 = (TH1D*)gDirectory->Get("hxdx50cm_1");
 hxdx50cm_1->SetMarkerStyle(20);
 hxdx50cm_1->SetMarkerColor(28);
 hxdx50cm_1->SetLineColor(28);
 TCanvas c1;
 // hxdx50cm->Draw("COLZ");
 hxdx0cm_1->GetYaxis()->SetRangeUser(-0.1,0.1);
 hxdx0cm_1->Draw();
 // hxdx10cm_1->Add(hxdx0cm_1,-1);
 //hxdx10cm_1->GetYaxis()->SetRangeUser(-0.1,0.1);
 hxdx10cm_1->Draw("same");
 // hxdx20cm_1->Add(hxdx0cm_1,-1);
 hxdx20cm_1->Draw("same");
 // hxdx50cm_1->Add(hxdx0cm_1,-1);
 hxdx50cm_1->Draw("same");
 c1.SaveAs("/panda/pandaroot/macro/lmd/Anastasia/tests/RestGas/result.pdf");
}
