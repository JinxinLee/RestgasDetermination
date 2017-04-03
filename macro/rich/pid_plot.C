
void pid_plot(Int_t nf = 15, Int_t pid = 1) {

   std::vector<TString> parnm;
   parnm.push_back("PidAlgoRich.fElectronPdf");
   parnm.push_back("PidAlgoRich.fMuonPdf");
   parnm.push_back("PidAlgoRich.fPionPdf");
   parnm.push_back("PidAlgoRich.fKaonPdf");
   parnm.push_back("PidAlgoRich.fProtonPdf");
   
   TFile *f1 = new TFile(Form("/home/konctbel/Downloads/fairsoft/fairsoft_mar15/pandaroot_old1/macro/rich/v3/pid_complete_v0_r%d.root",nf));
   //TFile *f1 = new TFile(Form("nid/pid_complete_v0_r%d.root",nf));
   TTree *s1 = (TTree*)f1->Get("cbmsim");
   s1->Draw(parnm.at(pid).Data(),Form("-10<%s&&%s<10000",parnm.at(pid).Data(),parnm.at(pid).Data()),"", 10000, 0);

   TFile *f2 = new TFile(Form("pid_complete_v0_r%d.root",nf));
   TTree *s2 = (TTree*)f2->Get("cbmsim");
   s2->SetMarkerSize(0.85);
   s2->SetMarkerColor(1);
   s2->SetMarkerStyle(20);
   //s2->Draw(parnm.at(pid).Data(),Form("-10<%s&&%s<10000",parnm.at(pid).Data(),parnm.at(pid).Data()),"esame", 10000, 0);
 
   leg = new TLegend(0.2,0.65,0.4,0.8);
   std::vector<TString> pn;
   pn.push_back("e");
   pn.push_back("#mu");
   pn.push_back("#pi");
   pn.push_back("K");
   pn.push_back("p");
   leg->SetHeader(Form("Particle:  %s as %s",pn.at(nf/10).Data(),pn.at(pid).Data()));
   leg->SetBorderSize(0);
   leg->AddEntry(s1,"Fast MC","l");
   leg->AddEntry(s2,"Full MC","p");
   leg->Draw();

   c1->SaveAs(Form("pid_%d_%d.png",nf,pid));

   c1->Clear();
   c1->SetLogy();
   s1->Draw(parnm.at(pid).Data(),Form("-10<%s&&%s<10000",parnm.at(pid).Data(),parnm.at(pid).Data()),"", 10000, 0);
   s2->Draw(parnm.at(pid).Data(),Form("-10<%s&&%s<10000",parnm.at(pid).Data(),parnm.at(pid).Data()),"esame", 10000, 0);
   leg->Draw();
   c1->SaveAs(Form("pid_%d_%d_log.png",nf,pid));

   c1->SetLogy(0);

}
