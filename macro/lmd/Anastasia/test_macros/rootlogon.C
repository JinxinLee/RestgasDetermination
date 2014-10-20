rootlogon()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  //gSystem->Load("libMemStat");
  // for counting rates drawing -----
  //  gStyle->SetOptStat(0); //no stat
  gStyle->SetOptFit(1);
  gStyle->SetLabelSize(0.05,"xyz");
  gStyle->SetTitleYSize(0.05);
  gStyle->SetTitleXSize(0.05);
  gStyle->SetTitleSize(0.05,"Z");
  gStyle->SetTitleOffset(0.9,"Y");
  gStyle->SetTitleOffset(0.9,"X");
  //  gStyle->SetTitleOffset(1.0,"X");
  gStyle->SetHatchesLineWidth(3);
  // gStyle->SetTitleOffset(1.5,"Y");
  // gStyle->SetPadLeftMargin(0.15);
  //  gStyle->SetPadRightMargin(0.17);
  // gStyle->SetTitleOffset(0.9,"Z");
  // gStyle->SetPalette(55,0);
  // for counting rates drawing -----
}

Bool_t isLibrary(const char* libName)
{
  if (TString(gSystem->DynamicPathName(libName, kTRUE)) != TString(""))
    return kTRUE;
  else
    return kFALSE;
}
