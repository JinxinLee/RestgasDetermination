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

  gSystem->Load("libMvd");

  gSystem->Load("libGeom.so");
  TFile* file = new TFile("Mvd_Test.root");

  TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
  c1->SetFillColor(10);

  geoMan->SetVisLevel(8);
  geoMan->DefaultColors();
  geoMan->GetMasterVolume()->Draw("");
  
  //drawing tracks
   TTree *t=file->Get("cbmsim") ;
    
   TClonesArray *fT=new TClonesArray("TGeoTrack");

   t->SetBranchAddress("GeoTracks",&fT) ;

   TGeoTrack *tr;
   TGeoTrack *tr1;
   
   for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
     t->GetEntry(j);
     for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
       tr=(TGeoTrack *)fT->At(i);
       tr->Draw("same");
     }
   }
}

