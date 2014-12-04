//plot values of number of event assymerty as a function of IP shift or tilt
void AssymN_IPpos(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/"){
  // gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
   gStyle->SetOptStat(0); //no stat
  const int Nmod_2=10;
  const int Nmod_4=5;

  int angx_sh = 0;
  int angy_sh = 0;
  double x_sh_array[15]={-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
  double y_sh_array[15]={-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
  TH2D *hA0 = new TH2D("hA0",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA2 = new TH2D("hA2",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA4 = new TH2D("hA4",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA6 = new TH2D("hA6",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA8 = new TH2D("hA8",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA1 = new TH2D("hA0",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA3 = new TH2D("hA2",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA5 = new TH2D("hA4",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA7 = new TH2D("hA6",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *hA9 = new TH2D("hA8",";x shift, mm;y shift, mm; x10^{-3}",20,-10,10,20,-10,10);
  TH2D *herrA0 = new TH2D("herrA0",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA2 = new TH2D("herrA2",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA4 = new TH2D("herrA4",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA6 = new TH2D("herrA6",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA8 = new TH2D("herrA8",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA1 = new TH2D("herrA0",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA3 = new TH2D("herrA2",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA5 = new TH2D("herrA4",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA7 = new TH2D("herrA6",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  TH2D *herrA9 = new TH2D("herrA8",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
 //  int x_sh = 0;
 //  int y_sh = 0;
 // double angx_sh_array[9]={-20, -15, -10, -5, 0, 5, 10, 15, 20};
 // double angy_sh_array[9]={-20, -15, -10, -5, 0, 5, 10, 15, 20};
 // TH2D *hA0 = new TH2D("hA0",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA2 = new TH2D("hA2",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA4 = new TH2D("hA4",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA6 = new TH2D("hA6",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA8 = new TH2D("hA8",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA1 = new TH2D("hA0",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA3 = new TH2D("hA2",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA5 = new TH2D("hA4",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA7 = new TH2D("hA6",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *hA9 = new TH2D("hA8",";x shift, mm;y shift, mm; x10^{-3}",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA0 = new TH2D("herrA0",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA2 = new TH2D("herrA2",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA4 = new TH2D("herrA4",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA6 = new TH2D("herrA6",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA8 = new TH2D("herrA8",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA1 = new TH2D("herrA0",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA3 = new TH2D("herrA2",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA5 = new TH2D("herrA4",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA7 = new TH2D("herrA6",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 // TH2D *herrA9 = new TH2D("herrA8",";x shift, mm;y shift, mm",12,-3.0,3.0,12,-3.0,3.0);
 

 int k=0;
  for(int i=0;i<15;i++){//x
    for(int j=0;j<15;j++){//y
      int x_sh = x_sh_array[i];
      int y_sh = y_sh_array[j];
  // for(int i=0;i<9;i++){//x
  //   for(int j=0;j<9;j++){//y
  //     int angx_sh = angx_sh_array[i];
  //     int angy_sh = angy_sh_array[j];
      TString this_name = "x_";
      this_name +=x_sh;
      this_name +="mm_y_";
      this_name +=y_sh;
      this_name +="mm_Xtlit_";
      this_name +=angx_sh;
      this_name +="mrad_Ytlit_";
      this_name +=angy_sh;
      this_name +="mrad";
      
      TString fout_name = path;
      fout_name +="/AssymOppRes_";
      fout_name += this_name;
      
      TString fout_name_root = fout_name;
      fout_name_root  +=".root";
      TFile *finOUT = new TFile(fout_name_root,"READ");
      if (finOUT->IsZombie()) continue;
      TGraphErrors *gr_ass = (TGraphErrors *)finOUT->Get("Nass_shift");
      //TODO: plot errors
      Double_t *Nass = new Double_t[Nmod_2];
      Nass = gr_ass->GetY();
    
      hA0->Fill(x_sh,y_sh,1e3*Nass[0]);
      hA1->Fill(x_sh,y_sh,1e3*Nass[1]);
      hA2->Fill(x_sh,y_sh,1e3*Nass[2]);
      hA3->Fill(x_sh,y_sh,1e3*Nass[3]); 
      hA4->Fill(x_sh,y_sh,1e3*Nass[4]);
      hA5->Fill(x_sh,y_sh,1e3*Nass[5]);
      hA6->Fill(x_sh,y_sh,1e3*Nass[6]);
      hA7->Fill(x_sh,y_sh,1e3*Nass[7]);
      hA8->Fill(x_sh,y_sh,1e3*Nass[8]);
      hA9->Fill(x_sh,y_sh,1e3*Nass[9]);

      // hA0->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[0]);
      // hA1->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[1]);
      // hA2->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[2]);
      // hA3->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[3]); 
      // hA4->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[4]);
      // hA5->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[5]);
      // hA6->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[6]);
      // hA7->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[7]);
      // hA8->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[8]);
      // hA9->Fill(1e-1*angx_sh,1e-1*angy_sh,1e3*Nass[9]);


      Double_t *errNass = new Double_t[Nmod_2];
      errNass = gr_ass->GetEY();
      herrA0->Fill(x_sh,y_sh,errNass[0]);
      herrA1->Fill(x_sh,y_sh,errNass[1]);
      herrA2->Fill(x_sh,y_sh,errNass[2]);
      herrA3->Fill(x_sh,y_sh,errNass[3]); 
      herrA4->Fill(x_sh,y_sh,errNass[4]);
      herrA5->Fill(x_sh,y_sh,errNass[5]);
      herrA6->Fill(x_sh,y_sh,errNass[6]);
      herrA7->Fill(x_sh,y_sh,errNass[7]);
      herrA8->Fill(x_sh,y_sh,errNass[8]);
      herrA9->Fill(x_sh,y_sh,errNass[9]);

      // herrA0->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[0]);
      // herrA1->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[1]);
      // herrA2->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[2]);
      // herrA3->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[3]); 
      // herrA4->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[4]);
      // herrA5->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[5]);
      // herrA6->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[6]);
      // herrA7->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[7]);
      // herrA8->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[8]);
      // herrA9->Fill(1e-1*angx_sh,1e-1*angy_sh,errNass[9]);

    }
  }
 
 TCanvas *c1 = new TCanvas("Nassym_modules", "Events assymmetry on each module", 600, 800);
 c1->Divide(2,5);
 c1->cd(1);
 hA0->SetTitle("A0");
 hA0->GetZaxis()->SetRangeUser(-50,50);
 //hA0->GetZaxis()->Set(1000,-50,50);
 hA0->Draw("colz");
c1->cd(2);
 hA1->SetTitle("A1");
 hA1->GetZaxis()->SetRangeUser(-50,50);
 hA1->Draw("colz");
c1->cd(3);
 hA2->SetTitle("A2");
 hA2->GetZaxis()->SetRangeUser(-50,50);
 hA2->Draw("colz");
 c1->cd(4);
 hA3->SetTitle("A3");
 hA3->GetZaxis()->SetRangeUser(-50,50);
 hA3->Draw("colz");
c1->cd(5);
 hA4->SetTitle("A4");
 hA4->GetZaxis()->SetRangeUser(-50,50);
 hA4->Draw("colz");
c1->cd(6);
 hA5->SetTitle("A5");
 hA5->GetZaxis()->SetRangeUser(-50,50);
 hA5->Draw("colz");
 c1->cd(7);
 hA6->SetTitle("A6");
 hA6->GetZaxis()->SetRangeUser(-50,50);
 hA6->Draw("colz");
c1->cd(8);
 hA7->SetTitle("A7");
 hA7->GetZaxis()->SetRangeUser(-50,50);
 hA7->Draw("colz");
c1->cd(9);
 hA8->SetTitle("A8");
 hA8->GetZaxis()->SetRangeUser(-50,50);
 hA8->Draw("colz");
c1->cd(10);
 hA9->SetTitle("A9");
 hA9->GetZaxis()->SetRangeUser(-50,50);
 hA9->Draw("colz");
 c1->SaveAs("resultsAss.pdf");
 c1->SaveAs("resultsAss.root");

TCanvas *c2 = new TCanvas("Nassym_modulesErr", "Error of events assymmetry on each module", 600, 800);
 c2->Divide(2,5);
 c2->cd(1);
 herrA0->SetTitle("#sigmaA0");
 herrA0->GetZaxis()->SetRangeUser(0,0.007);
 herrA0->Draw("colz");
c2->cd(2);
 herrA1->SetTitle("#sigmaA1");
 herrA1->GetZaxis()->SetRangeUser(0,0.007);
 herrA1->Draw("colz");
c2->cd(3);
 herrA2->SetTitle("#sigmaA2");
 herrA2->GetZaxis()->SetRangeUser(0,0.007);
 herrA2->Draw("colz");
 c2->cd(4);
 herrA3->SetTitle("#sigmaA3");
 herrA3->GetZaxis()->SetRangeUser(0,0.007);
 herrA3->Draw("colz");
c2->cd(5);
 herrA4->SetTitle("#sigmaA4");
 herrA4->GetZaxis()->SetRangeUser(0,0.007);
 herrA4->Draw("colz");
c2->cd(6);
 herrA5->SetTitle("#sigmaA5");
 herrA5->GetZaxis()->SetRangeUser(0,0.007);
 herrA5->Draw("colz");
 c2->cd(7);
 herrA6->SetTitle("#sigmaA6");
 herrA6->GetZaxis()->SetRangeUser(0,0.007);
 herrA6->Draw("colz");
c2->cd(8);
 herrA7->SetTitle("#sigmaA7");
 herrA7->GetZaxis()->SetRangeUser(0,0.007);
 herrA7->Draw("colz");
c2->cd(9);
 herrA8->SetTitle("#sigmaA8");
 herrA8->GetZaxis()->SetRangeUser(0,0.007);
 herrA8->Draw("colz");
c2->cd(10);
 herrA9->SetTitle("#sigmaA9");
 herrA9->GetZaxis()->SetRangeUser(0,0.007);
 herrA9->Draw("colz");
 c2->SaveAs("resultsErrAss.pdf");
 c2->SaveAs("resultsErrAss.root");



}
