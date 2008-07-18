{
gROOT->Reset();
gROOT->SetStyle("Plain");
gStyle->SetPalette(1);
gStyle->SetOptStat("emr");
gStyle->SetStatColor(10);
gStyle->SetStatW(0.2);
gStyle->SetStatH(0.16);

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
gStyle->SetTitleX(0.18);
gStyle->SetTitleY(0.965);
gStyle->SetTitleAlign(13);
gStyle->SetTitleBorderSize(0);
gStyle->SetStatFont(102);
gStyle->SetStatFontSize(0.03);

gStyle->SetLabelOffset(.01);
//gStyle->SetLabelSize(0.035);
gStyle->SetTitleOffset(1.2);
//gStyle->SetLabelSize(0.035,"Y");
gStyle->SetTitleOffset(1.3,"Y");

gStyle->SetOptFit(1111);

gStyle->SetTitleSize(0.05,"a");
gStyle->SetLabelSize(0.05,"xyz");
gStyle->SetTitleSize(0.05,"xyz");

}
