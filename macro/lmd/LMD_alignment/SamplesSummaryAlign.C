#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
//void TOTALSummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/tmpOutputAlignTMP/", double tr_sc=200, double rt_sc=9)
int SamplesSummaryAlign(TString pathG="/home/karavdin/datastorage/AlignmentLMDpixel/LargeSamplesStudyMay/BOX/mom_15/", double tr_sc=300, double rt_sc=3)
{
  const int ntrksSample = 2e4;
  const double i_TrksSimi = 100./ntrksSample; //relative to simulated in %
  const int nParDt=8;
  double v_Dt[nParDt]={0,50,100,200,300,400,500,600};
  const int nParDa=5;
  double v_Da[nParDa]={0,1,3,6,9};
  // const int nParDt=1;
  // double v_Dt[nParDt]={600};
 // const int nParDa=1;
 //  double v_Da[nParDa]={0};
  for(int iDt=0;iDt<nParDt;iDt++){
    tr_sc = v_Dt[iDt];
    for(int iDa=0;iDa<nParDa;iDa++){
      rt_sc= v_Da[iDa];
      cout<<"work with dt~"<<tr_sc<<" da~"<<rt_sc<<endl;
      const int nS=10;
      //   const int nS=3;
      double TrksSim[nS];
  //double TrksSim[nS]={10000, 100000, 200000, 400000, 600000, 800000, 1000000};
   //  const int nS=3;
   //  double TrksSim[nS]={1000,5000,10000};

  //  double TrksSim[nS]={10, 100, 200, 300, 400, 500, 700, 1000};
  double TrksBef[nS][nS];
  double TrksAft[nS][nS];
  double TrksDiff[nS][nS];
  //How to save data
  TString resname= pathG+"/ResultsSamplesSummary_";
  resname+=tr_sc;
  resname+="um_";
  resname+=rt_sc;
  resname+="mrad";

  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";

  double mis_b[nS][6][nS];
  double mis_a[nS][6][nS];

  double theta_mean_b[nS][nS], theta_mean_a[nS][nS], theta_mean_r[nS][nS];
  double theta_rms_b[nS][nS], theta_rms_a[nS][nS], theta_rms_r[nS][nS];
  // double theta_mean_b_sec[nS][10][nS], theta_mean_a_sec[nS][10][nS], theta_mean_r_sec[nS][10][nS];
  // double theta_rms_b_sec[nS][10][nS], theta_rms_a_sec[nS][10][nS], theta_rms_r_sec[nS][10][nS];
  TH1D *hthetabefore = new TH1D("hthetabefore","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaafter = new TH1D("hthetaafter","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaref = new TH1D("hthetaref","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);

  for(int ipart=0;ipart<nS;ipart++){
    for(int i=0;i<nS;i++){
      TrksSim[i]=(i+1)*ntrksSample ; //each sample contains ntrksSample events with 1trk/event
   
      theta_mean_b[ipart][i] = 0;
      theta_mean_a[ipart][i] = 0;
      theta_mean_r[ipart][i] = 0;
      theta_rms_b[ipart][i] = 0;
      theta_rms_a[ipart][i] = 0;
      theta_rms_r[ipart][i] = 0;
      // for(int isector=0;isector<10;isector++){
      // 	theta_mean_b_sec[ipart][isector][i] = 0;
      // 	theta_mean_a_sec[ipart][isector][i] = 0;
      // 	theta_mean_r_sec[ipart][isector][i] = 0;
      // 	theta_rms_b_sec[ipart][isector][i] = 0;
      // 	theta_rms_a_sec[ipart][isector][i] =0;
      // 	theta_rms_r_sec[ipart][isector][i] = 0;
      // }
    }
  }

 for(int i=0;i<nS;i++){
   cout<<"Sum of "<<i+1<<"sample(s)"<<endl;
  for(int ipart=0;ipart<nS;ipart++){
    cout<<"part #"<<ipart<<endl;
    int nTrks =  TrksSim[i];
    //Where search for files
    TString path = pathG+"/";
    path+=tr_sc;
    path+="mkm_";
    path+=rt_sc;
    path+="mrad/Sample";
    path +=ipart;
    path+="/results/";
   


/// Theta resolution--------------------------------------------------------------------------------------  
  TString name1 = path + "/Lumi_QA_before.root";
  TString name2 = path + "/Lumi_QA_after.root";
  // TString name2 = path + "/Lumi_QA_after_";
  // name2 +=i;
  // name2 +="SumAlign.root";


  TString name3 = pathG+"/0mkm_0mrad/Sample";
  name3+= ipart;
  name3+="/results/Lumi_QA_before.root";

  //  TH1F *hbeforetmp;   TH1F *haftertmp;
  TH1 *hbefore;   
  TH1 *hafter;
  TH1 *hrefer;
  // TNtuple *nsectors_b;
  // TNtuple *nsectors_a;
  // TNtuple *nsectors_r;

  TFile *f1 = new TFile(name1,"READ");
  if (f1->IsZombie()) {
    std::cout << "!!! Error opening file " <<name1<< std::endl;
    continue;
    //   return;
  }

  hbefore = (TH1*)f1->Get("NearIP/hResTheta");
  //hbefore->Print();
  hbefore->SetName("hResThetaBefore");
  hbefore->SetTitle("#theta resolution");
  //  hbefore->Print();
  TrksBef[ipart][i] = hbefore->GetEntries();
  TrksBef[ipart][i] *=i_TrksSimi;
  // //  if(i>0) TrksBef[ipart][i] += TrksBef[ipart][i-1];
 
  // //  cout<<" Trks["<<ipart<<"]["<<i<<"] = "<<Trks[ipart][i]<<endl;
  // TF1 *funrth_b = new TF1("fitrth_b","gaus",-0.01,0.01);
  // funrth_b->SetParameters(100,0,3e-3);
  // funrth_b->SetParNames("Constant","Mean","Sigma");
  // hbefore->Fit(funrth_b);
  // theta_mean_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Mean"));
  // theta_rms_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Sigma"));
  theta_mean_b[ipart][i]  = 1e6*(hbefore->GetMean());
  theta_rms_b[ipart][i]  = 1e6*(hbefore->GetRMS());
  // nsectors_b = (TNtuple*)f1->Get("nsectors");
  TFile *f2 = new TFile(name2,"READ");
  if (f2->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    continue;
    // return;
  }
  hafter = (TH1F*)f2->Get("NearIP/hResTheta");
  hafter->SetName("hResThetaAfter");
  hafter->SetTitle("#theta resolution");
  // Trks[ipart][i] = hafter->GetEntries();
  // if(i>0) Trks[ipart][i] += Trks[ipart][i-1];
  TrksAft[ipart][i] = hafter->GetEntries();
  TrksAft[ipart][i]  *=i_TrksSimi;
  //  if(i>0) TrksAft[ipart][i] += TrksAft[ipart][i-1];
 
  TrksDiff[ipart][i] = (TrksAft[ipart][i] - TrksBef[ipart][i]);

  // TF1 *funrth_a = new TF1("fitrth_a","gaus",-0.01,0.01);
  // funrth_a->SetParameters(100,0,3e-3);
  // funrth_a->SetParNames("Constant","Mean","Sigma");
  // hafter->Fit(funrth_a);

  // theta_mean_a[ipart][i] = 1e6*(funrth_a->GetParameter("Mean"));
  // theta_rms_a[ipart][i] = 1e6*(funrth_a->GetParameter("Sigma"));
  // //  nsectors_a = (TNtuple*)f2->Get("nsectors");
  theta_mean_a[ipart][i]  = 1e6*(hafter->GetMean());
  theta_rms_a[ipart][i]  = 1e6*(hafter->GetRMS());
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    continue;
    // return;
  }
  hrefer = (TH1F*)f3->Get("NearIP/hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");

  // TF1 *funrth_r = new TF1("fitrth_r","gaus",-0.01,0.01);
  // funrth_r->SetParameters(100,0,3e-3);
  // funrth_r->SetParNames("Constant","Mean","Sigma");
  // hrefer->Fit(funrth_r);
  // theta_mean_r[ipart][i] = 1e6*(funrth_r->GetParameter("Mean"));
  // theta_rms_r[ipart][i] = 1e6*(funrth_r->GetParameter("Sigma"));
  // nsectors_r = (TNtuple*)f3->Get("nsectors");
  theta_mean_r[ipart][i]  = 1e6*(hrefer->GetMean());
  theta_rms_r[ipart][i]  = 1e6*(hrefer->GetRMS());

  // for(int isector=0;isector<10;isector++){
  //   TString cond = "sector==";
  //   cond +=isector; 
  //   nsectors_b->Project("hthetabefore","thetares",cond.Data());
  //   nsectors_a->Project("hthetaafter","thetares",cond.Data());
  //   nsectors_r->Project("hthetaref","thetares",cond.Data());
  //   hthetabefore->Fit(funrth_b);
  //   hthetaafter->Fit(funrth_a);
  //   hthetaref->Fit(funrth_r);
  //   theta_mean_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Mean"));
  //   theta_rms_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Sigma"));
  //   theta_mean_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Mean"));
  //   theta_rms_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Sigma"));
  //   theta_mean_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Mean"));
  //   theta_rms_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Sigma"));
  // }

  /// END (Theta resolution) ---------------------------------------------------------------
  f1->Close();
  f2->Close();
  f3->Close();
 ///Misalignment constants -----------------------------------------------------------------
  TString namemisc = pathG+"/";
  namemisc+=tr_sc;
  namemisc+="mkm_";
  namemisc+=rt_sc;
  namemisc+="mrad/Sum";
  namemisc +=i;
  namemisc+="Samples/KnossosResults.root";
  TFile *fmisc = new TFile(namemisc,"READ");
    if (fmisc->IsZombie()) {
      std::cout << "!!! Error opening file " <<namemisc<< std::endl;
      // return;
    }
    TH2F *hmis_b_0 = (TH2F*)fmisc->Get("mis_before_0");
    mis_b[ipart][0][i] = 1e4*(hmis_b_0->ProjectionY()->GetRMS());
    TH2F *hmis_b_1 = (TH2F*)fmisc->Get("mis_before_1");
    mis_b[ipart][1][i] = 1e4*(hmis_b_1->ProjectionY()->GetRMS());
    TH2F *hmis_b_2 = (TH2F*)fmisc->Get("mis_before_2");
    mis_b[ipart][2][i] = 1e4*(hmis_b_2->ProjectionY()->GetRMS());
    TH2F *hmis_b_3 = (TH2F*)fmisc->Get("mis_before_3");
    mis_b[ipart][3][i] = 1e3*(hmis_b_3->ProjectionY()->GetRMS());
    TH2F *hmis_b_4 = (TH2F*)fmisc->Get("mis_before_4");
    mis_b[ipart][4][i] = 1e3*(hmis_b_4->ProjectionY()->GetRMS());
    TH2F *hmis_b_5 = (TH2F*)fmisc->Get("mis_before_5");
    mis_b[ipart][5][i] = 1e3*(hmis_b_5->ProjectionY()->GetRMS());
    
    TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_diff_0");
    mis_a[ipart][0][i] = 1e4*(hmis_a_0->ProjectionY()->GetRMS());
    TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_diff_1");
    mis_a[ipart][1][i] = 1e4*(hmis_a_1->ProjectionY()->GetRMS());
    TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_diff_2");
    mis_a[ipart][2][i] = 1e4*(hmis_a_2->ProjectionY()->GetRMS());
    TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_diff_3");
    mis_a[ipart][3][i] = 1e3*(hmis_a_3->ProjectionY()->GetRMS());
    TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_diff_4");
    mis_a[ipart][4][i] = 1e3*(hmis_a_4->ProjectionY()->GetRMS());
    TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_diff_5");
    mis_a[ipart][5][i] = 1e3*(hmis_a_5->ProjectionY()->GetRMS());
    /// END (Misalignment constants) -----------------------------------------------------------
    fmisc->Close();
  }
  }
  //return;
  double av_theta_mean_b[nS],av_theta_rms_b[nS];
  double av_theta_mean_a[nS],av_theta_rms_a[nS];
  double av_theta_mean_r[nS],av_theta_rms_r[nS];
  double av_TrksBef[nS];double av_TrksAft[nS];double av_TrksDiff[nS];
  double avEr_theta_mean_b[nS],avEr_theta_rms_b[nS];
  double avEr_theta_mean_a[nS],avEr_theta_rms_a[nS];
  double avEr_theta_mean_r[nS],avEr_theta_rms_r[nS];
  double avEr_TrksBef[nS]; double avEr_TrksAft[nS];double avEr_TrksDiff[nS];

  // double av_theta_mean_b_sec[10][nS],av_theta_rms_b_sec[10][nS];
  // double av_theta_mean_a_sec[10][nS],av_theta_rms_a_sec[10][nS];
  // double av_theta_mean_r_sec[10][nS],av_theta_rms_r_sec[10][nS];
  // double avEr_theta_mean_b_sec[10][nS],avEr_theta_rms_b_sec[10][nS];
  // double avEr_theta_mean_a_sec[10][nS],avEr_theta_rms_a_sec[10][nS];
  // double avEr_theta_mean_r_sec[10][nS],avEr_theta_rms_r_sec[10][nS];
 

  double av_mis_a_mean[6][nS];
  double av_mis_b_mean[6][nS];
  double avEr_mis_a_mean[6][nS];
  double avEr_mis_b_mean[6][nS];

  for(int i=0;i<nS;i++){
    av_theta_mean_b[i] = 0;
    av_theta_rms_b[i] = 0;
    av_theta_mean_a[i] = 0;
    av_theta_rms_a[i] = 0;
    av_theta_mean_r[i] = 0;
    av_theta_rms_r[i] = 0;
    av_TrksBef[i]=0;
    av_TrksAft[i]=0;
    av_TrksDiff[i]=0;
    avEr_theta_mean_b[i] = 0;
    avEr_theta_rms_b[i] =0;
    avEr_theta_mean_a[i] = 0;
    avEr_theta_rms_a[i] = 0;
    avEr_theta_mean_r[i] = 0;
    avEr_theta_rms_r[i] = 0;
    avEr_TrksBef[i] = 0;
    avEr_TrksAft[i] = 0;
    // for(int isector=0;isector<10;isector++){
    //   av_theta_mean_b_sec[isector][i] = 0;
    //   av_theta_rms_b_sec[isector][i] = 0;
    //   av_theta_mean_a_sec[isector][i] = 0;
    //   av_theta_rms_a_sec[isector][i] = 0;
    //   av_theta_mean_r_sec[isector][i] = 0;
    //   av_theta_rms_r_sec[isector][i] = 0;
    //   avEr_theta_mean_b_sec[isector][i] = 0;
    //   avEr_theta_rms_b_sec[isector][i] = 0;
    //   avEr_theta_mean_a_sec[isector][i] = 0;
    //   avEr_theta_rms_a_sec[isector][i] = 0;
    //   avEr_theta_mean_r_sec[isector][i] = 0;
    //   avEr_theta_rms_r_sec[isector][i] = 0; 
    // }
    for(int ialc=0;ialc<6;ialc++){
      av_mis_a_mean[ialc][i] = 0;
      av_mis_b_mean[ialc][i] = 0;
      avEr_mis_a_mean[ialc][i] = 0;
      avEr_mis_b_mean[ialc][i] = 0;
    }
  }
  double i_nS = 1./nS;
  for(int i=0;i<nS;i++){
    for(int ipart=0;ipart<nS;ipart++){
      cout<<"TrksAft["<<ipart<<"]["<<i<<"] = "<<TrksAft[ipart][i]<<" TrksBef["<<ipart<<"]["<<i<<"] = "<<TrksBef[ipart][i]
	  <<" TrksDiff["<<ipart<<"]["<<i<<"] = "<<TrksDiff[ipart][i]<<endl;
      // cout<<"theta_mean_b["<<ipart<<"]["<<i<<"] = "<<theta_mean_b[ipart][i]<<endl;
      // cout<<"theta_mean_a["<<ipart<<"]["<<i<<"] = "<<theta_mean_a[ipart][i]<<endl;
      // cout<<"theta_mean_r["<<ipart<<"]["<<i<<"] = "<<theta_mean_r[ipart][i]<<endl;
      av_theta_mean_b[i]+=theta_mean_b[ipart][i];
      av_theta_mean_a[i]+=theta_mean_a[ipart][i];
      av_theta_mean_r[i]+=theta_mean_r[ipart][i];
      av_theta_rms_b[i]+=theta_rms_b[ipart][i];
      av_theta_rms_a[i]+=theta_rms_a[ipart][i];
      av_theta_rms_r[i]+=theta_rms_r[ipart][i];
      av_TrksBef[i]+=TrksBef[ipart][i];
      av_TrksAft[i]+=TrksAft[ipart][i];
      av_TrksDiff[i]+=TrksDiff[ipart][i];
      //   cout<<" av_Trks["<<i<<"]="<<av_Trks[i]<<endl;
      //      cout<<"av_theta_mean_r["<<i<<"]="<<av_theta_mean_r[i]<<endl;
      // for(int isector=0;isector<10;isector++){
      // 	av_theta_mean_b_sec[isector][i]+=theta_mean_b_sec[ipart][isector][i];
      // 	av_theta_mean_a_sec[isector][i]+=theta_mean_a_sec[ipart][isector][i];
      // 	av_theta_mean_r_sec[isector][i]+=theta_mean_r_sec[ipart][isector][i];
      // 	av_theta_rms_b_sec[isector][i]+=theta_rms_b_sec[ipart][isector][i];
      // 	av_theta_rms_a_sec[isector][i]+=theta_rms_a_sec[ipart][isector][i];
      // 	av_theta_rms_r_sec[isector][i]+=theta_rms_r_sec[ipart][isector][i];
      // }
      for(int ialc=0;ialc<6;ialc++){
      av_mis_a_mean[ialc][i] += mis_a[ipart][ialc][i];
      av_mis_b_mean[ialc][i] += mis_b[ipart][ialc][i];
      }
    }
   
    av_theta_mean_b[i] *=i_nS;
    av_theta_mean_a[i] *=i_nS;
    av_theta_mean_r[i] *=i_nS;
    av_theta_rms_b[i] *=i_nS;
    av_theta_rms_a[i] *=i_nS;
    av_theta_rms_r[i] *=i_nS;
    av_TrksBef[i]*=i_nS;
    av_TrksAft[i]*=i_nS;
    av_TrksDiff[i]*=i_nS;
    // for(int isector=0;isector<10;isector++){
    //   av_theta_mean_b_sec[isector][i] *=i_nS;
    //   av_theta_mean_a_sec[isector][i] *=i_nS;
    //   av_theta_mean_r_sec[isector][i] *=i_nS;
    //   av_theta_rms_b_sec[isector][i] *=i_nS;
    //   av_theta_rms_a_sec[isector][i] *=i_nS;
    //   av_theta_rms_r_sec[isector][i] *=i_nS;
    // }
    for(int ialc=0;ialc<6;ialc++){
      av_mis_a_mean[ialc][i] *= i_nS;
      av_mis_b_mean[ialc][i] *= i_nS;
      }
  }


  double i_nS_r = 1./(nS-1);
  for(int i=0;i<nS;i++){
    for(int ipart=0;ipart<nS;ipart++){
      avEr_theta_mean_b[i]+=(theta_mean_b[ipart][i]-av_theta_mean_b[i])*(theta_mean_b[ipart][i]-av_theta_mean_b[i]);
      avEr_theta_mean_a[i]+=(theta_mean_a[ipart][i]-av_theta_mean_a[i])*(theta_mean_a[ipart][i]-av_theta_mean_a[i]);
      avEr_theta_mean_r[i]+=(theta_mean_r[ipart][i]-av_theta_mean_r[i])*(theta_mean_r[ipart][i]-av_theta_mean_r[i]);
      avEr_theta_rms_b[i]+=(theta_rms_b[ipart][i]-av_theta_rms_b[i])*(theta_rms_b[ipart][i]-av_theta_rms_b[i]);
      avEr_theta_rms_a[i]+=(theta_rms_a[ipart][i]-av_theta_rms_a[i])*(theta_rms_a[ipart][i]-av_theta_rms_a[i]);
      avEr_theta_rms_r[i]+=(theta_rms_r[ipart][i]-av_theta_rms_r[i])*(theta_rms_r[ipart][i]-av_theta_rms_r[i]);
      avEr_TrksBef[i]+=(TrksBef[ipart][i]-av_TrksBef[i])*(TrksBef[ipart][i]-av_TrksBef[i]);
      avEr_TrksAft[i]+=(TrksAft[ipart][i]-av_TrksAft[i])*(TrksAft[ipart][i]-av_TrksAft[i]);
      avEr_TrksDiff[i]+=(TrksDiff[ipart][i]-av_TrksDiff[i])*(TrksDiff[ipart][i]-av_TrksDiff[i]);
      // for(int isector=0;isector<10;isector++){
      // 	avEr_theta_mean_b_sec[isector][i]+=(theta_mean_b_sec[ipart][isector][i]-av_theta_mean_b_sec[isector][i])*(theta_mean_b_sec[ipart][isector][i]-av_theta_mean_b_sec[isector][i]);
      // 	avEr_theta_mean_a_sec[isector][i]+=(theta_mean_a_sec[ipart][isector][i]-av_theta_mean_a_sec[isector][i])*(theta_mean_a_sec[ipart][isector][i]-av_theta_mean_a_sec[isector][i]);
      // 	avEr_theta_mean_r_sec[isector][i]+=(theta_mean_r_sec[ipart][isector][i]-av_theta_mean_r_sec[isector][i])*(theta_mean_r_sec[ipart][isector][i]-av_theta_mean_r_sec[isector][i]);
      // 	avEr_theta_rms_b_sec[isector][i]+=(theta_rms_b_sec[ipart][isector][i]-av_theta_rms_b_sec[isector][i])*(theta_rms_b_sec[ipart][isector][i]-av_theta_rms_b_sec[isector][i]);
      // 	avEr_theta_rms_a_sec[isector][i]+=(theta_rms_a_sec[ipart][isector][i]-av_theta_rms_a_sec[isector][i])*(theta_rms_a_sec[ipart][isector][i]-av_theta_rms_a_sec[isector][i]);
      // 	avEr_theta_rms_r_sec[isector][i]+=(theta_rms_r_sec[ipart][isector][i]-av_theta_rms_r_sec[isector][i])*(theta_rms_r_sec[ipart][isector][i]-av_theta_rms_r_sec[isector][i]);
      // }
      for(int ialc=0;ialc<6;ialc++){
	avEr_mis_a_mean[ialc][i] +=(mis_a[ipart][ialc][i]-av_mis_a_mean[ialc][i])*(mis_a[ipart][ialc][i]-av_mis_a_mean[ialc][i]);
	avEr_mis_b_mean[ialc][i] +=(mis_b[ipart][ialc][i]-av_mis_b_mean[ialc][i])*(mis_b[ipart][ialc][i]-av_mis_b_mean[ialc][i]);
      }
    }
    avEr_theta_mean_b[i] *=i_nS_r;
    avEr_theta_mean_a[i] *=i_nS_r;
    avEr_theta_mean_r[i] *=i_nS_r;
    avEr_theta_rms_b[i] *=i_nS_r;
    avEr_theta_rms_a[i] *=i_nS_r;
    avEr_theta_rms_r[i] *=i_nS_r;
    avEr_TrksBef[i] *= i_nS_r;
    avEr_TrksAft[i] *= i_nS_r;
    avEr_TrksDiff[i] *= i_nS_r;
    avEr_theta_mean_b[i]  = sqrt(avEr_theta_mean_b[i]);
    avEr_theta_mean_a[i]  = sqrt(avEr_theta_mean_a[i]);
    avEr_theta_mean_r[i] = sqrt(avEr_theta_mean_r[i]);
    avEr_theta_rms_b[i] = sqrt(avEr_theta_rms_b[i]);
    avEr_theta_rms_a[i] = sqrt(avEr_theta_rms_a[i]);
    avEr_theta_rms_r[i] = sqrt(avEr_theta_rms_r[i]);
    avEr_TrksBef[i] = sqrt(avEr_TrksBef[i]);
    avEr_TrksAft[i] = sqrt(avEr_TrksAft[i]);
    avEr_TrksDiff[i] = sqrt(avEr_TrksDiff[i]);
    //    for(int ipart=0;ipart<nS;ipart++){
    //   for(int isector=0;isector<10;isector++){
    //   avEr_theta_mean_b_sec[isector][i] *=i_nS_r;
    //   avEr_theta_mean_a_sec[isector][i] *=i_nS_r;
    //   avEr_theta_mean_r_sec[isector][i] *=i_nS_r;
    //   avEr_theta_rms_b_sec[isector][i] *=i_nS_r;
    //   avEr_theta_rms_a_sec[isector][i] *=i_nS_r;
    //   avEr_theta_rms_r_sec[isector][i] *=i_nS_r;
    //   avEr_theta_mean_b_sec[isector][i] = sqrt(avEr_theta_mean_b_sec[isector][i]);
    //   avEr_theta_mean_a_sec[isector][i] = sqrt(avEr_theta_mean_a_sec[isector][i]);
    //   avEr_theta_mean_r_sec[isector][i] = sqrt(avEr_theta_mean_r_sec[isector][i]);
    //   avEr_theta_rms_b_sec[isector][i] = sqrt(avEr_theta_rms_b_sec[isector][i]);
    //   avEr_theta_rms_a_sec[isector][i] = sqrt(avEr_theta_rms_a_sec[isector][i]);
    //   avEr_theta_rms_r_sec[isector][i] = sqrt(avEr_theta_rms_r_sec[isector][i]);
    // }
    // cout<<"av_Trks["<<i<<"]="<<av_Trks[i]<<" avEr_Trks["<<i<<"]="<<avEr_Trks[i]<<endl;
    // cout<<"av_theta_mean_r["<<i<<"]="<<av_theta_mean_r[i]<<endl;
    for(int ialc=0;ialc<6;ialc++){
	avEr_mis_a_mean[ialc][i] *=i_nS_r;
	avEr_mis_b_mean[ialc][i] *=i_nS_r;
	avEr_mis_a_mean[ialc][i] = sqrt(avEr_mis_a_mean[ialc][i]);
	avEr_mis_b_mean[ialc][i] = sqrt(avEr_mis_b_mean[ialc][i]);
      }
  }


  TCanvas c1;  
  // cout<<"av_Trks = "<<av_Trks<<endl;
    //put together theta resolution results
  TGraphErrors *gr_theta_b = new TGraphErrors(nS,TrksSim, av_theta_mean_b,0, avEr_theta_mean_b);
  gr_theta_b->SetMarkerStyle(20); 
  gr_theta_b->SetMarkerColor(kGreen-3); 
  gr_theta_b->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_a = new TGraphErrors(nS,TrksSim, av_theta_mean_a,0, avEr_theta_mean_a);
  gr_theta_a->SetMarkerStyle(29); 
  gr_theta_a->SetMarkerColor(kOrange+7); 
  gr_theta_a->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_r = new TGraphErrors(nS,TrksSim, av_theta_mean_r,0, avEr_theta_mean_r);
  gr_theta_r->SetMarkerStyle(21); 
  gr_theta_r->SetMarkerColor(15); 
  gr_theta_r->SetMarkerSize(2.5); 


  TMultiGraph *mgr_theta = new TMultiGraph();
  //  mgr_theta->Add(gr_theta_b);
  mgr_theta->Add(gr_theta_r);
  mgr_theta->Add(gr_theta_a);

  mgr_theta->Draw("AP");
  mgr_theta->GetXaxis()->SetTitle("Average number of rec. trks");
  mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TGraphErrors *gr_theta_rms_b = new TGraphErrors(nS,TrksSim,av_theta_rms_b,0,avEr_theta_rms_b);
  gr_theta_rms_b->SetMarkerStyle(20); 
  gr_theta_rms_b->SetMarkerColor(kGreen-3); 
  gr_theta_rms_b->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_a = new TGraphErrors(nS,TrksSim,av_theta_rms_a,0,avEr_theta_rms_a);
  gr_theta_rms_a->SetMarkerStyle(29); 
  gr_theta_rms_a->SetMarkerColor(kOrange+7); 
  gr_theta_rms_a->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_r = new TGraphErrors(nS,TrksSim,av_theta_rms_r,0,avEr_theta_rms_a);
  gr_theta_rms_r->SetMarkerStyle(21); 
  gr_theta_rms_r->SetMarkerColor(15); 
  gr_theta_rms_r->SetMarkerSize(2.5); 
  TMultiGraph *mgr_theta_rms = new TMultiGraph();
  //  mgr_theta_rms->Add(gr_theta_rms_b);
  mgr_theta_rms->Add(gr_theta_rms_r);
  mgr_theta_rms->Add(gr_theta_rms_a);
  mgr_theta_rms->Draw("AP");
  mgr_theta_rms->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  mgr_theta_rms->GetYaxis()->SetTitle("#theta_{#sigma}, #murad");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();
 // for(int isector=0;isector<10;isector++){
 //   //put together theta resolution results
 //   TGraphErrors *gr_theta_b_sec = new TGraphErrors(nS,TrksSim, av_theta_mean_b_sec[isector],0, avEr_theta_mean_b_sec[isector]);
 //   gr_theta_b_sec->SetMarkerStyle(20); 
 //   gr_theta_b_sec->SetMarkerColor(kGreen-3); 
 //   gr_theta_b_sec->SetMarkerSize(2.5); 
 //   TGraphErrors *gr_theta_a_sec = new TGraphErrors(nS,TrksSim, av_theta_mean_a_sec[isector],0, avEr_theta_mean_a_sec[isector]);
 //   gr_theta_a_sec->SetMarkerStyle(29); 
 //   gr_theta_a_sec->SetMarkerColor(kOrange+7); 
 //   gr_theta_a_sec->SetMarkerSize(2.5); 
 //   TGraphErrors *gr_theta_r_sec = new TGraphErrors(nS,TrksSim, av_theta_mean_r_sec[isector],0, avEr_theta_mean_r_sec[isector]);
 //   gr_theta_r_sec->SetMarkerStyle(21); 
 //   gr_theta_r_sec->SetMarkerColor(15); 
 //   gr_theta_r_sec->SetMarkerSize(2.5); 

 //  TMultiGraph *mgr_theta_sec = new TMultiGraph();
 //  //  mgr_theta_sec->Add(gr_theta_b_sec);
 //  mgr_theta_sec->Add(gr_theta_r_sec);
 //  mgr_theta_sec->Add(gr_theta_a_sec);
 //  TString titlemean = "#theta_{mean}, #murad [sector No.";
 //  titlemean+=isector;
 //  titlemean +="]";
 //  mgr_theta_sec->Draw("AP");
 //  mgr_theta_sec->GetXaxis()->SetTitle("Average number of rec. trks ");
 //  mgr_theta_sec->GetYaxis()->SetTitle(titlemean.Data());
 //  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  c1.Clear();

 // TGraphErrors *gr_theta_rms_b_sec = new TGraphErrors(nS,TrksSim,av_theta_rms_b_sec[isector],0,avEr_theta_rms_b_sec[isector]);
 //  gr_theta_rms_b_sec->SetMarkerStyle(20); 
 //  gr_theta_rms_b_sec->SetMarkerColor(kGreen-3); 
 //  gr_theta_rms_b_sec->SetMarkerSize(2.5); 
 //  TGraphErrors *gr_theta_rms_a_sec = new TGraphErrors(nS,TrksSim,av_theta_rms_a_sec[isector],0,avEr_theta_rms_a_sec[isector]);
 //  gr_theta_rms_a_sec->SetMarkerStyle(29); 
 //  gr_theta_rms_a_sec->SetMarkerColor(kOrange+7); 
 //  gr_theta_rms_a_sec->SetMarkerSize(2.5); 
 //  TGraphErrors *gr_theta_rms_r_sec = new TGraphErrors(nS,TrksSim,av_theta_rms_r_sec[isector],0,avEr_theta_rms_a_sec[isector]);
 //  gr_theta_rms_r_sec->SetMarkerStyle(21); 
 //  gr_theta_rms_r_sec->SetMarkerColor(15); 
 //  gr_theta_rms_r_sec->SetMarkerSize(2.5); 
 //  TMultiGraph *mgr_theta_rms_sec = new TMultiGraph();
 //  //  mgr_theta_rms_sec->Add(gr_theta_rms_b_sec);
 //  mgr_theta_rms_sec->Add(gr_theta_rms_r_sec);
 //  mgr_theta_rms_sec->Add(gr_theta_rms_a_sec);
 //  mgr_theta_rms_sec->Draw("AP");
 //  mgr_theta_rms_sec->GetXaxis()->SetTitle("Average number of rec. trks ");
 //  TString titlerms = "#theta_{#sigma}, #murad [sector No.";
 //  titlerms+=isector;
 //  titlerms +="]";
 //  mgr_theta_rms_sec->GetYaxis()->SetTitle(titlerms.Data());
 //  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 //  c1.Clear();
 // }

 //put together missalignment consts results
 TGraphErrors *gr_mis_b[6];
 TGraphErrors *gr_mis_a[6];
 TMultiGraph *mgr_mis[6];
 for(int ialc=0;ialc<6;ialc++){
   gr_mis_b[ialc] = new TGraphErrors(nS,TrksSim,av_mis_b_mean[ialc],0,avEr_mis_b_mean[ialc]);
   gr_mis_b[ialc]->SetMarkerStyle(20);
   gr_mis_b[ialc]->SetMarkerColor(kGreen-3);
   gr_mis_b[ialc]->SetMarkerSize(2.5);
   gr_mis_a[ialc] = new TGraphErrors(nS,TrksSim,av_mis_a_mean[ialc],0,avEr_mis_a_mean[ialc]);
   gr_mis_a[ialc]->SetMarkerStyle(29);
   gr_mis_a[ialc]->SetMarkerColor(kOrange+7);
   gr_mis_a[ialc]->SetMarkerSize(2.5);
   mgr_mis[ialc] = new TMultiGraph();
   mgr_mis[ialc]->Add(gr_mis_b[ialc]);
   mgr_mis[ialc]->Add(gr_mis_a[ialc]);
   mgr_mis[ialc]->Draw("AP");
   mgr_mis[ialc]->GetXaxis()->SetTitle("Average number of rec. trks ");
   if(ialc==0) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{x}, #mum");
   if(ialc==1) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{y}, #mum");
   if(ialc==2) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{z}, #mum");
   if(ialc==3) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#alpha}, mrad");
   if(ialc==4) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#beta}, mrad");
   if(ialc==5) mgr_mis[ialc]->GetYaxis()->SetTitle("#Delta_{#gamma}, mrad");
   if(ialc<2 || ialc==5) c1.Print(resname_pdf_o); //write canvas and keep the file open
    c1.Clear();
 }
 TMultiGraph *mgr_stat = new TMultiGraph();
 TGraphErrors *grstatBef = new TGraphErrors(nS,TrksSim,av_TrksBef,0,avEr_TrksBef);
  grstatBef->SetMarkerStyle(24);
  grstatBef->SetMarkerSize(2.5);
  TGraphErrors *grstatAft = new TGraphErrors(nS,TrksSim,av_TrksAft,0,avEr_TrksAft);
  grstatAft->SetMarkerStyle(22);
  grstatAft->SetMarkerSize(2.5);
  grstatAft->SetTitle("Average number of rec. trks (empty = before Knossos; filled = after Knossos)");
  mgr_stat->Add(grstatBef);
  mgr_stat->Add(grstatAft);
  mgr_stat->Draw("AP");
  mgr_stat->GetXaxis()->SetTitle("Number of sim. trks");
  mgr_stat->GetYaxis()->SetTitle("N_{REC}/N_{SIM}, % [empty = N^{ before}_{REC}; filled = N^{ after}_{REC}]");
  //mgr_stat->SetTitle("Average number of rec. trks (empty = before Knossos; filled = after Knossos)");
  c1.Print(resname_pdf_o); //write canvas and keep the file open
  c1.Clear();
  TGraphErrors *grstatDiff = new TGraphErrors(nS,TrksSim,av_TrksDiff,0,avEr_TrksDiff);
  grstatDiff->SetMarkerStyle(22);
  grstatDiff->SetMarkerSize(2.5);
  grstatDiff->Draw("ALP");
  grstatDiff->GetXaxis()->SetTitle("Number of sim. trks");
  grstatDiff->GetYaxis()->SetTitle("(N^{ after}_{REC} - N^{ before}_{REC})/N_{SIM}, %");
  //  grstatDiff->SetTitle("Diff number of rec. trks (after - before), %");
  c1.Print(resname_pdf_c); //write canvas and close ps file
  c1.Close();
  TString out = resname+".root";
  TFile *f = new TFile(out,"RECREATE");
  // for (int g=0;g<6;g++){
  //   gr_mis_b[g]->Write();
  //   gr_mis_a[g]->Write();
  //   mgr_mis[g]->Write();
  // }
  gr_theta_b->Write();
  gr_theta_r->Write();
  gr_theta_a->Write();
  mgr_theta->Write();
  gr_theta_rms_b->Write();
  gr_theta_rms_r->Write();
  gr_theta_rms_a->Write();
  mgr_theta_rms->Write();

 for(int ialc=0;ialc<6;ialc++){
   gr_mis_b[ialc]->Write();
   gr_mis_a[ialc]->Write();
   mgr_mis[ialc]->Write();
 }
 mgr_stat->Write();
 grstatDiff->Write();
  f->Write();
  f->Close();
    }
  }
  return 0;
}
