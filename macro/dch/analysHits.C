{
  // Macro loads files with simulated MCpoints and reconstructed hits 
  // and plots difference between their positions
  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libgenfit");
  gSystem->Load("libDch");	 
  gSystem->Load("libGeom.so");
  
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(111111);
  gStyle->SetPalette(1,0);

  TDirectory* pwd = gDirectory;

  // define histograms
  TH1F* hdx = new TH1F("hdx","x_{point}-x_{hit} / mm", 40,-2.,2.);
  TH1F* hdy = new TH1F("hdy","y_{point}-y_{hit} / mm", 40,-2.,2.);
  TH1F* hdz = new TH1F("hdz","z_{point}-z_{hit} / mm", 40,-2.,2.);

  //input files
  TString inpointfile = "tst.root";
  TString inhitfile = inpointfile;
  inhitfile.ReplaceAll(".root","_idealHits.root");
  
  TFile* f = new TFile(inhitfile); //point-file you want to analyse
  TTree *thit=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hitArray=new TClonesArray("PndDchHit");
  thit->SetBranchAddress("PndDchHit",&hitArray);
  
  TFile* fsim = new TFile(inpointfile); //hit-file you want to analyse
  TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
  TClonesArray* pointArray=new TClonesArray("PndDchPoint");
  tsim->SetBranchAddress("PndDchPoint",&pointArray);
  
  Int_t nHits, nPoints;
  
  for (Int_t j=0; j< thit->GetEntriesFast(); j++){
    
    thit->GetEntry(j);
    tsim->GetEntry(j);
    
    nHits  = hitArray->GetEntriesFast();
    nPoints = pointArray->GetEntriesFast();
    for(Int_t i = 0; i<nHits; i++){
      cout<<"event "<<j<<endl;
      PndDchHit* hit = hitArray->At(i);
      Int_t pointidx = hit->GetRefIndex();
      PndDchPoint* point = pointArray->At(pointidx);

      hdx->Fill(10.*(point->GetX()-hit->GetX()));
      hdy->Fill(10.*(point->GetY()-hit->GetY()));
      hdz->Fill(10.*(point->GetZ()-hit->GetZ()));
    }
  }
  pwd->cd();
}

