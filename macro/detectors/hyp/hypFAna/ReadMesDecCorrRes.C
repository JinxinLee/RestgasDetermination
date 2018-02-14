{
TCanvas* c = new TCanvas("c1","c1");
c->Divide(2,2);
TFile* f1 = new TFile("AnaFSimpleP2Bup1TXXb.root");
TFile* f2 = new TFile("AnaFSimpleP4Bup1TXXb.root");


TFile* f3 = new TFile("AnaFSimpleP6Bup1TXXb.root");
TFile* f4 = new TFile("AnaFSimpleP8Bup1TXXb.root");
TH2D * h1 =(TH2D*)f1->Get("hvtx201");
TH2D * h2 =(TH2D*)f2->Get("hvtx201");

TH2D * h3 =(TH2D*)f3->Get("hvtx201");
TH2D * h4 =(TH2D*)f4->Get("hvtx201");
TH1D * h1p =(TH1D*)h1->ProjectionX(" proj_y1",0,-1,"");
TH1D * h2p =(TH1D*)h2->ProjectionX(" proj_y2",0,-1,"");

TH1D * h3p =(TH1D*)h3->ProjectionX(" proj_y3",0,-1,"");

TH1D * h4p =(TH1D*)h4->ProjectionX(" proj_y4",0,-1,"");
c->cd(1);

h1p->Draw();
c->cd(2);
h2p->Draw();

c->cd(3);
h3p->Draw();

c->cd(4);
h4p->Draw();
}
