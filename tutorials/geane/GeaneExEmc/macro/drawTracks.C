{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libGeom.so");

  gSystem->Load("libEmc");
  gSystem->Load("libStt");
  gSystem->Load("libMuo");
  gSystem->Load("libEmc");
  gSystem->Load("libTof");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libtpc");
  gSystem->Load("libMvd");
  gSystem->Load("libTrkBase");


  TFile* file = new TFile("mc.root");

  //drawing tracks
  TTree *t=file->Get("pndsim") ;

  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;


 TGeoTrack *tr;
 //TObjArray *TrList= geoMan->GetListOfTracks();
 for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(j);
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
    		tr->Draw("same");   // uncommit this to draw the tracks one by on
		//		TrList->AddLast(tr);
	}
 }

 // geoMan->AnimateTracks(0,1E-7, 500,"/G");	// uncommit this to animate the tracks

 //    geoMan->DrawTracks("same");  // this will draw all tracks added to the TrList at once

}

