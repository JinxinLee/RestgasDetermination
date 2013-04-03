#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TCanvas.h>

#include <sstream>
using namespace std;
//void TOTALSummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/tmpOutputAlignTMP/", double tr_sc=200, double rt_sc=9)
void SamplesSummaryAlign(TString pathG="/panda/pandaroot/macro/lmd/tmpOutputAlignTMPSamples/", double tr_sc=0, double rt_sc=0, const int nSamples=2)
{
  const int nS=8;
  double TrksSim[nS]={10000, 100000, 200000, 300000, 400000, 500000, 700000, 1000000};
  //  double TrksSim[nS]={10, 100, 200, 300, 400, 500, 700, 1000};
  double Trks[nSamples][nS];
  //How to save data
  TString resname= pathG+"/ResultsSamplesSummary_";
  resname+=tr_sc;
  resname+="um_";
  resname+=rt_sc;
  resname+="mrad";

  TString resname_pdf = resname+".pdf";
  TString resname_pdf_o = resname_pdf+"(";
  TString resname_pdf_c = resname_pdf+")";

  double mis_b[nSamples][6][nS];
  double mis_a[nSamples][6][nS];

  double theta_mean_b[nSamples][nS], theta_mean_a[nSamples][nS], theta_mean_r[nSamples][nS];
  double theta_rms_b[nSamples][nS], theta_rms_a[nSamples][nS], theta_rms_r[nSamples][nS];
  double theta_mean_b_sec[nSamples][10][nS], theta_mean_a_sec[nSamples][10][nS], theta_mean_r_sec[nSamples][10][nS];
  double theta_rms_b_sec[nSamples][10][nS], theta_rms_a_sec[nSamples][10][nS], theta_rms_r_sec[nSamples][10][nS];
  TH1D *hthetabefore = new TH1D("hthetabefore","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaafter = new TH1D("hthetaafter","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);
  TH1D *hthetaref = new TH1D("hthetaref","#theta_{MC}-#theta_{rec};#delta#theta,rad",1e2,-1e-3,1e-3);

  for(int ipart=0;ipart<nSamples;ipart++){
    for(int i=0;i<nS;i++){
      theta_mean_b[ipart][i] = 0;
      theta_mean_a[ipart][i] = 0;
      theta_mean_r[ipart][i] = 0;
      theta_rms_b[ipart][i] = 0;
      theta_rms_a[ipart][i] = 0;
      theta_rms_r[ipart][i] = 0;
      for(int isector=0;isector<10;isector++){
	theta_mean_b_sec[ipart][isector][i] = 0;
	theta_mean_a_sec[ipart][isector][i] = 0;
	theta_mean_r_sec[ipart][isector][i] = 0;
	theta_rms_b_sec[ipart][isector][i] = 0;
	theta_rms_a_sec[ipart][isector][i] =0;
	theta_rms_r_sec[ipart][isector][i] = 0;
      }
    }
  }

  for(int ipart=0;ipart<nSamples;ipart++){
  for(int i=0;i<nS;i++){
    int nTrks =  TrksSim[i];
    //Where search for files
    TString path = pathG+"/Sample";
    path +=ipart;
    path+="/results_";
    path+=tr_sc;
    path+="mkm_";
    path+=rt_sc;
    path+="mrad_";
    path+=nTrks;
    path+="events/";

  //   // ///Residuals files for estimate of trks/sector ------------------------------------------------------------------------------
  TString nameresb0 = path + "/hitsRes_before_alignment_sector0.root";
  TString nameresb1 = path + "/hitsRes_before_alignment_sector1.root";
  TString nameresb2 = path + "/hitsRes_before_alignment_sector2.root";
  TString nameresb3 = path + "/hitsRes_before_alignment_sector3.root";
  TString nameresb4 = path + "/hitsRes_before_alignment_sector4.root";
  TString nameresb5 = path + "/hitsRes_before_alignment_sector5.root";
  TString nameresb6 = path + "/hitsRes_before_alignment_sector6.root";
  TString nameresb7 = path + "/hitsRes_before_alignment_sector7.root";
  TString nameresb8 = path + "/hitsRes_before_alignment_sector8.root";
  TString nameresb9 = path + "/hitsRes_before_alignment_sector9.root";
  TFile *fresb0 = new TFile(nameresb0,"READ");
  if (fresb0->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb0<< std::endl;
    // return;
  }
  TFile *fresb1 = new TFile(nameresb1,"READ");
  if (fresb1->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb1<< std::endl;
    // return;
  }
 TFile *fresb2 = new TFile(nameresb2,"READ");
  if (fresb2->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb2<< std::endl;
    // return;
  }
 TFile *fresb3 = new TFile(nameresb3,"READ");
  if (fresb3->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb3<< std::endl;
    // return;
  }
  TFile *fresb4 = new TFile(nameresb4,"READ");
  if (fresb4->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb4<< std::endl;
    // return;
  }
 TFile *fresb5 = new TFile(nameresb5,"READ");
  if (fresb5->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb5<< std::endl;
    // return;
  }
 TFile *fresb6 = new TFile(nameresb5,"READ");
  if (fresb6->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb6<< std::endl;
    // return;
  }
  TFile *fresb7 = new TFile(nameresb7,"READ");
  if (fresb7->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb7<< std::endl;
    // return;
  }
 TFile *fresb8 = new TFile(nameresb8,"READ");
  if (fresb8->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb8<< std::endl;
    // return;
  }
  TFile *fresb9 = new TFile(nameresb9,"READ");
  if (fresb9->IsZombie()) {
    std::cout << "!!! Error opening file " <<nameresb9<< std::endl;
    // return;
  }
  int trksCur = 0;
  TNtuple* hntuple0 = (TNtuple*)fresb0->Get("nhits");
   trksCur+= 0.25*(hntuple0->GetEntries());
  TNtuple* hntuple1 = (TNtuple*)fresb1->Get("nhits");
  trksCur+= 0.25*(hntuple1->GetEntries());
  TNtuple* hntuple2 = (TNtuple*)fresb2->Get("nhits");
  trksCur+= 0.25*(hntuple2->GetEntries());
  TNtuple* hntuple3 = (TNtuple*)fresb3->Get("nhits");
  trksCur+= 0.25*(hntuple3->GetEntries());
  TNtuple* hntuple4 = (TNtuple*)fresb4->Get("nhits");
  trksCur+= 0.25*(hntuple4->GetEntries());
  TNtuple* hntuple5 = (TNtuple*)fresb5->Get("nhits");
  trksCur+= 0.25*(hntuple5->GetEntries());
  TNtuple* hntuple6 = (TNtuple*)fresb6->Get("nhits");
  trksCur+= 0.25*(hntuple6->GetEntries());
  TNtuple* hntuple7 = (TNtuple*)fresb7->Get("nhits");
  trksCur+= 0.25*(hntuple7->GetEntries());
  TNtuple* hntuple8 = (TNtuple*)fresb8->Get("nhits");
  trksCur+= 0.25*(hntuple8->GetEntries());
  TNtuple* hntuple9 = (TNtuple*)fresb9->Get("nhits");
  trksCur+= 0.25*(hntuple9->GetEntries());
  Trks[ipart][i] = int(trksCur*0.1);
  
    // ///Misalignment constants -----------------------------------------------------------------
    // TString namemisc = path + "/KnossosResults.root";
    // TFile *fmisc = new TFile(namemisc,"READ");
    // if (fmisc->IsZombie()) {
    //   std::cout << "!!! Error opening file " <<namemisc<< std::endl;
    //   // return;
    // }
    // TH2F *hmis_b_0 = (TH2F*)fmisc->Get("mis_before_0");
    // mis_b[ipart][0][i] = 1e4*(hmis_b_0->ProjectionY()->GetRMS());
    // TH2F *hmis_b_1 = (TH2F*)fmisc->Get("mis_before_1");
    // mis_b[ipart][1][i] = 1e4*(hmis_b_1->ProjectionY()->GetRMS());
    // TH2F *hmis_b_2 = (TH2F*)fmisc->Get("mis_before_2");
    // mis_b[ipart][2][i] = 1e4*(hmis_b_2->ProjectionY()->GetRMS());
    // TH2F *hmis_b_3 = (TH2F*)fmisc->Get("mis_before_3");
    // mis_b[ipart][3][i] = 1e3*(hmis_b_3->ProjectionY()->GetRMS());
    // TH2F *hmis_b_4 = (TH2F*)fmisc->Get("mis_before_4");
    // mis_b[ipart][4][i] = 1e3*(hmis_b_4->ProjectionY()->GetRMS());
    // TH2F *hmis_b_5 = (TH2F*)fmisc->Get("mis_before_5");
    // mis_b[ipart][5][i] = 1e3*(hmis_b_5->ProjectionY()->GetRMS());
    
    // TH2F *hmis_a_0 = (TH2F*)fmisc->Get("mis_diff_0");
    // mis_a[ipart][0][i] = 1e4*(hmis_a_0->ProjectionY()->GetRMS());
    // TH2F *hmis_a_1 = (TH2F*)fmisc->Get("mis_diff_1");
    // mis_a[ipart][1][i] = 1e4*(hmis_a_1->ProjectionY()->GetRMS());
    // TH2F *hmis_a_2 = (TH2F*)fmisc->Get("mis_diff_2");
    // mis_a[ipart][2][i] = 1e4*(hmis_a_2->ProjectionY()->GetRMS());
    // TH2F *hmis_a_3 = (TH2F*)fmisc->Get("mis_diff_3");
    // mis_a[ipart][3][i] = 1e3*(hmis_a_3->ProjectionY()->GetRMS());
    // TH2F *hmis_a_4 = (TH2F*)fmisc->Get("mis_diff_4");
    // mis_a[ipart][4][i] = 1e3*(hmis_a_4->ProjectionY()->GetRMS());
    // TH2F *hmis_a_5 = (TH2F*)fmisc->Get("mis_diff_5");
    // mis_a[ipart][5][i] = 1e3*(hmis_a_5->ProjectionY()->GetRMS());
    // /// END (Misalignment constants) -----------------------------------------------------------



/// Theta resolution--------------------------------------------------------------------------------------  
  TString name1 = path + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name1+=tr_sc;
  name1+="_da_";
  name1+=rt_sc;
  name1+="_before.root";

  TString name2 = path + "/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_";
  name2+=tr_sc;
  name2+="_da_";
  name2+=rt_sc;
  name2+="_after.root";

  TString name3 = pathG+"/Sample";
  name3+= ipart;
  name3+="/results_0mkm_0mrad_";
  name3+=nTrks;
  name3+="events/Lumi_out_MC_and_REC_trks_matches_with_IDs0_dr_0_da_0_before.root";

  //  TH1F *hbeforetmp;   TH1F *haftertmp;
  TH1F *hbefore;   
  TH1F *hafter;
  TH1F *hrefer;


  TFile *f1 = new TFile(name1,"READ");
  if (f1->IsZombie()) {
    std::cout << "!!! Error opening file " <<name1<< std::endl;
    //   return;
  }

  hbefore = (TH1F*)f1->Get("hResTheta");
  hbefore->SetName("hResThetaBefore");
  hbefore->SetTitle("#theta resolution");
  //  hbefore->Print();
  TF1 *funrth_b = new TF1("fitrth_b","gaus",-0.01,0.01);
  funrth_b->SetParameters(100,0,3e-3);
  funrth_b->SetParNames("Constant","Mean","Sigma");
  hbefore->Fit(funrth_b);
  theta_mean_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Mean"));
  theta_rms_b[ipart][i]  = 1e6*(funrth_b->GetParameter("Sigma"));
 
  TFile *f2 = new TFile(name2,"READ");
  if (f2->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }

  hafter = (TH1F*)f2->Get("hResTheta");
  hafter->SetName("hResThetaAfter");
  hafter->SetTitle("#theta resolution");
  
  TF1 *funrth_a = new TF1("fitrth_a","gaus",-0.01,0.01);
  funrth_a->SetParameters(100,0,3e-3);
  funrth_a->SetParNames("Constant","Mean","Sigma");
  hafter->Fit(funrth_a);

  theta_mean_a[ipart][i] = 1e6*(funrth_a->GetParameter("Mean"));
  theta_rms_a[ipart][i] = 1e6*(funrth_a->GetParameter("Sigma"));
   
  TFile *f3 = new TFile(name3,"READ");
  if (f3->IsZombie()) {
    std::cout << "!!! Error opening file " <<name2<< std::endl;
    // return;
  }

  hrefer = (TH1F*)f3->Get("hResTheta");
  hrefer->SetName("hResThetaRef");
  hrefer->SetTitle("#theta resolution");

  TF1 *funrth_r = new TF1("fitrth_r","gaus",-0.01,0.01);
  funrth_r->SetParameters(100,0,3e-3);
  funrth_r->SetParNames("Constant","Mean","Sigma");
  hrefer->Fit(funrth_r);
  theta_mean_r[ipart][i] = 1e6*(funrth_r->GetParameter("Mean"));
  theta_rms_r[ipart][i] = 1e6*(funrth_r->GetParameter("Sigma"));

 
  for(int isector=0;isector<10;isector++){
    TNtuple *nsectors_b = (TNtuple*)f1->Get("nsectors");
    TNtuple *nsectors_a = (TNtuple*)f2->Get("nsectors");
    TNtuple *nsectors_r = (TNtuple*)f3->Get("nsectors");
    // TTree *nsectors_b = (TNtuple*)f1->Get("nsectors");
    // TTree *nsectors_a = (TNtuple*)f2->Get("nsectors");
    // TTree *nsectors_r = (TNtuple*)f3->Get("nsectors");
    TString cond = "sector==";
    cond +=isector; 
    nsectors_b->Project("hthetabefore","thetares",cond.Data());
    nsectors_a->Project("hthetaafter","thetares",cond.Data());
    nsectors_r->Project("hthetaref","thetares",cond.Data());
    hthetabefore->Fit(funrth_b);
    hthetaafter->Fit(funrth_a);
    hthetaref->Fit(funrth_r);
    theta_mean_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Mean"));
    theta_rms_b_sec[ipart][isector][i] = 1e6*(funrth_b->GetParameter("Sigma"));
    theta_mean_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Mean"));
    theta_rms_a_sec[ipart][isector][i] = 1e6*(funrth_a->GetParameter("Sigma"));
    theta_mean_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Mean"));
    theta_rms_r_sec[ipart][isector][i] = 1e6*(funrth_r->GetParameter("Sigma"));
  }

  /// END (Theta resolution) ---------------------------------------------------------------
  f1->Close();
  f2->Close();
  f3->Close();
  }
  }
  //  return;
  double av_theta_mean_b[nS],av_theta_rms_b[nS];
  double av_theta_mean_a[nS],av_theta_rms_a[nS];
  double av_theta_mean_r[nS],av_theta_rms_r[nS];
  double avEr_theta_mean_b[nS],avEr_theta_rms_b[nS];
  double avEr_theta_mean_a[nS],avEr_theta_rms_a[nS];
  double avEr_theta_mean_r[nS],avEr_theta_rms_r[nS];
  double avEr_Trks[nS];

  double av_theta_mean_b_sec[10][nS],av_theta_rms_b_sec[10][nS];
  double av_theta_mean_a_sec[10][nS],av_theta_rms_a_sec[10][nS];
  double av_theta_mean_r_sec[10][nS],av_theta_rms_r_sec[10][nS];
  double avEr_theta_mean_b_sec[10][nS],avEr_theta_rms_b_sec[10][nS];
  double avEr_theta_mean_a_sec[10][nS],avEr_theta_rms_a_sec[10][nS];
  double avEr_theta_mean_r_sec[10][nS],avEr_theta_rms_r_sec[10][nS];
  double av_Trks[nS];
  for(int i=0;i<nS;i++){
    av_theta_mean_b[i] = 0;
    av_theta_rms_b[i] = 0;
    av_theta_mean_a[i] = 0;
    av_theta_rms_a[i] = 0;
    av_theta_mean_r[i] = 0;
    av_theta_rms_r[i] = 0;
    av_Trks[i]=0;
    avEr_theta_mean_b[i] = 0;
    avEr_theta_rms_b[i] =0;
    avEr_theta_mean_a[i] = 0;
    avEr_theta_rms_a[i] = 0;
    avEr_theta_mean_r[i] = 0;
    avEr_theta_rms_r[i] = 0;
    avEr_Trks[i] = 0;
    for(int isector=0;isector<10;isector++){
      av_theta_mean_b_sec[isector][i] = 0;
      av_theta_rms_b_sec[isector][i] = 0;
      av_theta_mean_a_sec[isector][i] = 0;
      av_theta_rms_a_sec[isector][i] = 0;
      av_theta_mean_r_sec[isector][i] = 0;
      av_theta_rms_r_sec[isector][i] = 0;
      avEr_theta_mean_b_sec[isector][i] = 0;
      avEr_theta_rms_b_sec[isector][i] = 0;
      avEr_theta_mean_a_sec[isector][i] = 0;
      avEr_theta_rms_a_sec[isector][i] = 0;
      avEr_theta_mean_r_sec[isector][i] = 0;
      avEr_theta_rms_r_sec[isector][i] = 0;
    }
  }
  double i_nSamples = 1./nSamples;
  for(int i=0;i<nS;i++){
    for(int ipart=0;ipart<nSamples;ipart++){
      cout<<"theta_mean_b["<<ipart<<"]["<<i<<"] = "<<theta_mean_b[ipart][i]<<endl;
      cout<<"theta_mean_a["<<ipart<<"]["<<i<<"] = "<<theta_mean_a[ipart][i]<<endl;
      cout<<"theta_mean_r["<<ipart<<"]["<<i<<"] = "<<theta_mean_r[ipart][i]<<endl;
      av_theta_mean_b[i]+=theta_mean_b[ipart][i];
      av_theta_mean_a[i]+=theta_mean_a[ipart][i];
      av_theta_mean_r[i]+=theta_mean_r[ipart][i];
      av_theta_rms_b[i]+=theta_rms_b[ipart][i];
      av_theta_rms_a[i]+=theta_rms_a[ipart][i];
      av_theta_rms_r[i]+=theta_rms_r[ipart][i];
      av_Trks[i]+=Trks[ipart][i];
      cout<<"av_theta_mean_r["<<i<<"]="<<av_theta_mean_r[i]<<endl;
      for(int isector=0;isector<10;isector++){
	av_theta_mean_b_sec[isector][i]+=theta_mean_b_sec[ipart][isector][i];
	av_theta_mean_a_sec[isector][i]+=theta_mean_a_sec[ipart][isector][i];
	av_theta_mean_r_sec[isector][i]+=theta_mean_r_sec[ipart][isector][i];
	av_theta_rms_b_sec[isector][i]+=theta_rms_b_sec[ipart][isector][i];
	av_theta_rms_a_sec[isector][i]+=theta_rms_a_sec[ipart][isector][i];
	av_theta_rms_r_sec[isector][i]+=theta_rms_r_sec[ipart][isector][i];
      }
    }
    av_theta_mean_b[i] *=i_nSamples;
    av_theta_mean_a[i] *=i_nSamples;
    av_theta_mean_r[i] *=i_nSamples;
    av_theta_rms_b[i] *=i_nSamples;
    av_theta_rms_a[i] *=i_nSamples;
    av_theta_rms_r[i] *=i_nSamples;
    av_Trks[i]*=i_nSamples;
    for(int isector=0;isector<10;isector++){
      av_theta_mean_b_sec[isector][i] *=i_nSamples;
      av_theta_mean_a_sec[isector][i] *=i_nSamples;
      av_theta_mean_r_sec[isector][i] *=i_nSamples;
      av_theta_rms_b_sec[isector][i] *=i_nSamples;
      av_theta_rms_a_sec[isector][i] *=i_nSamples;
      av_theta_rms_r_sec[isector][i] *=i_nSamples;
    }
  }


  double i_nSamples_r = 1./(nSamples-1);
  for(int i=0;i<nS;i++){
    for(int ipart=0;ipart<nSamples;ipart++){
      avEr_theta_mean_b[i]+=(theta_mean_b[ipart][i]-av_theta_mean_b[i])*(theta_mean_b[ipart][i]-av_theta_mean_b[i]);
      avEr_theta_mean_a[i]+=(theta_mean_a[ipart][i]-av_theta_mean_a[i])*(theta_mean_a[ipart][i]-av_theta_mean_a[i]);
      avEr_theta_mean_r[i]+=(theta_mean_r[ipart][i]-av_theta_mean_r[i])*(theta_mean_r[ipart][i]-av_theta_mean_r[i]);
      avEr_theta_rms_b[i]+=(theta_rms_b[ipart][i]-av_theta_rms_b[i])*(theta_rms_b[ipart][i]-av_theta_rms_b[i]);
      avEr_theta_rms_a[i]+=(theta_rms_a[ipart][i]-av_theta_rms_a[i])*(theta_rms_a[ipart][i]-av_theta_rms_a[i]);
      avEr_theta_rms_r[i]+=(theta_rms_r[ipart][i]-av_theta_rms_r[i])*(theta_rms_r[ipart][i]-av_theta_rms_r[i]);
      avEr_Trks[i]+=(Trks[ipart][i]-av_Trks[i])*(Trks[ipart][i]-av_Trks[i]);
      for(int isector=0;isector<10;isector++){
	avEr_theta_mean_b_sec[isector][i]+=(theta_mean_b_sec[ipart][isector][i]-av_theta_mean_b_sec[isector][i])*(theta_mean_b_sec[ipart][isector][i]-av_theta_mean_b_sec[isector][i]);
	avEr_theta_mean_a_sec[isector][i]+=(theta_mean_a_sec[ipart][isector][i]-av_theta_mean_a_sec[isector][i])*(theta_mean_a_sec[ipart][isector][i]-av_theta_mean_a_sec[isector][i]);
	avEr_theta_mean_r_sec[isector][i]+=(theta_mean_r_sec[ipart][isector][i]-av_theta_mean_r_sec[isector][i])*(theta_mean_r_sec[ipart][isector][i]-av_theta_mean_r_sec[isector][i]);
	avEr_theta_rms_b_sec[isector][i]+=(theta_rms_b_sec[ipart][isector][i]-av_theta_rms_b_sec[isector][i])*(theta_rms_b_sec[ipart][isector][i]-av_theta_rms_b_sec[isector][i]);
	avEr_theta_rms_a_sec[isector][i]+=(theta_rms_a_sec[ipart][isector][i]-av_theta_rms_a_sec[isector][i])*(theta_rms_a_sec[ipart][isector][i]-av_theta_rms_a_sec[isector][i]);
	avEr_theta_rms_r_sec[isector][i]+=(theta_rms_r_sec[ipart][isector][i]-av_theta_rms_r_sec[isector][i])*(theta_rms_r_sec[ipart][isector][i]-av_theta_rms_r_sec[isector][i]);
      }
    }
    avEr_theta_mean_b[i] *=i_nSamples_r;
    avEr_theta_mean_a[i] *=i_nSamples_r;
    avEr_theta_mean_r[i] *=i_nSamples_r;
    avEr_theta_rms_b[i] *=i_nSamples_r;
    avEr_theta_rms_a[i] *=i_nSamples_r;
    avEr_theta_rms_r[i] *=i_nSamples_r;
    avEr_Trks[i]*=i_nSamples_r;
    avEr_theta_mean_b[i]  = sqrt(avEr_theta_mean_b[i]);
    avEr_theta_mean_a[i]  = sqrt(avEr_theta_mean_a[i]);
    avEr_theta_mean_r[i] = sqrt(avEr_theta_mean_r[i]);
    avEr_theta_rms_b[i] = sqrt(avEr_theta_rms_b[i]);
    avEr_theta_rms_a[i] = sqrt(avEr_theta_rms_a[i]);
    avEr_theta_rms_r[i] = sqrt(avEr_theta_rms_r[i]);
    avEr_Trks[i] = sqrt(avEr_Trks[i]);
    //    for(int ipart=0;ipart<nSamples;ipart++){
      for(int isector=0;isector<10;isector++){
      avEr_theta_mean_b_sec[isector][i] *=i_nSamples_r;
      avEr_theta_mean_a_sec[isector][i] *=i_nSamples_r;
      avEr_theta_mean_r_sec[isector][i] *=i_nSamples_r;
      avEr_theta_rms_b_sec[isector][i] *=i_nSamples_r;
      avEr_theta_rms_a_sec[isector][i] *=i_nSamples_r;
      avEr_theta_rms_r_sec[isector][i] *=i_nSamples_r;
      avEr_theta_mean_b_sec[isector][i] = sqrt(avEr_theta_mean_b_sec[isector][i]);
      avEr_theta_mean_a_sec[isector][i] = sqrt(avEr_theta_mean_a_sec[isector][i]);
      avEr_theta_mean_r_sec[isector][i] = sqrt(avEr_theta_mean_r_sec[isector][i]);
      avEr_theta_rms_b_sec[isector][i] = sqrt(avEr_theta_rms_b_sec[isector][i]);
      avEr_theta_rms_a_sec[isector][i] = sqrt(avEr_theta_rms_a_sec[isector][i]);
      avEr_theta_rms_r_sec[isector][i] = sqrt(avEr_theta_rms_r_sec[isector][i]);
    }
    cout<<"avEr_Trks["<<i<<"]="<<avEr_Trks[i]<<endl;
    cout<<"av_theta_mean_r["<<i<<"]="<<av_theta_mean_r[i]<<endl;
  }


  TCanvas c1;  
  // cout<<"av_Trks = "<<av_Trks<<endl;
    //put together theta resolution results
  TGraphErrors *gr_theta_b = new TGraphErrors(nS,av_Trks, av_theta_mean_b,avEr_Trks, avEr_theta_mean_b);
  gr_theta_b->SetMarkerStyle(20); 
  gr_theta_b->SetMarkerColor(kGreen-3); 
  gr_theta_b->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_a = new TGraphErrors(nS,av_Trks, av_theta_mean_a,avEr_Trks, avEr_theta_mean_a);
  gr_theta_a->SetMarkerStyle(29); 
  gr_theta_a->SetMarkerColor(kOrange+7); 
  gr_theta_a->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_r = new TGraphErrors(nS,av_Trks, av_theta_mean_r,avEr_Trks, avEr_theta_mean_r);
  gr_theta_r->SetMarkerStyle(21); 
  gr_theta_r->SetMarkerColor(15); 
  gr_theta_r->SetMarkerSize(2.5); 


  TMultiGraph *mgr_theta = new TMultiGraph();
  //  mgr_theta->Add(gr_theta_b);
  mgr_theta->Add(gr_theta_r);
  mgr_theta->Add(gr_theta_a);

  mgr_theta->Draw("AP");
  mgr_theta->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  mgr_theta->GetYaxis()->SetTitle("#theta_{mean}, #murad");
 c1.Print(resname_pdf_o); //write canvas and keep the ps file open
 c1.Clear();

 TGraphErrors *gr_theta_rms_b = new TGraphErrors(nS,av_Trks,av_theta_rms_b,avEr_Trks,avEr_theta_rms_b);
  gr_theta_rms_b->SetMarkerStyle(20); 
  gr_theta_rms_b->SetMarkerColor(kGreen-3); 
  gr_theta_rms_b->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_a = new TGraphErrors(nS,av_Trks,av_theta_rms_a,avEr_Trks,avEr_theta_rms_a);
  gr_theta_rms_a->SetMarkerStyle(29); 
  gr_theta_rms_a->SetMarkerColor(kOrange+7); 
  gr_theta_rms_a->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_r = new TGraphErrors(nS,av_Trks,av_theta_rms_r,avEr_Trks,avEr_theta_rms_a);
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
 for(int isector=0;isector<10;isector++){
   //put together theta resolution results
   TGraphErrors *gr_theta_b_sec = new TGraphErrors(nS,av_Trks, av_theta_mean_b_sec[isector],avEr_Trks, avEr_theta_mean_b_sec[isector]);
   gr_theta_b_sec->SetMarkerStyle(20); 
   gr_theta_b_sec->SetMarkerColor(kGreen-3); 
   gr_theta_b_sec->SetMarkerSize(2.5); 
   TGraphErrors *gr_theta_a_sec = new TGraphErrors(nS,av_Trks, av_theta_mean_a_sec[isector],avEr_Trks, avEr_theta_mean_a_sec[isector]);
   gr_theta_a_sec->SetMarkerStyle(29); 
   gr_theta_a_sec->SetMarkerColor(kOrange+7); 
   gr_theta_a_sec->SetMarkerSize(2.5); 
   TGraphErrors *gr_theta_r_sec = new TGraphErrors(nS,av_Trks, av_theta_mean_r_sec[isector],avEr_Trks, avEr_theta_mean_r_sec[isector]);
   gr_theta_r_sec->SetMarkerStyle(21); 
   gr_theta_r_sec->SetMarkerColor(15); 
   gr_theta_r_sec->SetMarkerSize(2.5); 

  TMultiGraph *mgr_theta_sec = new TMultiGraph();
  //  mgr_theta_sec->Add(gr_theta_b_sec);
  mgr_theta_sec->Add(gr_theta_r_sec);
  mgr_theta_sec->Add(gr_theta_a_sec);
  TString titlemean = "#theta_{mean}, #murad [sector No.";
  titlemean+=isector;
  titlemean +="]";
  mgr_theta_sec->Draw("AP");
  mgr_theta_sec->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  mgr_theta_sec->GetYaxis()->SetTitle(titlemean.Data());
  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
  c1.Clear();

 TGraphErrors *gr_theta_rms_b_sec = new TGraphErrors(nS,av_Trks,av_theta_rms_b_sec[isector],avEr_Trks,avEr_theta_rms_b_sec[isector]);
  gr_theta_rms_b_sec->SetMarkerStyle(20); 
  gr_theta_rms_b_sec->SetMarkerColor(kGreen-3); 
  gr_theta_rms_b_sec->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_a_sec = new TGraphErrors(nS,av_Trks,av_theta_rms_a_sec[isector],avEr_Trks,avEr_theta_rms_a_sec[isector]);
  gr_theta_rms_a_sec->SetMarkerStyle(29); 
  gr_theta_rms_a_sec->SetMarkerColor(kOrange+7); 
  gr_theta_rms_a_sec->SetMarkerSize(2.5); 
  TGraphErrors *gr_theta_rms_r_sec = new TGraphErrors(nS,av_Trks,av_theta_rms_r_sec[isector],avEr_Trks,avEr_theta_rms_a_sec[isector]);
  gr_theta_rms_r_sec->SetMarkerStyle(21); 
  gr_theta_rms_r_sec->SetMarkerColor(15); 
  gr_theta_rms_r_sec->SetMarkerSize(2.5); 
  TMultiGraph *mgr_theta_rms_sec = new TMultiGraph();
  //  mgr_theta_rms_sec->Add(gr_theta_rms_b_sec);
  mgr_theta_rms_sec->Add(gr_theta_rms_r_sec);
  mgr_theta_rms_sec->Add(gr_theta_rms_a_sec);
  mgr_theta_rms_sec->Draw("AP");
  mgr_theta_rms_sec->GetXaxis()->SetTitle("Average number of rec. trks per sector");
  TString titlerms = "#theta_{#sigma}, #murad [sector No.";
  titlerms+=isector;
  titlerms +="]";
  mgr_theta_rms_sec->GetYaxis()->SetTitle(titlerms.Data());
  c1.Print(resname_pdf_o); //write canvas and keep the ps file open
  c1.Clear();
 }
 TGraphErrors *grstat = new TGraphErrors(nS,TrksSim,av_Trks,0,avEr_Trks);
  grstat->SetMarkerStyle(22);
  grstat->SetMarkerSize(2.5);
  grstat->Draw("APL");
  //  grstat->GetYaxis()->SetTitle("Average number of rec. trks per sector");
  grstat->GetXaxis()->SetTitle("Number of sim. trks");
  grstat->SetTitle("Average number of rec. trks per sector (used in Knossos)");
  c1.Print(resname_pdf_c); //write canvas and close ps file

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
  f->Write();
  f->Close();
}
