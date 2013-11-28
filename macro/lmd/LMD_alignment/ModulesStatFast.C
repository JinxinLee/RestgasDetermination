/// g++ $(root-config --cflags --glibs) -L${PANDAROOT}/buildP/lib ModulesStatFast.C -o /home/karavdin/pandaRoot12/macro/lmd/LMD_alignment/ModulesStat

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
using namespace std;
void ModulesStatFast(TString pathG="/home/karavdin/datastorage/AlignmentLMDpixel/100data_10consts_SamplesStudyOct2013/BOX/mom_15/", double daGL=0, int MisConstSamp=0)
{
  TStopwatch timerB;
  timerB.Start();
  int iSc = MisConstSamp;
   const int nDt = 8;
  double v_tr_sc[nDt]={0,50,100,200,300,400,500,600};//misalign, trans
  const int nMod = 10;
  const int nDa = 1; //TEST
  double v_rt_sc[nDa]={daGL};//misalign, trans

  const int nSdGL=100;//number of data samples
  const int nSdstep = 10; //step size for "number of used trks in Knossos"
  const int nuSd = nSdGL/nSdstep;
  const int nDS2 = 10;//number of samples simulated in stepC for the same case
  //  cout<<"nuSd = "<<nuSd<<endl;
  // const int nSc=10;//number of align.consts samples
  //  const int nSc=1;//number of align.consts samples TEST

 //  for(int iSc=0;iSc<nSc;iSc++){//diff output files
    for(int iDa=0;iDa<nDa;iDa++){//diff output files

      double av_evMod_b[nMod][nuSd+1][nDt];
      double av_evMod_a[nMod][nuSd+1][nDt];
      double err_av_evMod_b[nMod][nuSd+1][nDt];
      double err_av_evMod_a[nMod][nuSd+1][nDt];
      double av_resxMod_b[nMod][nuSd+1][nDt];
      double av_resxMod_a[nMod][nuSd+1][nDt];
      double err_av_resxMod_b[nMod][nuSd+1][nDt];
      double err_av_resxMod_a[nMod][nuSd+1][nDt];
      double av_resyMod_b[nMod][nuSd+1][nDt];
      double av_resyMod_a[nMod][nuSd+1][nDt];
      double err_av_resyMod_b[nMod][nuSd+1][nDt];
      double err_av_resyMod_a[nMod][nuSd+1][nDt];
      //How to save data
      TString resname= pathG+"/AlignmentLimits_EvModStat_";
      resname+=v_rt_sc[iDa];
      resname+="mrad_";
      resname+=iSc;
      resname+="constsSample";
      TString resname_pdf = resname+".pdf";
      TString resname_pdf_o = resname_pdf+"(";
      TString resname_pdf_c = resname_pdf+")"; 

	for(int idt=0;idt<nDt;idt++){//loop over dt values (x-axis of plots)
	  for(int imod=0;imod<nMod;imod++){// loop over modules (diff plots)
	    for(int ijsum=1;ijsum<nuSd+1;ijsum++){ //loop over diff stat.samples (plot with diff markers)
	      int isum = 1+nSdstep*(ijsum-1);
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
	      path+=isum; //sample for alignment with sum of samples
	      path+="Samples/resultsSUM/";
	      
	      /// Hits hists --------------------------------------------------------------------------------------  
	      TString name1 = path + "/hitsRes_before_alignment_sector";
	      name1+=imod;
	      name1+=".root";
	      TString name2 = path + "/hitsRes_after_alignment_sector"; 
	      name2+=imod;
	      name2+=".root";
	      TFile *f1 = new TFile(name1,"READ");
	      TNtuple *nb = (TNtuple*)f1->Get("nhits");
	      // nb->Print();
	      TH1I *ntrksb = (TH1I*)f1->Get("trks");

	      
	      av_resxMod_b[imod][ijsum][idt]=0;
	      av_resxMod_a[imod][ijsum][idt]=0;
	      err_av_resxMod_b[imod][ijsum][idt]=0;
	      err_av_resxMod_a[imod][ijsum][idt]=0;
	      av_resyMod_b[imod][ijsum][idt]=0;
	      av_resyMod_a[imod][ijsum][idt]=0;
	      err_av_resyMod_b[imod][ijsum][idt]=0;
	      err_av_resyMod_a[imod][ijsum][idt]=0;

	      av_evMod_b[imod][ijsum][idt]= 1e-3*double(ntrksb->GetEntries())/nDS2;
	      err_av_evMod_b[imod][ijsum][idt]= 0;
	      TH1D *hresx_b = new TH1D("hresx_b","",1e3,-1.,1.);
	      nb->Project("hresx_b","xrec-xmc");
	      av_resxMod_b[imod][ijsum][idt]=1e4*(hresx_b->GetMean());
	      err_av_resxMod_b[imod][ijsum][idt]=1e4*(hresx_b->GetRMS());
	      TH1D *hresy_b = new TH1D("hresy_b","",1e3,-1.,1.);
	      nb->Project("hresy_b","yrec-ymc");
	      av_resyMod_b[imod][ijsum][idt]=1e4*(hresy_b->GetMean());
	      err_av_resyMod_b[imod][ijsum][idt]=1e4*(hresy_b->GetRMS());
	      TFile *f2 = new TFile(name2,"READ");
	      TNtuple *na = (TNtuple*)f2->Get("nhits");
	      //	    ev_mod_a[ipart]= 1e-3*double(na->GetEntries());
	      TH1I *ntrksa = (TH1I*)f2->Get("trks");
	      av_evMod_a[imod][ijsum][idt]= 1e-3*double(ntrksa->GetEntries())/nDS2;
	      err_av_evMod_a[imod][ijsum][idt]= 0;
	      TH1D *hresx_a = new TH1D("hresx_a","",1e3,-1.,1.);
	      na->Project("hresx_a","xrec-xmc");
	      av_resxMod_a[imod][ijsum][idt]=1e4*(hresx_a->GetMean());
	      err_av_resxMod_a[imod][ijsum][idt]=1e4*(hresx_a->GetRMS());
	      TH1D *hresy_a = new TH1D("hresy_a","",1e3,-1.,1.);
	      na->Project("hresy_a","yrec-ymc");
	      //	      resy_mod_a[ipart]= 1e4*(hresy_a->GetMean());
	      av_resyMod_a[imod][ijsum][idt]=1e4*(hresy_a->GetMean());
	      err_av_resyMod_a[imod][ijsum][idt]=1e4*(hresy_a->GetRMS());
	      f1->Close();
	      f2->Close();

		cout<<"iS_AC:"<<iSc<<" ida:"<<iDa<<" dt:"<<idt<<" imod:"<<imod<<" isum:"<<isum<<endl;
		// timerB.Stop();
		// Double_t rtime = timerB.RealTime();
		// Double_t ctime = timerB.CpuTime();
		// printf("for the LOOP: RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	    }//end loop over diff sum values
	  }// end loop over modules (diff plots)	  
	}//end //loop over dt values 
    

	TString out = resname+".root";
	TFile *f = new TFile(out,"RECREATE");

      /// Let's draw now! ---------------------------------------------------------------------
	TCanvas cRes;  
      for(int imod=0;imod<nMod;imod++){// loop over modules (diff plots)
	TGraphErrors *gr_av_b[nuSd+1];
	TGraphErrors *gr_av_a[nuSd+1];
	TMultiGraph *mgr_av_b = new TMultiGraph();
	TMultiGraph *mgr_av_a = new TMultiGraph();
	TGraphErrors *gr_avResx_b[nuSd+1];
	TGraphErrors *gr_avResx_a[nuSd+1];
	TMultiGraph *mgr_avResx_b = new TMultiGraph();
	TMultiGraph *mgr_avResx_a = new TMultiGraph();
	TGraphErrors *gr_avResy_b[nuSd+1];
	TGraphErrors *gr_avResy_a[nuSd+1];
	TMultiGraph *mgr_avResy_b = new TMultiGraph();
	TMultiGraph *mgr_avResy_a = new TMultiGraph();
	TLegend *leg_b = new TLegend(0.91,0.65,0.99,0.98);
	leg_b->SetHeader("BEFORE");
	TLegend *leg_a = new TLegend(0.91,0.65,0.99,0.98);
	leg_a->SetHeader("AFTER");
	//	for(int isum=nSd;isum>0;isum--){//loop over diff stat.samples (plot with diff markers)
	//	for(int isum=nSd;isum>0;isum=isum-10){//loop over diff stat.samples (plot with diff markers)
	for(int ijsum=nuSd;ijsum>0;ijsum--){ //loop over diff stat.samples (plot with diff markers)
	  //	  int isum = nSdstep*ijsum;
	  int isum = 1+nSdstep*(ijsum-1);
	  TString sname = " ";
	  sname +=" sum of ";
	  sname += isum;
	  gr_av_b[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_evMod_b[imod][ijsum],0,err_av_evMod_b[imod][ijsum]);
	  gr_av_a[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_evMod_a[imod][ijsum],0,err_av_evMod_a[imod][ijsum]);
	  gr_avResx_b[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resxMod_b[imod][ijsum],0,err_av_resxMod_b[imod][ijsum]);
	  gr_avResx_a[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resxMod_a[imod][ijsum],0,err_av_resxMod_a[imod][ijsum]);
	  gr_avResx_b[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resxMod_b[imod][ijsum],0,err_av_resxMod_b[imod][ijsum]);
	  gr_avResx_a[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resxMod_a[imod][ijsum],0,err_av_resxMod_a[imod][ijsum]);
	  gr_avResy_b[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resyMod_b[imod][ijsum],0,err_av_resyMod_b[imod][ijsum]);
	  gr_avResy_a[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resyMod_a[imod][ijsum],0,err_av_resyMod_a[imod][ijsum]);
	  gr_avResy_b[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resyMod_b[imod][ijsum],0,err_av_resyMod_b[imod][ijsum]);
	  gr_avResy_a[ijsum] = new TGraphErrors(nDt,v_tr_sc, av_resyMod_a[imod][ijsum],0,err_av_resyMod_a[imod][ijsum]);
	  TString name_ev = "Ev_";
	  name_ev +=imod;
	  name_ev +="mod_sum";
	  name_ev += isum;
	  gr_av_b[ijsum]->SetName(name_ev+"_b");
	  gr_av_b[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_av_b[ijsum]->SetMarkerColor(kGreen-3); 
	  gr_av_b[ijsum]->SetMarkerSize(1.5); 
	  gr_av_a[ijsum]->SetName(name_ev+"_a");
	  gr_av_a[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_av_a[ijsum]->SetMarkerColor(kOrange+7); 
	  gr_av_a[ijsum]->SetMarkerSize(1.5); 
	  mgr_av_b->Add(gr_av_b[ijsum]);
	  mgr_av_a->Add(gr_av_a[ijsum]);
	  gr_av_b[ijsum]->Write();
	  gr_av_a[ijsum]->Write();
	  leg_b->AddEntry(gr_av_b[ijsum],sname,"P");
	  leg_a->AddEntry(gr_av_a[ijsum],sname,"P");

	  gr_avResx_b[ijsum]->SetName(name_ev+"_b");
	  gr_avResx_b[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_avResx_b[ijsum]->SetMarkerColor(kGreen-3); 
	  gr_avResx_b[ijsum]->SetMarkerSize(1.5); 
	  gr_avResx_a[ijsum]->SetName(name_ev+"_a");
	  gr_avResx_a[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_avResx_a[ijsum]->SetMarkerColor(kOrange+7); 
	  gr_avResx_a[ijsum]->SetMarkerSize(1.5); 
	  mgr_avResx_b->Add(gr_avResx_b[ijsum]);
	  mgr_avResx_a->Add(gr_avResx_a[ijsum]);
	  gr_avResx_b[ijsum]->Write();
	  gr_avResx_a[ijsum]->Write();
	  gr_avResy_b[ijsum]->SetName(name_ev+"_b");
	  gr_avResy_b[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_avResy_b[ijsum]->SetMarkerColor(kGreen-3); 
	  gr_avResy_b[ijsum]->SetMarkerSize(1.5); 
	  gr_avResy_a[ijsum]->SetName(name_ev+"_a");
	  gr_avResy_a[ijsum]->SetMarkerStyle(20+ijsum); 
	  gr_avResy_a[ijsum]->SetMarkerColor(kOrange+7); 
	  gr_avResy_a[ijsum]->SetMarkerSize(1.5); 
	  mgr_avResy_b->Add(gr_avResy_b[ijsum]);
	  mgr_avResy_a->Add(gr_avResy_a[ijsum]);
	  gr_avResy_b[ijsum]->Write();
	  gr_avResy_a[ijsum]->Write();

	}//end over diff stat.samples (plot with diff markers)
  
      TString smname = "Module_";
      smname += imod;
      mgr_av_b->SetTitle(smname+"_b");
      mgr_av_a->SetTitle(smname+"_a");
      leg_a->SetFillColor(0);
      leg_b->SetFillColor(0);
      mgr_av_b->SetName(smname+"_b");
      mgr_av_a->SetName(smname+"_a");
      TString smnamex,smnamey;
      smnamex =smname;
      smnamex+="_resX";
      smnamey =smname;
      smnamey+="_resY";
      mgr_avResx_b->SetName(smnamex+"_b");
      mgr_avResx_a->SetName(smnamex+"_a");
      mgr_avResy_b->SetName(smnamey+"_b");
      mgr_avResy_a->SetName(smnamey+"_a");
      mgr_av_b->Draw("AP");
      mgr_av_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_av_b->GetYaxis()->SetTitle("events, 10^{3}");
      leg_b->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_av_a->Draw("AP");
      mgr_av_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_av_a->GetYaxis()->SetTitle("events, 10^{3}");
      leg_a->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_av_a->Write();
      mgr_av_b->Write();
      if(imod<1){
	leg_b->SetName("LegBefore");
	leg_a->SetName("LegAfter");
	leg_b->Write();
	leg_a->Write();
      }

      mgr_avResx_b->Draw("AP");
      mgr_avResx_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_avResx_b->GetYaxis()->SetTitle("res_{X}, mean [#mum]");
      leg_b->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_avResx_a->Draw("AP");
      mgr_avResx_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_avResx_a->GetYaxis()->SetTitle("res_{X}, mean [#mum]");
      leg_a->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_avResx_a->Write();
      mgr_avResx_b->Write();

      mgr_avResy_b->Draw("AP");
      mgr_avResy_b->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_avResy_b->GetYaxis()->SetTitle("res_{Y}, mean [#mum]");
      leg_b->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_avResy_a->Draw("AP");
      mgr_avResy_a->GetXaxis()->SetTitle("#delta_{t}, #mum");
      mgr_avResy_a->GetYaxis()->SetTitle("res_{Y}, mean [#mum]");
      leg_a->Draw();
      cRes.Print(resname_pdf_o); //write canvas and keep the ps file open
      cRes.Clear();
      mgr_avResy_a->Write();
      mgr_avResy_b->Write();
     
      }//end loop over modules (diff plots)
      cRes.Print(resname_pdf_c); //write canvas and close the ps file 
      ///--------------------------------------------------------------------------------------------
      f->Write();
      f->Close();
    }
    timerB.Stop();
    Double_t rtime = timerB.RealTime();
    Double_t ctime = timerB.CpuTime();
    printf("On HIMSTER it took: RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
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
 ModulesStatFast(storePath,daGL,iSc);
 return 0;
}
