// root macro to analyze the simulation output
//void convertMCPoints()

Double_t fitf(Double_t *v, Double_t *par)
   {
      Double_t arg = 0;
      if (par[2] != 0) arg = (v[0] - par[1])/par[2];

      Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
      return fitval;
   }

void ana_track(){

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  gSystem->Load("libHyp");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gStyle->SetPalette(1);
  
  TFile* f = new TFile("HypIdrecdos2.root"); // the sim file you want to analyse
  TCanvas *c1 = new TCanvas("c1","the fit canvas",700,800);
  TH1F* h = (TH1F*)f->Get("PndHypDKalman/pH");
  h->Draw();
  
  cout<<" Peak search and gauss fit on the spectrum..."<<endl;
  //Use TSpectrum to find the peak candidates
  TSpectrum *s = new TSpectrum(50);
  Int_t nfound = s->Search(h,0.006,"",0.2);
  cout<<" **** "<<nfound <<" peaks are found by the peak search."<<endl;

  Float_t *xpeaks = s->GetPositionX();
  Float_t xp =xpeaks[0];
  Int_t bin = h->GetXaxis()->FindBin(xp);
  Float_t yp = h->GetBinContent(bin);
              
  cout<<" fit parameters "<<" Mean "<<xp<<" "<<" sigma "<<yp<<endl;
   

  // Creates a Root function based on function fitf above
  
  TF1 *func = new TF1("fitf",fitf,0.,0.2,3);
  
  // Sets initial values and parameter names
  
  func->SetParameter(0,yp);
 
  func->SetParName(0,"A");
  
  func->SetParameter(1,xp);
  //func->SetParLimits(1,,limr[1]);
  func->SetParName(1,"m2");
  
  func->SetParameter(2,0.003);
  //func->SetParLimits(2,0.1,10);//(1,19)
  func->SetParName(2,"sigma");
  
  cout << "Starting the fitting" << endl;


  h->Fit(func,"r");
    
  Float_t Mean =func->GetParameter(1);
  Float_t sig =func->GetParameter(2);



  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
