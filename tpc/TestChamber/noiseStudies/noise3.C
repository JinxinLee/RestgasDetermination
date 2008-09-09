

void noise3(TString filename,int padId,int jentry,double freq,double amp){
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
  double fftxvals[19200];
  double fftvals[19200];



  intree->GetEntry(jentry);
  int counter=0;
  for(int i=0;i<tNumberOfSamples;++i){
    if(tId[i]!=padId) continue;
    xvals[counter]=counter;
    yvals[counter]=tAmp[i];
    ++counter;
  }

  for(int i=0;i<counter;++i){
    yvals[i]+=amp*TMath::Cos(2.*TMath::Pi()*freq*i);

    //osc[i]=amp*TMath::Cos(freq*i);//+2.*amp*TMath::Cos(2.*freq*i);
  }

  for(int i=0;i<19200;++i){
    fftxvals[i]=0.5/counter * xvals[i];
  }

  fft(counter,yvals,fftvals);

  

  for(int i=0;i<19200;++i) if(fftvals[i]<0.) fftvals[i]*=-1.;
  TGraph *gfft = new TGraph(counter-1,&fftxvals[1],&fftvals[1]);
  gfft->SetTitle("");
  gfft->SetMarkerStyle(20);
  gfft->SetMarkerColor(12);
  gfft->SetMarkerSize(0.8);
  gfft->GetXaxis()->SetTitle("frequency from FFT [10 MHz]");
  TGraph *g = new TGraph(counter,xvals,yvals);
  g->SetTitle("");
  g->SetMarkerStyle(20);
  g->SetMarkerColor(12);
  g->SetMarkerSize(0.8);
  g->GetXaxis()->SetTitle("sample time [100 ns]");
  TCanvas *c1=new TCanvas("c1","",0,0,500,500);
  g->Draw("AP");
  TCanvas *c2=new TCanvas("c2","",550,0,500,500);

  //c2->SetLogy();
  gfft->Draw("AP");
  
  
  
  
}
