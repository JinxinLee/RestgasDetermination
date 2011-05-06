//Create TPC geometry
//Author: Felix Boehmer


// -------------------- DRIFT CATHODE SETUP ---------------------------------
TGeoVolumeAssembly* createDriftCathode() {
  double cath_pars[3] = { 15.5,   //rMin
			  43.,   //rMax
			  0.05/2.};   //dZ  HALF OF ACTUAL LENGTH 

  TGeoVolumeAssembly* Cathode = new TGeoVolumeAssembly("Cathode");
  
  //local positioning:
  double dz = 75.;
  
  //layers:
  unsigned int nL = 3;
  double thicks[3] = {0.05, 0.5, 0.002};
  TString mats[3] = {"kapton",
		     "rohacell",
		     "aluminium"};
  Color_t cols[3] = {kYellow+8,
		     kOrange+4,
		     kGray};
  
  TGeoVolume* vols[3];
  TGeoTube* shapes[3];
  double totT = 0.;
  
  for(unsigned int i=0; i<nL; i++) {
    cath_pars[2] = thicks[i]/2.;
    TString name("cath_");
    name.Append(mats[i]);
    shapes[i] = new TGeoTube(cath_pars);
    vols[i] = new TGeoVolume(name,
			     shapes[i],
			     gGeoManager->GetMedium(mats[i]));
    vols[i]->SetLineColor(cols[i]);
    Cathode->AddNode(vols[i], 1,
    		     new TGeoTranslation(0.,0.,dz + thicks[i]/2. + totT));
    totT+=thicks[i];
  }
  return Cathode;
}


// -------------------- DRIFT PADPLANE SETUP ---------------------------------
TGeoVolumeAssembly* createPadPlane() {
  double cath_pars[3] = { 15.5,   //rMin
			  43.,   //rMax
			  0.05/2.};   //dZ  HALF OF ACTUAL LENGTH 

  TGeoVolumeAssembly* PadPlane = new TGeoVolumeAssembly("PadPlane");
  
  //local positioning:
  double dz = -75.;
  
  //layers:
  unsigned int nL = 2;
  double thicks[2] = {0.0035, 0.3};
  TString mats[2] = {"copper",
		     "G10"};
  Color_t cols[2] = {kYellow,
		     kGreen+3};
		     
  TGeoVolume* vols[2];
  TGeoTube* shapes[2];
  double totT = 0.;
  
  for(unsigned int i=0; i<nL; i++) {
    double loc_pars[3];
    for(unsigned int l=0; l<3; l++)
      loc_pars[l]=cath_pars[l];
    cath_pars[2] = thicks[i]/2.;
    if(mats[i] == "copper")
      loc_pars[1]=41.;
    TString name("pplane_");
    name.Append(mats[i]);
    shapes[i] = new TGeoTube(loc_pars);
    vols[i] = new TGeoVolume(name,
			     shapes[i],
			     gGeoManager->GetMedium(mats[i]));
    vols[i]->SetLineColor(cols[i]);
    PadPlane->AddNode(vols[i], 1,
		      new TGeoTranslation(0.,0.,dz - thicks[i]/2. - totT));
    totT+=thicks[i];
  }
  return PadPlane;
}

// ----------------------- FE CARDS STRUCTURE ----------------------------------

TGeoVolumeAssembly* createFE() {
  unsigned int nCards = 42;
  double thickness = 0.2;  //real measures, not stupid half-measures
  double length = 19.;
  double height = 8.5;
  double chip_thickness = 0.2;
  double chip_side = 3.; 
  
  TGeoVolumeAssembly* FullFE = new TGeoVolumeAssembly("FullFE");

  //build template FE card:
  TGeoVolumeAssembly* FECard = new TGeoVolumeAssembly("FECard");
  TGeoBBox* pcb = new TGeoBBox(thickness/2., length/2., height/2.);
  TGeoVolume* pcbV = new TGeoVolume("pcb0", pcb,
				    gGeoManager->GetMedium("G10"));
  pcbV->SetLineColor(kGreen+3);
  FECard->AddNode(pcbV,1);
  //create chips:
  TGeoVolumeAssembly* ChipPair = new TGeoVolumeAssembly("ChipPair");
  TGeoBBox* chip = new TGeoBBox(chip_thickness/2., chip_side/2., chip_side/2.);
  TGeoVolume* chipV1 = new TGeoVolume("chip0", chip,
				     gGeoManager->GetMedium("silicon"));
  chipV1->SetLineColor(kBlack);
  TGeoVolume* chipV2 = chipV1->Clone();
  //equip FE card:
  TGeoTranslation* transCh1 = new TGeoTranslation(chip_thickness/2. + thickness/2.,0,0);
  TGeoTranslation* transCh2 = new TGeoTranslation(-(chip_thickness/2.+ thickness/2.),0,0);
  FECard->AddNode(chipV1,1,transCh1);
  FECard->AddNode(chipV2,1,transCh2);
  
  TGeoTranslation* transC1 = new TGeoTranslation(chip_thickness/2. + thickness/2.,length/3.,0);
  TGeoTranslation* transC2 = new TGeoTranslation(chip_thickness/2. + thickness/2.,-length/3.,0);
  TGeoTranslation* transC3 = new TGeoTranslation(-(chip_thickness/2. + thickness/2.),length/3.,0);
  TGeoTranslation* transC4 = new TGeoTranslation(-(chip_thickness/2. + thickness/2.),-length/3.,0);
  //FECard->AddNode(ChipPair,0);
  FECard->AddNode((TGeoVolume*)chipV1->Clone(),1, transC1);
  FECard->AddNode((TGeoVolume*)chipV1->Clone(),1, transC2);
  FECard->AddNode((TGeoVolume*)chipV1->Clone(),1, transC3);
  FECard->AddNode((TGeoVolume*)chipV1->Clone(),1, transC4);
    
  //finalize:
  for(unsigned int ife=0; ife<nCards; ife++) {
    TVector3 shift(0,(41.5-15.0)/2.+15.,0);
    double angle = 360./nCards;
    shift.RotateZ(ife*angle*TMath::Pi()/180.);
    TGeoTranslation* trans = new TGeoTranslation(shift.X(),shift.Y(),-76-height/2.);
    TGeoRotation* rot = new TGeoRotation();
    rot->SetAngles(angle*ife,0,0);
    TGeoCombiTrans* com = new TGeoCombiTrans(*trans,*rot);
    FullFE->AddNode((TGeoVolume*)FECard->Clone(),1,com);
  }
    
  return FullFE;

}




// ------------------------- CREATE COOLING --------------------------------
TGeoVolumeAssembly* createCooling() {
  TGeoVolumeAssembly* Cooling = new TGeoVolumeAssembly("Cooling");
  double cool_pars[6] = { 41.,   //rMin
			  43.,   //rMax
			  0.2./2.,   //dZ  HALF OF ACTUAL LENGTH 
			  95.,
			  265.};
  TGeoVolumeAssembly* Half1 = new TGeoVolumeAssembly("Cooling1");
  TGeoTubeSeg* seg1 = new TGeoTubeSeg(cool_pars);
  TGeoVolume* ring1 = new TGeoVolume("CoolingRing1",
				     seg1, gGeoManager->GetMedium("copper"));
  ring1->SetLineColor(kRed+3);
  TGeoVolume* ring2 = ring1->Clone();
  ring2->SetName("CoolingRing2");
  TGeoTranslation* trans1 = new TGeoTranslation(0.,0.,-86.);
  TGeoTranslation* trans2 = new TGeoTranslation(0.,0.,-88.);
  Half1->AddNode(ring1, 1, trans1);
  Half1->AddNode(ring2, 1, trans2);
  
  //create second half
  TGeoVolumeAssembly* Half2 = Half1->Clone();
  Half2->SetName("Cooling1");
  TGeoRotation* rot = new TGeoRotation();
  rot->SetAngles(180.,0,0);
  
  Cooling->AddNode(Half1,1);
  Cooling->AddNode(Half2,1,rot);

  return Cooling;
  
}


// ---------------------- CREATE GAS VOLUME ---------------------------------

TGeoVolumeAssembly* createGas() {
  double gas_pars[6] = { 15.5,   //rMin
			 41.,   //rMax
			 150./2.,   //dZ  HALF OF ACTUAL LENGTH 
			 95.,
			 265. };
  TGeoVolumeAssembly* Gas = new TGeoVolumeAssembly("Gas");
  TGeoTubeSeg* gs = new TGeoTubeSeg(gas_pars);
  TGeoVolume* gas = new TGeoVolume("gas1", gs,
				   gGeoManager->GetMedium("TPCmixture"));
  gas->SetLineColor(kBlue+11);
  gas->SetTransparency(50);
  Gas->AddNode(gas,1);
  TGeoRotation* rot = new TGeoRotation();
  rot->SetAngles(180.,0,0);
  TGeoVolume* gas2 = gas->Clone();
  Gas->AddNode(gas2,1, rot);
  return Gas;  
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

  double length = 150.;

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
  FairGeoMedium* gas = media->getMedium("TPCmixture");
  builder->createMedium(gas);
  FairGeoMedium* kapton = media->getMedium("kapton");
  builder->createMedium(kapton);
  FairGeoMedium* G10 = media->getMedium("G10");
  builder->createMedium(G10);
  FairGeoMedium* silicon = media->getMedium("silicon");
  builder->createMedium(silicon);
  // ------------------ DRAW -------------------------------------------

  TGeoVolume* top = new TGeoVolumeAssembly("TopNode");
  geoMan->SetTopVolume(top);
  TGeoTranslation* glob = new TGeoTranslation(0,0,-40+length/2.);
  top->AddNode(createFieldCageBarrel(),0,glob);
  top->AddNode(createGas(),0,glob);
  top->AddNode(createDriftCathode(),0,glob);
  top->AddNode(createPadPlane(),0,glob);
  top->AddNode(createCooling(), 0,glob);
  top->AddNode(createFE(), 0,glob);

  TEveManager::Create();
  
  TEveGeoTopNode* meh = new TEveGeoTopNode(geoMan, geoMan->GetTopNode());
  gEve->AddGlobalElement(meh);

  gEve->Redraw3D(kTRUE);
  
  TFile outfile("PANDA_TPC.root", "recreate");
  outfile.cd();
  top->Write();
  //geoMan->Write();

}





  



