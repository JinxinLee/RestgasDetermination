

void noise1(TString filename){
  gROOT->ProcessLine(".L /sw/lib/libgsl.dylib");
  gROOT->ProcessLine(".L /sw/lib/libgslcblas.dylib");
  gROOT->ProcessLine(".L fft.cxx+");


  TFile* file=TFile::Open(filename);
  TTree* intree = (TTree*)file->Get("datatree");


  UInt_t          tTriggerInRun;
  UInt_t          tNumberOfSamples;
  UInt_t          tId[19200];   //[tNumberOfSamples]
  UInt_t          tAmp[19200];   //[tNumberOfSamples]
  UInt_t          tTime[19200];   //[tNumberOfSamples]
  intree->SetBranchAddress("tTriggerInRun", &tTriggerInRun);
  intree->SetBranchAddress("tNumberOfSamples", &tNumberOfSamples);
  intree->SetBranchAddress("tId", tId);
  intree->SetBranchAddress("tAmp", tAmp);
  intree->SetBranchAddress("tTime", tTime);

  //  Long64_t nentries = intree->GetEntriesFast();
  const int nentries = 99;
  double xvals[nentries];
  double yvals[nentries];
  double yerrs[nentries];
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    intree->GetEntry(jentry);
    int padId=10;
    double mean=0;
    double sigma=0;
    int counter=0;
    for(int i=0;i<tNumberOfSamples;++i){
      if(tId[i]!=padId) continue;
      ++counter;
      mean+=tAmp[i];
    }
    mean/=(double)counter;
    for(int i=0;i<tNumberOfSamples;++i){
      if(tId[i]!=padId) continue;
      sigma+=(tAmp[i]-mean)*(tAmp[i]-mean);
    }
    sigma/=(double)counter;
    sigma=TMath::Sqrt(sigma);
    xvals[jentry]=jentry;
    yvals[jentry]=mean;
    yerrs[jentry]=sigma;
    // if (Cut(ientry) < 0) continue;
  }
  
  TGraphErrors *g = new TGraphErrors(nentries,xvals,yvals,NULL,yerrs);
  g->Draw("A*");
 

}
