rootlogon()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this libraries
  gSystem->Load("libDpmEvtGen.so");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libStt");
  gSystem->Load("libEmc"); 
  gSystem->Load("libgenfit");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpc"); 
  gSystem->Load("libtpcreco");
  gSystem->Load("libTrkBase");
  gSystem->Load("librecotasks");
  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libMdt");
  gSystem->Load("libDch");
  gSystem->Load("libLHETrack");
  gSystem->Load("libGeane");

  // Style options
  gStyle->SetPalette(1);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetLabelSize(0.05,"X");gStyle->SetLabelSize(0.05,"Y");
  gStyle->SetTitleSize(0.05,"X");gStyle->SetTitleSize(0.05,"Y");
  gStyle->SetTitleOffset(.9,"X");gStyle->SetTitleOffset(.9,"Y");
  gStyle->SetCanvasColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetLineWidth(2);
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat();
  //gStyle->SetOptFit(111);

}
