#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH2.h"
using namespace std;
void AnaResThetaPhi(TString path="/panda/myResults/LUMI_Trk/ResTheta_vs_ThetaPhi/mom_1_5/"){
  gStyle->SetOptStat("e");

   gStyle->SetLabelFont(42, "x");
   gStyle->SetLabelFont(42, "y");
   gStyle->SetLabelFont(42, "z");
   gStyle->SetLabelSize(0.06, "x");
   gStyle->SetLabelSize(0.06, "y");
   gStyle->SetLabelSize(0.06, "z");
   gStyle->SetTitleOffset(0.75, "x");
   gStyle->SetTitleOffset(0.75, "y");
   gStyle->SetTitleOffset(0.75, "z");
   gStyle->SetTitleSize(0.06, "x");
   gStyle->SetTitleSize(0.06, "y");
   gStyle->SetTitleSize(0.06, "z");

  TString filename = path+"/Lumi_out_MC_and_REC_trks_hitsmatches_CA_1trks.root";
  TFile * fin = new TFile(filename,"READ");
  TTree *trecmcinfo = (TTree*)fin->Get("tRECMC");
  //  TString cond = "trkstREC==0 && abs(xrec)<3*0.5 && abs(yrec)<3*0.5 && abs(zrec)<0.1 && 1e3*thetarec<15. && momrec<1.01*mommc";
  //  TString cond = "trkstREC==0 && abs(xrec)<3*0.5 && abs(yrec)<3*0.5 && abs(zrec)<0.1 && 1e3*thetarec<15.";//OLD files
  TString cond = "trkstREC==0";//NO CUTS
  // TH3D *hphithetarestheta = new TH3D("hphithetarestheta",";#phi_{MC},rad;#theta_{MC},mrad;(#theta_{MC}-#theta_{REC}), mrad",1e3,-3.14,3.14,1e3,3,10,1e3,-10.,10.);
  // trecmcinfo->Project("hphithetarestheta","phimc:1e3*thetamc:1e3*(thetamc-thetarec)",cond.Data());
  // hphithetarestheta->FitSlicesZ();
  // TCanvas c2;
  // c2.Divide(2,3);
  // c2.cd(1);
  // hphithetarestheta_1->Draw("colz");
  // c2.cd(2);
  // hphithetarestheta_2->Draw("colz");
  // c2.cd(3);
  // hphithetarestheta_chi2->Draw("colz");
  // c2.cd(4);
  // hphithetarestheta->Project3D("xy");
  // hphithetarestheta_xy->Draw("colz");
  // TString outnamepdf = path+"/resTheta_vs_phi_theta3DNOCUTS.pdf";
  // TString outnameroot = path+"/resTheta_vs_phi_theta3DNOCUTS.root";
  // c2.SaveAs(outnamepdf);
  // c2.SaveAs(outnameroot);


  TCanvas c1;
  c1.Divide(3,2);
  c1.cd(1);
  TH2D *hthetarestheta = new TH2D("hthetarestheta",";#theta_{MC},mrad;(#theta_{MC}-#theta_{REC}), mrad",1e3,3, 10,1e3,-10.,10.);
  trecmcinfo->Project("hthetarestheta","1e3*(thetamc-thetarec):(1e3*thetamc)",cond.Data());
  //  hthetarestheta->Draw("colz");
  hthetarestheta->FitSlicesY();
  //  c1.cd(3);
  hthetarestheta_1->SetMinimum(-0.3);//1.5
  hthetarestheta_1->SetMaximum(0.3);
  // hthetarestheta_1->SetMinimum(-0.05);//4.06 & 8.9
  // hthetarestheta_1->SetMaximum(0.05);
  // hthetarestheta_1->SetMinimum(-0.02);//11.91
  // hthetarestheta_1->SetMaximum(0.02);
  hthetarestheta_1->Draw();
  c1.cd(2);
  hthetarestheta_2->SetMinimum(0.7);//1.5
  hthetarestheta_2->SetMaximum(1.5);
  // hthetarestheta_2->SetMinimum(0.1);//4.06
  // hthetarestheta_2->SetMaximum(0.4);
  // hthetarestheta_2->SetMinimum(0.07);//8.9
  // hthetarestheta_2->SetMaximum(0.15);
  // hthetarestheta_2->SetMinimum(0.05);//11.91
  // hthetarestheta_2->SetMaximum(0.12);
  hthetarestheta_2->Draw();
  c1.cd(3);
  hthetarestheta_chi2->Draw();
  c1.cd(4);
  TH2D *hphirestheta = new TH2D("hphirestheta",";#phi_{MC},rad;(#theta_{MC}-#theta_{REC}), mrad",1e3,-3.14,3.14,1e3,-10.,10.);
  trecmcinfo->Project("hphirestheta","1e3*(thetamc-thetarec):phimc",cond.Data());
  //  hphirestheta->Draw("colz");
  hphirestheta->FitSlicesY();
  //  c1.cd(4);
  hphirestheta_1->SetMinimum(-0.3);
  hphirestheta_1->SetMaximum(0.3);
  hphirestheta_1->Draw();
  c1.cd(5);
  hphirestheta_2->SetMinimum(0.7);
  hphirestheta_2->SetMaximum(1.5);
  hphirestheta_2->Draw();
  c1.cd(6);
  hphirestheta_chi2->Draw();
  TString outnamepdf = path+"/resTheta_vs_phi_thetaNOCUTS.pdf";
  TString outnameroot = path+"/resTheta_vs_phi_thetaNOCUTS.root";
  c1.SaveAs(outnamepdf);
  c1.SaveAs(outnameroot);

  TCanvas c2;
  hthetarestheta->RebinX(5);
  hthetarestheta->RebinY(5);
  // hthetarestheta->Draw("BOX");
  hthetarestheta->Draw("colz");
  TString outnamepdf = path+"/resTheta_vs_theta3DNOCUTS.pdf";
  TString outnameroot = path+"/resTheta_vs_theta3DNOCUTS.root";
  c2.SaveAs(outnamepdf);
  c2.SaveAs(outnameroot);

 TCanvas c3;
 hphirestheta->RebinX(5);
 hphirestheta->RebinY(5);
 // hphirestheta->Draw("BOX");
 hphirestheta->Draw("colz");
 TString outnamepdf = path+"/resTheta_vs_phi3DNOCUTS.pdf";
 TString outnameroot = path+"/resTheta_vs_phi3DNOCUTS.root";
 c3.SaveAs(outnamepdf);
 c3.SaveAs(outnameroot);
}
