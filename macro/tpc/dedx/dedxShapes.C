
{
 TFile *_file0 = TFile::Open("/nfs/data/panda-mc/AliceHP/AliceHP/NoMergeAliceP211_0.5-0.5_40-40_40-40_0.raw.root");
cbmsim->Draw("fDE/fDx*1e6>>h5(300,0,5)");
cbmsim->Draw("TruncateAndMean(0.,0.6)*1e6>>trunc5","","SAME");
h5->SetLineColor(kRed);
TH1F *h5=(TH1F*)gROOT->FindObject("h5"); 
trunc5->SetLineColor(kRed);
TH1F *trunc5=(TH1F*)gROOT->FindObject("trunc5"); 

TFile *_file0 = TFile::Open("/nfs/data/panda-mc/Hitm/Hitm/Hitm4cmNRP211_0.5-0.5_40-40_40-40_0.raw.root");
cbmsim->Draw("fDE/fDx*1e6>>h","","SAME");
cbmsim->Draw("TruncateAndMean(0.,0.6)*1e6>>trunc","","SAME");
h->SetLineColor(kBlue);
TH1F *h=(TH1F*)gROOT->FindObject("h"); 
trunc->SetLineColor(kBlue);
TH1F *trunc=(TH1F*)gROOT->FindObject("trunc"); 

TFile *_file0 = TFile::Open("/nfs/data/panda-mc/AliceHP/AliceHP/AliceHPP211_0.5-0.5_40-40_40-40_0.raw.root");
cbmsim->Draw("fDE/fDx*1e6>>h3","","SAME");
cbmsim->Draw("TruncateAndMean(0.,0.6)*1e6>>trunc3","","SAME");
h3->SetLineColor(kGreen);
TH1F *h3=(TH1F*)gROOT->FindObject("h3"); 
trunc3->SetLineColor(kGreen);
TH1F *trunc3=(TH1F*)gROOT->FindObject("trunc3"); 

TFile *_file0 = TFile::Open("/nfs/data/panda-mc/Hitm/Hitm/HitmP211_0.5-0.5_40-40_40-40_0.raw.root");
 cbmsim->Draw("fDE/fDx*1e6>>h7","","SAME");
 cbmsim->Draw("TruncateAndMean(0.,0.6)*1e6>>trunc7","","SAME");
h7->SetLineColor(kMagenta);
TH1F *h7=(TH1F*)gROOT->FindObject("h7"); 
trunc7->SetLineColor(kMagenta);
TH1F *trunc7=(TH1F*)gROOT->FindObject("trunc7"); 

TCanvas *scaledCanvas=new TCanvas("scaled","scaled");


float rightmax=h->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
h->Scale(scale);
h->Draw();

float rightmax=h5->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
h5->Scale(scale);
h5->Draw("SAME");

float rightmax=h3->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
h3->Scale(scale);
h3->Draw("SAME");

float rightmax=h7->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
h7->Scale(scale);
h7->Draw("SAME");


/*
float rightmax=trunc->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
trunc->Scale(scale);
trunc->Draw();

float rightmax=trunc5->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
trunc5->Scale(scale);
trunc5->Draw("SAME");

float rightmax=trunc3->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
trunc3->Scale(scale);
trunc3->Draw("SAME");

float rightmax=trunc7->GetMaximum();
float scale=gPad->GetUymax()/rightmax;
trunc7->Scale(scale);
trunc7->Draw("SAME");
*/
}