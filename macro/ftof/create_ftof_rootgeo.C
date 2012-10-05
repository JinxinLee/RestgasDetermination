void create_ftof_rootgeo(Double_t distance = 775){
  // Forward tof geometry parameters
  //-----------------------------
  //-- macro created by Alicia S. based on
  //-- Panda TPR and on previous geant3 geometry for forward tof(Vladimir V.)
  const Double_t  kBCentX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBCentY    = 70.0;   //half length (cm) //?
  const Double_t  kBCentZ    = 0.75;   //half length (cm) //?

  const Double_t  kBBeamX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBBeamY    = 32.5;  //alf length (cm) //?
  const Double_t  kBBeamZ    = 0.75;   //half length (cm) //?

  const Double_t  kBVertX    = 5.0;   //half length (cm) //from EMC TDR
  const Double_t  kBVertY    = 70.0;   //half length (cm) //?
  const Double_t  kBVertZ    = 0.75;   //half length (cm) //from EMC TDR

 //--------------------------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  
  // Load this libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../geometry/ftofwall.root";
  TFile* fi = new TFile(outfile,"RECREATE");  
  
  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumSci  = Media->getMedium("polyvinyltoluene");
 
  
  Int_t nmed=geobuild->createMedium(CbmMediumSci);
 

  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  TGeoVolume *top = new TGeoVolumeAssembly("Ftof");
                    
  gGeoMan->SetTopVolume(top);
  

TGeoBBox *CentShape[22];
TGeoBBox *BeamShape[22];
TGeoBBox *VertShape[46];

TGeoVolume* CentVol[22];
TGeoVolume* BeamVol[22];
TGeoVolume* VertVol[46];


Double_t bx[22],by[22],bz[22];
Double_t bvx[46],bvy[46],bvz[46];
TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Ftof_strips");


char name[13];
char namB[13];
char namV[13];
char name1[13];
char namB1[13];
char namV1[20];
char name2[13];
char namB2[13];
char namV2[13];


for(int i=0;i<22;i++){
  bx[i]=-40+kBCentX*((2*i)+1);
}

for(int i=0;i<22;i++){
  if(i<9||i>12){
    
  Double_t offset;
  sprintf (name,"Centshape%d",i);
  sprintf (name1,"Ftof_Central_Strip%d",i);
 
  CentShape[i] = new TGeoBBox(name,kBCentX,kBCentY,kBCentZ);
  CentVol[i] = new TGeoVolume(name1,CentShape[i],gGeoMan->GetMedium("polyvinyltoluene"));
  if(i<9)TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[i],0,0.,new TGeoRotation ()); 
  else if (i>12)TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[i-2],0,0.,new TGeoRotation ());

  trc1->SetName(name);
  trc1->RegisterYourself();
  SubunitVol->AddNode(CentVol[i],i,trc1);
  }
  else{
    
  sprintf (namB,"Beamshape%d",i);
  sprintf (namB1,"Ftof_Beam_Strip%02d",i);
  BeamShape[i] = new TGeoBBox(namB,kBBeamX,kBBeamY,kBBeamZ);
  BeamVol[i] = new TGeoVolume(namB1,BeamShape[i],gGeoMan->GetMedium("polyvinyltoluene"));
  
  if(i==9||i==10){
    
    
    (i==9 ? (offset = -70+kBBeamY) : (offset = 70-kBBeamY));
    TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[9],offset,0.,new TGeoRotation ()); 
   
    trc1->SetName(namB);
    trc1->RegisterYourself();
    SubunitVol->AddNode(BeamVol[i],i,trc1);
   
  }
  if(i==11||i==12){
    
    (i==11 ?  (offset = -70+kBBeamY) : (offset = 70-kBBeamY));
    
    TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[10],offset,0.,new TGeoRotation ()); 
  
    trc1->SetName(namB);
    trc1->RegisterYourself();
    SubunitVol->AddNode(BeamVol[i],i,trc1);
    
  }
  }
  
}

for(int i=0;i<24;i++){
  bvx[i]=-280+kBVertX*((2*i)+1);
}

for(int i=0;i<22;i++){
  
  bvx[i+24]=60+kBVertX*((2*i)+1);
}

for(int i=0;i<46;i++){
  
    
 sprintf (namV,"VertShape%d",i);
  sprintf (namV1,"Ftof_Vertical_Strip%d",i);
 
  VertShape[i] = new TGeoBBox(namV,kBVertX,kBVertY,kBVertZ);
  VertVol[i] = new TGeoVolume(namV1,VertShape[i],gGeoMan->GetMedium("polyvinyltoluene"));
  TGeoCombiTrans* trc1= new TGeoCombiTrans(bvx[i],0,0.,new TGeoRotation ());

  trc1->SetName(namV);
  trc1->RegisterYourself();
  SubunitVol->AddNode(VertVol[i],i,trc1);
  }
  
  
top->AddNode(SubunitVol,0,new TGeoCombiTrans(0.,0.,distance,new TGeoRotation ()));

  gGeoMan->CloseGeometry();
  top->Write();
  fi->Close();
//   gGeoManager->Export(outfile);
  //top->Draw("ogl");
}  
