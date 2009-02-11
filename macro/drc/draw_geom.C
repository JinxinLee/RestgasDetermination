// Macro for displaying the tracks for the DRC simulation
// only the DRC detector is ON
// input file testrun.root contains the MC information
//28/09/2006 Pablo Genova
//28/06/2007 Annalisa Cecchi - modified some visualisation properties

{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  
  //  gSystem->Load("libEmc");
  gSystem->Load("libGen");
 
 
  TFile* file = new TFile("./testrun1.root");
  //  TFile* file = new TFile("Pi-100ev0.1_0.5.root");
  TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  
  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  c1->SetFillColor(10);

  geoMan->SetVisLevel(6);
  geoMan->GetMasterVolume()->Draw("same");

  TObjArray *TrArray=geoMan->GetListOfTracks();
  
  TView* view = c1->GetView();
  view->Top();
  view->Centered();
  //    view->SetParralel();
  
  //drawing tracks
  TTree *t=file->Get("cbmsim") ;
   
  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

  TGeoTrack *tr;
  TGeoTrack *tr1;
  Int_t N=  t->GetEntriesFast();
  // Int_t N=1;
  

 for (Int_t j=0; j< N; j++) {
      t->GetEntry(j);
      for (Int_t i=0; i<fT->GetEntriesFast(); i++) {
       tr=(TGeoTrack *)fT->At(i);
       //         
       tr->Draw("ogl");
  TrArray->AddLast(tr);
      }
   }
  //geoMan->AnimateTracks(0,1.,100, "ogl");

  //TrArray->Print();
  //geoMan->AnimateTracks();

  //  geoMan->DrawTracks();


}

