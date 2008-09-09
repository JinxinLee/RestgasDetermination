

void noise2(TString filename,int padId,int jentry){
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

  double yvals[19200];
  double xvals[19200];
  double fftvals[19200];



  //for (Long64_t jentry=0; jentry<nentries;jentry++) {
    intree->GetEntry(jentry);
    int counter=0;
    for(int i=0;i<tNumberOfSamples;++i){
      if(tId[i]!=padId) continue;
      xvals[counter]=counter;
      yvals[counter]=tAmp[i];
      ++counter;
    }
    fft(counter,yvals,fftvals);
    for(int i=0;i<19200;++i) if(fftvals[i]<0.) fftvals[i]*=-1.;
    TGraph *gfft = new TGraph(counter,xvals,fftvals);
    gfft->SetTitle("frequency domain");
    TGraph *g = new TGraph(counter,xvals,yvals);
    g->SetTitle("time domain");

    TCanvas *c1=new TCanvas("c1","",0,0,500,500);
    g->Draw("A*");
    TCanvas *c2=new TCanvas("c2","",550,0,500,500);
    c2->SetLogy();
    gfft->Draw("A*");
    



    //TCanvas c2("c2","");
    //gfft->Draw("A*");

    //}
  

 

}
