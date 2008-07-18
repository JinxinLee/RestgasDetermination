{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
    gSystem->Load("libMCStack");
  gSystem->Load("libStt1"); 
  gSystem->Load("libLHETrack");
  gSystem->Load("libMdt");
 
  

  gSystem->Load("libGeom.so");
  TFile* f1 = new TFile("dpmtest.root");

  TGeoManager *geoMan = (TGeoManager*) f1->Get("CBMGeom");
  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  c1->SetFillColor(10);
//  TView *v= c1->GetView();

  TFile* f = new TFile("test.raw.root");

  TTree *t=f->Get("cbmsim") ;


  TClonesArray *fCan=new TClonesArray("TGeoTrack");
  TClonesArray *fFit=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("TrackCand",&fCan) ;
  t->SetBranchAddress("TrackFit",&fFit) ;

  geoMan->GetTopVolume()->Draw("same");
  
  TGeoTrack *tr;
  TGeoTrack *tr1;
  TObjArray *TrList= geoMan->GetListOfTracks(); 
  for (Int_t j=0; j< 1; j++)	{ 
    t->GetEntry(j);
    for (Int_t i=0; i<fCan->GetEntriesFast(); i++)	{
      tr=  (TGeoTrack *)fCan->At(i);
      tr1= (TGeoTrack *)fFit->At(i);
      
      tr1->SetLineColor(kRed);
      tr->SetLineColor(kGreen);
      tr->Draw("same");   // uncommit this to draw the tracks one by on
      tr1->Draw("same");   // uncommit this to draw the tracks one by on
      
     TrList->AddLast(tr);
     TrList->AddLast(tr1);
   }
 }
 
 
  geoMan->SetVisLevel(4);
}

