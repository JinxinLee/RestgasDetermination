{

TFile* f1 = new TFile("AnaFSimpleBup1TXm_Bg.root");
// TFile* f2 = new TFile("AnaFSimpleP4Bup1TXXb.root");


// TFile* f3 = new TFile("AnaFSimpleP6Bup1TXXb.root");
// TFile* f4 = new TFile("AnaFSimpleP8Bup1TXXb.root");
TH2D * h1 =(TH2D*)f1->Get("hvtx201");
TH2D * h2 =(TH2D*)f1->Get("hvtx202");

TH2D * h3 =(TH2D*)f1->Get("hvtx203");
TH2D * h4 =(TH2D*)f1->Get("hvtx204");

TH1D * h1p =(TH1D*)h1->ProjectionX(" proj_x1",0,-1,"");
TH1D * h2p =(TH1D*)h1->ProjectionY(" proj_y2",0,-1,"");

TH1D * h3p =(TH1D*)h3->ProjectionY(" proj_y3",0,-1,"");

TH1D * h4p =(TH1D*)h4->ProjectionX(" proj_x4",0,-1,"");


TCanvas* c2 = new TCanvas("c2","c2");
//c2->Divide(1,2);
//c2->cd(1);
 h1->SetTitle("#pi_{L} + #pi_{H} correlation");
 h1->GetXaxis()->CenterTitle();
 h1->GetYaxis()->SetTitle("P_{Low} (GeV/c)");
 h1->GetXaxis()->CenterTitle();
 h1->GetXaxis()->SetTitle("P_{High} (GeV/c)");
 h1->GetYaxis()->SetTitleOffset(1.25);
 h1->GetXaxis()->SetTitleOffset(1.25);
 h1->Draw("box");
 // c2->cd(2);
 // h3->Draw("samebox");
 //h4->Draw("samebox");

TCanvas* c = new TCanvas("c1","c1");
c->Divide(1,3);

c->cd(1);
 h1p->GetXaxis()->SetTitle("P_{high} (GeV/c)P");
 h1p->GetYaxis()->SetTitle("Counts");
 h1p->GetXaxis()->SetLabelSize(0.05);
 h1p->GetYaxis()->SetLabelSize(0.05);
 h1p->GetYaxis()->SetTitleSize(0.05);
 h1p->GetXaxis()->SetTitleSize(0.05);
 h1p->GetYaxis()->SetTitleOffset(0.75);

 h1p->SetLineWidth(2);
 // h4p->SetLineWidth(2);
 h1p->SetLineColor(1);
 //h4p->SetLineColor(2);
h1p->Draw();
//h4p->Draw("same");

 // leg = new TLegend(0.2,0.6,0.45,0.85,0);
 // leg ->AddEntry(h1p," two pion events","L");
 // leg ->AddEntry(h4p,"one pion events ","L");
 // leg->SetBorderSize(0);
 // leg ->SetFillColor(0);
 // leg ->Draw();

c->cd(2);

h2p->GetXaxis()->SetTitle("P_{low}(GeV/c)");
 h2p->GetYaxis()->SetTitle("Counts");
 h2p->GetYaxis()->SetTitleSize(0.05);
 h2p->GetXaxis()->SetTitleSize(0.05);
 h2p->GetXaxis()->SetLabelSize(0.05);
 h2p->GetYaxis()->SetLabelSize(0.05);
 h2p->GetYaxis()->SetTitleOffset(0.75);

 h2p->SetLineWidth(2);
 //h3p->SetLineWidth(2);
 h2p->SetLineColor(1);
 h3p->SetLineColor(2);
h2p->Draw();
//h3p->Draw("same");

 // leg = new TLegend(0.2,0.6,0.45,0.85,0);
 // leg ->AddEntry(h2p," two pion events","L");
 // leg ->AddEntry(h3p,"one pion events ","L");
 // leg->SetBorderSize(0);
 // leg ->SetFillColor(0);
 // leg ->Draw();

c->cd(3);
 h3p->SetTitle("one pion events ");
h3p->GetXaxis()->SetTitle("P(GeV/c)");
 h3p->GetYaxis()->SetTitle("Counts");
 h3p->GetYaxis()->SetTitleSize(0.05);
 h3p->GetXaxis()->SetTitleSize(0.05);
 h3p->GetXaxis()->SetLabelSize(0.05);
 h3p->GetYaxis()->SetLabelSize(0.05);
 h3p->GetYaxis()->SetTitleOffset(0.75);
 h3p->SetLineWidth(2);
 h3p->SetLineColor(1);
 h4p->SetLineWidth(2);
h4p->SetLineColor(1);

h3p->Draw();
 h4p->Draw("same");
// c->cd(4);
// h4p->Draw();




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
	gStyle->SetTitleSize(.0525,"x");
	gStyle->SetTitleSize(.0525,"y");
	gStyle->SetTitleSize(.0525,"z");
	
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
	gStyle->SetLabelSize(.0525,"x");
	gStyle->SetLabelSize(.0525,"y");
	gStyle->SetLabelSize(.0525,"z");
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
