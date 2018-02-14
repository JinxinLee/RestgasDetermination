{
  // Macro created 19/09/2006 by S.Spataro
  // It loads a simulation file and plot the geometry
  
  // Loading libraries
  // If the macro gives error messages in loading libraries, please check the path of the libs and put it by hands

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  
  // add the detector libraries you need
  //gSystem->Load("libStt");
  //gSystem->Load("libMuo");
  gSystem->Load("libHyp");	 
  gSystem->Load("libGeom.so");
  
  TFile* f = new TFile("./sim_hyp.root"); // the sim file you want to analyse
  
  TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");

  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800); // to create a canvas where to plot the geometry
  c1->SetFillColor(10);
  
  geoMan->SetVisLevel(4); // Number of folded geo levels; 3 for emc. Try to change the number and to see how the geometry changes.
  
  geoMan->GetMasterVolume()->Draw("same");
  
  TView* view = c1->GetView();
  view->Top();
  view->Centered();

 //drawing tracks
/*
  TTree *t=file->Get("cbmsim") ;
   
  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

  TGeoTrack *tr;
  TGeoTrack *tr1;

  for (Int_t j=0; j< t->GetEntriesFast(); j++) 
    {
      t->GetEntry(j);
      for (Int_t i=0; i<fT->GetEntriesFast(); i++) 
	{
	  tr=(TGeoTrack *)fT->At(i);
	  tr->Draw("same");
	}
    }*/

}

