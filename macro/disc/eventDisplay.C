void eventDisplay(TString inFile = "sim_complete.root", TString parFile="simparams.root", TString outid="n"){
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  //fRun->AddFriend(digiFile);
  fRun->SetOutputFile("tst.root");
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile);      
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *DiscSensorPoint = new FairMCPointDraw ("DiscSensorMCPoint",kViolet, kFullSquare);
  
  fMan->AddTask(Track);
  fMan->AddTask(DiscSensorPoint);

  fMan->Init(1,4); 
  //fMan->Init(); 

  TFile* fi = new TFile("vgeo.root","RECREATE");
  TGeoVolume *topvol = gGeoManager->GetVolume("cave");
  topvol->CheckOverlaps(0.0001, "");
  gGeoManager->CheckOverlaps(0.00001,""); // [cm]
  TObjArray *listOfOverlaps = gGeoManager->GetListOfOverlaps();
  cout<<listOfOverlaps->GetEntries()<<endl;
  listOfOverlaps->Print(); 

  gGeoManager->SetTopVolume(topvol);
  gGeoManager->CloseGeometry();
  topvol->Write();
    
  FairEventManager* fm = FairEventManager::Instance();
  fm->NextEvent();
  TEveTrackList *eltrack = (TEveTrackList*) fm->FindChild("Cherenkov");

  int b=0;
  Double_t x,y,z;

  Bool_t storePYtracks = false;
  if(storePYtracks){
    TEveElement::List_t ll = eltrack->RefChildren();
    ofstream  file;
    file.open ("trackst.py");
    for ( TEveElement::List_i ii = ll.begin(); ii != ll.end();++ii){
      TEveTrack * tr = (TEveTrack*)(*ii);
      Int_t size = tr->Size();
      if(size<10) continue;
      TString  blendtrack = "listOfVectors = [";
      for(Int_t i=0; i<size; i++){
  	tr->GetPoint(i,x,y,z); 
  	blendtrack += Form("Vector((%f,%f,%f)),",x,y,z);
      }
      blendtrack = blendtrack.Strip(TString::kTrailing,',') + "] \n";
      blendtrack += Form("MakePolyLine(\"Track_%d\", \"TrackN_%d\", listOfVectors) \n",i,i);
      file<< blendtrack;
    }
    file.close();
  }
  
  // //fm->DisableListElements();
  // TEveElement *el = fm->FindChild("DrcPDPoint");
  // el->Destroy();
  // el = fm->FindChild("DrcEVPoint");
  // el->Destroy();

  TGLViewer *v = gEve->GetDefaultGLViewer();
  // v->SetClearColor(0);
  v->SetIgnoreSizesOnUpdate(kTRUE);
  v->UpdateScene(kTRUE);
  TGLSAViewer *sav = (TGLSAViewer *)v;
  sav->SetDrawCameraCenter(false);

  // TGLCamera & cam=(TGLOrthoCamera &)v->CurrentCamera();
  // Double_t c[] = {46.8, 8.9, -120.};
  // Double_t c[] = {10, 125, -125};
  // cam.Configure(16.5,1,c,-0.3,1.8);
  // cam.SetExternalCenter(false);

  TGLCamera & cam=(TGLOrthoCamera &)v->CurrentCamera();

  // Double_t c[] = {180, 0, -200}; // for hits 
  // cam.Configure(13,1,c,-0.2,1.3); // for hits

  // Double_t c[] = {90, 8.9, -150.};
  // cam.Configure(25,1,c,-0.4,0.9); // for geom
  // cam.SetExternalCenter(false);

  v->DoDraw();
  fm->EnableListElements();
  gEve->DoRedraw3D();
  v->DoDraw();
  v->SavePicture("geometry.png");
  // v->SavePicture("anim_"+outid+".png");
  
}


