#define plotPulls_cxx
#include "plotPulls.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include<map>
#include<string>
#include"TF1.h"
#include"TStyle.h"
#include"TROOT.h"
#include"TPaveText.h"
#include"TRandom3.h"
#include"TMath.h"

double myGausF(double* x, double* p){
  return p[0]*exp(-0.5*pow(x[0]-p[1],2.)/pow(p[2],2.));
}

void plotPulls::Loop(std::string title)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   gStyle->SetOptStat(0000);
   gStyle->SetOptFit(0000);

   char buf[100];

   TH1D* pullPP = new TH1D("pullP","",100,-10.,10.);
   TH1D* pullUU = new TH1D("pullU","",100,-10.,10.);
   TH1D* pullVV = new TH1D("pullV","",100,-10.,10.);
   TH1D* pullUpUp = new TH1D("pullUp","",100,-10.,10.);
   TH1D* pullVpVp = new TH1D("pullVp","",100,-10.,10.);

   TH1D* theta = new TH1D("theta","",100,-360.,360.);
   TH1D* phi = new TH1D("phi","",100,-360.,360.);

   std::map<std::string,std::string> axes;
   axes["PP"] = "pull of momentum";
   axes["UU"] = "pull of coordinate u";
   axes["VV"] = "pull of coordinate v";
   axes["UpUp"] = "pull of slope u'";
   axes["VpVp"] = "pull of slope v'";

   std::cout << "Tree has entries #" << nentries << std::endl;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      pullPP->Fill(momPu);
      pullUU->Fill(uPu);
      pullVV->Fill(upPu);
      pullUpUp->Fill(vPu);
      pullVpVp->Fill(vpPu);

      phi->Fill(phiTr/TMath::Pi()*180.);
      theta->Fill(thTr/TMath::Pi()*180.);

      if(jentry==7500) break;
      // if (Cut(ientry) < 0) continue;
   }

   TRandom3 rand;


   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cPP = new TCanvas(buf,buf);
   TF1 *gPP = new TF1("mygausPP",myGausF,-10.,10.,3);
   gPP->SetLineWidth(2);
   gPP->SetLineColor(12);
   gPP->SetParameters(100,0.,1.);
   pullPP->Fit("mygausPP");

   double mPP=gPP->GetParameter(1);
   double sPP=gPP->GetParameter(2);
   TPaveText *tPP = new TPaveText(0.6,0.68,.87,.8,"NDC");
   sprintf(buf,"mean = %1.3f",mPP);
   tPP->AddText(buf);
   sprintf(buf,"#sigma    = %1.3f",fabs(sPP));
   tPP->AddText(buf);
   tPP->SetTextFont(102);
   tPP->SetBorderSize(0);

   pullPP->SetTitle(title.c_str());
   pullPP->GetXaxis()->SetTitle(axes["PP"].c_str());
   pullPP->Draw();
   tPP->Draw();
   cPP->SaveAs("PP.eps");
   cPP->SaveAs("PP.pdf");
   //   cPP->SaveAs("PP.C");

   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cUU = new TCanvas(buf,buf);
   TF1 *gUU = new TF1("mygausUU",myGausF,-10.,10.,3);
   gUU->SetLineWidth(2);
   gUU->SetLineColor(12);
   gUU->SetParameters(100,0.,1.);
   pullUU->Fit("mygausUU");

   double mUU=gUU->GetParameter(1);
   double sUU=gUU->GetParameter(2);
   TPaveText *tUU = new TPaveText(0.6,0.68,.87,.8,"NDC");
   sprintf(buf,"mean = %1.3f",mUU);
   tUU->AddText(buf);
   sprintf(buf,"#sigma    = %1.3f",fabs(sUU));
   tUU->AddText(buf);
   tUU->SetTextFont(102);
   tUU->SetBorderSize(0);

   pullUU->SetTitle(title.c_str());
   pullUU->GetXaxis()->SetTitle(axes["UU"].c_str());
   pullUU->Draw();
   tUU->Draw();
   cUU->SaveAs("UU.eps");
   cUU->SaveAs("UU.pdf");
   //   cUU->SaveAs("UU.C");

   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cVV = new TCanvas(buf,buf);
   TF1 *gVV = new TF1("mygausVV",myGausF,-10.,10.,3);
   gVV->SetLineWidth(2);
   gVV->SetLineColor(12);
   gVV->SetParameters(100,0.,1.);
   pullVV->Fit("mygausVV");

   double mVV=gVV->GetParameter(1);
   double sVV=gVV->GetParameter(2);
   TPaveText *tVV = new TPaveText(0.6,0.68,.87,.8,"NDC");
   sprintf(buf,"mean = %1.3f",mVV);
   tVV->AddText(buf);
   sprintf(buf,"#sigma    = %1.3f",fabs(sVV));
   tVV->AddText(buf);
   tVV->SetTextFont(102);
   tVV->SetBorderSize(0);

   pullVV->SetTitle(title.c_str());
   pullVV->GetXaxis()->SetTitle(axes["VV"].c_str());
   pullVV->Draw();
   tVV->Draw();
   cVV->SaveAs("VV.eps");
   cVV->SaveAs("VV.pdf");
   //   cVV->SaveAs("VV.C");

   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cUpUp = new TCanvas(buf,buf);
   TF1 *gUpUp = new TF1("mygausUpUp",myGausF,-10.,10.,3);
   gUpUp->SetLineWidth(2);
   gUpUp->SetLineColor(12);
   gUpUp->SetParameters(100,0.,1.);
   pullUpUp->Fit("mygausUpUp");

   double mUpUp=gUpUp->GetParameter(1);
   double sUpUp=gUpUp->GetParameter(2);
   TPaveText *tUpUp = new TPaveText(0.6,0.68,.87,.8,"NDC");
   sprintf(buf,"mean = %1.3f",mUpUp);
   tUpUp->AddText(buf);
   sprintf(buf,"#sigma    = %1.3f",fabs(sUpUp));
   tUpUp->AddText(buf);
   tUpUp->SetTextFont(102);
   tUpUp->SetBorderSize(0);

   pullUpUp->SetTitle(title.c_str());
   pullUpUp->GetXaxis()->SetTitle(axes["UpUp"].c_str());
   pullUpUp->Draw();
   tUpUp->Draw();
   cUpUp->SaveAs("UpUp.eps");
   cUpUp->SaveAs("UpUp.pdf");
   //   cUpUp->SaveAs("UpUp.C");

   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cVpVp = new TCanvas(buf,buf);
   TF1 *gVpVp = new TF1("mygausVpVp",myGausF,-10.,10.,3);
   gVpVp->SetLineWidth(2);
   gVpVp->SetLineColor(12);
   gVpVp->SetParameters(100,0.,1.);
   pullVpVp->Fit("mygausVpVp");

   double mVpVp=gVpVp->GetParameter(1);
   double sVpVp=gVpVp->GetParameter(2);
   TPaveText *tVpVp = new TPaveText(0.6,0.68,.87,.8,"NDC");
   sprintf(buf,"mean = %1.3f",mVpVp);
   tVpVp->AddText(buf);
   sprintf(buf,"#sigma    = %1.3f",fabs(sVpVp));
   tVpVp->AddText(buf);
   tVpVp->SetTextFont(102);
   tVpVp->SetBorderSize(0);

   pullVpVp->SetTitle(title.c_str());
   pullVpVp->GetXaxis()->SetTitle(axes["VpVp"].c_str());
   pullVpVp->Draw();
   tVpVp->Draw();
   cVpVp->SaveAs("VpVp.eps");
   cVpVp->SaveAs("VpVp.pdf");
   //   cVpVp->SaveAs("VpVp.C");

   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cPhi = new TCanvas(buf,buf);
   phi->Draw();
   sprintf(buf,"%f",rand.Uniform());
   TCanvas *cTheta = new TCanvas(buf,buf);
   theta->Draw();

}
