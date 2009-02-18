{
  // Macro loads files before and after digitization (containing PndDchPoints 
  // and PndDchDigis, respectively) reconstruction and plots drift time spectrum
  // for each chamber
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
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

  // create the histograms
  TH1F* hdt[9];
  TString xtitle = "drift time / ns";
  for(Int_t ch = 0; ch<9; ch++){
    TString name = "hdt";
    name +=ch;
    TString title = "TDC output for chamber ";
    title+=ch;
    hdt[ch] = new TH1F(name, title, 50, -50.,200.);
  }
  
  // input files
  TString inpointfile = "tst.root";
  TString indigifile = inpointfile;
  indigifile.ReplaceAll(".root","_digis.root");
  
  TFile* f = new TFile(indigifile); //digi-file you want to analyse
  TTree *tdigi=(TTree *) f->Get("cbmsim") ;
  TClonesArray* digiArray=new TClonesArray("PndDchDigi");
  tdigi->SetBranchAddress("PndDchDigi",&digiArray);
  
  TFile* fsim = new TFile(inpointfile); //point-file you want to analyse
  TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
  TClonesArray* pointArray=new TClonesArray("PndDchPoint");
  tsim->SetBranchAddress("PndDchPoint",&pointArray);
  
  Int_t nDigis, nPoints;
  // loop over events
  for (Int_t j=0; j< tdigi->GetEntriesFast(); j++){
    
    tdigi->GetEntry(j);
    tsim->GetEntry(j);
    
    nDigis  = digiArray->GetEntriesFast();
    nPoints = pointArray->GetEntriesFast();
    for(Int_t i = 0; i<nDigis; i++){
      cout<<"event # "<<j<<endl;
      PndDchDigi* digi = digiArray->At(i);
      hdt[digi->GetChamber()]->Fill(digi->GetDriftTime());
    }
  }
  pwd->cd();
  TCanvas* can = new TCanvas();
  can->Divide(3,3);
  for(Int_t i=2; i<9; i++){
    can->cd(i);
    if(hdt[i])
      hdt[i]->Draw();
  }
}

