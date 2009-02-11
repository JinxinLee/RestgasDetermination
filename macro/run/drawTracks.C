{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libPndBase");
  gSystem->Load("libStt"); 
  gSystem->Load("libMdt");
  gSystem->Load("libEmc");
  gSystem->Load("libMvd");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");

  gSystem->Load("libGeom.so");
  TFile* f = new TFile("testsimu10.root");

  TTree *t=f->Get("cbmsim") ;


  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

 TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
 TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
 c1->SetFillColor(10);


 geoMan->GetTopVolume()->Draw("same");
 
TGeoTrack *tr;
 TObjArray *TrList= geoMan->GetListOfTracks(); 
 for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(j);
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
//     		tr->Draw("same");   // uncommit this to draw the tracks one by on
		TrList->AddLast(tr);
	}
 }
   geoMan->SetVisLevel(4);
 //geoMan->AnimateTracks(0,1E-7, 500,"/G");	// uncommit this to animate the tracks
  
   geoMan->DrawTracks("same");  // this will draw all tracks added to the TrList at once 

}

