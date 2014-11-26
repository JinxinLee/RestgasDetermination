//compute Fast Fourie only for events on modules (no additional points to increase resolution are added!)
void FFT_Nev_MODULES(TString path = "/panda/myResults/AssymIP/DPM/mom_1_5/", TString file_name="/HitsAssym_x_y_1_5GeV.root", int x_sh = -5, int y_sh = -5,int angx_sh = -5, int angy_sh = -5){
  gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/tests/Assym_IP/rootlogon.C");
  gStyle->SetOptStat(0); //no stat
  //read Ideal parameters ---------------------------------------------------
  TString file_ideal = path;
  file_ideal +="x_0mm_y_0mm_Xtlit_0mrad_Ytlit_0mrad";
  file_ideal +=file_name;
  TFile *finIDEAL = new TFile(file_ideal,"READ");
//N
  TGraphErrors *modN_ideal = (TGraphErrors*)finIDEAL->Get("modN");
  modN_ideal->SetMarkerSize(1.0);
  Double_t *N_ideal; Double_t *errN_ideal;
  N_ideal = modN_ideal->GetY();
  errN_ideal = modN_ideal->GetEY();
 double *modulesID = modN_ideal->GetX();

 // const int Nmod = (modN_ideal->GetN());
 const int Nmod = 20; //Number of points fixed to number of modules
  const int Nmod_2 = 0.5*Nmod;
  const int Nmod_4 = 0.25*Nmod;
 //calculate frequency
  double frequencyID[Nmod_2];
  frequencyID[0] = 0;
 
  for(int i=1; i<Nmod_2;i++){
    frequencyID[i] = frequencyID[i-1]+double((Nmod-1))/(Nmod*Nmod);
   }
 //[END] read Ideal parameters ------------------------------------------

  // read Shifted parameters -----------------------------------------------
  TString this_name = "x_";
  this_name +=x_sh;
  this_name +="mm_y_";
  this_name +=y_sh;
  this_name +="mm_Xtlit_";
  this_name +=angx_sh;
  this_name +="mrad_Ytlit_";
  this_name +=angy_sh;
  this_name +="mrad";


TString this_name2 = "x_";
 if(x_sh<0)  this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(x_sh);
  this_name2 +="mm_y_";
if(y_sh<0)  this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(y_sh);
 this_name2 +="mm_Xtlit_";
 if(angx_sh<0) this_name2 +="mn";
 else this_name2 +="pl";
 this_name2 +=fabs(angx_sh);
  this_name2 +="mrad_Ytlit_";
  if(angy_sh<0) this_name2 +="mn";
  else this_name2 +="pl";
  this_name2 +=fabs(angy_sh);
  this_name2 +="mrad";

  TString file_shifted = path;
  file_shifted +=this_name;
  file_shifted +=file_name;
  TFile *fin_SHIFT = new TFile(file_shifted,"READ");

  TGraphErrors *modN_SHIFT = (TGraphErrors*)fin_SHIFT->Get("modN");
  modN_SHIFT->SetMarkerColor(4);
  modN_SHIFT->SetMarkerSize(1.0);
  Double_t *N_SHIFT = new Double_t[Nmod];
  Double_t *errN_SHIFT = new Double_t[Nmod];
  N_SHIFT = modN_SHIFT->GetY();
  errN_SHIFT = modN_SHIFT->GetEY();
  Double_t *diffN_SHIFT = new Double_t[Nmod];
  Double_t *errdiffN_SHIFT = new Double_t[Nmod];
  double N_ideal_norm = N_ideal[0];
  double N_shifted_norm = N_SHIFT[0];
  for(int i=0;i<Nmod;i++){
    N_ideal[i] *= 1./N_ideal_norm;
    N_SHIFT[i] *= 1./N_shifted_norm;
    diffN_SHIFT[i] = 100.*(N_SHIFT[i] - N_ideal[i])/N_ideal[i];
    //   errdiffN_SHIFT[i] = 0.5*sqrt(errN_ideal[i]*errN_ideal[i]+errN_SHIFT[i]*errN_SHIFT[i]);
    errdiffN_SHIFT[i] = (1/N_ideal[i])*sqrt(TMath::Power(N_SHIFT[i]*errN_ideal[i]/N_ideal[i],2)+TMath::Power(errN_SHIFT[i],2));
  }
  //[END] read Shifted parameters ---------------------------------------

  //create histograms (FFT in ROOT works only for histograms) ----------------
  TH1D *h_Nideal = new TH1D("h_Nideal",";sector;N/N_{0};",Nmod,0,Nmod);
  h_Nideal->SetFillColor(kRed);
  h_Nideal->SetLineColor(kRed);
  h_Nideal->SetFillStyle(3004);
  TH1D *h_Nshifted = new TH1D("h_Nshifted",";sector;N/N_{0};",Nmod,0,Nmod);
  h_Nshifted->SetFillColor(kBlue);

  for(int i=0;i<Nmod;i++){
    h_Nideal->Fill(i,N_ideal[i]);
    h_Nshifted->Fill(i,N_SHIFT[i]);
  }
  //[END] create histograms (FFT in ROOT works only for histograms) -------


  //Do FFT ---------------------------------------------------------------------------------

  TVirtualFFT::SetTransform(0);
  //Compute the transform and look at the magnitude of the output
   TH1 *hm =0;
   hm = h_Nshifted->FFT(hm, "MAG R2C M");
   // hm = h_Nshifted->FFT(hm, "MAG DHT M");
   hm->SetTitle("Magnitude of FFT");
   //  hm->Scale(1./sqrt(Nmod));
   //NOTE: for "real" frequencies you have to divide the x-axes range with the range of your function 
   //(in this case 4*Pi); y-axes has to be rescaled by a factor of 1/SQRT(n) to be right: this is not done automatically!
   
   //Look at the phase of the output   
   TH1 *hp = 0;
   hp = h_Nshifted->FFT(hp, "PH R2C M");
   // hp = h_Nshifted->FFT(hp, "PH DHT M");
   hp->SetTitle("Phase of FFT");

   TH1 *him = 0;
   him = h_Nshifted->FFT(him, "IM R2C M");
   him->SetTitle("Imaginary part of FFT");
   TH1 *hre = 0;
   hre = h_Nshifted->FFT(hre, "RE R2C M");
   hre->SetTitle("Real part of FFT");

 // //That's the way to get the current transform object:
   TVirtualFFT *fft = TVirtualFFT::GetCurrentTransform();
   Double_t *re_full = new Double_t[Nmod_2];
   Double_t *im_full = new Double_t[Nmod_2];
   fft->GetPointsComplex(im_full,re_full);
   // im_full[0]=0;//TEST
   // re_full[0]=0;//TEST
   // for(int i=0;i<10;i++){
   //   cout<<i<<" "<<im_full[i]<<" "<<re_full[i]<<endl;
   // } 
   Double_t amp_full[Nmod_2];
   Double_t phase_full[Nmod_2];
   for(int i=0;i<Nmod_2;i++){
     amp_full[i] = sqrt(im_full[i]*im_full[i]+re_full[i]*re_full[i]);
     // cout<<"amp_full["<<i<<"]="<<amp_full[i]<<endl;
     phase_full[i] = hp->GetBinContent(i);
   }

   Double_t amp_full_even[Nmod_4];
   Double_t frequencyID_even[Nmod_4];
   for(int i=0;i<Nmod_4;i++){
     amp_full_even[i] = amp_full[2*i];
     frequencyID_even[i] = frequencyID[2*i];
   }

   TGraph *gr_m_ph = new TGraph(Nmod);
   gr_m_ph->SetMarkerStyle(20);
   gr_m_ph->SetTitle("");

   TGraph *gr_im = new TGraph(Nmod_2,frequencyID,im_full);
   gr_im->SetMarkerStyle(20);
   gr_im->SetTitle("");

   TGraph *gr_re = new TGraph(Nmod_2,frequencyID,re_full);
   gr_re->SetMarkerStyle(20);
   gr_re->SetTitle("");

  
   TGraph *gr_amp = new TGraph(Nmod_4,frequencyID_even,amp_full_even);
   gr_amp->SetMarkerStyle(20);
   gr_amp->SetMarkerSize(0.5);
   gr_amp->SetMarkerColor(kBlue);
   gr_amp->SetTitle("");
   // TF1 *f1 = new TF1("f1","[0]+[1]*x+[2]*x*x",0.1,0.5);
   // gr_amp->Fit(f1);
   TGraph *gr_ph = new TGraph(Nmod_2,frequencyID,phase_full);
   gr_ph->SetMarkerStyle(20);
   gr_ph->SetMarkerSize(0.5);
   gr_ph->SetMarkerColor(kBlue);
   gr_ph->SetTitle("");
   double Ampl_max = 0;
   for(int i=1;i<Nmod+1;i++)
     if(Ampl_max<hm->GetBinContent(i)) 
       Ampl_max = hm->GetBinContent(i);
   TH2D *hm_ph = new TH2D("hm_ph","; phase; magnitude",1e2,-3.15,3.15,1e2,0,1.1*Ampl_max);
   hm_ph->SetMarkerStyle(21);
   hm_ph->SetMarkerSize(1.1);

   //   gr_im_re->SetPoint(0,im_full[i],re_full[i]);
   for(int i=0;i<Nmod+1;i++){
     double ampl = hm->GetBinContent(i);
     double phase = hp->GetBinContent(i);
     gr_m_ph->SetPoint(i,phase,ampl);
     hm_ph->Fill(phase,ampl);
     //    gr_im_re->SetPoint(i,im_full[i],re_full[i]);
   }
 
//[END]Do FFT -------------------------------------------------------------------------
  

  TLegend *leg = new TLegend(0.21,0.75,0.59,0.99);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);
  leg->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
 TLegend *leg2 = new TLegend(0.2,0.8,0.58,0.99);
  leg2->SetFillColor(0);
  leg2->SetTextFont(42);
  leg2->SetTextSize(0.04);
  leg2->SetHeader("(X,Y)_{POS} [mm]; (X,Y)_{TILT} [mrad]");
  TString text_sh = "(";
  text_sh +=x_sh;
  text_sh +=",";
  text_sh +=y_sh;
  text_sh += "); (";
  text_sh += angx_sh;
  text_sh +=",";
  text_sh += angy_sh;
  text_sh += ")";
  leg->AddEntry(h_Nideal,"(0,0); (0,0)","f");	  
  leg->AddEntry(h_Nshifted,text_sh,"f");	  
  leg2->AddEntry(gr_amp,text_sh,"p");
  TCanvas *c1 = new TCanvas("Assym", "Assym", 800, 600);
  c1->Divide(2,2);
  c1->cd(1);
  h_Nshifted->GetXaxis()->SetRangeUser(0,20);
  h_Nshifted->Draw();
  h_Nideal->Draw("same");
  leg->Draw();
  c1->cd(3);
  hm->Draw();
  c1->cd(4);
  hp->Draw();
leg2->Draw();
 c1->cd(2); 
 gr_amp->Draw("AP");
 gr_amp->GetXaxis()->SetTitle("frequencyID");
 gr_amp->GetYaxis()->SetTitle("Magnitude");
 leg2->Draw();


 TString fout_name = path;
 fout_name +="/AssymResFFTsimple_";
 fout_name += this_name;
 TString fout_name_pdf = fout_name;
 fout_name_pdf  +=".pdf";
 TString fout_name_root = fout_name;
 fout_name_root  +=".root";
 c1->SaveAs(fout_name_pdf);
 TFile *finOUT = new TFile(fout_name_root,"RECREATE");
 h_Nshifted->Write();
 h_Nideal->Write();
 gr_amp->SetName("gr_amp");
 gr_amp->Write();
 leg->Write();
 finOUT->Write();
}
