/**

  create root-geo-file for disk DIRC

**/

{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPassive");

  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  geoFace->setMediaFile("../../geometry/media_pnd.geo");
  geoFace->readMedia();
  geoFace->print();

  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

  // Parameters
  TString fGeoFile= "../../geometry/dsk.root";

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  // Units
  Double_t const cm = 1.;
  Double_t const mm = cm/10.;
  Int_t const    colBlack  = 1;
  Int_t const    colYellow = 5;
  Int_t const    colRed    = 2;
  Int_t const    colBlue   = 4;
  Int_t const    colGreen  = 3;
  Int_t const    colGray   = 15;

  // number of edges of the disk
  Int_t const    fEdges = 8;
  // how many mirror types do we use?
  Int_t const    fDetectorTypes = 2;
  Int_t const    fDetectorColor[fDetectorTypes] = {colRed,colBlue};
  Int_t const    fMirrorColor[fDetectorTypes]   = {colRed,colBlue};
  Int_t const    fDetectorsPerEdge = 120;
  // center angle in each equal-sided triangle in the octagon
  Double_t const fAlpha    = 360. /fEdges;    // [degree]
  Double_t const fAlphaRad = fAlpha * TMath::DegToRad();
  // opening angle: disk is horizontal on top)
  Double_t const fPhi      = fAlpha /2.;      // [degree]
  //coating (air surrounding for reflections)
  Double_t const fCoatThickness = 1. *cm;
  // disk
  Double_t const fDiskThickness =   2. *cm;
  Double_t const fDiskRMin      =   3. *cm;  // could be something like the radius of beampipe 
  Double_t const fDiskDistanceZ = 196. *cm;
  Double_t const fDiskRMax      = fDiskDistanceZ * TMath::Tan(22.*TMath::DegToRad());   // ~79.19 cm
  // window
  Double_t const fWindowHeightHalf = fDiskDistanceZ * TMath::Tan( 5.*TMath::DegToRad()); // ~17.15 cm
  Double_t const fWindowWidthHalf  = fDiskDistanceZ * TMath::Tan(10.*TMath::DegToRad()); // ~34.56 cm
  Bool_t   const fWindowIsBox = kTRUE;
  // absorber in the window
  Double_t const fAbsorberThickness = 10 *mm; // arbitary number
  Bool_t   const fAbsorberUse = kTRUE;
  // mirror
  Double_t const fMirrorHeight = 0.5 *mm;     // arbitary value
  // mcp
  Double_t const fMcpHeight = 1. *cm;         // arbitary value
  // edge
  Double_t const fEdgeWidthHalf = fDiskRMax * TMath::Tan(fPhi * TMath::DegToRad());
  Double_t const fEdgeWidth     = 2. * fEdgeWidthHalf;
  // detector
  Double_t const fDetectorWidth  = fEdgeWidth/fDetectorsPerEdge;
  Double_t const fDetectorHeight = fMirrorHeight + fMcpHeight;

  // will help positioning
  TVector3     edgeStartPos;
  TVector3     corner;
  TGeoRotation nullRotation;
  TGeoRotation rotation;
  TString      name;
  TString      medium;

  FairGeoMedium* FairMediumAir      = geoMedia->getMedium("air");
  FairGeoMedium* FairMediumVacuum   = geoMedia->getMedium("vacuum");
  FairGeoMedium* FairMediumFusedSil = geoMedia->getMedium("FusedSil");
  FairGeoMedium* FairMediumDIRCAir  = geoMedia->getMedium("DIRCair");
  FairGeoMedium* FairMediumMirror   = geoMedia->getMedium("Mirror");

  geoBuild->createMedium(FairMediumAir);
  geoBuild->createMedium(FairMediumVacuum);
  geoBuild->createMedium(FairMediumFusedSil);
  geoBuild->createMedium(FairMediumDIRCAir);
  geoBuild->createMedium(FairMediumMirror);

  TGeoManager* gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");

  cout<<"-I- overall top"<<endl;
  // a copy of cave
  TGeoVolume* vTop;
  TGeoBBox*   lTop = new TGeoBBox(200,200,200);
  vTop = new TGeoVolume("top", lTop, gGeoManager->GetMedium("air"));
  gGeoManager->SetTopVolume(vTop);


  cout<<"-I- calculating local mother"<<endl;
  // now we create our local mother, that contains our detector. It has the same
  // octagonal shape as our whole detector, so we wont produce any overlap with
  // beampipe
  // z = -fCoatThickness ..... fDiskThickness + fCoatThickness
  TGeoVolume* vLocalMother;
  TGeoPgon*   lLocalMother = new TGeoPgon(fPhi, 360., fEdges, 2);
  // DefineSection(layer, z, inner Radius, outer Radius)
  lLocalMother->DefineSection(0, -fCoatThickness,
                                  fWindowHeightHalf - fAbsorberThickness - fCoatThickness,
                                  fDiskRMax + fMirrorHeight + fMcpHeight);
  lLocalMother->DefineSection(1,  fDiskThickness + fCoatThickness,
                                  fWindowHeightHalf - fAbsorberThickness - fCoatThickness,
                                  fDiskRMax + fMirrorHeight + fMcpHeight);
  vLocalMother = new TGeoVolume("mother", lLocalMother, gGeoManager->GetMedium("DIRCair"));
  vTop->AddNode(vLocalMother, 0, new TGeoCombiTrans(0, 0, fDiskDistanceZ, new TGeoRotation(0)));

  cout<<"-I- calculating radiator disk"<<endl;
  // the glass disk. Its an octagon with a window in the center. We will use a
  // box shape and substract it from the disk. The resulting shape should not
  // overlap with the beampipe volume.
  // z = 0 ..... fDiskThickness
  TGeoPgon* lDiskGlass = new TGeoPgon("DG",fPhi,360.,fEdges, 2);
  lDiskGlass->DefineSection(0,            0.,fDiskRMin,fDiskRMax);
  lDiskGlass->DefineSection(1,fDiskThickness,fDiskRMin,fDiskRMax);
  // i know, that the thickness is too high, but that doesnt matter, cause
  // we will cmoposite the shape, but now we are sure it wont overlapp!
  if (fWindowIsBox) {
    TGeoBBox* lDiskWindow = new TGeoBBox("DW",fWindowWidthHalf,fWindowHeightHalf,fDiskThickness);
  } else {
    TGeoEltu* lDiskWindow = new TGeoEltu("DW",fWindowWidthHalf,fWindowHeightHalf,fDiskThickness);
  }
  // we need to shift the box along z-axis, cause center should be fDiskThickness/2 further in z
  TGeoTranslation* trDW = new TGeoTranslation("trDW",0., 0., fDiskThickness/2.);
  trDW->RegisterYourself(); // and to register it for TGeoCompositeShape
  TGeoCompositeShape* lDisk = new TGeoCompositeShape("DG - DW:trDW");
  TGeoVolume* vDisk = new TGeoVolume("radiator", lDisk, gGeoManager->GetMedium("FusedSil"));
  vDisk->SetLineColor(colYellow);
  // AddNode(*daughter, copynumber, TGeoMatrix)
  vLocalMother->AddNode(vDisk,0,new TGeoCombiTrans(0., 0., 0., new TGeoRotation(0)));


  if (fAbsorberUse) {
  cout<<"-I- calculating absorber"<<endl;
  if (fWindowIsBox) {
  // in the window needs to be an absorber, (black colour) so the photons wont do
  // reflections back into the disk.
  // z = 0 ..... fDiskThickness
  TGeoBBox* lAbsorberOuter = new TGeoBBox("AO",fWindowWidthHalf,fWindowHeightHalf,fDiskThickness/2.);
  TGeoBBox* lAbsorberInner = new TGeoBBox("AI",fWindowWidthHalf-fAbsorberThickness,
                                               fWindowHeightHalf-fAbsorberThickness,fDiskThickness);
  } else {
  TGeoEltu* lAbsorberOuter = new TGeoEltu("AO",fWindowWidthHalf,fWindowHeightHalf,fDiskThickness/2.);
  TGeoEltu* lAbsorberInner = new TGeoEltu("AI",fWindowWidthHalf-fAbsorberThickness,
                                               fWindowHeightHalf-fAbsorberThickness,fDiskThickness);
  }
  TGeoCompositeShape* lAbsorber = new TGeoCompositeShape("AO - AI");
  TGeoVolume* vAbsorber = new TGeoVolume("absorber", lAbsorber, gGeoManager->GetMedium("vacuum"));
  vAbsorber->SetLineColor(colBlack);
  // again we need to shift them by the half fDiskThickness so place the center where it belongs
  vLocalMother->AddNode(vAbsorber,0,new TGeoCombiTrans(0., 0.,fDiskThickness/2., new TGeoRotation(0)));
  }


  cout<<"-I- calculating detectors (mother for mirror/mcp)"<<endl;
  // the detectors are container for a mirror and a mcp. We need as many types of detectors
  // as we have diffrent mirror types (usually 2)
  // their mother is the vArray, so we will have to use its reference frame
  TGeoBBox*   lDetector[fDetectorTypes];
  TGeoVolume* vDetector[fDetectorTypes];
  for (Int_t i=0; i<fDetectorTypes; i++) {
    lDetector[i] = new TGeoBBox(fDetectorWidth/2., fDetectorHeight/2., fDiskThickness/2.);
    name="detector"; name+=(i);
    vDetector[i] = new TGeoVolume(name.Data(),lDetector[i],gGeoManager->GetMedium("DIRCair"));
    vDetector[i]->SetLineColor(fDetectorColor[i]);
  }

  cout<<"-I- calculating mirrors"<<endl;
  // in every dectector is first a mirror.
  // their mother is vDetector[i], so we will have to use its reference frame
  TGeoBBox*   lMirror[fDetectorTypes];
  TGeoVolume* vMirror[fDetectorTypes];
  for (Int_t i=0; i<fDetectorTypes; i++) {
    lMirror[i] = new TGeoBBox(fDetectorWidth/2., fMirrorHeight/2., fDiskThickness/2.);
    name="mirror"; name+=(i);
    medium="Mirror";
    vMirror[i] = new TGeoVolume(name.Data(),lMirror[i],gGeoManager->GetMedium(medium.Data()));
    vMirror[i]->SetLineColor(fMirrorColor[i]);
    vDetector[i]->AddNode(vMirror[i],0,
                    new TGeoCombiTrans(0.,-fDetectorHeight/2.+fMirrorHeight/2.,0.,
                    new TGeoRotation(0)));
  }


  cout<<"-I- calculating mcps"<<endl;
  // The mcp is the actual detector. A photon that reaches the mcp will be detected.
  TGeoBBox*   lMcp = new TGeoBBox(fDetectorWidth/2.,fMcpHeight/2.,fDiskThickness/2.);
  TGeoVolume* vMcp[fDetectorTypes];
  for (Int_t i=0; i<fDetectorTypes; i++) {
    name="mcp"; name+=(i);
    vMcp[i] = new TGeoVolume(name.Data(),lMcp,gGeoManager->GetMedium("FusedSil"));
    vMcp[i]->SetLineColor(colGray);
    vDetector[i]->AddNode(vMcp[i],0,
                    new TGeoCombiTrans(0.,-fDetectorHeight/2.+fMirrorHeight+fMcpHeight/2.,0.,
                    new TGeoRotation(0)));
  }

  cout<<"-I- placing detectors"<<endl;
  edgeStartPos.SetXYZ(fEdgeWidthHalf-fDetectorWidth/2.,fDiskRMax+fDetectorHeight/2.,fDiskThickness/2.);
  for (Int_t iEdge=0; iEdge<fEdges; iEdge++) {
    for (Int_t iDet=0; iDet<fDetectorsPerEdge; iDet++) {
      corner = edgeStartPos;
      corner.SetX(corner.X()-iDet*fDetectorWidth);
      corner.RotateZ(iEdge*fAlphaRad);
      rotation = nullRotation;
      rotation.RotateZ(iEdge*fAlpha);
      vLocalMother->AddNode(vDetector[(fDetectorsPerEdge*iEdge+iDet)%fDetectorTypes],
                            fDetectorsPerEdge*iEdge + iDet,
                            new TGeoCombiTrans(corner.X(),corner.Y(),corner.Z(),
                            new TGeoRotation(rotation)));
    }
  }


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// End of detector construction code

  // close geometry
  gGeoManager->CloseGeometry();

  // save to file
  TFile* fi = new TFile(fGeoFile,"RECREATE");
  // writing the mother is sufficient, as it asks her daugthers to do the same.
  vTop->Write();
  fi->Close();

  cout << "Done." << endl;

  // lets see what we produced
  vTop->Raytrace();
}
