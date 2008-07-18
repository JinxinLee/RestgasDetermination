void buildOccuPlots(TString infilename, TString outfilename){

  TFile* infile=new TFile(infilename,"READ");
  TH1D* hTimes=(TH1D*)infile->Get("TimesOverThres");
  TH2D* hTimesXY=(TH2D*)infile->Get("TimesXY");
  TH2D* hTimesR=(TH2D*)infile->Get("TimesR");

  TFile* outfile=new TFile(outfilename,"update");
  TH1D* hAllTimes=(TH1D*)outfile->Get("TimesOverThres");
  if(hAllTimes==NULL)hAllTimes=new TH1D("TimesOverThres","TimesOverThres",200,0,20000);
  TH2D* hAllTimesXY=(TH2D*)outfile->Get("TimesXY");
  if(hAllTimesXY==NULL)hAllTimesXY=new TH2D("TimesXY","TimesOverThres on Pads",420,-42.01,39.99,420,-42.01,39.99);

TH2D* hAllTimesR=(TH2D*)outfile->Get("TimesR");
  if(hAllTimesR==NULL)hAllTimesR=new TH2D("TimesR","TimesOverThres vs R",42,0,42,200,0,20000);

  outfile->cd();
  if(hTimes!=NULL){
    hAllTimes->Add(hTimes);
    hAllTimes->Write("TimesOverThres",TObject::kOverwrite);
  }
  if(hTimesXY!=NULL){
    hAllTimesXY->Add(hTimesXY);
    hAllTimesXY->Write("TimesXY",TObject::kOverwrite);
  }
  if(hTimesR!=NULL){
    hAllTimesR->Add(hTimesR);
    hAllTimesR->Write("TimesR",TObject::kOverwrite);
  }

  outfile->Close();
  delete outfile;
}
