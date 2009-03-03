{
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
//setup dirc parameters
//note somewhere .geo size parameters are converted from mm to cm
//Here I define the .root geometry directly in cm
  const Int_t nsegs      =128;           // Number of segments
  //const Int_t npoints    = 8;           // Number of points in shape
  Double_t    dist_z1    = 0.;//cm      // distance from beam axis (=z)
  Double_t    dist_z2    = 1100./10;//cm      // outer distance from beam.
  Double_t thickness = 15./10;//cm             //thickness of the dirc
  Double_t    angle  = 2*TMath::Pi()/nsegs; 
  //double    angle  = 360./nsegs; 
  Double_t ztarget =1945./10; //cm distance to front face +thick/2
  Double_t LiFThick=50./10;//cm

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libPassive");
  
  TString outfile= "../../geometry/endrc.root";
  TFile* fi = new TFile(outfile,"RECREATE");  

  FairGeoLoader* geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface *geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../../pandaroot/geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumDIRCair  = Media->getMedium("DIRCair");
  FairGeoMedium *CbmMediumDIRCglass = Media->getMedium("FusedSil");
  FairGeoMedium *CbmMediumDIRClg = Media->getMedium("carbon");
  
  Int_t nmed=geobuild->createMedium(CbmMediumDIRCair);
  nmed=geobuild->createMedium(CbmMediumDIRCglass);
   nmed=geobuild->createMedium(CbmMediumDIRClg);
 
  TGeoManager* gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  //Make mother volume
  TGeoVolume *top = gGeoMan->MakeTube("EnDrc",gGeoMan->GetMedium("DIRCair"),0,dist_z2+20. ,5.);//cm
  gGeoMan->SetTopVolume(top);


  //Make the disc
  //first trapezoid segment shape
  TString segName="dseg";
  TGeoTrd1 *dsegShape=new TGeoTrd1(segName,0.,dist_z2*tan(angle/2),thickness/2,dist_z2/2);
  //dsegShape->RegisterYourself();

  TGeoRotation *rot;
  TString rotName;
  char rotNamech[10];
  TString compShape;
  TGeoTranslation *trans;
  TGeoCombiTrans* combi;
 
  //now loop over and create rotation matrices for segments 
  if(nsegs%4!=0) return; //can#t split into 4
  for(Int_t i=0;i<nsegs/4;i++){
  //others rotated by seg*angle
    sprintf(rotNamech,"drot%d",i);
    rotName=rotNamech;
    rot=new TGeoRotation(rotName);
    //rot->RegisterYourself();
    rot->RotateY((angle*i+angle/2)*TMath::RadToDeg());
    sprintf(rotNamech,"dtra%d",i);
    rotName=rotNamech;
    TVector3 temp(0,0,dist_z2/2);
    temp.RotateY(angle*i+angle/2);
    trans=new TGeoTranslation(rotNamech,temp.X(),temp.Y(),temp.Z());
    combi=new TGeoCombiTrans(*trans,*rot);
    sprintf(rotNamech,"dcom%d",i);
    rotName=rotNamech;
    combi->SetName(rotName);
    combi->RegisterYourself();
    compShape+="(";
    compShape+=segName;
    compShape+=":";
    compShape+=rotName;
    compShape+="-hole:thole)";
    //compShape+=")";
    if(i!=nsegs/4-1)compShape+="+";
  }
  //now make rectangular hole
  TGeoBBox* holeShape=new TGeoBBox("hole",ztarget*tan(5*TMath::DegToRad()),thickness,ztarget*tan(10*TMath::DegToRad()));
  trans=new TGeoTranslation("thole",0,0,0);
  trans->RegisterYourself();
  //   compShape+="-(hole:thole)";
  //now make composite shape of disc
  //  compShape="(dseg:dcom4-hole:thole)+(dseg:dcom5-hole:thole)";
  cout<<"Composite shape "<<compShape<<endl;
  TGeoCompositeShape* discShape=new TGeoCompositeShape("dshape",compShape);
  TGeoVolume* disc=new TGeoVolume("disc",discShape,gGeoMan->GetMedium("FusedSil"));
  // TGeoVolume* disc=new TGeoVolume("disc",dsegShape);
   TGeoRotation* rotCave=new TGeoRotation("drotCave");
   rotCave->RotateX(90);
   rotCave->RotateZ(90);
   //currently  a problem when loading into PandaRoot
   //After running simulation and drawing resulting detector get a crash on exit
   //seems to be due to the combination of TGeoCompositeShape placed with a TGeoMatrix
   //Note 6 times faster to split  disc into 4, than to have just one volume
    top->AddNode(disc,1,new TGeoCombiTrans(TGeoTranslation(0,0,0),*rotCave));
    rotCave->ReflectX(kFALSE);
    top->AddNode(disc,2,new TGeoCombiTrans(TGeoTranslation(0,0,0),*rotCave));
    //rotCave->ReflectY(kFALSE);
   rotCave->RotateZ(-180);
   top->AddNode(disc,3,new TGeoCombiTrans(TGeoTranslation(0,0,0),*rotCave));
   rotCave->ReflectX(kFALSE);
   top->AddNode(disc,4,new TGeoCombiTrans(TGeoTranslation(0,0,0),*rotCave));
  // Now add LiF elements to the perimeter of the disc
  TGeoBBox* LiFShape=new TGeoBBox("LiFShape",dist_z2*tan(angle/2),thickness/2,LiFThick/2);
  //Add the "light guides"
  TGeoBBox* LGShape=new TGeoBBox("LGShape",dist_z2*tan(angle/2),thickness/2,LiFThick); 
  TGeoVolume* lif;
  TGeoVolume* lg;
  for(Int_t i=0;i<nsegs;i++){
    sprintf(rotNamech,"lrot%d",i);
    rotName=rotNamech;
    rot=new TGeoRotation(rotName);
    //rot->RegisterYourself();
    rot->RotateX(90);
    rot->RotateZ(90);
    rot->RotateZ((angle*i+angle/2)*TMath::RadToDeg());
    TVector3 temp(0,0,dist_z2+LiFThick/2);
    temp.RotateY(angle*i+angle/2);
    temp.RotateX(90*TMath::DegToRad());
    temp.RotateZ(90*TMath::DegToRad());
    trans=new TGeoTranslation(rotNamech,temp.X(),temp.Y(),temp.Z());
    combi=new TGeoCombiTrans(*trans,*rot);
    sprintf(rotNamech,"lifc%d",i);
    rotName=rotNamech;
    combi->SetName(rotName);
    sprintf(rotNamech,"lif%d",i);
    rotName=rotNamech;
    //position LiF
    lif=new TGeoVolume(rotName,LiFShape,gGeoMan->GetMedium("FusedSil"));
    top->AddNode(lif,i+1,combi);
    //position light guide
    sprintf(rotNamech,"lg%d",i);
    rotName=rotNamech;
    trans=new TGeoTranslation(rotNamech,temp.X(),temp.Y(),temp.Z()-thickness);
    combi=new TGeoCombiTrans(*trans,*rot);
    //lg=new TGeoVolume(rotName,LGShape,gGeoMan->GetMedium("carbon"));
    lg=new TGeoVolume(rotName,LGShape,gGeoMan->GetMedium("FusedSil"));
    top->AddNode(lg,i+1,combi);
 

    //combi->RegisterYourself();
  }
  TGeoHMatrix *global = gGeoMan->GetHMatrix();             
  gGeoMan->GetListOfMatrices()->Remove(global);

  //Position in lab
  trans=new TGeoTranslation("EnDrcPos",0,0,ztarget);
  trans->Write();

  gGeoMan->CloseGeometry();
  top->Draw();
  //top->Raytrace();
  // top->Export("../../geometry/endrc_vonly.root","EnDrc","recreate");
  //TFile* ft=new TFile("../../geometry/endrc_vonly.root","update");
  //trans->Write();
  //ft->Close();
  top->Write();
  fi->Close();

}
