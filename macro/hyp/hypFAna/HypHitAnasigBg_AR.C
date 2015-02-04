
#include <TH1F.h>
#include <TLine.h>

void HypHitAnasigBg_AR()
{
  
  //gROOT->Reset();gROOT->LoadMacro("style.C");

  //gStyle->SetFillColor(10);
  style();
  
  TH1F* h[10];
  TH1F* b[10];
  //TFile *f0 = new TFile("Hit_hypIdReco.AnaFBg.root");
  //TFile *f1 = new TFile("Hit_hypIdReco.AnaF.root");
  TFile *f1 = new TFile("AnaFSimpleP4Bup1TXXb.root");
  //TFile *f1 = new TFile("Hit_hypIdReco.AnaFBg.root");

   char fil3_s[50]="spectra0%d"; 
   char fil3[50];
   TH1F *h0;h0=0.;
   
   TH1F *b0;b0=0.;
   
   
   for(int i=0;i<8;i++) {
     h[i]=0.;
     b[i]=0.;

   }
   //SpectraCuts.pdf
       
      
       
       
      //MesSeqDecColCut.pdf
      TCanvas *c4 = new TCanvas("c4","the fit canvas",800,800);

       TH2F* h5 = (TH2F*)f1->Get("hvtx202");
       
       h5->SetFillColor(0);
       
       //h5->SetMarkerColor(2);
       h5->SetMarkerSize(2);
       h5->SetMarkerStyle(7);

       
       TH2F* h2 = (TH2F*)f1->Get("hvtx203");
       
       //h2->SetMarkerColor(3);
       h2->SetMarkerStyle(7);
       h2->SetMarkerSize(2);
      


       TH2F* h3 = (TH2F*)f1->Get("hvtx204");
       
       //h3->SetMarkerColor(6);
       h3->SetMarkerStyle(7);
        h3->SetMarkerSize(2);
       

       TH2F* h4 = (TH2F*)f1->Get("hvtx205");
       //h4->SetMarkerColor(7);
       h4->SetMarkerStyle(7);
       h4->SetMarkerSize(2);

       TH2F* h6 = (TH2F*)f1->Get("hvtx206");
       //h6->SetMarkerColor(5);
       h6->SetMarkerStyle(7);
       h6->SetMarkerSize(2);

        TH2F* h7 = (TH2F*)f1->Get("hvtx207");
	//h7->SetMarkerColor(1);
       h7->SetMarkerStyle(7);
       h7->SetMarkerSize(2);

        TH2F* h8 = (TH2F*)f1->Get("hvtx208");
       //h8->SetMarkerColor(8);
       h8->SetMarkerStyle(7);
       h8->SetMarkerSize(2);
      

       /* TH2F* h9 = (TH2F*)f1->Get("hvtx209");
	//h9->SetMarkerColor(9);
       h9->SetMarkerSize(2);
       h9->SetMarkerStyle(7);
       h9->SetTitle("");*/
       
       
	 // ***background****
	 TH2F* h9 = (TH2F*)f1->Get("hvtx201");
	 //h9->SetMarkerColor(9);
	 // h9->SetMarkerSize(2);
	 //h9->SetMarkerStyle(7);
       h9->SetTitle("");
       h9->GetXaxis()->SetTitle("P_{High} (GeV/c)");
       h9->GetXaxis()->SetLabelSize(0.03);
      
       h9->GetXaxis()->CenterTitle();
       h9->GetYaxis()->SetTitle("P_{Low} (GeV/c)");
       h9->GetYaxis()->SetLabelSize(0.03);
       h9->GetYaxis()->SetTitleOffset(1.5);
      
       h9->GetYaxis()->CenterTitle();
       h9->Draw("box");
       /* h5->Draw("same"); 
       h2->Draw("same");
       h3->Draw("same");
       h4->Draw("same"); 
       h6->Draw("same"); 
       h7->Draw("same");
       h8->Draw("same");*/

       
        TBox* box0 = new TBox(0.12,0.065,0.14,0.08);
       box0->SetLineColor(2);
       box0->SetLineStyle(1);
       box0->SetLineWidth(3);
       box0->SetFillStyle(0);
       
       box0->Draw();
       TBox* box1 = new TBox(0.112,0.093,0.126,0.104);//0.112,0.09,0.126,0.103);
       box1->SetLineColor(8);
       box1->SetLineStyle(1);
       box1->SetLineWidth(3);
       box1->SetFillStyle(0);
       box1->Draw();
       TBox* box2 = new TBox(0.126,0.093,0.14,0.104);//0.126,0.09,0.14,0.103);
       box2->SetLineColor(4);
       box2->SetLineStyle(1);
       box2->SetLineWidth(3);
       box2->SetFillStyle(0);
       box2->Draw();
       TBox* box3 = new TBox(0.097,0.094,0.106,0.103);
       box3->SetLineColor(6);
       box3->SetLineStyle(1);
       box3->SetLineWidth(3);
       box3->SetFillStyle(0);
       box3->Draw();

       TBox* box4 = new TBox(0.106,0.104,0.112,0.113);
       box4->SetLineColor(9);
       box4->SetLineStyle(2);
       box4->SetFillStyle(0);
       //box4->Draw();
 
       TBox* box5 = new TBox(0.098,0.095,0.105,0.104);
       box5->SetLineColor(5);
       box5->SetLineStyle(1);
       box5->SetFillStyle(0);
       //box5->Draw();

       TBox* box6 = new TBox(0.112,0.104,0.123,0.115);
       box6->SetLineColor(7);
       box6->SetLineStyle(1);
       box6->SetFillStyle(0);
       //box6->Draw();

       TBox* box7 = new TBox(0.104,0.094,0.115,0.104);
       box7->SetLineColor(8);
       box7->SetLineStyle(1);
       box7->SetFillStyle(0);
       //box7->Draw();

       TBox* box8 = new TBox(0.098,0.088,0.107,0.096);
       box8->SetLineColor(1);
       box8->SetLineStyle(1);
       box8->SetFillStyle(0);
       //box8->Draw();

       leg = new TLegend(0.2,0.6,0.45,0.85,0);
       leg ->AddEntry(box0,"Be^{11}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box1,"Li ^{9}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box2,"Be^{9}_{#Lambda}+H ^{4}_{#Lambda}","F");
       leg ->AddEntry(box3," Be ^{10}_{#Lambda#Lambda}","F");
       /*leg ->AddEntry(box4,"He^{7}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box5,"He^{6}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box6,"Be^{12}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box7,"Li^{8}_{#Lambda#Lambda}","F");
       leg ->AddEntry(box8,"B^{12}_{#Lambda#Lambda}","F");*/
       leg->SetEntrySeparation(0.3);
       leg->SetBorderSize(0);
       leg ->SetFillColor(0);
       leg ->Draw();
       //c4->Print("MesSeqDecColCut.pdf");
       //c4->Print("MesSeqDecColCutG4ST.pdf");
       c4->Print("MesSeqDecColCutG4ST_P4.eps");
       //c4->Print("MesSeqDecColCutBg.pdf");
       //c4->Print("MesSeqDecColCut.eps");
    
 }//end

 void style()
{ 
	printf("\n...Panda Root Setup\n\n");
	
	//	printf("...Loading Palette\n");
	//gROOT->LoadMacro("palette.C");
	//gROOT->LoadMacro("palette2.C");
	
	printf("...Loading Modes\n");
	gStyle->SetPadBorderMode(1);
	gStyle->SetPadBorderSize(1);
	gStyle->SetPadColor(0);
	gStyle->SetDrawBorder(0);
	gStyle->SetCanvasBorderMode(0);
	gStyle->SetCanvasBorderSize(0);
	gStyle->SetTitleBorderSize(0);
	gStyle->SetCanvasColor(10);
	gStyle->SetStatBorderSize(1);
	gStyle->SetFrameBorderMode(0);
	gStyle->SetFrameBorderSize(1);
		
	printf("...Loading Fit/Stat\n");
	gStyle->SetFitFormat("2.3f");
	gStyle->SetOptFit(0);
	gStyle->SetOptStat(0) ; 	  
	gStyle->SetStatColor(0);		
	gStyle->SetStatX(0.46);
	gStyle->SetStatY(0.9);
	gStyle->SetStatW(0.15);
	gStyle->SetStatH(0.1);
	
	printf("...Loading Title Style\n");
	gStyle->SetTitleColor(1);
	gStyle->SetTitleFillColor(0);
	gStyle->SetTitleX(0.5);
	gStyle->SetTitleY(0.8);
	
	gStyle->SetTitleOffset(1.3,"x");
	gStyle->SetTitleOffset(1.7,"y");
	gStyle->SetTitleOffset(1.7,"z");
	gStyle->SetTitleSize(.0425,"x");
	gStyle->SetTitleSize(.0425,"y");
	gStyle->SetTitleSize(.0425,"z");
	
	printf("...Loading Label Style\n");
	gStyle->SetLabelOffset(.02,"x");
	gStyle->SetLabelOffset(.025,"y");
	gStyle->SetLabelOffset(.02,"z");	
	gStyle->SetLabelFont(42,"x");
	gStyle->SetLabelFont(42,"y");
	gStyle->SetLabelFont(42,"z");
	gStyle->SetTickLength(-0.02,"x");
	gStyle->SetTickLength(-0.02,"y");
	gStyle->SetTickLength(-0.02,"z");
	gStyle->SetLabelSize(.0425,"x");
	gStyle->SetLabelSize(.0425,"y");
	gStyle->SetLabelSize(.0425,"z");
	gStyle->SetPadTickX(1);
	gStyle->SetPadTickY(1);
	
	gStyle->SetPadTopMargin(0.03); 
	gStyle->SetPadRightMargin(0.03);
	gStyle->SetPadBottomMargin(0.12);
	gStyle->SetPadLeftMargin(0.15);
	
	printf("...Loading Fonts\n");
	gStyle->SetTitleFont(42);
	gStyle->SetStatFont(42);
	gStyle->SetTitleFont(42,"x");
	gStyle->SetTitleFont(42,"y");
	gStyle->SetTitleFont(42,"z");
	gStyle->GetAttDate()->SetTextFont(42);
	gStyle->GetAttDate()->SetTextSize(0.02);
	
	printf("...Loading Histo Options\n");
	gStyle->SetHistFillStyle(1001);
	gStyle->SetHistLineWidth(1);
		
	gStyle->SetOptDate(0);
	gStyle->SetPalette(1);
	printf("\n");
}
  

