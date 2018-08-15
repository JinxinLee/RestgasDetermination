/// g++ $(root-config --cflags --glibs) -L${PANDAROOT}/buildP/lib LimitsAlign.C -o /home/karavdin/pandaRoot12/macro/lmd/LMD_alignment/LimitsAlign

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>
#include <TNtuple.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TStopwatch.h>
#include <sstream>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
int LimitsAlign(TString pathG="/home/karavdin/datastorage/AlignmentLMDpixel/10data_20consts_SamplesStudyOct2013/BOX/mom_15/", double daGL=0,int iSc=0)
{

  const int nDt = 8;
  double v_tr_sc[nDt]={0,50,100,200,300,400,500,600};//misalign, trans
 
  const int nDa = 1; //TEST
  double v_rt_sc[nDa]={daGL};//misalign, trans
  const int nSdGL=100;//number of data samples
  const int nSdstep = 10; //step size for "number of used trks in Knossos"
  const int nuSd = nSdGL/nSdstep;
  const int nDS2 = 10;//number of samples simulated in stepC for the same case

  //  for(int iSc=0;iSc<nSc;iSc++){//diff output files
    for(int iDa=0;iDa<nDa;iDa++){//diff output files
      double  rt_sc1 = v_rt_sc[iDa];
      double av_theta_mean_b[nuSd+1][nDt],av_theta_rms_b[nuSd+1][nDt];
      double av_theta_mean_a[nuSd+1][nDt],av_theta_rms_a[nuSd+1][nDt];
      double av_theta_mean_r[nuSd+1][nDt],av_theta_rms_r[nuSd+1][nDt];
      double avEr_theta_mean_b[nuSd+1][nDt],avEr_theta_rms_b[nuSd+1][nDt];
      double avEr_theta_mean_a[nuSd+1][nDt],avEr_theta_rms_a[nuSd+1][nDt];
      double avEr_theta_mean_r[nuSd+1][nDt],avEr_theta_rms_r[nuSd+1][nDt];
      double av_events[nuSd+1][nDt], avEr_events[nuSd+1][nDt];
      double av_mis_a_mean[6][nuSd+1][nDt];
      double av_mis_b_mean[6][nuSd+1][nDt];
      double avEr_mis_a_mean[6][nuSd+1][nDt];
      double avEr_mis_b_mean[6][nuSd+1][nDt];
      

    TMultiGraph *mgr_mis[6];
    TMultiGraph *mgr_theta = new TMultiGraph("mgr_theta_mean","");
    TMultiGraph *mgr_theta_rms = new TMultiGraph("mgr_theta_rms","");
    TMultiGraph *mgr_theta_b = new TMultiGraph("mgr_theta_mean_b","");
    TMultiGraph *mgr_theta_rms_b = new TMultiGraph("mgr_theta_rms_b","");
    TMultiGraph *mgr_events = new TMultiGraph("mgr_events","");
 
    
 //How to save data
  TString resname= pathG+"/AlignmentLimits_";
  resname+=v_rt_sc[iDa];
  resname+="mrad_";
  resname+=iSc;
  resname+="constsSample";
  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")"; 
  for(int ijsum=1;ijsum<nuSd+1;ijsum++){ //loop over diff stat.samples (plot with diff markers)
    int isum = 1+nSdstep*(ijsum-1);
    for(int idt=0;idt<nDt;idt++){//loop over dt values (x-axis of plots)
      // for(int ipart=0;ipart<nSd;ipart++){//loop over samples (take average)
	double tr_sc = v_tr_sc[idt];
	double rt_sc = v_rt_sc[iDa];
	//Where search for files
	TString path = pathG+"/";
	path+=tr_sc;
	path+="mkm_";
	path+=rt_sc;
	path+="mrad/";
	path+=iSc;
	path+="Sum";
	//	path+=nSd; //sample aligned on max number of tracks
	path+=isum; //sample for alignment with sum of samples
	path+="Samples/resultsSUM/";

	/// Theta resolution--------------------------------------------------------------------------------------  
	TString name1 = path + "/Lumi_QA_before.root";
	TString name2 = path + "/Lumi_QA_after.root";
	TString name3 = pathG+"/0mkm_0mrad/";
	name3+=iSc;
	name3+="Sum";
	name3+=isum; //sample for alignment with sum of samples
	name3+="Samples/resultsSUM/Lumi_QA_before.root";
	TH1 *hbefore;   
	TH1 *hafter;
	TH1 *hrefer;

	TFile *f1 = new TFile(name1,"READ");
	if (f1->IsZombie()) {
	  std::cout << "!!! Error opening file " <<name1<< std::endl;
	  continue;
	  //   return;
	}
	
	hbefore = (TH1*)f1->Get("NearIP/hResTheta");
	hbefore->SetName("hResThetaBefore");
	hbefore->SetTitle("#theta resolution");
	av_theta_mean_b[isum][idt]  = 1e6*(hbefore->GetMean());
	av_theta_rms_b[isum][idt]  = 1e6*(hbefore->GetRMS());
	avEr_theta_mean_b[isum][idt]  = 1e6*(hbefore->GetMeanError());
	avEr_theta_rms_b[isum][idt]  = 1e6*(hbefore->GetRMSError());
	//	events[isum][idt] =  1e-3*(hbefore->GetEntries());
	//	cout<<"ipart = "<<ipart<<" for sum of "<<isum<<" samlpes, with iDa = "<<iDa<<" idt = "<<idt<<" has "<<events[isum][idt]<<" events"<<endl;
	TFile *f2 = new TFile(name2,"READ");
	if (f2->IsZombie()) {
	  std::cout << "!!! Error opening file " <<name2<< std::endl;
	  continue;
	}
	hafter = (TH1F*)f2->Get("NearIP/hResTheta");
	hafter->SetName("hResThetaAfter");
	hafter->SetTitle("#theta resolution");
	av_theta_mean_a[isum][idt]  = 1e6*(hafter->GetMean());
	av_theta_rms_a[isum][idt]  = 1e6*(hafter->GetRMS());
	avEr_theta_mean_a[isum][idt]  = 1e6*(hafter->GetMeanError());
	avEr_theta_rms_a[isum][idt]  = 1e6*(hafter->GetRMSError());
	TFile *f3 = new TFile(name3,"READ");
	if (f3->IsZombie()) {
	  std::cout << "!!! Error opening file " <<name2<< std::endl;
	  continue;
	}
	hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
	hrefer->SetName("hResThetaRef");
	hrefer->SetTitle("#theta resolution");
	av_theta_mean_r[isum][idt]  = 1e6*(hrefer->GetMean());
	av_theta_rms_r[isum][idt]  = 1e6*(hrefer->GetRMS());
	avEr_theta_mean_r[isum][idt]  = 1e6*(hrefer->GetMeanError());
	avEr_theta_rms_r[isum][idt]  = 1e6*(hrefer->GetRMSError());
	/// END (Theta resolution) ---------------------------------------------------------------
	f1->Close();
	f2->Close();
	f3->Close();

	///Misalignment constants -----------------------------------------------------------------
	TString namemisc = pathG+"/";
	namemisc+=tr_sc;
	namemisc+="mkm_";
	namemisc+=rt_sc;
	namemisc+="mrad/";
	namemisc+=iSc;
	namemisc+="Sum";
	//	namemisc+=nSd; //sample aligned on max number of tracks [take result from Knossos with the largerst num of rec.trks]
	namemisc+=isum; //sample for alignment with sum of samples
	namemisc+="Samples/KnossosResults.root";
	TFile *fmisc = new TFile(namemisc,"READ");
	if (fmisc->IsZombie()) {
	  std::cout << "!!! Error opening file " <<namemisc<< std::endl;
	}

	TH2F *hmis_b[6];	
	TH2F *hmis_a[6];
	for(int imdf=0;imdf<6;imdf++){
	  TString nb = "mis_before_";
	  nb+=imdf;
	  hmis_b[imdf] = (TH2F*)fmisc->Get(nb);
	  av_mis_b_mean[imdf][isum][idt] = 1e4*(hmis_b[imdf]->ProjectionY()->GetRMS());
	  avEr_mis_b_mean[imdf][isum][idt] = 1e4*(hmis_b[imdf]->ProjectionY()->GetRMSError());

	  TString na = "mis_diff_";
	  na+=imdf;
	  hmis_a[imdf] = (TH2F*)fmisc->Get(na);
	  av_mis_a_mean[imdf][isum][idt] = 1e4*(hmis_a[imdf]->ProjectionY()->GetRMS());
	  avEr_mis_a_mean[imdf][isum][idt] = 1e4*(hmis_a[imdf]->ProjectionY()->GetRMSError());
	}
	/// END (Misalignment constants) -----------------------------------------------------------
	fmisc->Close();
     
    }//end //loop over dt values 
  }//end loop over diff sum values

 /// Let's draw now! ---------------------------------------------------------------------
 TCanvas cRes;  
 TGraphErrors *gr_theta_b[nuSd+1];
 TGraphErrors *gr_theta_a[nuSd+1];
 TGraphErrors *gr_theta_r[nuSd+1];
 TGraphErrors *gr_events[nuSd+1];
 TLegend *legThMb = new TLegend(0.1,0.2,0.38,0.58);
 TLegend *legThMa = new TLegend(0.1,0.2,0.38,0.58);
  for(int isum=1;isum<nuSd+1;isum++){//loop over diff stat.samples (plot with diff markers)
    TString sname = " ";
    sname += isum;
    sname +=" samples";

    gr_events[isum] = new TGraphErrors(nDt,v_tr_sc, av_events[isum],0, avEr_events[isum]);
    TString name_ev = "number_ev_sum";
    name_ev += isum;
    gr_events[isum]->SetName(name_ev);
    gr_events[isum]->SetMarkerStyle(20+isum); 
    gr_events[isum]->SetMarkerColor(kGreen-3); 
    gr_events[isum]->SetMarkerSize(1.5); 
    mgr_events->Add(gr_events[isum]);

    gr_theta_b[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_mean_b[isum],0, avEr_theta_mean_b[isum]);
    TString name_thm_b = "th_mean_b_sum";
    name_thm_b += isum;
    gr_theta_b[isum]->SetName(name_thm_b);
    gr_theta_b[isum]->SetMarkerStyle(20+isum); 
    gr_theta_b[isum]->SetMarkerColor(kGreen-3); 
    gr_theta_b[isum]->SetMarkerSize(1.5); 
    legThMb->AddEntry(gr_theta_b[isum],sname,"P");

   gr_theta_a[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_mean_a[isum],0, avEr_theta_mean_a[isum]);
   //   gr_theta_a[isum]->SetMarkerStyle(30+isum); 
   TString name_thm_a = "th_mean_a_sum";
   name_thm_a += isum;
   gr_theta_a[isum]->SetName(name_thm_a);
   gr_theta_a[isum]->SetMarkerStyle(20+isum); 
   gr_theta_a[isum]->SetMarkerColor(kOrange+7); 
   gr_theta_a[isum]->SetMarkerSize(1.5); 
   //
   legThMa->AddEntry(gr_theta_a[isum],sname,"P");
   gr_theta_r[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_mean_r[isum],0, avEr_theta_mean_r[isum]);
   //   gr_theta_r[isum]->SetMarkerStyle(26+isum); 
  TString name_thm_r = "th_mean_r_sum";
   name_thm_r += isum;
   gr_theta_r[isum]->SetName(name_thm_r);
   gr_theta_r[isum]->SetMarkerStyle(20+isum); 
   gr_theta_r[isum]->SetMarkerColor(15);
   gr_theta_r[isum]->SetMarkerSize(1.5); 
   mgr_theta_b->Add(gr_theta_b[isum]);
   mgr_theta->Add(gr_theta_a[isum]);
   mgr_theta->Add(gr_theta_r[isum]);
   
  }
 mgr_theta->Draw("AP");
 mgr_theta->GetXaxis()->SetTitle("#delta_{t}, #mum");
 mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");

 legThMa->SetFillColor(0);
 legThMb->SetFillColor(0);
 legThMa->Draw();
 cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
 cRes.Clear();
 mgr_theta_b->Draw("AP");
 mgr_theta_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
 mgr_theta_b->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 legThMb->Draw();
 cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
 cRes.Clear();
 TGraphErrors *gr_theta_rms_b[nuSd+1];
 TGraphErrors *gr_theta_rms_a[nuSd+1];
 TGraphErrors *gr_theta_rms_r[nuSd+1];
  for(int isum=1;isum<nuSd+1;isum++){//loop over diff stat.samples (plot with diff markers)
   gr_theta_rms_b[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_rms_b[isum],0, avEr_theta_rms_b[isum]);
   gr_theta_rms_b[isum]->SetMarkerStyle(20+isum); 
   gr_theta_rms_b[isum]->SetMarkerColor(kGreen-3); 
   gr_theta_rms_b[isum]->SetMarkerSize(1.5); 
   TString name_thm_b = "th_rms_b_sum";
   name_thm_b += isum;
   gr_theta_rms_b[isum]->SetName(name_thm_b);

   gr_theta_rms_a[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_rms_a[isum],0, avEr_theta_rms_a[isum]);
   //   gr_theta_rms_a[isum]->SetMarkerStyle(30+isum); 
   gr_theta_rms_a[isum]->SetMarkerStyle(20+isum); 
   gr_theta_rms_a[isum]->SetMarkerColor(kOrange+7); 
   gr_theta_rms_a[isum]->SetMarkerSize(1.5); 
   TString name_thm_a = "th_rms_a_sum";
   name_thm_a += isum;
   gr_theta_rms_a[isum]->SetName(name_thm_a);
   gr_theta_rms_r[isum] = new TGraphErrors(nDt,v_tr_sc, av_theta_rms_r[isum],0, avEr_theta_rms_r[isum]);
   //   gr_theta_rms_r[isum]->SetMarkerStyle(26+isum); 
   gr_theta_rms_r[isum]->SetMarkerStyle(20+isum); 
   gr_theta_rms_r[isum]->SetMarkerColor(15);
   gr_theta_rms_r[isum]->SetMarkerSize(1.5); 
   TString name_thm_r = "th_rms_r_sum";
   name_thm_r += isum;
   gr_theta_rms_r[isum]->SetName(name_thm_r);
   mgr_theta_rms_b->Add(gr_theta_rms_b[isum]);
   mgr_theta_rms->Add(gr_theta_rms_a[isum]);
   mgr_theta_rms->Add(gr_theta_rms_r[isum]);
  
  }
 mgr_theta_rms->Draw("AP");
 mgr_theta_rms->GetXaxis()->SetTitle("#delta_{t}, #mum");
 mgr_theta_rms->GetYaxis()->SetTitle("#theta_{#sigma}, #murad");
 cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
 cRes.Clear();
 mgr_theta_rms_b->Draw("AP");
 mgr_theta_rms_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
 mgr_theta_rms_b->GetYaxis()->SetTitle("#theta_{#sigma}, #murad");
 cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
 cRes.Clear();
 mgr_events->Draw("APL");
 mgr_events->GetXaxis()->SetTitle("#delta_{t}, #mum");
 mgr_events->GetYaxis()->SetTitle("events, 10^{3}");
 cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
 cRes.Clear();
 //put together missalignment consts results
 TGraphErrors *gr_mis_b[6][nuSd+1];
 TGraphErrors *gr_mis_a[6][nuSd+1];
 
 for(int ialc=0;ialc<6;ialc++){
  for(int isum=1;isum<nuSd+1;isum++){//loop over diff stat.samples (plot with diff markers)
     gr_mis_b[ialc][isum] = new TGraphErrors(nDt,v_tr_sc,av_mis_b_mean[ialc][isum],0,avEr_mis_b_mean[ialc][isum]);
     gr_mis_b[ialc][isum]->SetMarkerStyle(20+isum);
     gr_mis_b[ialc][isum]->SetMarkerColor(kGreen-3);
     gr_mis_b[ialc][isum]->SetMarkerSize(1.5);
     gr_mis_a[ialc][isum] = new TGraphErrors(nDt,v_tr_sc,av_mis_a_mean[ialc][isum],0,avEr_mis_a_mean[ialc][isum]);
     gr_mis_a[ialc][isum]->SetMarkerStyle(20+isum);
     gr_mis_a[ialc][isum]->SetMarkerColor(kOrange+7);
     gr_mis_a[ialc][isum]->SetMarkerSize(1.5);
     TString name_miss_a = "mis_a_";
     name_miss_a +=ialc;
     name_miss_a +="_sum";
     name_miss_a +=isum;
     TString name_miss_b = "mis_b_";
     name_miss_b +=ialc;
     name_miss_b +="_sum";
     name_miss_b +=isum;
     gr_mis_b[ialc][isum]->SetName(name_miss_b);
     gr_mis_a[ialc][isum]->SetName(name_miss_a);
     mgr_mis[ialc]->Add(gr_mis_b[ialc][isum]);
     mgr_mis[ialc]->Add(gr_mis_a[ialc][isum]);
  }
   mgr_mis[ialc]->Draw("AP");
   mgr_mis[ialc]->GetXaxis()->SetTitle("#delta_{t}, #mum");
   if(ialc==0) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
   if(ialc==1) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
   if(ialc==2) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
   if(ialc==3) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
   if(ialc==4) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
   if(ialc==5) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
   if(ialc<5){
     if(ialc==0 || ialc==1)
       cRes.Print(resname_pdf_o); //write canvas and keep the file open
   }
   else cRes.Print(resname_pdf_c); //write canvas and close pdf file
   cRes.Clear();
 }


   
 ///--------------------------------------------------------------------------------------------
 TString out = resname+".root";
 TFile *f = new TFile(out,"RECREATE");
 mgr_theta->Write();
 mgr_theta_rms->Write();
 mgr_theta_b->Write();
 mgr_theta_rms_b->Write();
 for(int ialc=0;ialc<6;ialc++){
   mgr_mis[ialc]->Write();
 }
 
  for(int isum=1;isum<nuSd+1;isum++){//loop over diff stat.samples (plot with diff markers)
    gr_events[isum]->Write();
    gr_theta_b[isum]->Write();
    gr_theta_a[isum]->Write();
    gr_theta_r[isum]->Write();
    gr_theta_rms_b[isum]->Write();
    gr_theta_rms_a[isum]->Write();
    gr_theta_rms_r[isum]->Write();
    for(int ialc=0;ialc<6;ialc++){
      gr_mis_b[ialc][isum]->Write();
      gr_mis_a[ialc][isum]->Write();
    }
  }
 
  f->Write();
  f->Close();
    }
  return 0;
}

int main(int __argc,char *__argv[]) {
  TString storePath="/home/karavdin/datastorage/AlignmentLMDpixel/100data_10consts_SamplesStudyOct2013/BOX/mom_15/";
  double daGL=0;
  int iSc=0;
  std::string pathStr="",daStr="",iScStr="";
 // decode arguments
  if( __argc>1 && ( strcmp( __argv[1], "-help" ) == 0
		    || strcmp( __argv[1], "--help" ) == 0 ) ){
    std::cout << "This is script for alignment data analysis \n"
	      << "with parameters\n"
	      <<"-path path to the file(s) \n"
	      <<"-da scale for rotation misalignment \n"
	      <<"-iSc number of misalignment consts sample \n"
	      <<"Have fun! \n"
	      << std::endl;
    return 0;
  }
 while ((optind < (__argc-1) ) && (__argv[optind][0]=='-')) {
    bool found=false;
    std::string sw = __argv[optind];
    if (sw=="-path"){
      optind++;
      pathStr = __argv[optind];
      found=true;
    }
    if (sw=="-da"){
      optind++;
      daStr = __argv[optind];
      found=true;
    }
  
    if (sw=="-iSc"){
      optind++;
      iScStr = __argv[optind];
      found=true;
    }
    if (!found){
      std::cout<< "Unknown switch: "
	       << __argv[optind] <<std::endl;
      optind++;
    }
  while ( (optind < __argc ) && __argv[optind][0]!='-' ) optind++; 
 }

 std::stringstream pathSStr(pathStr), daSStr(daStr),iScSStr(iScStr);
 pathSStr >> storePath;
 daSStr >> daGL;
 // isumSStr >> isum;
 iScSStr >> iSc;
 LimitsAlign(storePath,daGL,iSc);
 return 0;
}
