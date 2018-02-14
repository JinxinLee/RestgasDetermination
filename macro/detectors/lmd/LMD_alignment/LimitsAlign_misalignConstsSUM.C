
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
int LimitsAlign_misalignConstsSUM(TString pathG="/home/karavdin/datastorage/AlignmentLMDpixel/10data_20consts_SamplesStudyOct2013/BOX/mom_15/")
{

  const int nDt = 8;
  double v_tr_sc[nDt]={0,50,100,200,300,400,500,600};//misalign, trans
  // const int nDa = 6;
  // double v_rt_sc[nDa]={0,1,3,5,7,9};//misalign, trans
  const int nDa = 4; //TEST
  double v_rt_sc[nDa]={0,1,3,5};//misalign, trans

  const int nSd=5;//number of data samples
  const int nSc=10;//number of align.consts samples
  TGraphErrors *gr_theta_b[nSc];
  TGraphErrors *gr_theta_a[nSc];
  TGraphErrors *gr_theta_r[nSc];
  TGraphErrors *gr_events[nSc];
  TGraphErrors *gr_theta_rms_b[nSc];
  TGraphErrors *gr_theta_rms_a[nSc];
  TGraphErrors *gr_theta_rms_r[nSc];
  TGraphErrors *gr_mis_b[6][nSc];
  TGraphErrors *gr_mis_a[6][nSc];

  Double_t *theta_mean_b[nSc], *theta_mean_a[nSc], *theta_mean_r[nSc];
  Double_t *theta_rms_b[nSc], *theta_rms_a[nSc], *theta_rms_r[nSc];
  Double_t *events[nSc];
  Double_t *err_theta_mean_b[nSc], *err_theta_mean_a[nSc], *err_theta_mean_r[nSc];
  Double_t *err_theta_rms_b[nSc], *err_theta_rms_a[nSc], *err_theta_rms_r[nSc];
  Double_t *err_events[nSc];
  Double_t *mis_b[nSc][6], *mis_a[nSc][6];

  //  Double_t *events;
  for(int iDa=0;iDa<nDa;iDa++){//diff output files Delta_a
  // for(int iDa=0;iDa<1;iDa++){//diff output files Delta_a //TEST
   TString resnameout= pathG+"/MisalignConstSUM_AlignmentLimits_";
   resnameout+=v_rt_sc[iDa];
   resnameout+="mrad";
   resnameout+=5;// Maximum available statistic
   resnameout+="SumDataSample";
   TString resnameout_pdf = resnameout+".pdf";
   TString resnameout_pdf_o = resnameout_pdf+"(";
   TString resnameout_pdf_c = resnameout_pdf+")"; 

   for(int iSc=0;iSc<nSc;iSc++){//diff output files consts sample
      ///READ file
      TString resname= pathG+"/AlignmentLimits_";
      resname+=v_rt_sc[iDa];
      resname+="mrad_";
      resname+=iSc;
      resname+="constsSample";
      TString in = resname+".root";
      TFile *f = new TFile(in,"READ");
      gr_events[iSc] = (TGraphErrors *)f->Get("number_ev_sum5");
      //      gr_events[iSc]->Print();
      gr_theta_b[iSc] = (TGraphErrors *)f->Get("th_mean_b_sum5");
      gr_theta_a[iSc] = (TGraphErrors *)f->Get("th_mean_a_sum5");
      gr_theta_r[iSc] = (TGraphErrors *)f->Get("th_mean_r_sum5");
      gr_theta_rms_b[iSc] = (TGraphErrors *)f->Get("th_rms_b_sum5");
      gr_theta_rms_a[iSc] = (TGraphErrors *)f->Get("th_rms_a_sum5");
      gr_theta_rms_r[iSc] = (TGraphErrors *)f->Get("th_rms_r_sum5");
      for(int ialc=0;ialc<6;ialc++){
	TString nameb = "mis_b_";
	nameb +=ialc;
	nameb +="_sum5";
	gr_mis_b[ialc][iSc] = (TGraphErrors *)f->Get(nameb);
	TString namea = "mis_a_";
	namea +=ialc;
	namea +="_sum5";
	gr_mis_a[ialc][iSc] = (TGraphErrors *)f->Get(namea);
	mis_b[iSc][ialc] = gr_mis_b[ialc][iSc]->GetY();
	mis_a[iSc][ialc] = gr_mis_a[ialc][iSc]->GetY();
      }
  
      /// read data
      events[iSc] = gr_events[iSc]->GetY();
      err_events[iSc] = gr_events[iSc]->GetEY();
      theta_mean_b[iSc] = gr_theta_b[iSc]->GetY();
      err_theta_mean_b[iSc] = gr_theta_b[iSc]->GetEY();
      theta_mean_a[iSc] = gr_theta_a[iSc]->GetY();
      err_theta_mean_a[iSc] = gr_theta_a[iSc]->GetEY();
      theta_mean_r[iSc] = gr_theta_r[iSc]->GetY();
      err_theta_mean_r[iSc] = gr_theta_r[iSc]->GetEY();
      theta_rms_b[iSc] = gr_theta_rms_b[iSc]->GetY();
      err_theta_rms_b[iSc] = gr_theta_rms_b[iSc]->GetEY();
      theta_rms_a[iSc] = gr_theta_rms_a[iSc]->GetY();
      err_theta_rms_a[iSc] = gr_theta_rms_a[iSc]->GetEY();
      theta_rms_r[iSc] = gr_theta_rms_r[iSc]->GetY();
      err_theta_rms_r[iSc] = gr_theta_rms_r[iSc]->GetEY();
    }//end diff output files consts sample

   // --- calculate average and av.error ---
   double av_theta_mean_b[nDt],av_theta_rms_b[nDt];
   double av_theta_mean_a[nDt],av_theta_rms_a[nDt];
   double av_theta_mean_r[nDt],av_theta_rms_r[nDt];
   double av_err_theta_mean_b[nDt],av_err_theta_rms_b[nDt]; //Errors of avarage of prev. sum
   double av_err_theta_mean_a[nDt],av_err_theta_rms_a[nDt];
   double av_err_theta_mean_r[nDt],av_err_theta_rms_r[nDt];

   double avEr_theta_mean_b[nDt],avEr_theta_rms_b[nDt]; //Errors of avarage of this sum
   double avEr_theta_mean_a[nDt],avEr_theta_rms_a[nDt];
   double avEr_theta_mean_r[nDt],avEr_theta_rms_r[nDt];
   double av_events[nDt], av_err_events[nDt], avEr_events[nDt];
   double av_mis_a_mean[6][nDt];
   double av_mis_b_mean[6][nDt];
   // double av_err_mis_a_mean[6][nDt]; //zero, since const sample was fixed in data sample sum!
   // double av_err_mis_b_mean[6][nDt];
   double avEr_mis_a_mean[6][nDt];
   double avEr_mis_b_mean[6][nDt];
   for(int idt=0;idt<nDt;idt++){//loop over dt values (x-axis of plots)
	av_theta_mean_b[idt] = 0;
	av_theta_rms_b[idt] = 0;
	av_theta_mean_a[idt] = 0;
	av_theta_rms_a[idt] = 0;
	av_theta_mean_r[idt] = 0;
	av_theta_rms_r[idt] = 0;
	av_err_theta_mean_b[idt] = 0;
	av_err_theta_rms_b[idt] = 0;
	av_err_theta_mean_a[idt] = 0;
	av_err_theta_rms_a[idt] = 0;
	av_err_theta_mean_r[idt] = 0;
	av_err_theta_rms_r[idt] = 0;

	avEr_theta_mean_b[idt] = 0;
	avEr_theta_rms_b[idt] =0;
	avEr_theta_mean_a[idt] = 0;
	avEr_theta_rms_a[idt] = 0;
	avEr_theta_mean_r[idt] = 0;
	avEr_theta_rms_r[idt] = 0;
      
	av_events[idt] = 0;
	av_err_events[idt] = 0;
	avEr_events[idt] = 0;
           
      for(int ialc=0;ialc<6;ialc++){
	av_mis_a_mean[ialc][idt] = 0;
	av_mis_b_mean[ialc][idt] = 0;
	avEr_mis_a_mean[ialc][idt] = 0;
	avEr_mis_b_mean[ialc][idt] = 0;
      }//end loop over misalign.const
    } // end loop over dt values (x-axis of plots)

   double i_nS = 1./nSc;
   double i_nS_r = 1./(nSc-1);
  /// average calculatuon -------------------------------------------
    for(int idt=0;idt<nDt;idt++){//loop over dt values (x-axis of plots)
      for(int isum=0;isum<nSc;isum++){//loop over diff stat.samples (plot with diff markers)
	//	cout<<"events_sum[isum][idt] = "<<events[isum][idt]<<endl;
 	av_theta_mean_b[idt]+=theta_mean_b[isum][idt];
	av_theta_mean_a[idt]+=theta_mean_a[isum][idt];
	av_theta_mean_r[idt]+=theta_mean_r[isum][idt];
 	av_theta_rms_b[idt]+=theta_rms_b[isum][idt];
 	av_theta_rms_a[idt]+=theta_rms_a[isum][idt];
 	av_theta_rms_r[idt]+=theta_rms_r[isum][idt];
 	av_events[idt] += events[isum][idt];

	av_err_theta_mean_b[idt]+=err_theta_mean_b[isum][idt];
	av_err_theta_mean_a[idt]+=err_theta_mean_a[isum][idt];
	av_err_theta_mean_r[idt]+=err_theta_mean_r[isum][idt];
 	av_err_theta_rms_b[idt]+=err_theta_rms_b[isum][idt];
 	av_err_theta_rms_a[idt]+=err_theta_rms_a[isum][idt];
 	av_err_theta_rms_r[idt]+=err_theta_rms_r[isum][idt];
 	av_err_events[idt] += err_events[isum][idt];

	for(int ialc=0;ialc<6;ialc++){
 	  av_mis_a_mean[ialc][idt] += mis_a[isum][ialc][idt];
 	  av_mis_b_mean[ialc][idt] += mis_b[isum][ialc][idt];
 	}
      }
      av_theta_mean_b[idt] *=i_nS;
      av_theta_mean_a[idt] *=i_nS;
      av_theta_mean_r[idt] *=i_nS;
      av_theta_rms_b[idt] *=i_nS;
      av_theta_rms_a[idt] *=i_nS;
      av_theta_rms_r[idt] *=i_nS;
      av_events[idt] *=i_nS;
      av_err_theta_mean_b[idt] *=i_nS;
      av_err_theta_mean_a[idt] *=i_nS;
      av_err_theta_mean_r[idt] *=i_nS;
      av_err_theta_rms_b[idt] *=i_nS;
      av_err_theta_rms_a[idt] *=i_nS;
      av_err_theta_rms_r[idt] *=i_nS;
      av_err_events[idt] *=i_nS;
      for(int ialc=0;ialc<6;ialc++){
      	av_mis_a_mean[ialc][idt] *= i_nS;
      	av_mis_b_mean[ialc][idt] *= i_nS;
      }
    }
  /// END average calculatuon --------------------------------------

 // /// average err. calculatuon -------------------------------------------
    for(int idt=0;idt<nDt;idt++){//loop over dt values (x-axis of plots)
      for(int isum=0;isum<nSc;isum++){//loop over samples
 	avEr_events[idt] += (events[isum][idt]-av_events[idt])*(events[isum][idt]-av_events[idt]);
 	avEr_theta_mean_b[idt]+=(theta_mean_b[isum][idt]-av_theta_mean_b[idt])*(theta_mean_b[isum][idt]-av_theta_mean_b[idt]);
 	avEr_theta_mean_a[idt]+=(theta_mean_a[isum][idt]-av_theta_mean_a[idt])*(theta_mean_a[isum][idt]-av_theta_mean_a[idt]);
 	avEr_theta_mean_r[idt]+=(theta_mean_r[isum][idt]-av_theta_mean_r[idt])*(theta_mean_r[isum][idt]-av_theta_mean_r[idt]);
 	avEr_theta_rms_b[idt]+=(theta_rms_b[isum][idt]-av_theta_rms_b[idt])*(theta_rms_b[isum][idt]-av_theta_rms_b[idt]);
 	avEr_theta_rms_a[idt]+=(theta_rms_a[isum][idt]-av_theta_rms_a[idt])*(theta_rms_a[isum][idt]-av_theta_rms_a[idt]);
 	avEr_theta_rms_r[idt]+=(theta_rms_r[isum][idt]-av_theta_rms_r[idt])*(theta_rms_r[isum][idt]-av_theta_rms_r[idt]);
 	for(int ialc=0;ialc<6;ialc++){
 	  avEr_mis_a_mean[ialc][idt] +=(mis_a[isum][ialc][idt]-av_mis_a_mean[ialc][idt])*(mis_a[isum][ialc][idt]-av_mis_a_mean[ialc][idt]);
 	  avEr_mis_b_mean[ialc][idt] +=(mis_b[isum][ialc][idt]-av_mis_b_mean[ialc][idt])*(mis_b[isum][ialc][idt]-av_mis_b_mean[ialc][idt]);
 	}
      }
      avEr_theta_mean_b[idt] *=i_nS_r;
      avEr_theta_mean_a[idt] *=i_nS_r;
      avEr_theta_mean_r[idt] *=i_nS_r;
      avEr_theta_rms_b[idt] *=i_nS_r;
      avEr_theta_rms_a[idt] *=i_nS_r;
      avEr_theta_rms_r[idt] *=i_nS_r;
      avEr_events[idt] *=i_nS_r;
      avEr_theta_mean_b[idt]  = sqrt(avEr_theta_mean_b[idt]+av_err_theta_mean_b[idt]*av_err_theta_mean_b[idt]);
      avEr_theta_mean_a[idt]  = sqrt(avEr_theta_mean_a[idt]+av_err_theta_mean_a[idt]*av_err_theta_mean_a[idt]);
      avEr_theta_mean_r[idt]  = sqrt(avEr_theta_mean_r[idt]+av_err_theta_mean_r[idt]*av_err_theta_mean_r[idt]);
      // avEr_theta_mean_a[idt]  = sqrt(avEr_theta_mean_a[idt]);
      // avEr_theta_mean_r[idt] = sqrt(avEr_theta_mean_r[idt]);
      avEr_theta_rms_b[idt] = sqrt(avEr_theta_rms_b[idt]+av_err_theta_rms_b[idt]*av_err_theta_rms_b[idt]);
      avEr_theta_rms_a[idt] = sqrt(avEr_theta_rms_a[idt]+av_err_theta_rms_a[idt]*av_err_theta_rms_a[idt]);
      avEr_theta_rms_r[idt] = sqrt(avEr_theta_rms_r[idt]+av_err_theta_rms_r[idt]*av_err_theta_rms_r[idt]);
      // avEr_theta_rms_a[idt] = sqrt(avEr_theta_rms_a[idt]);
      // avEr_theta_rms_r[idt] = sqrt(avEr_theta_rms_r[idt]);
      avEr_events[idt] = sqrt(avEr_events[idt]+av_err_events[idt]*av_err_events[idt]);
      for(int ialc=0;ialc<6;ialc++){
 	avEr_mis_a_mean[ialc][idt] *=i_nS_r;
 	avEr_mis_b_mean[ialc][idt] *=i_nS_r;
 	avEr_mis_a_mean[ialc][idt] = sqrt(avEr_mis_a_mean[ialc][idt]);
 	avEr_mis_b_mean[ialc][idt] = sqrt(avEr_mis_b_mean[ialc][idt]);
      }
      cout<<"av_events["<<idt<<"]="<<av_events[idt]<<" +/- "<< avEr_events[idt]<<endl;
 	}
  /// END average err. calculatuon -------------------------------------
  // end --- calculate average and av.error ---

    //Draw ans save results
    TString out = resnameout+".root";
    TFile *fout = new TFile(out,"RECREATE");
    TCanvas cRes;  
    TGraphErrors *gr_av_theta_b = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_mean_b,avEr_mis_b_mean[0],avEr_theta_mean_b);
    gr_av_theta_b->SetName("thetaMean_b");
    gr_av_theta_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
    gr_av_theta_b->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    gr_av_theta_b->SetMarkerStyle(20+iDa); 
    gr_av_theta_b->SetMarkerSize(2.5); 
    gr_av_theta_b->SetMarkerColor(kGreen-3);
    gr_av_theta_b->Draw("AP");
    cRes.Print(resnameout_pdf_o); //write canvas and keep the ps file open
    cRes.Clear();
    gr_av_theta_b->Write();
    TMultiGraph *mgr_theta_a = new TMultiGraph("mgr_theta_mean_a","");
    TGraphErrors *gr_av_theta_a = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_mean_a,avEr_mis_b_mean[0],avEr_theta_mean_a);
    gr_av_theta_a->SetName("thetaMean_a");
    // gr_av_theta_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
    // gr_av_theta_a->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    gr_av_theta_a->SetMarkerStyle(20+iDa); 
    gr_av_theta_a->SetMarkerSize(2.5); 
    gr_av_theta_a->SetMarkerColor(kOrange+7);
    //   gr_av_theta_a->Draw("AP");
    gr_av_theta_a->Write();
    mgr_theta_a->Add(gr_av_theta_a);
    TGraphErrors *gr_av_theta_r = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_mean_r,avEr_mis_b_mean[0],avEr_theta_mean_r);
    gr_av_theta_r->SetName("thetaMean_r");
    // gr_av_theta_r->GetXaxis()->SetTitle("#delta_{t}, #mum");
    // gr_av_theta_r->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    gr_av_theta_r->SetMarkerStyle(20+iDa); 
    gr_av_theta_r->SetMarkerSize(1.5); 
    gr_av_theta_r->SetMarkerColor(15);
    //    gr_av_theta_r->Draw("AP");
    gr_av_theta_r->Write();
    mgr_theta_a->Add(gr_av_theta_r);
    mgr_theta_a->Draw("AP");
    mgr_theta_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
    mgr_theta_a->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    cRes.Print(resnameout_pdf_o); //write canvas and keep the ps file open
    cRes.Clear();

    TGraphErrors *gr_av_theta_rms_b = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_rms_b,avEr_mis_b_mean[0],avEr_theta_rms_b);
    gr_av_theta_rms_b->SetName("thetaMean_b");
    gr_av_theta_rms_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
    gr_av_theta_rms_b->GetYaxis()->SetTitle("#theta_{\sigma}, #murad");
    gr_av_theta_rms_b->SetMarkerStyle(20+iDa); 
    gr_av_theta_rms_b->SetMarkerSize(2.5); 
    gr_av_theta_rms_b->SetMarkerColor(kGreen-3);
    gr_av_theta_rms_b->Draw("AP");
    gr_av_theta_rms_b->Write();
    cRes.Print(resnameout_pdf_o); //write canvas and keep the ps file open
    cRes.Clear();
    TMultiGraph *mgr_theta_rms_a = new TMultiGraph("mgr_theta_rms_a","");
    TGraphErrors *gr_av_theta_rms_a = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_rms_a,avEr_mis_b_mean[0],avEr_theta_rms_a);
    gr_av_theta_rms_a->SetName("thetaMean_a");
    // gr_av_theta_rms_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
    // gr_av_theta_rms_a->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    gr_av_theta_rms_a->SetMarkerStyle(20+iDa); 
    gr_av_theta_rms_a->SetMarkerSize(2.5); 
    gr_av_theta_rms_a->SetMarkerColor(kOrange+7);
    //   gr_av_theta_rms_a->Draw("AP");
    gr_av_theta_rms_a->Write();
    mgr_theta_rms_a->Add(gr_av_theta_rms_a);
    TGraphErrors *gr_av_theta_rms_r = new TGraphErrors(nDt,av_mis_b_mean[0],av_theta_rms_r,avEr_mis_b_mean[0],avEr_theta_rms_r);
    gr_av_theta_rms_r->SetName("thetaMean_r");
    // gr_av_theta_rms_r->GetXaxis()->SetTitle("#delta_{t}, #mum");
    // gr_av_theta_rms_r->GetYaxis()->SetTitle("#theta_{mean}, #murad");
    gr_av_theta_rms_r->SetMarkerStyle(20+iDa); 
    gr_av_theta_rms_r->SetMarkerSize(1.5); 
    gr_av_theta_rms_r->SetMarkerColor(15);
    //    gr_av_theta_rms_r->Draw("AP");
    gr_av_theta_rms_r->Write();
    mgr_theta_rms_a->Add(gr_av_theta_rms_r);
    mgr_theta_rms_a->Draw("AP");
    mgr_theta_rms_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
    mgr_theta_rms_a->GetYaxis()->SetTitle("#theta_{\sigma}, #murad");
    cRes.Print(resnameout_pdf_o); //write canvas and keep the ps file open
    cRes.Clear();
    for(int ialc=0;ialc<6;ialc++){
      TGraphErrors *gr_av_mis_a = new TGraphErrors(nDt,av_mis_b_mean[0],av_mis_a_mean[ialc],avEr_mis_b_mean[0],avEr_mis_a_mean[ialc]);
      TString misconstname = "mis_";
      misconstname+=ialc;
      misconstname+="_a";
      gr_av_mis_a->SetName(misconstname);
      if(ialc==0) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{x}, #mum");
      if(ialc==1) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{y}, #mum");
      if(ialc==2) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{z}, #mum");
      if(ialc==3) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
      if(ialc==4) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
      if(ialc==5) gr_av_mis_a->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
      gr_av_mis_a->SetMarkerStyle(29); 
      gr_av_mis_a->SetMarkerSize(2.5); 
      gr_av_mis_a->SetMarkerColor(kOrange+7);
      gr_av_mis_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
      gr_av_mis_a->Draw("AP");
      cRes.Print(resnameout_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      gr_av_mis_a->Write();
      delete gr_av_mis_a;
    }
    TGraphErrors *gr_av_ev = new TGraphErrors(nDt,av_mis_b_mean[0],av_events,avEr_mis_b_mean[0],avEr_events);
    gr_av_ev->SetName("ev_b");
    gr_av_ev->GetYaxis()->SetTitle("number of tracks used in alignment, 10^{3}");
    gr_av_ev->GetXaxis()->SetTitle("#delta_{t}, #mum");
    gr_av_ev->SetMarkerStyle(21); 
    gr_av_ev->SetMarkerSize(2.5); 
    gr_av_ev->SetMarkerColor(kGreen-3);
    gr_av_ev->Draw("AP");
    cRes.Print(resnameout_pdf_c); //write canvas and keep the ps file open
    cRes.Clear();
    gr_av_ev->Write();
    //  fout->Write();
    fout->Close();
  }//end diff output files Delta_a

  return 0;
}
