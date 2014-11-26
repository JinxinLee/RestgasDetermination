//parametrase even ampl coef
void FFT_Amp_Params_SHIFT(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/"){
  // gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
   gStyle->SetOptStat(0); //no stat
  const int Nmod_4=5;

  // int angx_sh = 0;
  // int angy_sh = 0;
  // double x_sh_array[15]={-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
  // double y_sh_array[15]={-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7};
  // TH2D *hA0 = new TH2D("hA0",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  // TH2D *hA2 = new TH2D("hA2",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  // TH2D *hA4 = new TH2D("hA4",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  // TH2D *hA6 = new TH2D("hA6",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);
  // TH2D *hA8 = new TH2D("hA8",";x shift, mm;y shift, mm",20,-10,10,20,-10,10);

  int x_sh = 0;
  int y_sh = 0;
 double angx_sh_array[9]={-20, -15, -10, -5, 0, 5, 10, 15, 20};
 double angy_sh_array[9]={-20, -15, -10, -5, 0, 5, 10, 15, 20};
  TH2D *hA0 = new TH2D("hA0",";x tilt, mrad; y tilt, mrad",12,-3.0,3.0,12,-3.0,3.0);
  TH2D *hA2 = new TH2D("hA2",";x tilt, mrad; y tilt, mrad",12,-3.0,3.0,12,-3.0,3.0);
  TH2D *hA4 = new TH2D("hA4",";x tilt, mrad; y tilt, mrad",12,-3.0,3.0,12,-3.0,3.0);
  TH2D *hA6 = new TH2D("hA6",";x tilt, mrad; y tilt, mrad",12,-3.0,3.0,12,-3.0,3.0);
  TH2D *hA8 = new TH2D("hA8",";x tilt, mrad; y tilt, mrad",12,-3.0,3.0,12,-3.0,3.0);

 int k=0;
  for(int i=0;i<9;i++){//x
    for(int j=0;j<9;j++){//y
      // int x_sh = x_sh_array[i];
      // int y_sh = y_sh_array[j];
  int angx_sh = angx_sh_array[i];
  int angy_sh = angy_sh_array[j];
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
      fout_name +="/AssymResFFTsimple_";
      fout_name += this_name;
      
      TString fout_name_root = fout_name;
      fout_name_root  +=".root";
      TFile *finOUT = new TFile(fout_name_root,"READ");
      TGraph *gr_amp = (TGraph *)finOUT->Get("gr_amp");
      
      Double_t *amp_full_even = new Double_t[Nmod_4];
      Double_t *frequencyID_even = new Double_t[Nmod_4];
      amp_full_even = gr_amp->GetY();
      frequencyID_even = gr_amp->GetX();

   
      // hA0->Fill(x_sh,y_sh,amp_full_even[0]);
      // hA2->Fill(x_sh,y_sh,amp_full_even[1]);
      // hA4->Fill(x_sh,y_sh,amp_full_even[2]);
      // hA6->Fill(x_sh,y_sh,amp_full_even[3]);
      // hA8->Fill(x_sh,y_sh,amp_full_even[4]);
      hA0->Fill(1e-1*angx_sh,1e-1*angy_sh,amp_full_even[0]);
      hA2->Fill(1e-1*angx_sh,1e-1*angy_sh,amp_full_even[1]);
      hA4->Fill(1e-1*angx_sh,1e-1*angy_sh,amp_full_even[2]);
      hA6->Fill(1e-1*angx_sh,1e-1*angy_sh,amp_full_even[3]);
      hA8->Fill(1e-1*angx_sh,1e-1*angy_sh,amp_full_even[4]);
    }
  }
 
 TCanvas *c1 = new TCanvas("FFT_params", "FFT params", 600, 800);
 c1->Divide(2,3);
 c1->cd(1);
 // gA0->SetTitle("F0");
 // gA0->Draw("surf1");
 hA0->SetTitle("F0");
 hA0->Draw("colz");
 c1->cd(2);
 // gA2->SetTitle("F2");
 // gA2->Draw("surf1");
 hA2->SetTitle("F2");
 hA2->Draw("colz");
  c1->cd(3);
 // gA4->SetTitle("F4");
 // gA4->Draw("surf1");
 hA4->SetTitle("F4");
 hA4->Draw("colz");
 c1->cd(4);
 // gA6->SetTitle("F6");
 // gA6->Draw("surf1");
 hA6->SetTitle("F6");
 hA6->Draw("colz");
 c1->cd(5);
 // gA8->SetTitle("F8");
 // gA8->Draw("surf1");
 hA8->SetTitle("F8");
 hA8->Draw("colz");
 c1->SaveAs("results.pdf");
 c1->SaveAs("results.root");
}
