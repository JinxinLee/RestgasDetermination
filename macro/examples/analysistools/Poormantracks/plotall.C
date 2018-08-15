
int plotall(TString filename="Data/HistoVertexing.root")
{
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/Tools.C");
  LoadPandaStyle();
  //swotch things off
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  // use bigger font for many plots on a page
  gStyle->SetTextSize(0.10);
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetTitleSize(0.07,"x");
  gStyle->SetLabelSize(0.07,"y");
  gStyle->SetTitleSize(0.07,"y");
  gStyle->SetLabelSize(0.07,"z");
  gStyle->SetTitleSize(0.07,"z");
  gStyle->SetPadBottomMargin(0.17);

  plothistosfromfile(filename.Data(),"pdf",3,3);
  return 0;

}
