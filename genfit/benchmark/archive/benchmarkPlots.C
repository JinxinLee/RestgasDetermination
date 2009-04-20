#define benchmarkPlots_cxx
#include "benchmarkPlots.h"
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>

#include<iostream>
#include<string>
#include"TFile.h"
#include"TH1D.h"


void benchmarkPlots::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L benchmarkPlots.C
//      Root > benchmarkPlots t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   std::cout << "Processing " << nentries << " entries..." << std::endl;
   TH1D *momPuH = new TH1D("momPuH","pull distr. of Momentum",50,-4.,4.);
   TH1D *uPuH = new TH1D("uPuH","pull distr. of coord. u",50,-4.,4.);
   TH1D *vPuH = new TH1D("vPuH","pull distr. of coord. v",50,-4.,4.);
   TH1D *upPuH = new TH1D("upPuH","pull distr. of slope u'",50,-4.,4.);
   TH1D *vpPuH = new TH1D("vpPuH","pull distr. of slope v'",50,-4.,4.);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      momPuH->Fill(momPu);
      uPuH->Fill(uPu);
      vPuH->Fill(vPu);
      upPuH->Fill(upPu);
      vpPuH->Fill(vpPu);
   }



   //gROOT->Macro("christian_style.C");
   //   gROOT->SetStyle("Plain");
   //   gStyle->SetOptFit(1111);


   gROOT->SetStyle("Plain");
   
   gStyle->SetPalette(1);
   gStyle->SetOptStat("");
   gStyle->SetStatColor(10);
   gStyle->SetStatW(0.125);
   gStyle->SetStatH(0.1);
   gStyle->SetStatX(0.89);
   gStyle->SetStatY(0.89);
   gStyle->SetStatFontSize(0.015);
   gStyle->SetStatBorderSize(0);
   gStyle->SetTitleBorderSize(0);
   /*
   gStyle->SetFillStyle(0);
   gStyle->SetFillColor(10);
   gStyle->SetCanvasColor(10);
   gStyle->SetDrawBorder(0);
   gStyle->SetPadColor(10);
   gStyle->SetPadBorderSize(0);
   gStyle->SetPadLeftMargin(0.13);
   gStyle->SetPadRightMargin(0.12);
   gStyle->SetPadBottomMargin(0.13);
   
   gStyle->SetTitleFont(102,"XYZ");
   gStyle->SetLabelFont(102,"XYZ");
   gStyle->SetTitleFont(102,"A");
   gStyle->SetTitleX(0.05);
   gStyle->SetTitleY(0.965);
   gStyle->SetTitleAlign(13);
   gStyle->SetTitleBorderSize(0);
   gStyle->SetStatFont(102);

   
   gStyle->SetLabelOffset(.01);
   //gStyle->SetLabelSize(0.035);
   gStyle->SetTitleOffset(1.2);
   //gStyle->SetLabelSize(0.035,"Y");
   gStyle->SetTitleOffset(1.3,"Y");
   

   
   
   gStyle->SetTitleSize(0.05,"a");
   gStyle->SetLabelSize(0.05,"XYZ");
   gStyle->SetTitleSize(0.05,"XYZ");
   */
   gStyle->SetOptStat(00000);
   gStyle->SetOptFit(0011);

   
   std::string pointRes;
   std::cout << std::endl << "give pointRes: ";
   std::cin >> pointRes;


   char buf[100];
   sprintf(buf,"%2.1f GeV/c -- %2.1f deg -- %s #mum point resolution",momTr,thTr/TMath::Pi() * 180.,pointRes.c_str());


   char filenameBase[50];
   sprintf(filenameBase,"%2.1fGeV_%2.1fdeg_%sres.eps",momTr,thTr/TMath::Pi() * 180.,pointRes.c_str());
   char filename[50];
   TCanvas c1;

   momPuH->SetTitle(buf);
   momPuH->GetXaxis()->SetTitle("pull of momentum");
   momPuH->Draw();
   momPuH->Fit("gaus","L");
   sprintf(filename,"momPu_%s",filenameBase);
   c1.Print(filename);

   uPuH->SetTitle(buf);
   uPuH->GetXaxis()->SetTitle("pull of coord. u");
   uPuH->Draw();
   uPuH->Fit("gaus","L");
   sprintf(filename,"uPu_%s",filenameBase);
   c1.Print(filename);

   vPuH->SetTitle(buf);
   vPuH->GetXaxis()->SetTitle("pull of coord. v");
   vPuH->Draw();
   vPuH->Fit("gaus","L");
   sprintf(filename,"vPu_%s",filenameBase);
   c1.Print(filename);

   upPuH->SetTitle(buf);
   upPuH->GetXaxis()->SetTitle("pull of slope u'");
   upPuH->Draw();
   upPuH->Fit("gaus","L");
   sprintf(filename,"upPu_%s",filenameBase);
   c1.Print(filename);

   vpPuH->SetTitle(buf);
   vpPuH->GetXaxis()->SetTitle("pull of slope v'");
   vpPuH->Draw();
   vpPuH->Fit("gaus","L");
   sprintf(filename,"vpPu_%s",filenameBase);
   c1.Print(filename);

}
