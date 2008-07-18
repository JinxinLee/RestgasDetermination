// Macro for displaying the tracks for the STT1 simulation
// only the STT1 detector is ON
// input file testrun.root contains the MC information
//13/09/2006 Pablo Genova


{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libGen");
 
 
  TFile* file = new TFile("data/testChamber.mc.root");
  TGeoManager *geoMan = (TGeoManager*) file->Get("CBMGeom");

  
  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  c1->SetFillColor(10);

  geoMan->SetVisLevel(3);
  geoMan->GetMasterVolume()->Draw("same");
  

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
  
  for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
    t->GetEntry(j);
    for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
      tr=(TGeoTrack *)fT->At(i);
      tr->Draw("same");
    }
  }

    

}

