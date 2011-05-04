// ---------------------- CREATE GAS VOLUME ----------------------------------

TGeoVolumeAssenbly* createGas() {
  ;
}



  //---------------- CONSTRUCT FIELDCAGE BARREL ------------------------------
TGeoVolumeAssembly* createFieldCageBarrel() {
  double cageOut_meas[6] = { 41.,   //rMin
			     41.5,   //rMax
			     150./2.,   //dZ  HALF OF ACTUAL LENGTH (GEANT STYLE)
			     95.,
			     265. };
  double cageIn_meas[6] = { 15.,   //rMin
			    15.5,   //rMax
			    150./2.,   //dZ  HALF OF ACTUAL LENGTH (GEANT STYLE)
			    95.,
			    265. };

  TGeoRotation* rot = new TGeoRotation();
  rot->SetAngles(180.,0,0);

  //complete cage to return
  TGeoVolumeAssembly* FieldCage = new TGeoVolumeAssembly("FieldCage");
  //just one half:
  TGeoVolumeAssembly* FieldCage1 = new TGeoVolumeAssembly("FieldCage1");

  addFieldCageBarrelComponent("FC_ground", "aluminium", 
			      cageIn_meas, cageOut_meas, 
			      0.495,0.005,FieldCage1,kGray);
  addFieldCageBarrelComponent("FC_roha", "rohacell", 
			      cageIn_meas, cageOut_meas, 
			      0.001,0.494,FieldCage1,kYellow-2);
  addFieldCageBarrelComponent("FC_copper", "copper", 
			      cageIn_meas, cageOut_meas, 
			      0.,0.001,FieldCage1, kOrange+3);
    
  
  
  //now clone and rotate -----------------------------------
  TGeoVolumeAssembly* FieldCage2 = FieldCage1->Clone();
  
  FieldCage->AddNode(FieldCage1,1);
  FieldCage->AddNode(FieldCage2,1, rot);
  
  return FieldCage;
}

//helper function for creating the fieldcage more convenient
//distFromMin: taking as reference the OUTER fieldcage barrel wall
void addFieldCageBarrelComponent(TString name, TString matName, const double* tubeSegParsIn,
				 const double* tubeSegParsOut, 
				 double distFromMin, double thickness,
				 TGeoVolumeAssembly* ass, Color_t color) {
  
  //copy pars  and modify accordingly
  double locIn[6];
  double locOut[6];
  for(unsigned int i=0; i<6; i++) {
    locIn[i] = tubeSegParsIn[i];
    locOut[i] = tubeSegParsOut[i];
  }
  locIn[1] = locIn[1] - distFromMin;
  locOut[0] += distFromMin;
  locIn[0] = locIn[1] - thickness;
  locOut[1] = locOut[0] + thickness;
  
  //create outer and inner half cylinder 
  TString outname = "out_";
  outname.Append(name);
  TGeoTubeSeg* out1 = new TGeoTubeSeg(locOut);
  TGeoVolume* OUT1 = new TGeoVolume(outname, out1,
				    gGeoManager->GetMedium(matName));
  OUT1->SetLineColor(color);
  ass->AddNode(OUT1,1);
  TString inname = "in_";
  inname.Append(name);
  TGeoTubeSeg* in1 = new TGeoTubeSeg(locIn);
  TGeoVolume* IN1 = new TGeoVolume(inname, in1,
				   gGeoManager->GetMedium(matName));
  IN1->SetLineColor(color);
  ass->AddNode(IN1,1);

  //now make the flat bits in the target pipe wedge
  double length = locOut[1] - locIn[0];
  cout<<"Length: "<<length<<endl;
  TString wallName1 = "wall1_";
  TString wallName2 = "wall2_";
  wallName1.Append(name);
  wallName2.Append(name);
  TGeoBBox* wall1 = new TGeoBBox(thickness/2., length/2., locIn[2]);
  TGeoBBox* wall2 = wall1->Clone();
  //displace and rotate:
  TVector3 trans(distFromMin+thickness/2., length/2. + locIn[0], 0.);
  double angle = tubeSegParsIn[3] - 90.; //in deg
  trans.RotateZ(TMath::Pi()*angle/180.);
  TGeoTranslation* t1 = new TGeoTranslation(trans.X(), trans.Y(), trans.Z());
  TGeoRotation* rot1 = new TGeoRotation();
  rot1->SetAngles(angle,0.,0.); 
  TGeoCombiTrans* com1 = new TGeoCombiTrans(*t1,*rot1);
  TGeoVolume* vWall1 = new TGeoVolume(wallName1, wall1,
				      gGeoManager->GetMedium(matName));
  vWall1->SetLineColor(color);
  ass->AddNode(vWall1,1,com1);  

  
  TVector3 trans2(distFromMin+thickness/2., -(length/2. + locIn[0]), 0.);
  trans2.RotateZ(-TMath::Pi()*angle/180.);
  TGeoTranslation* t2 = new TGeoTranslation(trans2.X(), trans2.Y(), trans2.Z());
  TGeoRotation* rot2 = new TGeoRotation();
  rot2->SetAngles(-angle,0.,0.); 
  TGeoCombiTrans* com2 = new TGeoCombiTrans(*t2,*rot2); 
  TGeoVolume* vWall2 = new TGeoVolume(wallName2, wall2,
				      gGeoManager->GetMedium(matName));

  vWall1->SetLineColor(color);
  ass->AddNode(vWall1,1,com1);  
  vWall2->SetLineColor(color);
  ass->AddNode(vWall2,1,com2);  

  delete rot1;
  delete t1;
  delete rot2;
  delete t2;
}

void make_pandaTPC_geom() {
  using namespace std;

  //----------------------- INIT --------------------------------------

  //get geometry definitions:
  FairGeoLoader* loader = new FairGeoLoader("TGeo", "FairGeoLoader");
  FairGeoBuilder* builder = loader->getGeoBuilder();

  FairGeoInterface* geoIFC = loader->getGeoInterface();
  TString workdir = gSystem->Getenv("VMCWORKDIR");
  cout << workdir.Data() << endl;
  geoIFC->setMediaFile(workdir+"/geometry/media_pnd.geo");
  geoIFC->readMedia();
  //geoIFC->print();
  FairGeoMedia* media = geoIFC->getMedia();
  TGeoManager* geoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");

  //------------------ LOAD MATERIALS ----------------------------------

  FairGeoMedium* rohacell = media->getMedium("rohacell");
  builder->createMedium(rohacell);
  FairGeoMedium* aluminium = media->getMedium("aluminium");
  builder->createMedium(aluminium);
  FairGeoMedium* copper = media->getMedium("copper");
  builder->createMedium(copper);

  // ------------------ DRAW -------------------------------------------


  geoMan->SetTopVolume(createFieldCageBarrel());

  TEveManager::Create();
  TGeoNode* top = geoMan->GetTopNode();

  TEveGeoTopNode* meh = new TEveGeoTopNode(geoMan, top);
  gEve->AddGlobalElement(meh);

  gEve->Redraw3D(kTRUE);
  
  TFile outfile("PANDA_TPC.root", "recreate");
  outfile.cd();
  geoMan->Write();

}





  



