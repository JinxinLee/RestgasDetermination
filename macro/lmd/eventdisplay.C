eventdisplay(TString storePath="/panda/pandaroot/macro/lmd/testPixelNewGeo/mom_15/")
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

  FairMCTracks *Track =  new FairMCTracks ("GeoTracks");
  fMan->AddTask(Track);
  FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("LMDPoint",kBlue,kFullSquare);
  fMan->AddTask(MvdPoints);
  FairHitDraw *stripHits = new FairHitDraw("LMDHitsPixel");
  fMan->AddTask(stripHits);

  // cout<<"And now we try draw reconstructed tracks"<<endl;
  // // /// Try to draw reconstructed tracks --------
  // PndTrackDraw* TrkRec = new PndTrackDraw("GeoTracks");
  // fMan->AddTask(TrkRec);
  // // ///------------------------------------------
  //  fRun->Init();
  fMan->Init();

  ///Draw everything -------------------------------------
  TGeoNode *N=  gGeoManager->GetTopNode();
  gGeoManager->DefaultColors(); // repair buggy coloring
  TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
  //TNod->SetVisLevel(3); // mandatory to see the Mvd
  //  TNod->SetVisLevel(9);
  TNod->SetVisLevel(20);
  gEve->AddGlobalElement(TNod);
  gEve->FullRedraw3D(kTRUE);
   ///---------------------------------------------

  // /// Show projections -----------------------------
  //   // camera
  //  TEveScene* s = gEve->SpawnNewScene("Projected Event");
  //  gEve->GetDefaultViewer()->AddScene(s);
  //  TGLViewer* v = gEve->GetDefaultGLViewer();
  //  v->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  //  TGLOrthoCamera* cam = (TGLOrthoCamera*) v->CurrentCamera();
  //  cam->SetZoomMinMax(0.2, 20);

  //  // projections
  //  TEveProjectionManager* mng =
  //     new TEveProjectionManager(TEveProjection::kPT_RPhi);
  //  s->AddElement(mng);
  //  TEveProjectionAxes* axes = new TEveProjectionAxes(mng);
  //  axes->SetTitle("TEveProjections demo");
  //  s->AddElement(axes);
  //  gEve->AddToListTree(axes, kTRUE);
  //  gEve->AddToListTree(mng, kTRUE);
  //  gEve->Redraw3D(kTRUE);
  // ///---------------------------------------------

}
