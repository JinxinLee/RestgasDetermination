eventdisplay(TString storePath="tmpOutput")
{
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libPndEventDisplay");
  gSystem->Load("libSds");
  gSystem->Load("libLmd");

  TString parFile = storePath+"/Lumi_Params_0.root";
  TString inSimuFile = storePath+"/Lumi_MC_0.root";
  //  TString inSimuFile = storePath+"/Lumi_MC_pp_4Pi_copy.root";
  //Lumi_MC_pp_4Pi_copy.root
  TString recoFile = storePath+"/Lumi_reco_0.root";
  TString trackingCandFile = storePath+"/Lumi_TCand_0.root";
  TString trackingFile = storePath+"/Lumi_Track_0.root";
  // TString trackingGeaneFile = storePath+"/Lumi_Geane_0.root";
  TString outFile = storePath+"/EVEout.root";

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inSimuFile.Data());
  fRun->AddFriend(recoFile.Data());
  fRun->AddFriend(trackingCandFile.Data());
  fRun->AddFriend(trackingFile.Data());
  // fRun->AddFriend(trackingGeaneFile.Data());
  fRun->SetOutputFile(outFile.Data());

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();

  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  fMan->AddTask(Track);
  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("LMDPoint",kBlue,kFullSquare);
  fMan->AddTask(MvdPoints);
  FairHitDraw *stripHits = new FairHitDraw("LMDHitsStrip");
  fMan->AddTask(stripHits);

  // cout<<"And now we try draw reconstructed tracks"<<endl;
  // // /// Try to draw reconstructed tracks --------
  // PndTrackDraw* TrkRec = new PndTrackDraw("GeoTracks");
  // fMan->AddTask(TrkRec);
  // // ///------------------------------------------
  //  fRun->Init();
  fMan->Init();
}
