// DOESN'T WORK ATM!!!!! paths to input files need to be adapted

eventDisplay(TString InputFileExt = "TripleBall40Offset20STT_1MeV_1000EvtswithSecTar.root", TString InputSubDir = "Gamma")
{
   // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  //gSystem->Load("libHypGe");
  //gSystem->Load("librazhyp");
  gSystem->Load("libHypGe");
  //gSystem->Load("libTof"); 
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile("points.x3872.jpsipipi.phsp.root");
 // fRun->SetInputFile("../qa/lhetrack/points_tpccombi.root");
	TString InputFile = "$SIMDATADIR";
	InputFile+= "/";
	InputFile+= InputSubDir;
	InputFile+= "/";
	InputFile += InputFileExt;
	cout << InputFile << endl;
  fRun->SetInputFile(InputFile.Data());//sim_pidC.root");
  // /d/pndint02/asanchez/FOct09/sim_hypFSG41TCIPAxxbG.root");//sim_with_vis.root");
  //  fRun->SetInputFile("../drc/testrun1.root");
  // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("test.root");
  // fRun->LoadGeometry();

  TString ParFile = InputFile;
  if (ParFile.EndsWith(".root"))
		ParFile.ReplaceAll(".root", "__Simparams.root");
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  //  parInput1->open("../drc/testparams.root");
  parInput1->open(ParFile.Data());
  //../hyp/SimG41TCIPAxxbGparams.root");//params_with_vis.root");
       
  rtdb->setFirstInput(parInput1);
	
	parInput1->print();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  // FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
//   FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint",kOrange,  kFullSquare);
  //FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kYellow,  kFullSquare);
  //FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint",kTeal, kFullSquare);
  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("HypGePoint",kAzure, kFullSquare);
  // FairMCPointDraw *PndDrcPoint = new FairMCPointDraw ("PndDrcPoint",kViolet, kFullSquare);
//   FairMCPointDraw *PndDchPoint = new FairMCPointDraw ("PndDchPoint",kPink, kFullSquare);
  //FairMCPointDraw *PndTpcPoint = new FairMCPointDraw ("PndTpcPoint",kCyan,  kFullSquare);
 // FairMCPointDraw *PndSTTPoint = new FairMCPointDraw ("STTPoint",kMagenta, kFullSquare);

                                                               
  fMan->AddTask(Track);
  
  // fMan->AddTask(MvdPoints);
//   fMan->AddTask(EMCPoints);   
  //fMan->AddTask(TofPoint);   
  //fMan->AddTask( TofSciFPoint);
  fMan->AddTask( MuoPoint);
  // fMan->AddTask( PndDrcPoint);
//   fMan->AddTask( PndDchPoint);
  //fMan->AddTask( PndTpcPoint);
 // fMan->AddTask( PndSTTPoint);

  fMan->Init();                     
  char str[80];
   char str1[80];
   char str2[80];
   char  str3[80];
   
   TGeoNode* trk;
   TGeoNode* trl;
   TGeoNode* trab;
   TGeoNode* trsi;
   
  std::cout<<" gGeoM "<<gGeoManager<<std::endl;
  int k;
  
    
  TGeoVolume* top = gGeoManager->GetTopVolume();
  gGeoManager->SetVisLevel(12);

      
	//TEveScene* s = gEve->SpawnNewScene("Projected Event");
	//gEve->GetDefaultViewer()->AddScene(s);
	//TGLViewer* v = gEve->GetDefaultGLViewer();
	//v->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
	//TGLOrthoCamera* cam = (TGLOrthoCamera*) v->CurrentCamera();
	//cam->SetZoomMinMax(0.2, 20);
	
	//// projections
	//TEveProjectionManager* mng =
	//new TEveProjectionManager(TEveProjection::kPT_RPhi);
	//s->AddElement(mng);
	//TEveProjectionAxes* axes = new TEveProjectionAxes(mng);
	//axes->SetTitle("TEveProjections demo");
	//s->AddElement(axes);
	//gEve->AddToListTree(axes, kTRUE);
	//gEve->AddToListTree(mng, kTRUE);
	
	
	//TGeoNode *N=  gGeoManager->GetTopNode();
	//TEveGeoTopNode *TNod=new  TEveGeoTopNode(gGeoManager, N);
	//TEveGeoNode *TRealtop = new TEveGeoNode(gGeoManager->GetTopNode()->GetDaughter(0));
	//TEveGeoNode *TCrystal = new TEveGeoNode(gGeoManager->GetTopNode()->GetDaughter(0)->GetDaughter(0)->GetDaughter(0)->GetDaughter(1));
	//TNod->Print();
	//TRealtop->Print();
	//TCrystal->Print();
	//TCrystal->SetMainColor(kRed);
	//cout << "Children : " <<TNod->GetNItems()<<endl;
	////gEve->AddGlobalElement(TNod);
	//gEve->GetGlobalScene()->SetRnrState(kFALSE);
	//gEve->AddGlobalElement(TCrystal);
	//cout << "Import : " <<mng->ImportElements(TCrystal)<< endl;
	////TEveGeoShapeExtract* gse = (TEveGeoShapeExtract*) 
	////TEveGeoShapeExtract* gse = (TEveGeoShapeExtract*) gGeoManager->GetTopNode()->GetDaughter(0)->GetDaughter(0)->GetDaughter(3)->GetVolume()->GetShape();
	////TEveGeoShape* gsre = (TEveGeoShape*)gGeoManager->GetTopNode()->GetDaughter(0)->GetDaughter(0)->GetDaughter(3)->GetVolume()->GetShape();//::ImportShapeExtract(gse, 0);
	////cout << "Import : " <<mng->ImportElements(gsre)<< endl;
	//gEve->Redraw3D(kTRUE);
	
	
	
// the secondary target (detetctor volume) consist of 4 blocks
// denominated by "stg0%d ", each of this block acts as a mother volume
// contains 20 layers which also mother volume for a double layer "stglay%d_1".
// the double layer is composed by one silicon and one absorber layers
// These are the "real" detector volumes,
// the names of these volumes are given by "stglAb%d_1" and "stglSi%d_1".
/*
  for(int i=0;i<4;i++)
    {
      
  
      sprintf(str,"stg0%d_1",i+1);
      std::cout<<" name "<<str<<std::endl;
      
      TGeoNode* trk = top->FindNode(str);
      cout<<" tr "<<trk<<endl;
      
      for(int j=0;j<20;j++){
	if(i==0)  k = j;
	if(i==1)  k = j+20;
	if(i==2)  k = j+40;
	if(i==3)  k = j+60;
	
	sprintf(str1,"stglay%d_1",k);
	TGeoNode* trl = trk->GetVolume()->FindNode(str1);
	//cout<<" trl "<<trl<<" name "<<str1<<endl;
	
	
	sprintf(str2,"stglAb%d_1",k);
	TGeoNode* tra = trl->GetVolume()->FindNode(str2);
	//cout<<" tra "<<tra<< " same "<<str2<<endl;
	tra->GetVolume()->SetLineColor(kYellow);
	sprintf(str3,"stglSi%d_1",k);
	TGeoNode* trs = trl->GetVolume()->FindNode(str3);
	//cout<<" tra "<<tra<< " same "<<str2<<endl;
	trs->GetVolume()->SetLineColor(kMagenta);
	//fMan->AddGlobalElement(tra[k]);
      }
    }
*/
   
}
