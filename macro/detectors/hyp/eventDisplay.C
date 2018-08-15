

eventDisplay()
{
   // Load basic libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
  //gSystem->Load("libHypGe");
  //gSystem->Load("librazhyp");
  gSystem->Load("libHyp");
  //gSystem->Load("libTof"); 
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  //fRun->SetInputFile("points.x3872.jpsipipi.phsp.root");
 // fRun->SetInputFile("../qa/lhetrack/points_tpccombi.root");
  fRun->SetInputFile("sim_PiRec.root");//sim_pidC.root");
  ///d/pndint02/asanchez/FOct09/sim_hypFSG41TCIPAxxbG.root");//sim_with_vis.root");
  //  fRun->SetInputFile("../drc/testrun1.root");
  // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("tst.root");
  // fRun->LoadGeometry();

  
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  //  parInput1->open("../drc/testparams.root");
  parInput1->open("piRecparams.root");
  //../hyp/SimG41TCIPAxxbGparams.root");//params_with_vis.root");
       
  rtdb->setFirstInput(parInput1);
 

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  // FairMCPointDraw *MvdPoints =   new FairMCPointDraw ("MVDPoint",kBlue,  kFullSquare);
//   FairMCPointDraw *EMCPoints =   new FairMCPointDraw ("EmcPoint",kOrange,  kFullSquare);
  //FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kYellow,  kFullSquare);
  //FairMCPointDraw *TofSciFPoint= new FairMCPointDraw ("TofSciFPoint",kTeal, kFullSquare);
  FairMCPointDraw *MuoPoint =    new FairMCPointDraw ("HypPoint",kAzure, kFullSquare);
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
  gGeoManager->SetVisLevel(0);
    
  TGeoVolume* top = gGeoManager->GetTopVolume();
  //gGeoManager->SetVisLevel(4);

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
