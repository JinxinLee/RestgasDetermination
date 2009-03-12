{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TFile* file = new TFile("lumisim.root");

  TGeoManager *geoMan = (TGeoManager*) file->Get("FAIRGeom");

  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 1100, 1100);
  c1->SetFillColor(10);

  geoMan->SetVisLevel(2);
  geoMan->GetMasterVolume()->Draw("same");
 // TTree *t=file->Get("cbmsim") ;
/*
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
*/

}
