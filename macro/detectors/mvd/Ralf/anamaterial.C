// root macro to analyze the material budget in the MVD
{
  int  nEvents = 1000;
  bool verbose = false;
  // -----  Load libraries   ------------------------------------------------
  gROOT->ProcessLine(".x ../Libs.C");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  PndFileNameCreator namecreator("/home/ralfk/MVD/mvdmacros/data/mvdevalg4.root");
  std::string inFile = namecreator.GetSimFileName(false);

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("pndsim") ;

//   TClonesArray* point_array=new TClonesArray("PndSdsMCPoint");
//   t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");


  for(Int_t event=0;event<nEvents && event<mc_array.GetEntriesFast();event++)
  {



  }










  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
