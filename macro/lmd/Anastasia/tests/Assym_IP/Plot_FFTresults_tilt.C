void Plot_FFTresults_tilt(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/", int x_sh = 0, int y_sh = -2,int angx_sh = 0, int angy_sh = 0){
  //  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
  gStyle->SetOptStat(0); //no stat

  //  int x_sh_array[7] = {-7,-5,-2,0,2,5,7};
  int angx_sh_array[5] = {-2,-1,0,1,2};
  TMultiGraph *mgr_amp = new TMultiGraph();
 TLegend *leg = new TLegend(0.4,0.4,0.85,0.99);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.045);
  leg->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");

  for(int i=0;i<5;i++){
    // y_sh = y_sh_array[i];
    // x_sh = x_sh_array[i];
    angx_sh = angx_sh_array[i];
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
    fout_name +="/AssymResFFT_";
    fout_name += this_name;
    TString fout_name_pdf = fout_name;
    fout_name_pdf  +=".pdf";
    TString fout_name_root = fout_name;
    fout_name_root  +=".root";
    TFile *finOUT = new TFile(fout_name_root,"READ");
    TGraph *gr_amp = (TGraph *)finOUT->Get("gr_amp");
    gr_amp->SetMarkerColor(1+i);
    gr_amp->SetMarkerStyle(20);
    gr_amp->SetMarkerSize(1.5-0.1*i);
    mgr_amp->Add(gr_amp);
    TString text_sh = "(";
    text_sh +=x_sh;
    text_sh +=",";
    text_sh +=y_sh;
    text_sh += "); (";
    text_sh += angx_sh;
    text_sh +=",";
    text_sh += angy_sh;
    text_sh += ")";
    leg->AddEntry(gr_amp,text_sh,"p");	  
  }

 TString fres_name = path;
 fres_name +="/TOT_AssymResFFT_x_";
 // fres_name +="/TOT_AssymResFFT_x_ALL_y_";
 fres_name +=x_sh;
 fres_name +="mm_y_";
 fres_name +=y_sh;
 fres_name +="mm_Xtlit_ALL_Ytlit_";
 //fres_name +="_Xtlit_";
 // fres_name +=angx_sh;
 // fres_name +="mrad_Ytlit_";
 fres_name +=angy_sh;
 fres_name +="mrad";
 TString fres_name_pdf = fres_name;
 fres_name_pdf +=".pdf";
TString fres_name_root = fres_name;
 fres_name_root +=".root";
 TCanvas *c1 = new TCanvas("Assym", "Assym", 800, 600);
 mgr_amp->Draw("AP");
 mgr_amp->GetXaxis()->SetTitle("frequencyID");
 mgr_amp->GetYaxis()->SetTitle("Magnitude");
 leg->Draw();
 c1->SaveAs(fres_name_pdf);
 c1->SaveAs(fres_name_root);


}
